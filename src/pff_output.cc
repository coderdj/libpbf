// *************************************************************
// 
// Protobuf File Format
//
// File   :     pff_output.cc
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access for writing .pff files
// 
// *************************************************************

#include "pff_output.hh"

pff_output::pff_output()
{
   SetDefaults();
}

pff_output::pff_output(string path, string options)
{
   SetDefaults();
   if(open_file(path,options)!=0)
     throw runtime_error("pff_output: Could not open file.");   
}

void pff_output::SetDefaults()
{
   m_protoOut = NULL;
   m_protoCOut = NULL;
   m_iCurrentHandle = m_iCurrentFileNumber = m_iEventsPerFile = m_iMaxBufferSize = 0;
   m_uiEventNumber = 0;
   m_bCompress_snappy = false;
   m_HeaderInfo.identifier="default";
   m_HeaderInfo.started_by=m_HeaderInfo.run_mode=m_HeaderInfo.notes="";
   m_HeaderInfo.start_date=m_HeaderInfo.creation_date=0;
   bHeaderWritten=false;
}

pff_output::~pff_output()
{
   write();
   m_mapOpenEvents.clear();
   close_file();
}

int pff_output::open_file(string path, string options)
{
   if(ParseOptions(options)!=0)
     cerr<<"Warning: pff file initialized with bad options string!"<<endl;
   
   m_iCurrentFileNumber =0;
   m_sFilePathBase = path;
   
   return OpenNextFile();
}

int pff_output::create_event(unsigned long long int timestamp, int &handle)
{
   InsertEvent tEvent;
   tEvent.timestamp=timestamp;
   m_mapOpenEvents.insert(pair<int,InsertEvent>(m_iCurrentHandle, tEvent));
   handle=m_iCurrentHandle;
   m_iCurrentHandle++;
   return 0;
}

int pff_output::add_data(int handle, int channel, char* data, 
			 size_t dataSize, u_int64_t datatime)
{
   return add_data(handle,channel,-1,data,dataSize,datatime);
}

int pff_output::add_data(int handle, int channel, int module,  char* data,
			 size_t dataSize, u_int64_t datatime)
{
   if(dataSize==0) return -1;
   MCPair mc; 
   mc.module = module;
   mc.channel = channel;
   InsertData idata;
  idata.timestamp = datatime;
 
   //compression with snappy
   if(m_bCompress_snappy)  {
      char* compressed = new char[snappy::MaxCompressedLength(dataSize)];
      size_t compressedLength = 0;
      snappy::RawCompress((const char*)(data),dataSize,compressed,
			  &compressedLength);
      idata.payload = compressed;
      idata.size = compressedLength;
      //delete[] data;
   }   
   //no compression
   else{	
      idata.payload = data;
      idata.size = dataSize;
   }   
   m_mapOpenEvents[handle].channels[mc].data.insert(idata);

   return 0;
}

int pff_output::close_event(int handle, bool writeout)
{  
   m_setWriteBuffer.insert(m_mapOpenEvents[handle]);
   map<int,InsertEvent>::iterator it;
   it = m_mapOpenEvents.find(handle);
   m_mapOpenEvents.erase(it);
   if(it==m_mapOpenEvents.end()) return -1;
   if(writeout)
     return write(0);
   return 0;
}

int pff_output::write(u_int64_t timestamp)
{
   if(!m_outfile.is_open() || m_protoOut==NULL || m_protoCOut==NULL) return -1;

   if(!bHeaderWritten) {
      if(WriteHeader()!=0) {
	 cerr<<"Error writing file header"<<endl;
	 return -1;
      }
   }
   
   set<InsertEvent>::iterator it = m_setWriteBuffer.begin();
   
   while(it!=m_setWriteBuffer.end())  {
      InsertEvent ev = *it;
      if(timestamp!=0 && timestamp>ev.timestamp)
	break;
      
      //write event
      pbf::Event pbEvent;
      pbEvent.set_time(ev.timestamp);
      pbEvent.set_number(m_uiEventNumber);
      m_uiEventNumber++;
      
      //loop though channels
      map<MCPair,InsertChannel>::iterator chIt;
      for(chIt = ev.channels.begin(); chIt!=ev.channels.end(); chIt++)	{
	 
	 //make channel obj
	 pbf::Event_Channel *pbChannel = pbEvent.add_channel();
	 pbChannel->set_id(chIt->first.channel);
	 if(chIt->first.module!=-1)
	   pbChannel->set_module(chIt->first.module);
	 
	 //loop through data
	 for(set<InsertData>::iterator dataIt = chIt->second.data.begin();
	     dataIt!=chIt->second.data.end(); dataIt++)  {
	    pbf::Event_Channel_Data *ecd = pbChannel->add_data();
	    ecd->set_payload((const void*)(*dataIt).payload,(*dataIt).size);
	    if((*dataIt).timestamp!=0) ecd->set_time((*dataIt).timestamp);
	    delete[] (*dataIt).payload;
	 }//end data loop
      }//end channel loop
      
      //create string with data
      string s="";
      pbEvent.SerializeToString(&s);
      //write size
      m_protoCOut->WriteVarint32(s.size());
      //write data
      m_protoCOut->WriteRaw(s.data(),s.size());
            
      //check if we went over the maximum file size
      if((int)m_uiEventNumber > (m_iEventsPerFile * (m_iCurrentFileNumber))
	 && m_iEventsPerFile>0)	{
	 cout<<"SWITCHING"<<endl;
	 if(OpenNextFile()!=0) {
	    cerr<<"Failed to open next file!"<<endl;
	    return -1;
	 }	 
	 cout<<"SWITCHED"<<endl;
      }
      set<InsertEvent>::iterator rmit = it;
      it++;
      m_setWriteBuffer.erase(rmit);
   }//end event loop   
   return 0;
}

void pff_output::close_file(bool quiet)
{
   if(!quiet) write();
   if(m_protoCOut!=NULL) delete m_protoCOut;
   if(m_protoOut!=NULL) delete m_protoOut;
   m_protoOut = NULL;
   m_protoCOut = NULL;
   if(m_outfile.is_open()) m_outfile.close();
   return;
}

int pff_output::WriteHeader()
{
   if(!m_outfile.is_open() || m_protoOut==NULL || m_protoCOut==NULL) return -1;
   
   //Check header values and set defaults where applicable
   //Optional fields with no value provided will not be written
   pbf::Header header;
   
   header.set_zipped(m_bCompress_snappy);
   time_t starttime=0;
   time(&starttime);
   header.set_creationdate((long long int)starttime);
   if(m_HeaderInfo.start_date==0) header.set_startdate((long long int) starttime);
   else header.set_startdate(m_HeaderInfo.start_date);
   header.set_filenumber(m_iCurrentFileNumber-1);
   header.set_runidentifier(m_HeaderInfo.identifier);
   
   //optional fields
   if(m_HeaderInfo.run_mode!="") header.set_runmode(m_HeaderInfo.run_mode);
   if(m_HeaderInfo.started_by!="") header.set_startedby(m_HeaderInfo.started_by);
   if(m_HeaderInfo.notes!="") header.set_notes(m_HeaderInfo.notes);
   
   //Write to file
   //create string with data
   string s="";
   header.SerializeToString(&s);
   //write size
   m_protoCOut->WriteVarint32(s.size());
   //write data
   m_protoCOut->WriteRaw(s.data(),s.size());
   
   bHeaderWritten = true;
   
   return 0;
}

int pff_output::OpenNextFile()
{
   if(m_outfile.is_open()) close_file(true);
   
   string extension = ".pff";
   stringstream fName;
   fName<<m_sFilePathBase<<setfill('0')<<setw(6)<<m_iCurrentFileNumber<<extension;
   m_iCurrentFileNumber++;
   
   m_outfile.open(fName.str().c_str(), ios::out | ios::trunc | ios::binary);
   if(!m_outfile.is_open())  {
      cerr<<"Failed to open outfile."<<endl;
      return -1;
   }
   m_protoOut = new google::protobuf::io::OstreamOutputStream(&m_outfile);
   m_protoCOut = new google::protobuf::io::CodedOutputStream(m_protoOut);
   bHeaderWritten=false;
   return 0;
}

int pff_output::ParseOptions(string options)
{
   //possibles options
   // n{int} num events per file
   // z zip with snappy
   // b{int} max buffer size
   
   //parse
   vector<string> items;
   stringstream ss(options);
   string item;
   while(getline(ss,item,':')){
      if(!item.empty())
	items.push_back(item);
   }
   
   for(unsigned int x=0;x<items.size();x++)  {
      if(items[x]=="z")
	m_bCompress_snappy=true;
      else if(items[x][0]=='n'){	      
	 string num = items[x].substr(1,items[x].size()-1);
	 istringstream(num) >> m_iEventsPerFile;
      }
      else if(items[x][0]=='b'){
	 string num = items[x].substr(1,items[x].size()-1);
	 istringstream(num) >> m_iMaxBufferSize;
      }
      else
	cerr<<"WARNING: Unknown option."<<endl;
   }
   return 0;
}

// *************************************************************
// 
// Protobuf File Format
//
// File   :     pbf_output.cc
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access for writing .pbf files
// 
// *************************************************************

#include "pbf_output.hh"

pbf_output::pbf_output()
{
   SetDefaults();
}

pbf_output::pbf_output(string path, string options)
{
   SetDefaults();
   if(open_file(path,options)!=0)
     throw runtime_error("pbf_output: Could not open file.");   
}

void pbf_output::SetDefaults()
{
   m_protoOut = NULL;
   m_protoCOut = NULL;
   m_iCurrentFileNumber = m_iEventsPerFile =0;
   m_iMaxBufferSize=1000;
   m_uiEventNumber = 0;
   m_bAlreadySnappified = m_bCompress_snappy = false;
   m_HeaderInfo.identifier="default";
   m_HeaderInfo.started_by=m_HeaderInfo.run_mode=m_HeaderInfo.notes="";
   m_HeaderInfo.start_date=m_HeaderInfo.creation_date=0;
   bHeaderWritten=false;
   bCloseFile=false;
   m_Lasttimestamp =1;
   pthread_mutex_init(&m_xOutputMutex,NULL);
   pthread_mutex_init(&m_xClosedEventsMutex,NULL);
}

pbf_output::~pbf_output()
{
   write();
   m_OpenEvents.clear();
   close_file();
   pthread_mutex_destroy(&m_xOutputMutex);
   pthread_mutex_destroy(&m_xClosedEventsMutex);   
}

int pbf_output::open_file(string path, string options)
{
   if(ParseOptions(options)!=0)
     cerr<<"Warning: pbf file initialized with bad options string!"<<endl;

   //resize open event buffer
   m_OpenEvents.resize(m_iMaxBufferSize);
   
   m_iCurrentFileNumber =0;
   m_sFilePathBase = path;

   bCloseFile=false;
   int ret = OpenNextFile();
   pthread_create(&m_WriteThread,NULL,pbf_output::WriteThreadWrapper,
		                                      static_cast<void*>(this));
   pthread_create(&m_CopyThread,NULL,pbf_output::CopyThreadWrapper,
		                                      static_cast<void*>(this));
   
   return ret;
}

int pbf_output::create_event(unsigned long long int timestamp, int &handle)
{
//   pthread_mutex_lock(&m_xEventBufferMutex);
      
   InsertEvent *tEvent = new InsertEvent;
   pthread_mutex_init(&(tEvent->EventMutex),NULL);
   tEvent->timestamp=timestamp;
   handle = m_OpenEvents.insert(tEvent);
   if(handle==-1) return -1;   
//   pthread_mutex_unlock(&m_xEventBufferMutex);   
   return 0;
}

int pbf_output::add_data(int handle, int channel, char* data, 
			 size_t dataSize, u_int64_t datatime)
{
   return add_data(handle,channel,-1,data,dataSize,datatime);
}

int pbf_output::add_data(int handle, int channel, int module,  char* data,
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
   }   
   //no compression
   else{	
      idata.payload = new char[dataSize];
      copy(data,data+dataSize,idata.payload);// = data;
      idata.size = dataSize;
   }   
//   pthread_mutex_lock(&m_mapOpenEvents[handle].EventMutex);
   m_OpenEvents[handle]->channels[mc].data.insert(idata);
//   pthread_mutex_unlock(&m_mapOpenEvents[handle].EventMutex);
   
   return 0;
}

void pbf_output::close_event(int handle, bool writeout)
{  
//   m_setWriteBuffer.insert(m_OpenEvents[handle]);
   pthread_mutex_lock(&m_xClosedEventsMutex);
   m_ClosedEvents.push_back(m_OpenEvents[handle]);
   pthread_mutex_unlock(&m_xClosedEventsMutex);
   m_OpenEvents.erase(handle);
   
   if(writeout)
     write(0);
   return;
}

void pbf_output::write(u_int64_t timestamp)
{
   m_Lasttimestamp = timestamp;
}

void* pbf_output::WriteThreadWrapper(void* data)
{
   pbf_output *po = static_cast<pbf_output*>(data);
   po->WriteThread();
   return (void*)data;
}

void* pbf_output::CopyThreadWrapper(void* data)
{
   pbf_output *po = static_cast<pbf_output*>(data);
   po->CopyThread();
   return (void*)data;
}

void pbf_output::CopyThread()
{
   while(!bCloseFile){
      usleep(100);
      
      unsigned int vecsize;
      pthread_mutex_lock(&m_xClosedEventsMutex);
      vecsize = m_ClosedEvents.size();
      pthread_mutex_unlock(&m_xClosedEventsMutex);
      
      while(vecsize>0)	{
	 InsertEvent *ev = m_ClosedEvents[0];
	 pbf::Event pbEvent;
	 pbEvent.set_time(ev->timestamp);
	 
	 //loop though channels
	 map<MCPair,InsertChannel>::iterator chIt;
	 for(chIt = ev->channels.begin(); chIt!=ev->channels.end(); chIt++)     {	      
	      
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
	 EventWrapper wrap;
	 wrap.event = pbEvent;
	 pthread_mutex_lock(&m_xOutputMutex);
	 m_setWriteBuffer.insert(wrap);
	 pthread_mutex_unlock(&m_xOutputMutex);
	 pthread_mutex_lock(&m_xClosedEventsMutex);
	 m_ClosedEvents.erase(m_ClosedEvents.begin());
	 vecsize = m_ClosedEvents.size();
	 pthread_mutex_unlock(&m_xClosedEventsMutex);
      }//end while closedevents size
   }   //end while !bClosedFile
   return;
}

void pbf_output::WriteThread()
{
   if(!m_outfile.is_open() || m_protoOut==NULL || m_protoCOut==NULL) return;
   
   while(!bCloseFile) {	
      
      set<EventWrapper>::iterator it = m_setWriteBuffer.begin();
      usleep(10); //avoid 100% cpu
      while(it!=m_setWriteBuffer.end())  {
	 if(!bHeaderWritten)    {	      
	    if(WriteHeader()!=0) {		   
	       cerr<<"Error writing file header"<<endl;
	       return;
	    }
	 }	 
	 EventWrapper ev = *it;
	 if(m_Lasttimestamp!=0 && m_Lasttimestamp>ev.event.time())
	   break;
	 
	 //have to set event number here
	 ev.event.set_number(m_uiEventNumber);
	 m_uiEventNumber++;
	 
	 //WRITING STARTS HERE
	 //create string with data
	 string s="";
	 ev.event.SerializeToString(&s);
	 //write size
	 m_protoCOut->WriteVarint32(s.size());
	 //write data
	 //ask if there is enough space in buffer for s
	 void **dummy;
	 int datasize = s.size();
	 while(!m_protoCOut->GetDirectBufferPointer((dummy),
						    (int*)(&(datasize)))){
	    usleep(1);
//	    	    cout<<"NotEnoughSpace"<<endl;
	 }
	    
	 m_protoCOut->WriteRaw(s.data(),s.size());
	 
	 //check if we went over the maximum file size
	 if((int)m_uiEventNumber > (m_iEventsPerFile * (m_iCurrentFileNumber))
	    && m_iEventsPerFile>0)	{
	    if(OpenNextFile()!=0) {
	       cerr<<"Failed to open next file!"<<endl;
	       return;
	    }	 
	 }
	 pthread_mutex_lock(&m_xOutputMutex);
//	 cout<<"Size = "<<m_setWriteBuffer.size()<<" "<<s.size()<<" "<<m_iCurrentFileNumber<<" "<<m_iEventsPerFile<<" "<<m_uiEventNumber<<endl;
	 set<EventWrapper>::iterator rmit = it;
	 it++;
	 m_setWriteBuffer.erase(rmit);
//	 cout<<"Size2 = "<<m_setWriteBuffer.size()<<endl;
	 
	 pthread_mutex_unlock(&m_xOutputMutex);
      }//end event loop   
   }//end bCloseFileLoop
   
   return;
}

void pbf_output::close_file(bool quiet)
{
   if(!quiet && m_outfile.is_open()) {
      write();
      while(m_setWriteBuffer.size()!=0 || m_ClosedEvents.size()!=0)
	bCloseFile=false;
      bCloseFile=true;
      pthread_join(m_WriteThread,NULL);
      pthread_join(m_CopyThread,NULL);
   }
      
   if(m_protoCOut!=NULL) delete m_protoCOut;
   if(m_protoOut!=NULL) delete m_protoOut;
   m_protoOut = NULL;
   m_protoCOut = NULL;
   if(m_outfile.is_open()) m_outfile.close();
   return;
}

int pbf_output::WriteHeader()
{
   if(!m_outfile.is_open() || m_protoOut==NULL || m_protoCOut==NULL) return -1;
   
   //Check header values and set defaults where applicable
   //Optional fields with no value provided will not be written
   pbf::Header header;
   
   if(m_bCompress_snappy || m_bAlreadySnappified)
     header.set_zipped(true);
   else
     header.set_zipped(false);
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

int pbf_output::OpenNextFile()
{
   if(m_outfile.is_open()) close_file(true);
   
   string extension = ".pbf";
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

int pbf_output::ParseOptions(string options)
{
   //possibles options
   // n{int} num events per file
   // z zip with snappy
   // pz pre-zipped (already zipped) with snappy
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
      else if(items[x]=="pz")
	m_bAlreadySnappified=true;
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

// *************************************************************
// 
// Protobuf File Format
//
// File   :     pbf_input.cc
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access to .pbf infiles
// 
// *************************************************************

#include "pbf_input.hh"

pbf_input::pbf_input()
{
   Initialize();
}

pbf_input::pbf_input(string path)
{
   Initialize();
   if(open_file(path)!=0)
     throw runtime_error("pbf_input: Could not open file.");   
}

void pbf_input::Initialize()
{
   m_Header.start_date = m_Header.creation_date = 0;
   m_Header.identifier = m_Header.run_mode = m_Header.started_by = m_Header.notes = "";
   m_iCurrentFileNumber = 0;
   m_bSnappyCompression = false;
   m_gCodedInput=NULL;
   m_gZCInput=NULL;
   m_bHasEvent=false;
   m_sFilePathBase="";
}

pbf_input::~pbf_input()
{
   close_file();
}

int pbf_input::open_file(string path)
{
   m_sFilePathBase = path;
   m_iCurrentFileNumber = 0;
   return OpenNextFile();
}

void pbf_input::close_file()
{
   if(!m_infile.is_open()) return;
   m_infile.close();
   if(m_gCodedInput!=NULL) delete m_gCodedInput;
   if(m_gZCInput!=NULL) delete m_gZCInput;
   m_gZCInput=NULL;
   m_gCodedInput=NULL;
}

int pbf_input::get_event(long long int eventID)
{
   if(m_bHasEvent)  {
      if(event_number()==eventID)
	return 0;
   }
   while(get_next_event()==0)  {
      if(event_number()==eventID)
	return 0;
   }   
   return -1;
}

int pbf_input::get_next_event()
{
   if(m_infile.eof()){
      if(OpenNextFile()!=0)	{
	 cout<<"End of input."<<endl;
	 return -1;
      }      
   }
   
   //get size
   u_int32_t dataSize = 0;
   m_gCodedInput->ReadVarint32(&dataSize);
   
   //read from file
   pbf::Event event;
   string dataString="";
   m_gCodedInput->ReadString(&dataString,dataSize);
      
   //reinterpret string as header
   m_bHasEvent = false;
   if(event.ParseFromString(dataString)){
      m_CurrentEvent = event;
      m_bHasEvent = true;
      return 0;
   }
   cerr<<"Could not parse event from data. Is the file a valid .pbf file?"<<endl;  
   return -1;
}

int pbf_input::event_number()
{
   if(!m_bHasEvent) return -1;
   return m_CurrentEvent.number();
}

int pbf_input::num_channels()
{
   if(!m_bHasEvent) return -1;
   return m_CurrentEvent.channel_size();
}

int pbf_input::num_data(int channelindex)
{
   if(!m_bHasEvent) return -1;
   if(channelindex>m_CurrentEvent.channel_size()) return -1;
   
   pbf::Event_Channel channel = m_CurrentEvent.channel(channelindex);
   
   return channel.data_size();
}

int pbf_input::num_data_id(int channelid, int moduleid)
{
   if(!m_bHasEvent) return -1;
   //simple linear search. make this smarter in the future
   for(int x=0;x<m_CurrentEvent.channel_size();x++)  {
      pbf::Event_Channel channel = m_CurrentEvent.channel(x);
      if(channel.id()==channelid && ((moduleid==-1 && !channel.has_module()) || 
				     (channel.has_module() && moduleid==channel.module())))	{
	 return channel.data_size();
      }      
   }
   return -1;
}

int pbf_input::channel_id(int channelindex, int &channelid, int &moduleid)
{
   if(!m_bHasEvent) return -1;
   if(channelindex>m_CurrentEvent.channel_size()) return -1;
   
   pbf::Event_Channel channel = m_CurrentEvent.channel(channelindex);
   if(channel.has_module()) moduleid = channel.module();
   else moduleid=-1;
   channelid = channel.id();
   return 0;
}

int pbf_input::get_channel_handle(int channelid, int moduleid)
{
   if(!m_bHasEvent) return -1;
   for(int x=0; x<num_channels();x++){	
      pbf::Event_Channel channel = m_CurrentEvent.channel(x);
      if((channel.id()==channelid) && ((channel.has_module() && channel.module()==moduleid) ||
				       (!channel.has_module() && moduleid==-1)))
	return (int)x;
   }
   return -1;//not found
}

int pbf_input::get_data(int channelindex, int dataindex, char*&data, 
			unsigned int &size, long long int &dataTime)
{
   if(!m_bHasEvent) return -1;
   if(channelindex>m_CurrentEvent.channel_size()) return -1;
   pbf::Event_Channel Channel = m_CurrentEvent.channel(channelindex);
   if(dataindex>Channel.data_size()) return -1;
   pbf::Event_Channel_Data Data = Channel.data(dataindex);
   
   //get time if there
   if(Data.has_time()) dataTime = Data.time();
   else dataTime = 0;
  
   //extract data
   string payload = Data.payload();
   if(!m_bSnappyCompression)  {
      data = (char*) payload.data();
      size = payload.size();
      return 0;
   }
   //decompress
   size_t extractLength;
   snappy::GetUncompressedLength((const char*)payload.data(),(size_t)payload.size(),
				 &extractLength);
   //char extracted[extractLength];
   delete[] data;
   data = new char[extractLength];
   snappy::RawUncompress((const char*)payload.data(),(size_t)payload.size(),data);
   size = extractLength;
   
   return 0;
   
}


int pbf_input::OpenNextFile()
{   
   if(m_infile.is_open()) close_file();
   
   string extension = ".pbf";
   string filepath = "";
   
   //Check if file has extension
   if(m_sFilePathBase[m_sFilePathBase.size()]=='f' &&
      m_sFilePathBase[m_sFilePathBase.size()-1]=='b' &&
      m_sFilePathBase[m_sFilePathBase.size()-2]=='p' &&
      m_sFilePathBase[m_sFilePathBase.size()-3]=='.')
     filepath = m_sFilePathBase;
   else{   //if not, assume you have a stub
      stringstream fName;
      fName<<m_sFilePathBase<<setfill('0')<<setw(6)<<m_iCurrentFileNumber<<extension;
      filepath=fName.str();
   }   
   
   m_infile.open(filepath.c_str(), ios::in | ios::binary);
   if(!m_infile.is_open())  {
      cerr<<"Error opening file."<<endl;
      return -1;
   }
   m_gZCInput = new google::protobuf::io::IstreamInputStream(&m_infile);
   m_gCodedInput = new google::protobuf::io::CodedInputStream(m_gZCInput);
   
   if(ReadHeader()!=0){
      cerr<<"pbf_input::OpenNextFile - No header found, not a valid pbf file?"<<endl;
      return -1; 
   }   
   
   return 0;
}

int pbf_input::ReadHeader()
{
   //get size
   u_int32_t headerSize = 0;
   m_gCodedInput->ReadVarint32(&headerSize);
   
   //read from file
   pbf::Header header;
   string dataString="";
   m_gCodedInput->ReadString(&dataString,headerSize);

   //reinterpret string as header
   if(header.ParseFromString(dataString)){
      //Read data in
      if(header.zipped()) m_bSnappyCompression = true;
      else m_bSnappyCompression = false;
      m_iCurrentFileNumber = header.filenumber();
      m_Header.start_date = header.startdate();
      m_Header.creation_date = header.creationdate();
      m_Header.identifier = header.runidentifier();
      if(header.has_runmode()) m_Header.run_mode = header.runmode();
      else m_Header.run_mode = "";
      if(header.has_startedby()) m_Header.started_by = header.startedby();
      else m_Header.started_by = "";
      if(header.has_notes()) m_Header.notes = header.notes();
      else m_Header.notes = "";
   }
   else  {
      cerr<<"Error reading file header. Is this a valid .pbf file?"<<endl;
      return -1;
   }
   
   return 0;
}

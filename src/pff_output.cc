// *************************************************************
// 
// Protobuf File Format
//
// File   :     pff_output.cc
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access for writing .pff files
// 
// *************************************************************

#include <stdexcept>
#include "pff_output.hh"

pff_output::pff_output()
{
}

pff_output::pff_output(string path, string options)
{
   if(open_file(path,options)!=0)
     throw Exception("pff_output: Could not open file.");   
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
   pbf::Event_Channel_Data idata;
   if(datatime!=0) idata.set_time(datatime);
   //compression with snappy
   if(m_bCompress_snappy)  {
      char* compressed = new char[snappy::MaxCompressedLength(dataSize)];
      size_t compressedLength = 0;
      snappy::RawCompress((const char*)(data),dataSize,compressed,
			  &compressedLength);
      idata.set_data(compressed,compressedLength);
      delete[] compressed;
   }   
   //no compression
   else
     idata.set_data(data,dataSize);
   
   m_mapOpenEvents[handle][mc].data.insert(idata);

   return 0;
}

int pff_output::CloseEvent(int handle, bool write=false)
{  
   m_setWriteBuffer.insert(m_mapOpenEvents[handle]);
   map<int,InsertEvent>::iterator it;
   it = m_mapOpenEvents.find(handle);
   m_mapOpenEvents.erase(it);
   if(it==m_mapOpenEvents.end()) return -1;
   if(write)
     return write(-1);
   return 0;
}

int pff_output::write(u_int64_t timestamp)
{
   if(!m_outfile.is_open()) return -1;
   
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
      for(chIt = ev.channels.begin(); chIt<ev.channels.end(); chIt++)	{
	 
	 //make channel obj
	 pbf::Event_Channel pbChannel;
	 pbChannel.set_channel(chIt->first.channel);
	 if(chIt->first.module!=-1)
	   pbChannel.set_module(chIt->first.module);
	 
	 //loop through data
	 for(set::iterator dataIt = chIt->second().data.begin(),
	     dataIt<chIt->second().data.end(); dataIt++)  {
	    pbChannel.set_data(*dataIt);
	 }//end data loop
      }//end channel loop
      
      //create string with data
      string s="";
      ev.SerializeToString(&s);
      //write size
      m_protoCOut->WriteVarint32(s.size());
      //write data
      m_protoCOut->WriteRaw(s.data(),s.size());
      
      m_setWriteBuffer.erase(it);
      
      //check if we went over the maximum file size
      if(m_uiEventNumber > (m_iEventsPerFile * (m_iCurrentFileNumber+1))
	 && m_iEventsPerFile!=-1)	{
	 if(OpenNextFile()!=0) {
	    cerr<<"Failed to open next file!"<<endl;
	    return -1;
	 }	 
      }
      it++;
   }//end event loop   
   return 0;
}

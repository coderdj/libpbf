#ifndef PBF_OUTPUT_HH_
#define PBF_OUTPUT_HH_

// *************************************************************
// 
// Protobuf File Format
//
// File   :     pbf_output.hh
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access for writing .pbf files
// 
// *************************************************************

#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include "protocDef.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <fcntl.h>
#include <snappy.h>
#include <pthread.h>
#include "dcIndexedList.hh"

using namespace std;

//Header Struct
//    : Used to set data for the file header
struct HeaderData
{
   //user data
   long long int start_date;     //start date and time of run 
   long long int creation_date;  //creation date
   string identifier;            //run identifier (run name)   
   string run_mode;              //run mode
   string started_by;            //who started the run
   string notes;                 //misc stuff
};

// Insert Structs
//    : Used to ensure proper ordering of data files. The sets are sorted such that 
//      events are sorted by time stamp. Channels are sorted by module/channel number
//      and data is sorted by time stamp. These will be converted to pbf objects upon write
//      
struct EventWrapper
{
   pbf::Event event;
   bool operator<(const EventWrapper &rhs) const{
      if(event.time()==0) return true;
      return (event.time()<rhs.event.time() ? true : false);
   }   
};
struct InsertData
{
   char* payload;
   size_t size;
   u_int64_t timestamp;
   bool operator<(const InsertData& rhs)  const {
      if(timestamp==0) return true;
      return (timestamp<rhs.timestamp ? true : false);
   }
   
};
struct InsertChannel
{
   set<InsertData> data;
};
struct MCPair
{
   int module;
   int channel;
   bool operator<(const MCPair& rhs) const {
      if(module==rhs.module)
	return (channel<rhs.channel ? true : false);
      return (module < rhs.module ? true : false);
   }   
};
struct InsertEvent
{
   pthread_mutex_t EventMutex;
   u_int64_t timestamp;
   bool operator<(const InsertEvent& rhs)  const {
      return (timestamp<rhs.timestamp ? true : false);
   }
   map<MCPair,InsertChannel> channels;
};


class pbf_output
{
 public: 
   
   // 
   // Constructors
   // 
   // pbf_output()
   //   : Default constructor
   // 
   pbf_output();
   //
   // pbf_output(string path, string options)
   //   : Initialize object and call open_file. Raise exception on failure
   // 
   pbf_output(string path, string options);
   //
   // ~pbf_output()
   //    : Will write out buffer (if not empty) and close file (if open)
   // 
   virtual ~pbf_output();
   
   //
   // int open_file(string path, string options)
   //   : Opens the file at path. Path should not specify a file extension. 
   //     Options are separated by a colon. Possible options are: 
   //                    - n{int} where int is the number of events per file
   //                             zero means all events in one file (default)
   //                    - z zip output using snappy
   //                    - b{int} give maximum buffer size in events before 
   //                             buffer is forced to be written (default 1000)
   //     Example string "n1000:b100:z" 
   // 
   int open_file(string path, string options);
   //
   // HeaderData header()
   //   : Direct access to header data struct. Set the header information as 
   //     follows:
   //                   header.start_date - long long int, run start time
   //                   header.identifier - string, run identifier
   //                   header.run_mode   - string, operation mode
   //                   header.started_by - string, who started the run
   //                   header.notes      - string, misc information
   //     This must be done before writing any data. Once writing has started
   //     the header is locked and this will raise an exception.
   // 
   HeaderData header() {
      if(m_uiEventNumber==0) return m_HeaderInfo;
      else throw runtime_error("pff_output: Header must be set before writing");
   };   
   
   // int create_event(long int TimeStamp, long int &handle)
   //   : Create an event and give it an identifying handle. Note the handle is 
   //     not the event number. The event number is only defined upon writing so 
   //     that event numbers in the file are consecutive. If timeStamp is defined
   //     as -1, it will be set to the earliest dataTime stamp provided. If no dataTime 
   //     stamp is provided it is set to zero and written in arbitrary order.
   // 
   int create_event(unsigned long long int timestamp, int &handle);
   //
   // int add_data(int handle, int channel, char* data, long int dataTime=-1)
   // int add_data(int handle, int channel, int module, char* data, longInt dataTime=-1)
   //   : Add the data contained in the data buffer to an event. dataTime represents the 
   //     time of the first sample in the data buffer. The default value of -1 means that
   //     the first sample has the same time as the event timestamp. It is important
   //     to note that these functions TAKE OWNERSHIP of char* data and will delete
   //     it when they are finished with it.
   // 
   int add_data(int handle, int channel, char* data, size_t dataSize, u_int64_t dataTime=0);
   int add_data(int handle, int channel, int module, char* data, size_t dataSize, u_int64_t dataTime=0);
   //
   // void close_event(int handle, bool write=false)
   //   : If you are done adding data to an event, close it and put it in the output buffer
   //     The output buffer is sorted by time stamp. The output buffer is either written out
   //     by a manual call to the write function or when it reaches the size defined when the
   //     file it opened. Setting the write flag to true puts the event directly into the file
   //     along with any other contents of the output buffer.
   // 
   void close_event(int handle, bool writeout=false);
   //
   // void write(u_int64_t timestamp=0)
   //   : Write all events up to timestamp. If timestamp is set to zero, write 
   //     all events in the output buffer. 
   // 
   void write(u_int64_t timestamp=0);
   
   //
   // void close_file(bool quiet = false);
   //   : Write all events in the output buffer and close the file. Note: any events that were
   //     created but not closed will be lost! quiet determines if the buffer is written before
   //     closing. true=no.
   // 
   void close_file(bool quiet=false);

   static void*            WriteThreadWrapper(void *data);
   static void*            CopyThreadWrapper(void *data);
   
 private:
   
   u_int32_t        m_uiEventNumber;        //current event number
   
   //Internal Options
   int              m_iEventsPerFile;
   int              m_iMaxBufferSize;
   bool             m_bCompress_snappy,m_bAlreadySnappified;
   string           m_sFilePathBase;
   int              m_iCurrentFileNumber;
   HeaderData       m_HeaderInfo;
   
   //Internal functions
   void             SetDefaults();
   int              ParseOptions(string options);
   int              OpenNextFile();
   int              WriteHeader();
   void             WriteThread();
   void             CopyThread();
   
   //Data storage
   bool       bHeaderWritten;
   bool       bCloseFile;
   u_int64_t  m_Lasttimestamp;

   dcIndexedList<InsertEvent*> m_OpenEvents;
   vector<InsertEvent*> m_ClosedEvents;
   set<EventWrapper>    m_setWriteBuffer;

   //the current file handle and weird google::protobuf write objects
   ofstream            m_outfile;
   google::protobuf::io::ZeroCopyOutputStream *m_protoOut;
   google::protobuf::io::CodedOutputStream    *m_protoCOut;

   //thread-safe stuff
   pthread_mutex_t m_xOutputMutex;
   pthread_mutex_t m_xClosedEventsMutex;
   pthread_t m_WriteThread, m_CopyThread;
   //   pthread_mutex_t    m_xEventBufferMutex;
//   pthread_mutex_t    m_xWriteBufferMutex;
};
#endif
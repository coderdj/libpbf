#ifndef PBF_INPUT_HH_
#define PBF_INPUT_HH_

// *************************************************************
// 
// Protobuf File Format
//
// File   :     pbf_input.hh
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access to .pbf infiles
// 
// *************************************************************


#include "pbf_output.hh"

using namespace std;

class pbf_input
{
 public: 

   // Constructors
   // 
   // pbf_input()
   //   : Default constructor
   // 
   pbf_input();
   //
   // pbf_input(string path)
   //   : Initialize with a file path. Raises an exception if it can't open the file.
   // 
   pbf_input(string path);
   //
   // ~pbf_input()
   //   : Will close file if open.
   // 
   virtual ~pbf_input();
   
   //
   // int open_file(string path)
   //   : In case file was not opened with constructor can use this one. Can specify either
   //     an absolute path (with extension) or a path stub where the numbering and extension
   //     are omitted. In the latter case this will start with file {stub}000000.pbf and increment
   //     through files as events are browsed.
   // 
   int open_file(string path);
   
   //Navigation through file
   //
   // int get_next_event()
   //   : Gets the next event. Returns 0 if there is one and -1 if not.
   // 
   int get_next_event();
   //
   // int get_event(long long int eventID)
   //   : Get an event with a specific ID. Returns 0 if found and -1 if not
   // 
   int get_event(long long int eventID); 
   
   //Info about current event
   //
   // int event_number()
   //   : index number of event
   //
   int event_number();
   //
   // int num_channels()
   //   : number of channels included in current event
   // 
   int num_channels();
   //
   // int num_data(int channel)
   //   : number of data fields in specific channel. In the 'id' version you can search for a 
   //     specific channel and module id. The other version looks for a channel by index (allowing
   //     a loop through all channels).
   // 
   int num_data_id(int channelid, int moduleid=-1);
   int num_data(int channelindex);
   //
   // int get_channel_handle(int channelid, int moduleid=-1)
   //   : look up channel index handle by module/channel id numbers
   //     return -1 on error or if not found
   int get_channel_handle(int channelid, int moduleid=-1);
  
   //
   // int channel_id(int channelindex, int &channelid, int &moduleid)
   //   : Get modules and channel number for a specific index. If a module id is not provided this
   //     value is set to -1.
   // 
   int channel_id(int channelindex, int &channelid, int &moduleid);
   //
   // int get_data(int channel, int data, char *&data, long long int &dataTime)
   //   : Get the data from channel at index data. Puts it into the data field. Sets dataTime
   //     to the data timestamp if there is one, otherwise datatime is set to zero.
   // 
   int get_data(int channelindex, int dataindex, char *&data, 
		unsigned int &size, long long int &dataTime, int &integral);
   
   HeaderData Header()  {
      return m_Header;
   };
   
   void close_file();
   
 private:
   //file handle
   string m_sFilePathBase;
   ifstream m_infile;
   google::protobuf::io::ZeroCopyInputStream *m_gZCInput;
   google::protobuf::io::CodedInputStream *m_gCodedInput;
   
   //header info
   HeaderData  m_Header;
   int m_iCurrentFileNumber;
   bool m_bSnappyCompression;
   
   //current event 
   bool m_bHasEvent;
   pbf::Event m_CurrentEvent;
   
   // Private methods
   int OpenNextFile();
   int ReadHeader();
   void Initialize();
};
#endif

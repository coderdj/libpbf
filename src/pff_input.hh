#ifndef PFF_INPUT_HH_
#define PFF_INPUT_HH_

// *************************************************************
// 
// Protobuf File Format
//
// File   :     pff_input.hh
// Author :     Daniel Coderre, LHEP, Universitaet Bern
// Brief  :     Top end access to .pff infiles
// 
// *************************************************************

//
// includes
#include "protobufDef.pb.h"

class pff_input
{
 public: 
   
   pff_input();
   pff_input(string path);
   virtual ~pff_input();
   
   int open_file(string path);
   
   //move through file
   int get_next_event();
   int get_event(long int eventID);
   
   //info about current event
   int num_channels();
   int num_data(int channel);
   int get_data(channel, data, char *&data, long int &dataTime);
   
   int close_file();
 private:
   //file handle
   //current event object pointer
   
};
#endif
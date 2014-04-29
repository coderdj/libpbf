#include <getopt.h>
#include <sstream>
#include <iostream>
#include <TH1F.h>
#include <pff_input.hh>

using namespace std;

int main(int argc, char *argv[])
{
   //process command line options
   string sPath = "/home/dan/data/test";
   int iEventNumber =0;
   
   int c;
   stringstream tempss;
   while(1)  {
      static struct option long_options[] =
	{	   
	   {"file", required_argument, 0, 'f'},
	   {"event", required_argument, 0, 'e'},
	   {0,0,0,0}
	};

      int option_index = 0;
      c = getopt_long(argc, argv, "f:e:",
		      long_options, &option_index);
      if(c==-1) break;

      switch(c) {	   
       case 'f':
	 sPath = optarg;
	 break;
       case 'e':
	 tempss<<optarg;
	 tempss>>iEventNumber;
	 cout<<"EVENT NUMBER SEARCH NOT YET SUPPORTED"<<endl;
	 break;
       default: 
	 break;
      }//end switch
      
   }//end while   
      
   cout<<"Event "<<iEventNumber<<" Path "<<sPath<<endl;
   return 0;
   pff_input FileReader;
   if(FileReader.open_file(sPath)!=0)  {
      cout<<"Bad path"<<endl;
      return -1;
   }
   
   //now loop though events
   
};
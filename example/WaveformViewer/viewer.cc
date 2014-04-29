#include <getopt.h>
#include <sstream>
#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TApplication.h>
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

   pff_input FileReader;
   if(FileReader.open_file(sPath)!=0)  {
      cout<<"Bad path"<<endl;
      return -1;
   }
   cout<<FileReader.Header().creation_date<<endl;
   
   TApplication *theApp = new TApplication("App",&argc,argv);
   TCanvas *can = new TCanvas("can","can");
   can->cd();
   TH1F *waveform = NULL;
   while(FileReader.get_next_event()==0)  {
      for(int x=0;x<FileReader.num_channels();x++)	{
	 char *data;
	 unsigned int datasize;
	 long long int dataTime;
       
	 for(int y=0;y<FileReader.num_data(x);y++){	      
	    if(waveform!=NULL) delete waveform;
	    
	    FileReader.get_data(x,y,data,datasize,dataTime);
	    u_int32_t *easierData = (u_int32_t*)data;
	    cout<<"Got some data with size "<<datasize<<endl;
	    waveform = new TH1F("waveform","",datasize/4,0,datasize/4);
	    for(int z=0;z<datasize/4;z+=2)  {
	       waveform->SetBinContent(z,easierData[z/2]&0xFFFF);
	       waveform->SetBinContent(z+1,(easierData[z/2]>>16)&0xFFFF);
	    }
	    waveform->Draw();
	    can->Update();
	    char dat;
	    cin>>dat;
	 }	    
      }      
   }
   
   //now loop though events
   return 0;
};
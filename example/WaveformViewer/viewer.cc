#include <getopt.h>
#include <sstream>
#include <iostream>
#include <TH1F.h>
#include <TFile.h>
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

   //Stuff for long time waveform
   TH1F *longwaveform = new TH1F("longwaveform","",100000,0,100000);
   int chosenChannel=-1;
   int64_t startTime=0;
   int chosenModule=-1;
   //
   
   can->cd();
   TH1F *waveform = NULL;
   while(FileReader.get_next_event()==0)  {
          
      int hEventNumber = FileReader.event_number();
      for(int x=0;x<FileReader.num_channels();x++)	{
	 char *data;
	 unsigned int datasize;
	 long long int dataTime;
       
	 for(int y=0;y<FileReader.num_data(x);y++){	      
	    if(waveform!=NULL) delete waveform;
	    
	    FileReader.get_data(x,y,data,datasize,dataTime);
	    u_int32_t *easierData = (u_int32_t*)data;
	    int hChannelID=0,hModuleID=0;
	    FileReader.channel_id(x,hChannelID,hModuleID);
	    cout<<"*****************************************"<<endl;
	    cout<<endl;
	    cout<<"Event      : "<<hEventNumber<<endl;
	    cout<<"Channel    : "<<hChannelID<<endl;
	    cout<<"Module     : "<<hModuleID<<endl;
	    cout<<"Size(bytes): "<<datasize<<endl;
	    cout<<"Time       : "<<dataTime<<endl;
	    cout<<endl;
	    cout<<"******************************************"<<endl<<endl;
	    waveform = new TH1F("waveform","",datasize/2,0,datasize/2);	    
	    
	    bool recordLong=false;
	    int startBin=0;
	    if(chosenChannel==-1 && hEventNumber>900)  {
	       chosenChannel = hChannelID;
	       chosenModule = hModuleID;
	       startTime = dataTime;
	       recordLong=true;
	    }
	    else if(hModuleID==chosenModule && hChannelID==chosenChannel)  {
	       
	       startBin = dataTime - startTime;
	       cout<<"LONG ONE: STARTBIN="<<startBin<<" "<<dataTime<<" "<<startTime<<" "<<dataTime-startTime<<endl;
	       recordLong=true;
	    }
	    if(!recordLong) continue;
	    
	    for(int z=0;z<datasize/2;z+=2)  {
	       waveform->SetBinContent(z+1,easierData[z/2]&0xFFFF);
	       waveform->SetBinContent(z+2,(easierData[z/2]>>16)&0xFFFF);
	       
	       if(recordLong && startBin<99000)	 {
		  longwaveform->SetBinContent(z+1+startBin,easierData[z/2]&0xFFFF);
		  longwaveform->SetBinContent(z+2+startBin,(easierData[z/2]>>16)&0xFFFF);
	       }
	       
	    }
	    longwaveform->Draw();
	    can->Update();
	    char dat;
	    cin>>dat;
	    if(dat=='s') {
	       TFile *file = TFile::Open("lastGraph.root","RECREATE");
	       can->Write();
	       file->Close();
	    }
	    
	 }	    
      }      
   }
   
   //now loop though events
   return 0;
};
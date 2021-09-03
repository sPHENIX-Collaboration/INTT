#include <iomanip>

#include <iostream>
#include <fstream>

#include <cstdio>

#include "TNtuple.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

int getFileSize(std::ifstream& s);

void check_chip_prototype2(char* fname="C:/Users/sphenix/Documents/INTT_testbench/data",int usemod=3,int maxbuf=0,int n_meas=64,float maxscale=200.)
{
  gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);

  char cf_name[100];
  unsigned long int dataIn[1000000];
  long lSize;
  size_t result;

  int bco,adc,ampl,col_raw,col,chip_id,fpga_id,module,mchip;
  int chan_id,n_hits,nhit_event,nchip_event,fem_id;
  int chiphit[8];
  int event_fem,old_event_fem,bco_full,old_bco_full,old_bco;

  FILE* cal_file;

  const int NCHAN   =128;
  const int NMODULES=8;
  const int NCHIPS  =26;

  TNtuple *hits =new TNtuple("hits", "test", "chan_id:ampl:adc:rawchip:event_fem:bco_full:fem_id:module:chip_id:event");
  TNtuple *event=new TNtuple("event","event","nhits:event_fem:bco:bco_full:nchip_event:ampl");
  
  TTree *tree=new TTree("tree","chip info");
  tree->Branch("adc",     &adc,     "adc/I");
  tree->Branch("ampl",    &ampl,    "ampl/I");
  tree->Branch("chip_id", &chip_id, "chip_id/I");
  tree->Branch("fpga_id", &fpga_id, "fpga_id/I");
  tree->Branch("module",  &module,  "module/I");
  tree->Branch("chan_id", &chan_id, "chan_id/I");
  tree->Branch("fem_id",  &fem_id,  "fem_id/I");
  tree->Branch("bco",     &bco,     "bco/I");
  tree->Branch("bco_full",&bco_full,"bco_full/I");

  std::ifstream f(fname,std::ifstream::binary);

  if(!f.is_open()){
    std::cout<<"Failed to open input file "<<fname<<std::endl;
    return;
  }

  // get size of file
  int size=getFileSize(f);
  std::cout<<"Input file "<<fname<<std::endl;
  std::cout<<"Number of bytes in file = "<<size<<std::endl;

  // Brute force approach: allocate a single array to hold the entire
  // contents of the file
  UInt_t* data=new UInt_t [size/sizeof(UInt_t)];

  f.read((char*)data,size);
  if(f.fail()){
    std::cout<<"eof, fail, or bad condition during read"<<std::endl;
    delete [] data;
    return;
  }

  // Counter to count the number of hits for each channel and amplitude:
  int nhits[128][1024];

  for(int ichan=0;ichan<NCHAN;ichan++)
    for(int iamp=0;iamp<n_meas;iamp++) nhits[chan_id][iamp]=0;

  // Extract bco, channel, amplitude, and adc from each hit, and count the number of 
  // hits at each amplitude for each channel:

  int ievent          =0;
  int bufcnt          =0;
  int found[13]       ={0};
  int chipnotfound[13]={0};
  int countchips      =0;
  old_event_fem=-999;
  tree->Branch("event",&ievent,"event/I");

  for(int index=0;index<size/sizeof(int);bufcnt++){
    if(maxbuf && bufcnt>=maxbuf) break;
#ifdef OLD
    // Get record size
    int start=index;
    int cnt  =data[start];
    // Unpack the record
    int word1=data[start+1];
    int word2=data[start+2];
#else
    int buflen=data[index];
    int bufid =data[index+1];
    int cnt   =buflen-1;
    int start =index+2;
    //std::cout << buflen << " " << bufid << " " << cnt << std::endl;
    
    // Fake the old fmt logic
    int word1=0xFFFFFFFF;
    int word2=0xFFFFFFFF;
    if(bufid==100) word1=100;
    if(bufid==101){word1=0xFFFFFFFF; word2=0xFFFFFFFF;}
    if(bufid==102) word1=102;
#endif
    if(word1==0xFFFFFFFF && word2==0xFFFFFFFF){
      if(bufid==101){
	// This record is a time stamp
	// Format of timestamp record is
	//   data[0] = word count, excluding this word and checksum
	//   data[1] = 0xFFFFFFFF
	//   data[2] = 0xFFFFFFFF
	//   data[3] = time in clock ticks for cpu
	//   data[4] = CLOCKS_PER_SEC taken from the system value
	//   data[5] = checksum for buffer
	std::cout<<"Buffer "<<bufcnt<<": Timestamp"<<std::endl;
#ifdef OLD
	index=start+cnt+2;
#else
	index=start+cnt;
#endif
	//index = start + cnt + 2;
	continue;
      }
    }
    else if(word1==100){
      // This buffer is a configuration record
      std::cout<<"Buffer "<<bufcnt<<": Configuration "<<std::endl;
      index+=2;
      int runno=data[index++];
      std::cout<<"    Run "<<runno<<std::endl;
      unsigned short* p=(unsigned short*)&data[index];
      for(int n=0;n<9;){
	unsigned short chipid=p[n++];
	unsigned short masks[8];
	for(int m=0;m<8;m++,n++) masks[m]=p[n];
	std::cout<<"    Chip "<<chipid<<std::endl;
	std::cout<<"      enable masks ";
	for(int m=0;m<8;m++) std::cout<<"0x"<<std::hex<<masks[m]<< " ";
	std::cout<<std::dec<<std::endl;
      }
      unsigned char* p2=&p[n];
      for(int n=0;n<16;n++){
	std::cout<<"      Reg "<<n<<": "<<(int)p2[n]<<std::endl;
      }
#ifdef OLD
      index=start+cnt+2;
#else
      index=start+cnt;
#endif
    }
    else{
      // Format of record is 
      //   data[0] = # of data words
      //   data[1..n] = data words
      //   data[n+1] = checksum for buffer
      if(bufcnt<10) std::cout<<"Buffer "<<bufcnt<<": Data record, "
			     << "nwords = "<<cnt<<" checksum = " 
			     << "0x"<<std::hex<<data[index+cnt+1]<<std::dec<<std::endl;
      
      int checksum=0;

#ifdef OLD
      for(index++;index<start+cnt+1;index++)
#else
	for(index+=2;index<start+cnt-1;index++)
#endif
	  {
	    if((index+1)*sizeof(unsigned int)>size){
	      // Safety belt against a partially-written buffer (it will have the full length
	      // field, but the whole buffer hasn't been read in).  This can happen, for instance,
	      // if we are reading a file that is actively being written.
	      std::cout<<"Partial buffer detected, bailing"<<std::endl;
	      break;
	    }
	    
	    checksum^=data[index];

	    // Check whether this word holds an event#, a full 16-bit BCO# or hit data:
	    if((data[index]&0xFFFF)==1){
	      event_fem=(data[index] & 0xFFFF0000)>>16;
	      //cout << "event_fem = " << event_fem <<endl;
	      continue;
	    }
	    else if((data[index]&0xFFFF)==2){
	      bco_full=(data[index] & 0xFFFF0000)>>16;
	      //cout << "bco_full = " << bco_full << endl;
	      continue;
	    }
	    else{
	      //check if this is a FEM_ID (bottom 12 bits = 0) or a data word:
	      if((data[index]&0xfff)==0){ 
		fem_id=(data[index]&0xf000)>>12;
		continue;
	      }
	      else{
		rawchip=(data[index]>>3) & 0x3F;

		if(rawchip<27) chip_id=rawchip;
		else chip_id=rawchip-26;
		
		fpga_id=0;
                  
		if(fem_id==1){
		  if(rawchip<27) module=1;
		  else           module=2;
		}
		else if(fem_id==2){
		  if(rawchip<27) module=3;
		  else           module=4;
		}
		else if(fem_id==4){
		  if(rawchip<27) module=7;
		  else           module=8;
		}
		else if(fem_id==8){
		  if(rawchip<27) module=5;
		  else           module=6;
		}

		mchip  = 0;
		ampl   =(data[index]>>24) & 0x7F;
		bco    =(data[index]>>16) & 0x7F;
		chan_id=(data[index]>> 9) & 0x7F; //((data[index] & 0x200) >>3) | ((data[index] & 0xFC00)>>10); //data[index]>>9) & 0x7F; //
		adc    =(data[index] & 0x07);
		
		nhits[chan_id][ampl]++;
	      }
	    }
	    /*
	      if (ievent < 10) cout<< "Raw data = " << hex << data[index]
	      <<", chip_id =  " << chip_id << ", chan_id =  " << chan_id
	      <<", bco = " << bco << ", ampl = " << ampl
	      <<", fpga = " << fpga_id << endl;
	    */	    
	    
	    int noise,inoise,ihealthy;
	    
	    if(ampl!=0)                      noise=1;
	    else if(chip_id==0||chip_id>26)  noise=1;
	    else if(chan_id==0||chan_id>126) noise=1;
	    else if(fem_id!=4 ||module!=7)   noise=1;
	    else                             noise=0;

	    if(noise!=0) inoise++;
	    if(noise==0) ihealthy++;
	    /*cout << "ievent = " << ievent << " event_fem = " << event_fem 
	      << " fem_id = " << fem_id << " module = " << module
	      << " chip_id = " << chip_id << " chan_id = " << chan_id 
	      << " adc = " << adc << " ampl = " << ampl 
	      << " noise = " << noise << endl;
	    */ 
	    //tree->Fill();
	    //h1->Fill(adc);
	    //if(noise==0)h2->Fill(adc);
	    	    
	    if(event_fem!=old_event_fem){
	      if(old_event_fem>=0){
		for(int ichip=0;ichip<8;ichip++){
		  if(chiphit[ichip]!=0) nchip_event++;
		}
		event->Fill(nhit_event,old_event_fem,old_bco,old_bco_full,nchip_event,ampl);
	      }
	      nhit_event =1;
	      nchip_event=0;
	      
	      for(int ichip=0;ichip<8;ichip++) chiphit[ichip]=0;
	      if((chip_id<9) && (chip_id!=0))  chiphit[chip_id-1]++;
	      old_event_fem=event_fem;
	      old_bco      =bco;
	      old_bco_full =bco_full;
	    }
	    else{
	      nhit_event++;
	      if((chip_id<9) && (chip_id!=0))  chiphit[chip_id-1]++;	      
	    }

	    hits->Fill(chan_id,ampl,adc,rawchip,event_fem,bco_full,fem_id,module,chip_id,ievent);
	    tree->Fill();


	    //Note:  we seem to get some odd chip_ids out of the new DAQ VHDL code
	    //after the event gets larger than some value.  Need to understand this:
	    
	    ievent++;
	  }  //for loop on data[index]
      
      //if ( checksum != data[index] ) 
      //std::cout << "WARNING: bad checksum = "
      //      << std::hex << checksum << std::dec << std::endl;
#ifdef OLD
      index=start+cnt+2;
#else
      index=start+cnt;
#endif
      
    }  // if block on record type
  }  //Loop over data
  
  cout<<"inoise   = "<<inoise  <<endl;
  cout<<"ihealthy = "<<ihealthy<<endl;

  int chip_order[26]={26,25,24,23,22,21,20,19,18,17,16,15,14,
                      13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  //TCanvas *c1=new TCanvas("c1","amplitude vs ADC",1625,250);
  TCanvas *c1=new TCanvas("c1","hit vs chan_id",1625,250);
  c1->Divide(13,2);
  for(int i=0;i<26;i++){
    c1->cd(i+1);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);
    //std::string stmp=Form("ampl<70&&module==%d&&chip_id==%d&&chan_id<127",usemod,chip_order[i]);
    //tree->Draw("adc:ampl",stmp.c_str(),"box");
    tree->Draw("chan_id:hit",stmp.c_str(),"box");
    stmp=Form("chip_id=%d",chip_order[i]);
    TPaveText *tx1b=new TPaveText(10,8.3,60,8.3,"br");
    tx1b->AddText(stmp.c_str());
    tx1b->SetTextSize(0.1);
    tx1b->SetBorderSize(0);
    tx1b->Draw("same");
    c1->Update();
  }

  //TCanvas *c2=new TCanvas("c2","channel_id vs amplitude",1625,250);
  TCanvas *c2=new TCanvas("c2","adc vs hit",1625,250);
  c2->Divide(13,2);
  for(int i=0;i<26;i++){
    c2->cd(i+1);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);
    //std::string stmp=Form("ampl<70&&module==%d&&chip_id==%d&&chan_id<127",usemod,chip_order[i]);
    //tree->Draw("ampl:chan_id",stmp.c_str(),"colz");
    tree->Draw("hit:adc",stmp.c_str());
    stmp=Form("chip_id=%d",chip_order[i]);
    TPaveText *tx2b=new TPaveText(25,75,105,75,"br");
    tx2b->AddText(stmp.c_str());
    tx2b->SetTextSize(0.1);
    tx2b->SetBorderSize(0);
    tx2b->Draw("same");
    c2->Update();
  }
  
  char* date=new char[30];
  strncpy(date,fname,24);
  date[24]='\0';
  cout<<"date     = "<<date<<endl;
  char* filename=new char[30];
  sprintf(filename,"%s.root",date);
  cout<<"filename = "<<filename<<endl;

  TFile *file=new TFile(filename,"RECREATE");
  //hits->Write();
  tree->Write();
  file->Close();
  
  
}

int getFileSize(std::ifstream& f){
  f.seekg(0,std::ifstream::end);
  if(f.fail()){
    std::ifstream::iostate state=f.rdstate();
    std::cout << "error seeking to end, read state = " << state << std::endl;
    if(state & std::ios::eofbit)  std::cout << " eof bit set" << std::endl;
    if(state & std::ios::failbit) std::cout << " fail bit set" << std::endl;
    if(state & std::ios::badbit)  std::cout << " bad bit set" << std::endl;
    return;
  }
  int size=f.tellg();
  if(size<0){
    std::ifstream::iostate state=f.rdstate();
    std::cout << "error in tellg, read state = " << state << std::endl;
    if(state & std::ios::eofbit)  std::cout << " eof bit set" << std::endl;
    if(state & std::ios::failbit) std::cout << " fail bit set" << std::endl;
    if(state & std::ios::badbit)  std::cout << " bad bit set" << std::endl;
    return;
  }

  // Return the file stream to the start of the file
  f.seekg(0,std::ifstream::beg);

  return size;
}

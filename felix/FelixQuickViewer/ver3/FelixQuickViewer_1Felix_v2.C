#include <TTree.h>
#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>
#include <TCut.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TAttText.h>
#include <string>
#include <TH3D.h>
#include <TFile.h>
#include <TStyle.h>

using namespace std;

int FelixQuickViewer_1Felix_v2( string filename = "data/IR_DAQ_server/calib_packv5_050523_1043_intt1_Calib_all_FC.root", string output_type = ".png" ){

  //std::string save_dir = "/sphenix/tg/tg01/commissioning/INTT/root_files/";
 /*
  std::string tree_file_name = filename;
  while(tree_file_name.find("/") != std::string::npos)
  {
      tree_file_name = tree_file_name.substr(tree_file_name.find("/") + 1);
  }
  tree_file_name = tree_file_name.substr(0, tree_file_name.find("."));
  tree_file_name += ".root";
  tree_file_name = save_dir + tree_file_name;
  filename=tree_file_name;
*/ 
  //gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gROOT->SetBatch( true ); // change to false to show canvases
  //  string output_type = ".jpg";
  
  int adc,ampl,chip_id,fpga_id, module, chan_id, fem_id, bco, bco_full, event;
  int nEntries1;

  // char mode;
  // char*date=new char[20];
  // char*year=new char[20];
  // char*time=new char[20];
  //char*filename=new char[50];

  /*
  cout<<"c(calib) or s(selftriger) : ";
  cin>>mode;
  //cout<<"year (ex. 23) : ";
  //cin>>year;
  cout<<"month & date(ex.0323) : ";
  cin>>date;
  cout<<"time(ex.0050) : ";
  cin>>time;

  //cout<<"test="<<date<<"-"<<time<<endl;
  if(mode=='c'){
    sprintf(filename,"calibration_data/calib_packv5_%s23_%s.root",date,time);
    //sprintf(filename,"calibration_data/calib_packv5_%s%s_%s.root",date,year,time);
  }
  if(mode=='s'){
    sprintf(filename,"selftriger_data/selftrig_packv5_%s23_%s.root",date,time);
    //sprintf(filename,"selftriger_data/selftrig_packv5_%s%s_%s.root",date,year,time);
  }
  */
  
  
  //--------------------tree----------------------//
  TFile*f1=new TFile(filename.c_str(), "READ" );
  //TFile*f1=new TFile("calibration_data/calib_packv5_032323_0050.root");

  TTree*tr1=(TTree*)f1->Get("tree");

  tr1->SetBranchAddress("adc",&adc);
  tr1->SetBranchAddress("ampl",&ampl);
  tr1->SetBranchAddress("chip_id",&chip_id);
  tr1->SetBranchAddress("module",&module);
  //  tr1->SetBranchAddress("fpga_id",&fpga_id);
  tr1->SetBranchAddress("chan_id",&chan_id);
  //tr1->SetBranchAddress("fem_id",&fem_id);
  //  tr1->SetBranchAddress("bco",&bco);
  //  tr1->SetBranchAddress("bco_full",&bco_full);
  //  tr1->SetBranchAddress("event",&event);

  //-------------- canvas ----------------//
  
  TPad*chip[14][26];
  TCanvas*c[2];
  string canvas_name[2];
  double check;

  canvas_name[0]="ADC vs channel_id ";
  canvas_name[1]="entry vs channel_id ";

  for(int k=0;k<2;k++){
    c[k]=new TCanvas(Form("c[%d]",k),canvas_name[k].c_str(),2560,1600);
  }

  //cout<<"canvasOK"<<endl;
  
  //-------------- Ladder name ----------------//
  
  TText*L_posi[14];
  string nmodule("module =");
  string ladder_name[14];

  //string pos_45[14]={"B0L106","B0L006","B0L107","B1L108","B1L008","B1L109","B1L009","B0L007","B0L108","B0L008","B1L110","B1L010","B1L111","B1L011"};

  for(int i=0;i<14;i++){
    ladder_name[i]= Form("%d",i);
    ladder_name[i]= nmodule + ladder_name[i];

    if(fmod(i,2)==0) L_posi[i]=new TText(0.01,1-(0.12+i*0.07),ladder_name[i].c_str());
    if(fmod(i,2)==1) L_posi[i]=new TText(0.51,1-(0.12+(i-1)*0.07),ladder_name[i].c_str());
  }

  for(int k=0;k<2;k++){
    for(int i=0;i<14;i++){
      c[k]->cd();
      //int p;
      //p=fmod(k,3);
      L_posi[i]->SetTextSize(0.02);

      L_posi[i]->SetTextColor(1);
      L_posi[i]->SetTextAngle(90);
      L_posi[i]->Draw();
    }
  }

  //---------------- hist --------------------//

  nEntries1=(tr1->GetEntries());
  
  TH2D*hist_adc_ch[14][26];
  TH1D*hist_adc[14][26];
  TH1D*hist_ch[14][26];

  for(int i=0;i<14;i++){
    for(int j=0;j<26;j++){
      hist_adc_ch[i][j]=new TH2D(Form("hist_adc_ch[%d][%d]",i,j),Form("hist_adc_ch[%d][%d]",i,j),128,0,128,10,0,10);
      hist_ch[i][j]=new TH1D(Form("hist_ch[%d][%d]",i,j),Form("hist_ch[%d][%d]",i,j),128,0,128);
      hist_adc[i][j]=new TH1D(Form("hist_adc[%d][%d]",i,j),Form("hist_adc[%d][%d]",i,j),8, 0, 8);
    }
  }

  for(int n=0;n<nEntries1;n++){
    tr1->GetEntry(n);
    for(int i=0;i<14;i++){
      for(int j=0;j<26;j++){
	if(ampl<70 && chan_id<127 && chip_id<27 && chip_id==j+1 && module==i ){
	  hist_adc_ch[i][j]->Fill( chan_id, adc );
	  hist_ch[i][j]->Fill( chan_id );
	  hist_adc[i][j]->Fill( adc );
	}
      }
    }
  }

  //---------------- adc vs channel ---------------------//

  c[0]->cd();
  c[0]->Divide(2,7);

  for(int i=0;i<14;i++){
    c[0]->cd(i+1);
    gPad->SetTopMargin(0.);
    gPad->SetBottomMargin(0.);
    gPad->SetRightMargin(0.);
    gPad->SetLeftMargin(0.1);
    //cout<<Form("i=%d_OK",i)<<endl;

    for(int j=0;j<26;j++){
      if(j<13){
	chip[i][j]=new TPad(Form("chip[%d]",j),Form("chip%d",j+1),1-(0.075*(j+1)),0.0,1-(0.075*j),0.5);
      }
      if(j>=13){
	chip[i][j]=new TPad(Form("chip[%d]",j),Form("chip%d",j+1),1-(0.075*(j-12)),0.5,1-(0.075*(j-13)),1.0);
      }
      
      chip[i][j]->SetTopMargin(0.);
      chip[i][j]->SetBottomMargin(0.);
      chip[i][j]->SetLeftMargin(0.);
      chip[i][j]->SetRightMargin(0.);
      chip[i][j]->Draw();
      chip[i][j]->cd();
      //cout<<Form("chip[%d]OK",j)<<endl;

      //hist_adc_ch[i][j]->GetZaxis()->SetRange(j,j+1);
      hist_adc_ch[i][j]->GetZaxis()->SetRangeUser(0,100);

      //      hist_adc_ch[i][j]->Draw("colz");
      hist_adc[i][j]->Draw();
      //gPad->SetLogy();
      c[0]->cd(i+1);
    }
  }
  //  c[0]->Print(Form("test_pdf/%s23-%s_adcvschan.jpg",date,time));
  //c[0]->Print(Form("test_pdf/%s%s-%s_adcvschn.pdf",date,year,time));
  //string outfilename = "./p
  string output_adc_vs_chan = filename.substr( 0, filename.find_last_of( "." ) )
    //+ "_adcvschan" + output_type ;
    + "_adc" + output_type ;
  c[0]->Print( output_adc_vs_chan.c_str() );

  //------------------- entry vs channel ------------//

  c[1]->cd();
  c[1]->Divide(2,7);

  for(int i=0;i<14;i++){
    c[1]->cd(i+1);
    gPad->SetTopMargin(0.);
    gPad->SetBottomMargin(0.);
    gPad->SetRightMargin(0.);
    gPad->SetLeftMargin(0.1);
    //cout<<Form("i=%d_OK",i)<<endl;

    for(int j=0;j<26;j++){
      if(j<13){
	chip[i][j]=new TPad(Form("chip[%d]",j),Form("chip%d",j+1),1-(0.075*(j+1)),0.0,1-(0.075*j),0.5);
      }
      if(j>=13){
	chip[i][j]=new TPad(Form("chip[%d]",j),Form("chip%d",j+1),1-(0.075*(j-12)),0.5,1-(0.075*(j-13)),1.0);
      }
      chip[i][j]->SetTopMargin(0.);
      chip[i][j]->SetBottomMargin(0.);
      chip[i][j]->SetLeftMargin(0.);
      chip[i][j]->SetRightMargin(0.);     
      chip[i][j]->Draw();
      chip[i][j]->cd();
      //cout<<Form("chip[%d]OK",j)<<endl;

      //hist_ch[i][j]->SetMaximum(600);
      hist_ch[i][j]->Draw();
      //      gPad->SetLogy();
      //hist_ch[i][j]->GetYaxis()->SetRangeUser(0,600);
      c[1]->cd(i+1);
    }
  }
  //c[1]->Print(Form("test_pdf/%s23-%s_entryvschan.jpg",date,time));
  //c[1]->Print(Form("test_pdf/%s%s-%s_entryvschan.pdf",date,year,time));
  //cout<<Form("canvas[%d]_OK",k);
  string output_entry_vs_chan = filename.substr( 0, filename.find_last_of( "." ) )
    + "_entryvschan" + output_type;;
  c[1]->Print( output_entry_vs_chan.c_str() );

  return 0;  
}

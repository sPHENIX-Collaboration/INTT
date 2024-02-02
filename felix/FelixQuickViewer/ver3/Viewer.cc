#include "Viewer.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
Viewer::Viewer( string filename_arg ) :
  filename_( filename_arg )
{
  this->Init();
}


//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////
void Viewer::Init()
{

  //  this->BaseClass::Init();
  //--------------------tree----------------------//
  f1_ = new TFile(filename_.c_str(), "READ" );
  //f1_->ls();

  for(int i=0; i<kLadder_num_; i++)
    {
      string name_module = "module" + to_string( i ) + "_";
      
      for(int j=0; j<kChip_num_; j++)
	{
	  string name_chip = "chip" + to_string( j ) ;
	  
	  // hist_adc_ch_[i][j] = new TH2D( Form("hist_adc_ch_module%d_chip%d", i, j ),
	  // 				 Form("hist_adc_ch_module%d_chip%d", i, j ),
	  // 				 128, 0, 128,
	  // 				 10, 0, 10 );
      
	  // hist_ch_[i][j] = new TH1D( Form("hist_ch_module%d_chip%d", i, j ),
	  // 			     Form("hist_ch_module%d_chip%d", i, j ),
	  // 			     128, 0, 128 );

	  // hist_adc_[i][j] = new TH1D( Form("hist_adc_module%d_chip%d", i, j ),
	  // 			      Form("hist_adc_module%d_chip%d", i, j ),
	  // 			      8, 0, 8 );

	  
	  string name_hist_adc_ch = "hist_adc_ch_" + name_module + name_chip;
	  hist_adc_ch_[i][j] = (TH2D*)f1_->Get( name_hist_adc_ch.c_str() );
	  
	  string name_hist_ch = "hist_ch_" + name_module + name_chip;
	  hist_ch_[i][j] = (TH1D*)f1_->Get( name_hist_ch.c_str() );

	  string name_hist_adc = "hist_adc_" + name_module + name_chip;
	  hist_adc_[i][j] = (TH1D*)f1_->Get( name_hist_adc.c_str() );

	}
    }

  
  string canvas_name[2];
  canvas_name[0] = "ADC vs channel_id ";
  canvas_name[1] = "entry vs channel_id ";

  // for(int k=0;k<2;k++)
  //   {
  //     c_[k] = new TCanvas(Form("c[%d]",k),canvas_name[k].c_str(),2560,1600);
  //     c_[k]->Divide(2,7);
      
  //   }

  // //cout<<"canvasOK"<<endl;
  
  // c_[0]->cd();
  // c_[0]->Divide(2,7);

  c_adc_ch_ = new TCanvas( "ADC_vs_CH", canvas_name[0].c_str() ,2560,1600);
  c_ch_     = new TCanvas( "CH", canvas_name[1].c_str(),2560,1600);
  c_adc_    = new TCanvas( "ADC", "ADC distribution",2560,1600);

  //this->InitLadderMap();
  this->SetStyle();
  
}

/*
void Viewer::InitLadderMap()
{
  // /sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root
  string felix_num = filename_.substr( filename_.find_last_of( "_" ) + 1 + string( "intt" ).size(), 1);
  
  cout << "felix num: " << felix_num << endl;
  
  //LadderMap
  ladder_map_path_ = map_dir + "intt" + felix_num + "_map.txt";
  ladder_map_ = new LadderMap( ladder_map_path_ );
  ladder_map_->Print();
  
}
*/

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////
int Viewer::DoAll()
{
  //this->Process();

  this->Draw();
  
  c_adc_ch_->Update();
  c_ch_->Update();
  c_adc_->Update();

  return 0;
}

int Viewer::Process()
{
  
  return 0;
}

int Viewer::Draw()
{
  TPad* chip[kLadder_num_][kChip_num_];

  //-------------- Ladder name ----------------//
  
  TText* L_posi[kLadder_num_];
  string nmodule("module =");
  string ladder_name[kLadder_num_];

  //string pos_45[kLadder_num_]={"B0L106","B0L006","B0L107","B1L108","B1L008","B1L109","B1L009","B0L007","B0L108","B0L008","B1L110","B1L010","B1L111","B1L011"};

  for(int i=0;i<kLadder_num_;i++)
    {
      ladder_name[i]= Form("%d",i);
      ladder_name[i]= nmodule + ladder_name[i];

      if( fmod(i,2)==0 )
	{
	  L_posi[i] = new TText(0.01, 1-(0.12+i*0.07), ladder_name[i].c_str() );
	}
      
      if( fmod(i,2)==1 )
	{
	  L_posi[i] = new TText(0.51, 1-(0.12+(i-1)*0.07), ladder_name[i].c_str() );
	}
    }


  vector < TCanvas* > canvases;
  canvases.push_back( c_adc_ch_ );
  canvases.push_back( c_ch_ );
  canvases.push_back( c_adc_ );
  for( int k=0; k<canvases.size(); k++ )
    {
      for(int i=0; i<kLadder_num_; i++ )
	{
	  canvases[k]->cd();
	  //int p;
	  //p=fmod(k,3);
	  L_posi[i]->SetTextSize(0.02);
	  
	  L_posi[i]->SetTextColor(1);
	  L_posi[i]->SetTextAngle(90);
	  L_posi[i]->Draw();
	}
  }

  
  this->Draw_AdcChannel();
  this->Draw_Channel();
 
  return 0;
}

int Viewer::Draw_AdcChannel()
{

  TPad* chip[kLadder_num_][kChip_num_];
  auto c = c_adc_ch_;
  c->Divide(2,7);
  //  gPad->cd( 
  
  for(int i=0;i<kLadder_num_;i++)
    {
      c->cd(i+1);
      gPad->SetTopMargin(0.);
      gPad->SetBottomMargin(0.);
      gPad->SetRightMargin(0.);
      gPad->SetLeftMargin(0.1);
      //cout<<Form("i=%d_OK",i)<<endl;
      
      for(int j=0;j<kChip_num_;j++){
	if(j<kChip_num_/2)
	  {
	    chip[i][j] = new TPad( Form("chip[%d]",j),
				   Form("chip%d",j+1),
				   1-(0.075*(j+1)), 0.0,
				   1-(0.075*j), 0.5 );
	  }
	else
	  {
	    chip[i][j] = new TPad( Form("chip[%d]",j),
				   Form("chip%d",j+1),
				   1-(0.075*(j-12)), 0.5,
				   1-(0.075*(j-kChip_num_/2)), 1.0 );
	  }
	
	chip[i][j]->SetTopMargin(0.);
	chip[i][j]->SetBottomMargin(0.);
	chip[i][j]->SetLeftMargin(0.);
	chip[i][j]->SetRightMargin(0.);
	chip[i][j]->Draw();
	chip[i][j]->cd();
	//cout<<Form("chip[%d]OK",j)<<endl;
	
	//hist_adc_ch_[i][j]->GetZaxis()->SetRange(j,j+1);
	hist_adc_ch_[i][j]->GetZaxis()->SetRangeUser(0,100);
	
	//      hist_adc_ch_[i][j]->Draw("colz");
	hist_adc_[i][j]->Draw();
	//gPad->SetLogy();
	c->cd(i+1);
      }
    }

  //  c->Print(Form("test_pdf/%s23-%s_adcvschan.jpg",date,time));
  //c->Print(Form("test_pdf/%s%s-%s_adcvschn.pdf",date,year,time));
  //string outfilename = "./p
  string output_adc_vs_chan = filename_.substr( 0, filename_.find_last_of( "." ) )
    //+ "_adcvschan" + output_type ;
    + "_adc" + figure_suffix_ ;
  c->Print( output_adc_vs_chan.c_str() );
  
  return 0;
}

int Viewer::Draw_Channel()
{

  TPad* chip[kLadder_num_][kChip_num_];
  TCanvas* c = c_ch_;
  
  //------------------- entry vs channel ------------//

  c->cd();
  c->Divide(2,7);

  for(int i=0;i<kLadder_num_;i++)
    {
      c->cd(i+1);
      gPad->SetTopMargin(0.);
      gPad->SetBottomMargin(0.);
      gPad->SetRightMargin(0.);
      gPad->SetLeftMargin(0.1);
      //cout<<Form("i=%d_OK",i)<<endl;
      
      for(int j=0;j<kChip_num_;j++)
	{
	  if(j<kChip_num_/2) // from chip 0 - 13, lower side of the plot
	    {
	      //
	      //   +------------+----- yup
	      //   |            |  |
	      //   |            |  |
	      //   |            |  0.5
	      //   |            |  |
	      //   |            |  |
	      //   +------------+----- ylow
	      //   |<--0.0075-->|
	      //   xlow         xup
	      
	      chip[i][j] = new TPad( Form("chip[%d]",j),
				     Form("chip%d",j+1),
				     1 - ( 0.075*(j+1) ), // xlow
				     0.0,                 // ylow
				     1 - ( 0.075*j ),     // xup
				     0.5 );               // yup
	    }
	  else // from chip 14 - 26, upper side of the plot
	    {
	      chip[i][j] = new TPad( Form("chip[%d]",j),
				     Form("chip%d",j+1),
				     1-(0.075*(j-12)), 0.5,
				     1-(0.075*(j-kChip_num_/2)), 1.0 );
	    }
	  chip[i][j]->SetTopMargin(0.);
	  chip[i][j]->SetBottomMargin(0.);
	  chip[i][j]->SetLeftMargin(0.);
	  chip[i][j]->SetRightMargin(0.);     
	  chip[i][j]->Draw();
	  chip[i][j]->cd();
	  //cout<<Form("chip[%d]OK",j)<<endl;
	  
	  //hist_ch_[i][j]->SetMaximum(600);
	  hist_ch_[i][j]->Draw();
	  //      gPad->SetLogy();
	  //	  hist_ch_[i][j]->GetYaxis()->SetRangeUser(0,600);
	  hist_ch_[i][j]->GetYaxis()->SetLabelOffset(0);
	  
	  c->cd(i+1);
	}
    }
  
  //c->Print(Form("test_pdf/%s23-%s_entryvschan.jpg",date,time));
  //c->Print(Form("test_pdf/%s%s-%s_entryvschan.pdf",date,year,time));
  //cout<<Form("canvas[%d]_OK",k);
  string output_entry_vs_chan = filename_.substr( 0, filename_.find_last_of( "." ) )
    + "_entryvschan" + figure_suffix_;;
  c->Print( output_entry_vs_chan.c_str() );
  
  return 0;
}

void Viewer::Print()
{
  this->PrintLine( "+", "-", "+", "FelixQuickViewer" );
  this->PrintLine( "|", " ", "|", "Data:", filename_ );
  this->PrintLine( "|", " ", "|", "Output Root file:", output_basename_ + root_suffix_ );
  this->PrintLine( "|", " ", "|", "Output picture file:", output_basename_ + "_*_" + figure_suffix_ );
  this->PrintLine( "+", "-", "+" );

  int width_max = 0;
  for( auto& it : print_buffer_ )
    if( width_max < it.size() )
      width_max = it.size();

  for( int i=0; i<print_buffer_.size(); i++ )
    {
      cout << "    " << print_buffer_[i];
      char padding = ( i==0 || i==(print_buffer_.size()-1) ) ? '-' : ' ';
      cout << string( width_max + 1 - print_buffer_[i].size(), padding );
      cout << print_buffer_[i][0] << endl;
    }
  cout << endl;
  
  print_buffer_.erase( print_buffer_.begin(), print_buffer_.end() );  
}

void Viewer::SaveHists()
{
  string output = output_basename_ + root_suffix_;
  TFile* tf = new TFile( output.c_str(), "RECREATE" );

  for( int i=0; i<kLadder_num_; i++ )
    {
      for( int j=0; j<kChip_num_; j++ )
	{
	  tf->WriteTObject( hist_adc_ch_[i][j], hist_adc_ch_[i][j]->GetName() );
	  tf->WriteTObject( hist_adc_[i][j], hist_adc_[i][j]->GetName() );
	  tf->WriteTObject( hist_ch_[i][j], hist_ch_[i][j]->GetName() );

	}
    }
  
  tf->Close();  
}


void Viewer::SetStyle()
{

  //gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle( 0 );
  
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gROOT->SetBatch( true ); // change to false to show canvases
}

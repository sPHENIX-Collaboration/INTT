#include "BaseClass.hh"

using namespace std;

////////////////////////////////
// protected members          //
////////////////////////////////

BaseClass::BaseClass( string filename_arg )
{
  filename_ = filename_arg;
  Init();

}

void BaseClass::Init()
{
  cout << "BaseClass::filename_: " << filename_ << endl;
  //--------------------tree----------------------//
  //  TH1D* aaa = new TH1D( "aaaa", "title", 128, 0., 128 );
  f1_ = new TFile(filename_.c_str(), "READ" );
  //  TH1D* aaaa = new TH1D( "aaaav", "title", 128, 0., 128 );
}

void BaseClass::InitLadderMap()
{
  // /sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root
  string felix_num = filename_.substr( filename_.find_last_of( "_" ) + 1 + string( "intt" ).size(), 1);
    
  cout << "felix num: " << felix_num << endl;
    
  //LadderMap
  ladder_map_path_ = map_dir + "intt" + felix_num + "_map.txt";
  ladder_map_ = new LadderMap( ladder_map_path_ );
  ladder_map_->Print();
    
}

////////////////////////////////
// public members             //
////////////////////////////////
void BaseClass::Print()
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

/*
bool BaseClass::ReadHistograms()
{
  for(int i=0; i<kLadder_num_; i++)
    {
      string name_module = "module" + to_string( i ) + "_";
      
      for(int j=0; j<kChip_num_; j++)
	{
	  string name_chip = "chip" + to_string( j ) ;
	  	  
	  string name_hist_adc_ch = "hist_adc_ch_" + name_module + name_chip;
	  hist_adc_ch_[i][j] = (TH2D*)f1_->Get( name_hist_adc_ch.c_str() );

	  string name_hist_ch = "hist_ch_" + name_module + name_chip;
	  hist_ch_[i][j] = (TH1D*)f1_->Get( name_hist_ch.c_str() );

	  string name_hist_adc = "hist_adc_" + name_module + name_chip;
	  hist_adc_[i][j] = (TH1D*)f1_->Get( name_hist_adc.c_str() );

	}
    }

  return true;
}
*/

void BaseClass::SetStyle()
{
    
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
};

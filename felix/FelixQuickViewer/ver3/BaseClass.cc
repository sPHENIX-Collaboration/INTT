#include "BaseClass.hh"

using namespace std;

////////////////////////////////
// protected members          //
////////////////////////////////

BaseClass::BaseClass( string filename_arg, int year ) :
  filename_( filename_arg ),
  year_( year )
{
  Init();
}

void BaseClass::Init()
{
  /////////////////////////////////////////////////////
  // Processes for/related to intput                 //
  /////////////////////////////////////////////////////
  // /sphenix/tg/tg01/commissioning/INTT/root_files/calib_intt7-00025922-0000.root
  // file name extraction
  string stemp	= filename_.substr( filename_.find_last_of( "/" ) + 1, filename_.size() ); // get only the file name
  felix_num_	= stemp.substr( stemp.find_last_of( "_" ) + string( "intt" ).size() + 1, 1);
  intt_server_	= "intt" + felix_num_;
  run_type_	= stemp.substr( 0, stemp.find_first_of( "_" ) );
  
  // cout << "BaseClass::Init" << endl;
  // cout << "stemp = " << stemp << endl;
  // cout << "felix_num = " << felix_num_ << endl;
  // cout << "intt server = " << intt_server_ << endl;
  // cout << "run type = " << run_type_ << endl;

  /////////////////////////////////////////////////////
  // Processes for/related to output                 //
  /////////////////////////////////////////////////////
  output_basename_ = filename_.substr( 0,  filename_.find_last_of( "." ) );
  output_hist_root_ = output_basename_ + root_suffix_;
  
  //--------------------tree----------------------//
  //  TH1D* aaa = new TH1D( "aaaa", "title", 128, 0., 128 );
  f1_ = new TFile(filename_.c_str(), "READ" );
  //  TH1D* aaaa = new TH1D( "aaaav", "title", 128, 0., 128 );

  this->InitLadderMap();
}

void BaseClass::InitLadderMap()
{
  
  //LadderMap
  ladder_map_path_ = filename_.substr( 0, filename_.size() - string( "-0000.root").size() ) + "_map.txt";
  if( year_ == 2023 )
    {
      ladder_map_path_ = kMap_dir_SDCC_ + to_string( year_ ) + "/" + intt_server_ + "_map.txt";
    }

  // evt files no longer transferred by ourself so the maps are also not send. Let's use the default one
  ladder_map_path_ = kMap_dir_SDCC_ + to_string( year_ ) + "/" + intt_server_ + "_map.txt";

  
  cout << ladder_map_path_ << endl;
  ladder_map_ = new LadderMap( ladder_map_path_ );
  //  ladder_map_->Print();    
}

string BaseClass::GetCanvasName()
{
  canvas_counter++;
  return string( "canvas" + to_string(canvas_counter) );
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

bool BaseClass::ReadHistograms()
{
  
  for(int i=0; i<kLadder_num_; i++)
    {
      string name_module = "module" + to_string( i ) + "_";
      
      for(int j=0; j<kChip_num_; j++)
	{
	  string name_chip = "chip" + to_string( j ) ;

	  ///////////////////////////////////////////
	  // under test
	  ///////////////////////////////////////////
	  string name_hist_ch_adc_ampl = "hist_ch_adc_ampl_" + name_module + name_chip;
	  try
	    {
	      hist_ch_adc_ampl_[i][j] = (TH3D*)f1_->Get( name_hist_ch_adc_ampl.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_ch_adc_ampl_[i][j] = new TH3D();
	    }

	  ///////////////////////////////////////////
	  string name_hist_adc_ch = "hist_adc_ch_" + name_module + name_chip;
	  try
	    {
	      hist_adc_ch_[i][j] = (TH2D*)f1_->Get( name_hist_adc_ch.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_adc_ch_[i][j] = new TH2D();
	    }
	  
	  // just in case
	  string name_hist_ampl_adc = "hist_ampl_adc_" + name_module + name_chip;
	  try
	    {
	      hist_ampl_adc_[i][j] = (TH2D*)f1_->Get( name_hist_ampl_adc.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_ampl_adc_[i][j] = new TH2D();
	    }

	  // just in case
	  string name_hist_ch_ampl = "hist_ch_ampl_" + name_module + name_chip;
	  try
	    {
	      hist_ch_ampl_[i][j] = (TH2D*)f1_->Get( name_hist_ch_ampl.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_ch_ampl_[i][j] = new TH2D();
	    }
	  
	  string name_hist_ch = "hist_ch_" + name_module + name_chip;
	  try
	    {
	      hist_ch_[i][j] = (TH1D*)f1_->Get( name_hist_ch.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_ch_[i][j] = new TH1D();
	    }

	  string name_hist_adc = "hist_adc_" + name_module + name_chip;
	  try
	    {
	      hist_adc_[i][j] = (TH1D*)f1_->Get( name_hist_adc.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_adc_[i][j] = new TH1D();
	    }

	  string name_hist_bco_diff = "hist_bco_diff_" + name_module + name_chip;
	  try
	    {
	      hist_bco_diff_[i][j] = (TH1D*)f1_->Get( name_hist_bco_diff.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_bco_diff_[i][j] = new TH1D();
	    }

	  string name_hist_bco_diff_raw = "hist_bco_diff_raw_" + name_module + name_chip;
	  try
	    {
	      hist_bco_diff_raw_[i][j] = (TH1D*)f1_->Get( name_hist_bco_diff_raw.c_str() );
	    }
	  catch( const std::runtime_error& error )
	    {
	      hist_bco_diff_raw_[i][j] = new TH1D();
	    }


	}
    }

  return true;
}

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


  gROOT->SetStyle("Modern") ;
  gStyle->SetPalette(kBird, 0, 1 );
  gStyle->SetGridColor( kGray );
  gStyle->SetGridStyle( 3 );
  gStyle->SetEndErrorSize(0) ;
  
};

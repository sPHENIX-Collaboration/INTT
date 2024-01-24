#include "BaseClass.hh"

using namespace std;

////////////////////////////////
// protected members          //
////////////////////////////////
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


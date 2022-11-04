#pragma once

// Review plots are made using a TTree in the given ROOT file
void DrawPlotsMultipleLadders(string root_file, int usemod, string mode, string plot_type = "", string cut = "" );

void DrawPlots_AmplADC( string root_file, TTree* tree, string cut, bool reverse_chip_order );
void DrawPlots_ChAmpl( string root_file, TTree* tree, string cut, bool reverse_chip_order );
void DrawPlots_Ch( string root_file, TTree* tree, string cut, bool reverse_chip_order, string mode );
void DrawPlots_ADC( string root_file, TTree* tree, string cut, bool reverse_chip_order, string mode );
void DrawPlots_Hitmap( string root_file, TTree* tree, string cut, bool reverse_chip_order );

int GetChipNum( int i, bool reverse_chip_order )
{
  int chip_num = 26 - i; // 26, 25, 24, ...
  if( reverse_chip_order )
    chip_num = i + 1; // 1, 2, 3, ...
  
  return chip_num;
}

string GetOutputName( string output_base, string words, string suffix = "" )
{
  string output_first = output_base.substr(0, output_base.find( "__" ) + 1 );
  string output_last =  output_base.substr( output_base.find( "__" ) + 1, output_base.size() );
  string output = output_first + words + output_last + suffix;
  return output;
}

// no infinit loop happen, I think
#include "DrawPlotsMultipleLadders.cc"

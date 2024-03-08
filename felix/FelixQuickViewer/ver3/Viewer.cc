#include "Viewer.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
Viewer::Viewer( string filename_arg, int year ) :
  BaseClass( filename_arg, year )
  //  filename_( filename_arg )
{
  this->Init();
}


//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////
void Viewer::Init()
{

  this->BaseClass::Init();
  //--------------------tree----------------------//
  f1_->Close();
  f1_ = new TFile(output_hist_root_.c_str(), "READ" );
  //f1_->ls();

  this->BaseClass::ReadHistograms();
  
  string canvas_name[2];
  canvas_name[0] = "ADC vs channel_id ";
  canvas_name[1] = "entry vs channel_id ";

  // for(int k=0;k<2;k++)
  //   {
  //     c_[k] = new TCanvas(Form("c[%d]",k),canvas_name[k].c_str(),2560,1600);
  //     c_[k]->Divide(2,7);
      
  //   }

  // c_[0]->cd();
  // c_[0]->Divide(2,7);

  c_adc_ch_ = new TCanvas( "ADC_vs_CH", canvas_name[0].c_str() ,2560,1600);
  c_ch_     = new TCanvas( "CH", canvas_name[1].c_str(),2560,1600);
  c_adc_    = new TCanvas( "ADC", "ADC distribution",2560,1600);
  //c_pedestal_ = new TCanvas( "Pedestal", "Pedestal", 2560, 1600 );
  
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

unsigned int Viewer::GetMaxBinContent( TH1D* hists[kLadder_num_][kChip_num_], int rank = 1, int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  assert( r > 0 );
  
  vector < unsigned int > values;
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      for( int j=chip_min; j<chip_max; j++ ) // in x direction
	{
	  for( int k=1; k<hists[i][j]->GetNbinsX()+1; k++ )
	    {
	      values.push_back( hists[i][j]->GetBinContent( k ) );
	      // cout << i << "\t" << j << "\t" << k << "\t"
	      // 	   << hists[i][j]->GetBinContent( k ) << endl;
		
	    }
	}
    }

  int return_value = 0 ;
  if( rank == 1 )
    {
      return_value = *max_element( values.begin(), values.end() );
    }
  else
    {

      assert( rank < values.size() );
      sort( values.rbegin(), values.rend()  );
      
      for( int i=0; i<rank; i++ )
	cout << i << "\t" << values[i] << endl;
      
      return_value = values[ rank ];
    }

  //return_value = *max_element( values.begin(), values.end() );
  return return_value;
}

unsigned int Viewer::GetMaxBinContentRatio( TH1D* hists[kLadder_num_][kChip_num_], double remove_top = 10, int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  assert( 0 <= remove_top && remove_top <= 100 );
  int number = 0 ;
  for( int i=0; i<kLadder_num_; i++ )
      for( int j=0; j<kChip_num_; j++ )
	number += hists[i][j]->GetNbinsX();

  return this->GetMaxBinContent( hists, number * remove_top / 100,  ladder_min, ladder_max, chip_min, chip_max );
}

void Viewer::DivideCanvas( TCanvas* c, int ladder_min, int ladder_max, int chip_min, int chip_max )
{

  int dladder = ladder_max - ladder_min;
  int dchip = chip_max - chip_min;

  pad_width  = ( 1.0 - margin_left - margin_right ) / dchip;
  pad_height = (1.0 - margin_top - margin_bottom ) / dladder;
  ////////////////////////////////////////////////////////////////////////
  // Making pads                                                        //
  ////////////////////////////////////////////////////////////////////////
  TPad* pads[dladder][dchip];
  // +--------------------------------------------------------------------------------------------+
  // |Felix ch13 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch12 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch11 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch10 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch09 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch08 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch07 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch06 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch05 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch04 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch03 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch02 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch01 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // |Felix ch00 |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|
  // +--------------------------------------------------------------------------------------------+

  double x_low = margin_left;
  double y_low = margin_bottom;
  // Pad generation
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      string ladder = "Ladder" + to_string( i );
      double y_high = y_low + pad_height;

      for( int j=chip_min; j<chip_max; j++ ) // in x direction
	{
	  string chip_str = "Chip" + to_string( j + 1);
	  x_low = margin_left + j * pad_width;
	  double x_high = x_low + pad_width;
	  
	  string pad_name = ladder + "_" + chip_str;
	  TPad* pad = new TPad( pad_name.c_str(), pad_name.c_str(),
				 x_low, y_low, x_high, y_high  );
	  pad->SetLeftMargin(0);
	  pad->SetRightMargin(0);
	  pad->SetBottomMargin(0);
	  //pad->SetBottomMargin( 0.2 );
	  pad->SetTopMargin(0);

	  c->cd( 0 );
	  pad->Draw();
	  pads[ i - ladder_min ][ j - chip_min ] = pad;

	}
      y_low = y_high;
      
    }

  int counter = 0;
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    for( int j=chip_min; j<chip_max; j++ ) // in x direction
      pads[ i - ladder_min ][ j - chip_min ]->SetNumber( ++counter );

}

///////////////////////////////////////////////////////////////////////////
// functions for label                                                   //
///////////////////////////////////////////////////////////////////////////
void Viewer::WriteLabelFelix( int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );
  tex->SetTextSize( 0.03 );

  // INTT server label: intt0
  tex->DrawLatexNDC( margin_left * 0.1, 1.0 - margin_top  / 3, intt_server_.c_str() );
}

void Viewer::WriteLabelRoc( int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  
  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );
  
  string comment = "";
  tex->SetTextSize( 0.02 );
  
  double x_roc_label = margin_left * 0.1;
  double y_roc_label = 1.0 - margin_top * 0.8; // - pad_height / 10;
  if( ladder_min == 0 && ladder_max == kLadder_num_ / 2 )
    {
      comment = "RC-" + to_string( 2 * (stoi( felix_num_ ) % 4 ) )
	+ ( stoi( felix_num_ ) < 4 ? "S" : "N" );
    }
  else if( ladder_min == kLadder_num_ / 2 && ladder_max == kLadder_num_ )
    {      
      comment = "RC-" + to_string( 2 * (stoi( felix_num_ ) % 4 ) + 1 )
	+ ( stoi( felix_num_ ) < 4 ? "S" : "N" );
      //tex->DrawLatexNDC( margin_left * 0.1, 1.0 - margin_top - pad_height / 10, comment.c_str() );     
    }
  tex->DrawLatexNDC( x_roc_label, y_roc_label, comment.c_str() );
}

void Viewer::WriteLabelLadder( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ )
{

  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );

  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      string ladder = "FELIX CH" + to_string( i );
      double y_low = margin_bottom + pad_height * (i-ladder_min);
      double y_high = y_low + pad_height;
      tex->SetTextSize( 0.0175 );

      // Ladder Name, ROC port
      auto config = ladder_map_->GetLadderConfig( i );
      //cout << config->module_ << "\t" << config->roc_port_ << "\t" << config->ladder_name_ << endl;
      // FELIX CH
      tex->DrawLatexNDC( margin_left / 10 , y_high - pad_height * 0.1, // (2 * y_low + y_high)/3,
			 ladder.c_str() );

      tex->SetTextSize( 0.015 );
      // Ladder Name
      tex->DrawLatexNDC( margin_left / 10 , y_high - pad_height * 0.33, // (2 * y_low + y_high)/3,
			 config->ladder_name_.c_str() );
      // ROC port
      tex->DrawLatexNDC( margin_left / 10 , y_high - pad_height * 0.56, // (2 * y_low + y_high)/3,
       			 ("ROC port " + config->roc_port_).c_str() );
      
    }

}

void Viewer::WriteLabelChip( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ )
{
  
  int dladder = ladder_max - ladder_min;
  int dchip = chip_max - chip_min;
  
  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );
  
  for( int j=chip_min; j<chip_max; j++ ) // in x direction
    {
      string chip_str = "Chip" + to_string( j + 1 );
      double x_low = margin_left + j * pad_width;
      double x_high = x_low + pad_width;
      
      // Chip number
      tex->SetTextSize( 0.02 );
      if( j== chip_min )
	{
	  tex->DrawLatexNDC( x_low, margin_bottom / 10, chip_str.c_str() ); // bottom, 1st
	  tex->DrawLatexNDC( x_low, margin_bottom + pad_height * dladder + margin_top * 0.1,
			     chip_str.c_str() ); // top, 1st
	}
      else
	{
	  tex->DrawLatexNDC( (5.5 * x_low + 4.5 * x_high)/10, margin_bottom / 10, to_string(j + 1 ).c_str() ); // bottom, 2nd-
	  tex->DrawLatexNDC( (5.5 * x_low + 4.5 * x_high)/10, margin_bottom + pad_height * dladder + margin_top * 0.1,
			     to_string(j + 1 ).c_str() ); // top, 2nd-
	}
      
    }
}

void Viewer::WriteLabelXaxis( int ladder_min, int ladder_max, int chip_min, int chip_max, string label_low, string label_high )
{

  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );
  
  for( int j=chip_min; j<chip_max; j++ ) // in x direction
    {
      double x_low = margin_left + (j-chip_min) * pad_width;
      double x_high = x_low + pad_width;

      // Channel numbers
      tex->SetTextSize( 0.015 );
      tex->DrawLatexNDC( x_low + pad_width * 0.05,   0.75 * margin_bottom , label_low.c_str() );
      tex->DrawLatexNDC( x_high - pad_width * label_high.size() * 0.15, // 0.45
			 0.75 * margin_bottom , label_high.c_str() );
      
    }  

}

void Viewer::WriteLabelYmax( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_, int y_max )
{
  TLatex* tex = new TLatex();
  tex->SetTextColor( color_ );
  tex->SetTextSize( 0.0125 );
  
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      double y_high = margin_bottom + pad_height * (i - ladder_min + 1);

      // minimum label of y-axis
      int y_max_order = to_string(y_max).size();
      tex->DrawLatexNDC( (9 - 0.35 * y_max_order) * margin_left / 10 , // (9 - 0.525 * y_max_order) * margin_left / 10 ,
			 y_high - 0.5 * pad_height / 10,
			 to_string(y_max).c_str() );

    }
}

void Viewer::CanvasPreparation( TCanvas* c,
				  //TH1* hists[kLadder_num_][kChip_num_],
				  int x_min, int x_max, int y_min, int y_max,
				  int ladder_min, int ladder_max, int chip_min, int chip_max,
				  string hist_name )
{

  ////////////////////////////////////////////////////////////////////////
  // Making pads                                                        //
  ////////////////////////////////////////////////////////////////////////
  this->DivideCanvas( c, ladder_min, ladder_max, chip_min, chip_max );

  ////////////////////////////////////////////////////////////////////////
  // Label                                                              //
  ////////////////////////////////////////////////////////////////////////
  this->WriteLabelLadder( ladder_min, ladder_max, chip_min, chip_max );
  this->WriteLabelChip( ladder_min, ladder_max, chip_min, chip_max );
  this->WriteLabelXaxis( ladder_min, ladder_max, chip_min, chip_max, to_string(x_min), to_string(x_max) );
  this->WriteLabelYmax( ladder_min, ladder_max, chip_min, chip_max, y_max );

  ////////////////////////////////////////////////////////////////////////
  // Additional items                                                   //
  ////////////////////////////////////////////////////////////////////////
  this->WriteLabelFelix( ladder_min, ladder_max, chip_min, chip_max );
  this->WriteLabelRoc( ladder_min, ladder_max, chip_min, chip_max );
    
  return;
}

string Viewer::GetOutputPath(int ladder_min, int ladder_max, int chip_min, int chip_max, string keyword )
{
  string output = filename_.substr( 0, filename_.find_last_of( "." ) ) + "_" + keyword;
  string ladder = "";
  if( ladder_min == 0 && ladder_max == kLadder_num_ / 2 )
    {
      ladder = "RC-" + to_string( 2 * (stoi( felix_num_ ) % 4 ) )
	+ ( stoi( felix_num_ ) < 4 ? "S" : "N" );
    }
  else if( ladder_min == kLadder_num_ / 2 && ladder_max == kLadder_num_ )
    {
      ladder = "RC-" + to_string( 2 * (stoi( felix_num_ ) % 4 ) + 1 )
	+ ( stoi( felix_num_ ) < 4 ? "S" : "N" );
    }
  else if( ladder_min != 0 && ladder_max != kLadder_num_ )
    {
      ladder = "ladder" + to_string( ladder_min ) + "-" + to_string( ladder_max );
    }

  string chip = "";
  if( chip_min != 0 && chip_max == kChip_num_ )
    {
      chip = "chip"  + to_string( chip_min ) + "-" + to_string( chip_max );
    }
  
  if( ladder != "" )
    output += "_" + ladder;
  if( chip != "" )
    output += "_" + chip;
  if( figure_suffix_ != "" )
    output += figure_suffix_;
  
  return output;
}

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////
int Viewer::DoAll()
{
  //this->Process();

  this->Draw();
  
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

  if( run_type_ != "junk" )
    {
      
      if( run_type_ == "calibration" || run_type_ == "calib" ) 
	{
	  this->Draw_AmplAdc();
	  this->Draw_AmplAdc( 0, kLadder_num_/2); // 0 - 14/2  --> 0 - 7
	  this->Draw_AmplAdc(kLadder_num_/2, kLadder_num_); // 
	  this->Draw_ChAmpl();
	  this->Draw_ChAmpl(0, kLadder_num_/2); // 0 - 14/2  --> 0 - 7
	  this->Draw_ChAmpl(kLadder_num_/2, kLadder_num_); // 
	  this->Draw_HitDist( 0, kLadder_num_/2); // 0 - 14/2  --> 0 - 7
	  this->Draw_HitDist( kLadder_num_/2, kLadder_num_); // 
	  this->Draw_HitDist();
	  
	}
      else if( run_type_ == "pedestal" )
	{
	  //this->Draw_Channel();
	  //this->Draw_AdcChannel();
	  this->Draw_HitDist();
	  this->Draw_HitDist( 0, kLadder_num_/2); // 0 - 14/2  --> 0 - 7
	  this->Draw_HitDist( kLadder_num_/2, kLadder_num_); // 
	}
      else if( run_type_ == "cosmics" )
	{
	  this->Draw_HitDist();
	}
      else if( run_type_ == "beam" )
	{
	  this->Draw_Channel();
	  this->Draw_AdcChannel();
	  this->Draw_HitDist();
	}
    }
  else // for junk data
    {
      this->Draw_Channel();
      this->Draw_AdcChannel();

      //this->Draw_HitDist();
      this->Draw_HitDist( 0, kLadder_num_/2); // 0 - 14/2  --> 0 - 7
      this->Draw_HitDist( kLadder_num_/2, kLadder_num_); //
      
    }
  
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
    + "_entryvschan" + figure_suffix_;
  c->Print( output_entry_vs_chan.c_str() );
  
  return 0;
}

int Viewer::Draw_AmplAdc( int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  //  TCanvas* c = c_pedestal_;
  TCanvas* c = new TCanvas( this->GetCanvasName().c_str(), this->GetCanvasTitle().c_str(), 2560, 1600 );
  //auto y_max = this->CanvasPreparation( c, hist_ampl_adc_, ladder_min, ladder_max, chip_min, chip_max );
  this->CanvasPreparation( c,
			   0, 64, 0, 8,
			   ladder_min, ladder_max, chip_min, chip_max,
			   hist_ampl_adc_[0][0]->GetName() );
  
  ////////////////////////////////////////////////////////////////////////
  // Draw!                                                              //
  ////////////////////////////////////////////////////////////////////////  
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      string ladder = "Ladder" + to_string( i );

      for( int j=chip_min; j<chip_max; j++ ) // in x direction
	{
	  string chip_str = "Chip" + to_string( j + 1 );

	  c->cd( (i-ladder_min) * kChip_num_ + (j-chip_min) + 1);
	  string hist_name = ladder + "_" + chip_str;

	  hist_ampl_adc_[i][j]->Draw( "colz" );
	  this->SetStyle();
	  
	}
    }

  string output = this->GetOutputPath( ladder_min, ladder_max, chip_min, chip_max, "adc_ampl" );
  c->Print( output.c_str() );
  cout << output << endl << endl;

  return 0;
}

int Viewer::Draw_ChAmpl( int ladder_min, int ladder_max, int chip_min, int chip_max )
{
  TCanvas* c = new TCanvas( this->GetCanvasName().c_str(), this->GetCanvasTitle().c_str(), 2560, 1600 );

  auto margin_right_temp = margin_right;
  margin_right = 0.05;
  this->CanvasPreparation( c,
			   0, 127, 0, 64,
			   ladder_min, ladder_max, chip_min, chip_max,
			   hist_ch_ampl_[0][0]->GetName() );
  
  ////////////////////////////////////////////////////////////////////////
  // Draw!                                                              //
  ////////////////////////////////////////////////////////////////////////  
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      string ladder = "Ladder" + to_string( i );

      for( int j=chip_min; j<chip_max; j++ ) // in x direction
	{
	  string chip_str = "Chip" + to_string( j + 1 );

	  c->cd( (i-ladder_min) * kChip_num_ + (j-chip_min) + 1);
	  string hist_name = ladder + "_" + chip_str;

	  this->SetStyle();
	  gStyle->SetPalette( kRainBow );
	  hist_ch_ampl_[i][j]->Draw( "colz" );
	  hist_ch_ampl_[i][j]->GetZaxis()->SetRangeUser( 0, 20 );
	  
	}
    }

  c->cd( 0 );
  gPad->Update();
  hist_ch_ampl_[0][0]->GetZaxis()->SetTickSize( 0.0 );
  TPaletteAxis* pa = new TPaletteAxis(1.0 - margin_right * 0.9,
				      margin_bottom,
				      1.0 - margin_right * 0.7,
				      1.0 - margin_top,
				      hist_ch_ampl_[0][0]);
  pa->Draw();
  gPad->Update();
  
  string output = this->GetOutputPath( ladder_min, ladder_max, chip_min, chip_max, "ch_ampl" );
  c->Print( output.c_str() );
  cout << output << endl << endl;

  margin_right = margin_right_temp;
  
  return 0;
}

int Viewer::Draw_HitDist( int ladder_min, int ladder_max, int chip_min, int chip_max )
{

  //  TCanvas* c = c_pedestal_;
  TCanvas* c = new TCanvas( this->GetCanvasName().c_str(), this->GetCanvasTitle().c_str(), 2560, 1600 );

  auto y_max = GetMaxBinContentRatio( hist_ch_, 0.01, ladder_min, ladder_max, chip_min, chip_max ); // Bins on the top 1% are ignored

  if( run_type_ == "calib" || run_type_ == "calibration" )
    y_max = 500;
  else if( y_max < 0.9 )
    y_max = 1;
  
  this->CanvasPreparation( c,
			   0, 127, 0.9, y_max,
			   ladder_min, ladder_max, chip_min, chip_max,
			   hist_ch_[0][0]->GetName() );

  ////////////////////////////////////////////////////////////////////////
  // Draw!                                                              //
  ////////////////////////////////////////////////////////////////////////  
  for( int i=ladder_min; i<ladder_max; i++ ) // in y direction
    {
      string ladder = "Ladder" + to_string( i );

      for( int j=chip_min; j<chip_max; j++ ) // in x direction
	{
	  string chip_str = "Chip" + to_string( j + 1 );

	  //c->cd( i * kChip_num_ + j + 1);
	  c->cd( (i-ladder_min) * kChip_num_ + (j-chip_min) + 1);
	  string hist_name = ladder + "_" + chip_str;

	  hist_ch_[i][j]->SetLineColor( kAzure );
	  hist_ch_[i][j]->Draw();
	  hist_ch_[i][j]->GetYaxis()->SetRangeUser( 0.9, y_max );

	  if( !(run_type_ == "calib" || run_type_ == "calibration" ) )
	    gPad->SetLogy( true );
	  
	  this->SetStyle();
	}
    }

  string output = this->GetOutputPath( ladder_min, ladder_max, chip_min, chip_max, "hit_dist" );
  c->Print( output.c_str() );
  cout << output << endl << endl;
  
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
  this->BaseClass::SetStyle();
  
  // //gStyle->SetPalette(1);
  // gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  // gStyle->SetOptTitle( 0 );
  
  // gStyle->SetFrameBorderMode(0);
  // gStyle->SetCanvasColor(0);
  // gStyle->SetCanvasBorderMode(0);
  // gStyle->SetPadColor(0);
  // gStyle->SetPadBorderMode(0);
  // gROOT->SetBatch( true ); // change to false to show canvases
  // gROOT->SetStyle("Modern") ;
  gPad->SetGridx( true );
  gPad->SetGridy( true );
  gPad->SetTicks( true, true );
  //  gStyle->SetPalette(55, 0, 1 );
  gStyle->SetGridColor( kGray );
  gStyle->SetGridStyle( 3 );
  gStyle->SetEndErrorSize(0) ;
}

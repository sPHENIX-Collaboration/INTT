#include "Plotter.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
/*
  Plotter::Plotter( string filename_arg ) :
  filename_( filename_arg )
  {
  this->Init();
  }
*/


//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////
void Plotter::Init()
{
  this->SetStyle();  
}

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////
int Plotter::DoAll()
{
  this->Process();

  this->Draw();

  return 0;
}

int Plotter::Process()
{
  return 0;
}

int Plotter::Draw()
{
  TPad* chip[kLadder_num_][kChip_num_];
  TCanvas* c[2];
  string canvas_name[2];
  canvas_name[0] = "ADC vs channel_id ";
  canvas_name[1] = "entry vs channel_id ";

  for(int k=0;k<2;k++)
    {
      c[k] = new TCanvas(Form("c[%d]",k),canvas_name[k].c_str(),2560,1600);
    }

  //cout<<"canvasOK"<<endl;
  
  c[0]->cd();
  c[0]->Divide(2,7);

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

  for( int k=0; k<2; k++ )
    {
      for(int i=0; i<kLadder_num_; i++ )
	{
	  c[k]->cd();
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

int Plotter::Draw_AdcChannel()
{

  TPad* chip[kLadder_num_][kChip_num_];
  TCanvas* c = new TCanvas( "canvas", "ADC vs channel_id ", 2560, 1600 );  
  c->Divide(2,7);
  
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

int Plotter::Draw_Channel()
{

  TPad* chip[kLadder_num_][kChip_num_];
  TCanvas* c =new TCanvas("canvas", "entry vs channel_id ", 2560, 1600);
  
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
	  
	  //hist_ch_[i][j]->SetMaximum(600);
	  hist_ch_[i][j]->Draw();
	  //      gPad->SetLogy();
	  //hist_ch_[i][j]->GetYaxis()->SetRangeUser(0,600);
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


void Plotter::Print()
{



};

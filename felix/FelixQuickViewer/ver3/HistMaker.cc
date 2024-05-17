#include "HistMaker.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
HistMaker::HistMaker( string filename_arg, int year ) : 
  BaseClass( filename_arg, year )
{
  filename_ = filename_arg;
  output_basename_ = filename_.substr( 0,  filename_.find_last_of( "." ) );
  this->Init();
}

//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////
void HistMaker::Init()
{

  tr1_ = (TTree*)f1_->Get("tree");
  tr1_->SetBranchAddress	("adc"		,&adc_		);
  tr1_->SetBranchAddress	("ampl"		,&ampl_		);
  tr1_->SetBranchAddress	("chip_id"	,&chip_id_	);
  tr1_->SetBranchAddress	("module"	,&module_	);
  //  tr1_->SetBranchAddress	("fpga_id"	,&fpga_id_	);
  tr1_->SetBranchAddress	("chan_id"	,&chan_id_	);
  //tr1_->SetBranchAddress	("fem_id"	,&fem_id_	);
  tr1_->SetBranchAddress	("bco"	,&bco_		);
  tr1_->SetBranchAddress	("bco_full"	,&bco_full_	);
  //  tr1_->SetBranchAddress	("event"	,&event_	);

  for(int i=0; i<kLadder_num_; i++)
    {
      for(int j=0; j<kChip_num_; j++)
	{
	  ////////////////////////////////////////////////////////
	  // under test
	  ////////////////////////////////////////////////////////
	  hist_ch_adc_ampl_[i][j] = new TH3D( Form("hist_ch_adc_ampl_module%d_chip%d", i, j ),
					      Form("hist_ch_adc_ampl_module%d_chip%d;Channel;ADC;Amplitude", i, j ),
					      129, -1, 128,
					      10, 0, 10,
					      64, 0, 64
					      );
	  hist_ch_adc_ampl_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
	  
	  ////////////////////////////////////////////////////////
	  hist_adc_ch_[i][j] = new TH2D( Form("hist_adc_ch_module%d_chip%d", i, j ),
					 Form("hist_adc_ch_module%d_chip%d;Channel;ADC", i, j ),
					 129, -1, 128,
					 10, 0, 10 );
	  hist_adc_ch_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
	  
	  hist_ampl_adc_[i][j] = new TH2D( Form("hist_ampl_adc_module%d_chip%d", i, j ),
					    Form("hist_ampl_adc_module%d_chip%d;Amplitude;ADC", i, j ),
					   64, 0, 64,
					   8, 0, 8 );	  

	  hist_ch_ampl_[i][j] = new TH2D( Form("hist_ch_ampl_module%d_chip%d", i, j ),
					    Form("hist_ch_ampl_module%d_chip%d;Amplitude;ADC", i, j ),
					  128, 0, 128,
					  64, 0, 64 );
	  
	  //hist_adc__[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
	  
	  hist_ch_[i][j] = new TH1D( Form("hist_ch_module%d_chip%d", i, j ),
				     Form("hist_ch_module%d_chip%d;Chennel;Entries", i, j ),
				     129, -1, 128 );
	  hist_ch_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
	  
	  hist_adc_[i][j] = new TH1D( Form("hist_adc_module%d_chip%d", i, j ),
				      Form("hist_adc_module%d_chip%d;ADC;Entries", i, j ),
				      8, 0, 8 );
	  hist_adc_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );

	  
	  hist_bco_diff_[i][j] = new TH1D( Form("hist_bco_diff_module%d_chip%d", i, j ),
				      Form("hist_bco_diff_module%d_chip%d;BCO full&0x7f - FPHX BCO;Entries", i, j ),
					   128, 0, 128 );
	  
	  hist_bco_diff_[i][j]->SetLineColor( kLadder_colors[i] );
	  hist_bco_diff_[i][j]->SetFillColorAlpha( hist_bco_diff_[i][j]->GetLineColor(), 0.15 );

	  hist_bco_diff_raw_[i][j] = new TH1D( Form("hist_bco_diff_raw_module%d_chip%d", i, j ),
					       Form("hist_bco_diff_raw_module%d_chip%d;BCO full&0x7f - FPHX BCO;Entries", i, j ),
					       256, -128, 128 );
	  
	  hist_bco_diff_raw_[i][j]->SetLineColor( kLadder_colors[i] );
	  hist_bco_diff_raw_[i][j]->SetFillColorAlpha( hist_bco_diff_[i][j]->GetLineColor(), 0.15 );
	}
    }
  
  return ;
  
  output_basename_ = filename_.substr( 0,  filename_.find_last_of( "." ) );
  this->InitLadderMap();
  this->SetStyle();
  
}

int HistMaker::DoAll()
{
  this->Process();

  return 0;
}

int HistMaker::Process()
{

  //---------------- hist --------------------//
  int nEntries1 = ( tr1_->GetEntries() );
  
  for(int n=0; n<nEntries1; n++)
    {
      tr1_->GetEntry( n );
      for(int i=0; i<kLadder_num_; i++)
	{
	  for(int j=0; j<kChip_num_; j++)
	    {
	      // all condition checks are needed otherwise the process takes time more (somehow)
	      if( ampl_<70 && chan_id_<127 && chip_id_<27 && chip_id_==j+1 && module_==i )
		{
		  hist_ch_adc_ampl_[i][j]->Fill( chan_id_, adc_, ampl_ );

		  // noise rejection should be good for BCO diff hists
		  if( adc_ > 0 )
		    {
		      auto diff = (bco_full_&0x7f) - bco_;
		      hist_bco_diff_raw_[i][j]->Fill( diff );
		      
		      if( diff < 0 )
			diff += 128;
		      
		      hist_bco_diff_[i][j]->Fill( diff );
		    }

		}
	    }
	}
    }

  // Make 2D and 1D hisgotramgs from 3D histograms. It's slightly faster.
  for(int i=0; i<kLadder_num_; i++)
    {
      
      for(int j=0; j<kChip_num_; j++)
	{
	  //hist_adc_ch_[i][j]
	  hist_ampl_adc_[i][j] = hist_ch_adc_ampl_[i][j]->Project3DProfile( "zy" );
	  hist_ch_ampl_[i][j] = hist_ch_adc_ampl_[i][j]->Project3DProfile( "xz" );
	  hist_ch_[i][j]  =  hist_ch_adc_ampl_[i][j]->ProjectionX( hist_ch_[i][j]->GetName() );
	  hist_adc_[i][j] = hist_ch_adc_ampl_[i][j]->ProjectionY( hist_adc_[i][j]->GetName() );	  
	}      
    }

  
  return 0;
}

void HistMaker::SaveHists()
{

  TFile* tf = new TFile( output_hist_root_.c_str(), "RECREATE" );

  for( int i=0; i<kLadder_num_; i++ )
    {
      for( int j=0; j<kChip_num_; j++ )
	{
	  tf->WriteTObject( hist_ch_adc_ampl_[i][j], hist_ch_adc_ampl_[i][j]->GetName() );
	  tf->WriteTObject( hist_adc_ch_[i][j], hist_adc_ch_[i][j]->GetName() );
	  tf->WriteTObject( hist_adc_[i][j], hist_adc_[i][j]->GetName() );
	  tf->WriteTObject( hist_ch_[i][j], hist_ch_[i][j]->GetName() );
	  tf->WriteTObject( hist_ampl_adc_[i][j], hist_ampl_adc_[i][j]->GetName() );
	  tf->WriteTObject( hist_ch_ampl_[i][j], hist_ch_ampl_[i][j]->GetName() );
	  tf->WriteTObject( hist_bco_diff_[i][j], hist_bco_diff_[i][j]->GetName() );
	  tf->WriteTObject( hist_bco_diff_raw_[i][j], hist_bco_diff_raw_[i][j]->GetName() );
	}
    }

  
  tf->Close();  
}

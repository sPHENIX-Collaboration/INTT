#include "HistMaker.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
HistMaker::HistMaker( string filename_arg )
// :  filename_( filename_arg )
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
  cout << "HistMaker::filename_: " << filename_ << endl;
  //this->InitBaseClass();

  //--------------------tree----------------------//
  // cout << "here!" << endl;
  // TH1D* aaa = new TH1D( "aaaa", "title", 128, 0., 128 );
  f1_ = new TFile(filename_.c_str(), "READ" );
  
  tr1_ = (TTree*)f1_->Get("tree");
  tr1_->SetBranchAddress	("adc"		,&adc_		);
  tr1_->SetBranchAddress	("ampl"		,&ampl_		);
  tr1_->SetBranchAddress	("chip_id"	,&chip_id_	);
  tr1_->SetBranchAddress	("module"	,&module_	);
  //  tr1_->SetBranchAddress	("fpga_id"	,&fpga_id_	);
  tr1_->SetBranchAddress	("chan_id"	,&chan_id_	);
  //tr1_->SetBranchAddress	("fem_id"	,&fem_id_	);
  //  tr1_->SetBranchAddress	("bco"	,&bco_		);
  //  tr1_->SetBranchAddress	("bco_full"	,&bco_full_	);
  //  tr1_->SetBranchAddress	("event"	,&event_	);

  //hist_adc_ch_[0][0] = new TH2D( "aaaa", "title", 128, 0., 128., 10, 0., 10.);
  //TH2D* aaa = new TH2D( "aaaa", "title", 128, 0., 128., 10, 0., 10.);
  
  for(int i=0; i<kLadder_num_; i++)
    {
      for(int j=0; j<kChip_num_; j++)
	{
	  hist_adc_ch_[i][j] = new TH2D( Form("hist_adc_ch_module%d_chip%d", i, j ),
					 Form("hist_adc_ch_module%d_chip%d;Channel;ADC", i, j ),
					 128, 0, 128,
					 10, 0, 10 );
	  hist_adc_ch_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
      
	  hist_ch_[i][j] = new TH1D( Form("hist_ch_module%d_chip%d", i, j ),
				     Form("hist_ch_module%d_chip%d;Chennel;Entries", i, j ),
				     128, 0, 128 );
	  hist_ch_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );

	  hist_adc_[i][j] = new TH1D( Form("hist_adc_module%d_chip%d", i, j ),
				      Form("hist_adc_module%d_chip%d;ADC;Entries", i, j ),
				      8, 0, 8 );
	  hist_adc_[i][j]->SetFillColorAlpha( kAzure + 1, 0.3 );
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
	      if( ampl_<70 && chan_id_<127 && chip_id_<27 && chip_id_==j+1 && module_==i )
		{
		  hist_adc_ch_[i][j]->Fill( chan_id_, adc_ );
		  hist_ch_[i][j]->Fill( chan_id_ );
		  hist_adc_[i][j]->Fill( adc_ );
		}
	    }
	}
    }
  
  return 0;
}

void HistMaker::SaveHists()
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

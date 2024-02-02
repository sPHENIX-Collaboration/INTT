int make_dummy()
{
  TFile* tf = new TFile( "calib_intt0-12345678-0000.root", "RECREATE" );
  TTree* tr = new TTree( "tree", "tree" );

  int pid = 0;
  tr->Branch( "pid", &pid, "pid/I" );
  int adc = 3;
  tr->Branch( "adc", &adc, "adc/I" );
  int ampl = 55;
  tr->Branch( "ampl", &ampl, "ampl/I" );
  int chip_id = 0;
  tr->Branch( "chip_id", &chip_id, "chip_id/I" );
  int module = 0;
  tr->Branch( "module", &module, "module/I" );
  int chan_id = 0;
  tr->Branch( "chan_id", &chan_id, "chan_id/I" );
  int bco = 0;
  tr->Branch( "bco", &bco, "bco/I" );
  Long64_t bco_full = 0;
  tr->Branch( "bco_full", &bco_full, "bco_full/L" );
  int evt = 0;
  tr->Branch( "evt", &evt, "evt/I" );
  int roc = 0;
  tr->Branch( "roc", &roc, "roc/I" );
  int barrel = 0;
  tr->Branch( "barrel", &barrel, "barrel/I" );
  int layer = 0;
  tr->Branch( "layer", &layer, "layer/I" );
  int ladder = 0;
  tr->Branch( "ladder", &ladder, "ladder/I" );
  int arm = 0;
  tr->Branch( "arm", &arm, "arm/I" );
   //full_fphx : full_fphx/I
   //full_roc  : full_roc/I

  pid = 3001;
  //  for( int l=32; l<64; l++ )
    {
      //ampl = l;
      for( int k=0; k<14; k++ )
	{
	  module = k;
	  for( int j=0; j<26; j++ )
	    {
	      chip_id = j+1;
	      for( int i=0; i<128; i++ )
		{
		  chan_id = i;
		  
		  for( int trial=(k%3);
		       trial<10 * (j%2);
		       trial++ )
		    tr->Fill();
		}
	    }
	}
    }
  
  tf->WriteTObject( tr, tr->GetName() );
  tf->Close();
  return 0;
}


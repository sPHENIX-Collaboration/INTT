//20211108
//MiuMorita
//
//
//get tree_both in rootfile
//convert for beamtest hitmap
//version 21.11.09

using namespace std;

#include "../header.h"
#include "../eye_cal_func.h"
#include "readconfig_make_tree.cc"
//#include "../beam_test/readconfig.cc"
//#include "../beam_test/effana.cpp"

void make_tree_new( const char *fname = "beamtest.root" )
{
 gStyle -> SetOptStat( 0 );
 // c) off plot information

 ReadConfig rc;
 rc.read( "make_tree_new.ini" );
 //setParameter( &rc );
 // c) read ~.ini file
 // c) read setting parameter of the experiment

 bool status;

 int LADDER_0_Z_POSITION = rc.getParameteri( "LADDER_0_Z_POSITION", status );
 int LADDER_1_Z_POSITION = rc.getParameteri( "LADDER_1_Z_POSITION", status );
 int LADDER_2_Z_POSITION = rc.getParameteri( "LADDER_2_Z_POSITION", status );
 int LADDER_3_Z_POSITION = rc.getParameteri( "LADDER_3_Z_POSITION", status );

 int DAC_VALUE_0 = rc.getParameteri( "DAC_VALUE_0", status );
 int DAC_VALUE_1 = rc.getParameteri( "DAC_VALUE_1", status );
 int DAC_VALUE_2 = rc.getParameteri( "DAC_VALUE_2", status );
 int DAC_VALUE_3 = rc.getParameteri( "DAC_VALUE_3", status );
 int DAC_VALUE_4 = rc.getParameteri( "DAC_VALUE_4", status );
 int DAC_VALUE_5 = rc.getParameteri( "DAC_VALUE_5", status );
 int DAC_VALUE_6 = rc.getParameteri( "DAC_VALUE_6", status );
 int DAC_VALUE_7 = rc.getParameteri( "DAC_VALUE_7", status );

 int L0 = LADDER_0_Z_POSITION;
 int L1 = LADDER_1_Z_POSITION;
 int L2 = LADDER_2_Z_POSITION;
 int L3 = LADDER_3_Z_POSITION;

 int DAC0 = DAC_VALUE_0;
 int DAC1 = DAC_VALUE_1;
 int DAC2 = DAC_VALUE_2;
 int DAC3 = DAC_VALUE_3;
 int DAC4 = DAC_VALUE_4;
 int DAC5 = DAC_VALUE_5;
 int DAC6 = DAC_VALUE_6;
 int DAC7 = DAC_VALUE_7;
 int DAC8 = 300;

 // c) get parameter from ~.ini file

 //cout << LADDER_0_Z_POSITION << endl;
 //cout << LADDER_1_Z_POSITION << endl;
 //cout << LADDER_2_Z_POSITION << endl;
 //cout << LADDER_3_Z_POSITION << endl;
 

 // ** read tree from the file

 TFile *file0 = TFile::Open( fname );
 TTree *tree_both_tmp = (TTree*)file0 -> Get( "tree_both" );

 string sbeam = "beam_";
 TFile *foroot = ReCreateRoot( fname, &sbeam );

 vector<int> *nmodule   = 0;
 vector<int> *nchip_id  = 0;
 vector<int> *nchan_id  = 0;
 vector<int> *nbco      = 0;
 vector<int> *nbco_full = 0;
 vector<int> *nadc      = 0;

 Bool_t nINTT_event; 
 vector<int> *ncamac_adc = 0; 
 vector<int> *ncamac_tdc = 0; 

 TBranch *tb = 0;

 tree_both_tmp -> SetBranchAddress( "module",   &nmodule,   &tb );
 tree_both_tmp -> SetBranchAddress( "chip_id",  &nchip_id,  &tb );
 tree_both_tmp -> SetBranchAddress( "chan_id",  &nchan_id,  &tb );
 tree_both_tmp -> SetBranchAddress( "bco",      &nbco,      &tb );
 tree_both_tmp -> SetBranchAddress( "bco_full", &nbco_full, &tb );
 tree_both_tmp -> SetBranchAddress( "adc",      &nadc,      &tb );

 tree_both_tmp -> SetBranchAddress( "INTT_event", &nINTT_event, &tb );
 tree_both_tmp -> SetBranchAddress( "camac_adc",  &ncamac_adc,  &tb );
 tree_both_tmp -> SetBranchAddress( "camac_tdc",  &ncamac_tdc,  &tb );

 // c) read tree branch

 int nevent = tree_both_tmp -> GetEntries();
 // c) nevent is the number of events of tree_both

 //cout << "event : " << nevent << endl;
 
 // ** test to read tree branch
 //TH1D *hcamac_tdc[6];

 //for( int i = 0; i < 6; i++ )
 //{
 //   hcamac_tdc[i] = new TH1D( Form( "hcamac_tdc_%d", i ), Form( "camac_tdc[%d];tdc;counts", i ), 3000, 0, 3000 );
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   //tree_both -> GetEntry( i );
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );
 //   // c) get a event number

 //   for( UInt_t j = 0; j < ncamac_tdc -> size(); j++ )
 //   {
 //      //cout << ncamac_tdc -> at( j ) << endl;

 //  	 hcamac_tdc[j] -> Fill( ncamac_tdc -> at( j ) );
 //  	 // c) get the value
 //   }
 //}

 //TCanvas *ccamac_tdc = new TCanvas( "ccamac_tdc", "ccamac_tdc", 1800, 900 );
 //ccamac_tdc -> Divide( 3, 2, 0 );
 //for( int i = 0; i < 6; i++ )
 //{
 //   ccamac_tdc -> cd( i + 1 );
 //   hcamac_tdc[i] -> Draw();
 //}

 // c) test to draw plots of camac_tdc


 // ** make new tree
 

 vector<int> ladders;
 vector<int> cell_ids;
 vector<int> strip_ids;
 vector<int> deposits;
 vector<int> bcos;
 vector<int> bco_fulls;
 vector<int> camac_adcs;
 vector<int> camac_tdcs;

 int events = 0;
 Bool_t INTT_coincidences;

 TTree *tree_beam = new TTree( "tree_beam", "tree_beam" );
 tree_beam -> Branch( "ladder",           &ladders );
 tree_beam -> Branch( "cell_id",          &cell_ids );
 tree_beam -> Branch( "strip_id",         &strip_ids );
 tree_beam -> Branch( "deposit",          &deposits );
 tree_beam -> Branch( "bco",              &bcos );
 tree_beam -> Branch( "bco_full",         &bco_fulls );
 tree_beam -> Branch( "event",            &events );
 tree_beam -> Branch( "INTT_coincidence", &INTT_coincidences, "INTT_coincidences/O" );
 tree_beam -> Branch( "camac_adc",        &camac_adcs );
 tree_beam -> Branch( "camac_tdc",        &camac_tdcs );

 // c) make tree_beam and branch of tree_beam

 ladders   .erase( ladders   .begin(), ladders   .end() );
 cell_ids  .erase( cell_ids  .begin(), cell_ids  .end() );
 strip_ids .erase( strip_ids .begin(), strip_ids .end() );
 deposits  .erase( deposits  .begin(), deposits  .end() );
 bcos      .erase( bcos      .begin(), bcos      .end() );
 bco_fulls .erase( bco_fulls .begin(), bco_fulls .end() );
 camac_adcs.erase( camac_adcs.begin(), camac_adcs.end() );
 camac_tdcs.erase( camac_tdcs.begin(), camac_tdcs.end() );

 int NL0 = 0;
 int NL1 = 0;
 int NL2 = 0;
 int NL3 = 0;

 for( int i = 0; i < nevent; i++ )
 {
    tree_both_tmp -> GetEntry( i );
    Long64_t tentry = tree_both_tmp -> LoadTree( i );
    tb -> GetEntry( tentry );

    for( UInt_t j = 0; j < nmodule -> size(); ++j )
    {
       if( nmodule -> at( j ) == L0 )
   	 {
          ladders.push_back( 0 );
   		 NL0 = 1;
   	 }
       if( nmodule -> at( j ) == L1 )
   	 {
          ladders.push_back( 1 );
   		 NL1 = 1;
   	 }
       if( nmodule -> at( j ) == L2 )
   	 {
          ladders.push_back( 2 );
   		 NL2 = 1;
   	 }
       if( nmodule -> at( j ) == L3 )
   	 {
          ladders.push_back( 3 );
   		 NL3 = 1;
   	 }
    }

    for( UInt_t j = 0; j < nchip_id -> size(); ++j )
    {
       if(  0 < nchip_id -> at( j ) && nchip_id -> at( j ) < 14 )
       {
          cell_ids .push_back( nchip_id -> at( j ) );
          strip_ids.push_back( nchan_id -> at( j ) );
       }
       if( 13 < nchip_id -> at( j ) && nchip_id -> at( j ) < 27 )
       {
          cell_ids .push_back( nchip_id -> at( j ) - 13 );
          strip_ids.push_back( abs( nchan_id -> at( j ) - 127 ) + 128 );
   		 //cout << nchan_id -> at( j ) << endl;
       }
    }

    for( UInt_t j = 0; j < nadc -> size(); ++j )
    {
       if( nadc -> at( j ) == 0 )deposits.push_back( ( ( DAC1 + DAC0 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 1 )deposits.push_back( ( ( DAC2 + DAC1 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 2 )deposits.push_back( ( ( DAC3 + DAC2 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 3 )deposits.push_back( ( ( DAC4 + DAC3 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 4 )deposits.push_back( ( ( DAC5 + DAC4 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 5 )deposits.push_back( ( ( DAC6 + DAC5 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 6 )deposits.push_back( ( ( DAC7 + DAC6 ) / 2. ) * 4 + 210 );
       if( nadc -> at( j ) == 7 )deposits.push_back( ( ( DAC8 + DAC7 ) / 2. ) * 4 + 210 );
    }

    for( UInt_t j = 0; j < nbco -> size(); ++j )
    {
       bcos     .push_back( nbco -> at( j ) );
       bco_fulls.push_back( nbco_full -> at( j ) );
    }
       
    events++;

    if( NL0 == 1 
     && NL1 == 1
     && NL2 == 1
     && NL3 == 1 )INTT_coincidences = true;
    else INTT_coincidences = false;

    for( UInt_t j = 0; j < ncamac_adc -> size(); ++j )
    {
       camac_adcs.push_back( ncamac_adc -> at( j ) );
    }

    for( UInt_t j = 0; j < ncamac_tdc -> size(); ++j )
    {
       camac_tdcs.push_back( ncamac_tdc -> at( j ) );
    }
    
    tree_beam -> Fill();

    NL0 = 0;
    NL1 = 0;
    NL2 = 0;
    NL3 = 0;

    ladders   .erase( ladders   .begin(), ladders   .end() );
    cell_ids  .erase( cell_ids  .begin(), cell_ids  .end() );
    strip_ids .erase( strip_ids .begin(), strip_ids .end() );
    deposits  .erase( deposits  .begin(), deposits  .end() );
    bcos      .erase( bcos      .begin(), bcos      .end() );
    bco_fulls .erase( bco_fulls .begin(), bco_fulls .end() );
    camac_adcs.erase( camac_adcs.begin(), camac_adcs.end() );
    camac_tdcs.erase( camac_tdcs.begin(), camac_tdcs.end() );
 }

 //for( int i = 0; i < nevent; i++ )
 //{
 //   tree_both -> GetEntry( i );
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < nmodule -> size(); j++ )
 //   {
 //      if( nmodule -> at( j ) == L0 )
 //  	 {
 //         ladders.push_back( 0 );
 //  		 NL0 = 1;
 //  	 }
 //      if( nmodule -> at( j ) == L1 )
 //  	 {
 //         ladders.push_back( 1 );
 //  		 NL1 = 1;
 //  	 }
 //      if( nmodule -> at( j ) == L2 )
 //  	 {
 //         ladders.push_back( 2 );
 //  		 NL2 = 1;
 //  	 }
 //      if( nmodule -> at( j ) == L3 )
 //  	 {
 //         ladders.push_back( 3 );
 //  		 NL3 = 1;
 //  	 }
 //   }

 //   if( NL0 == 1 
 //    && NL1 == 1
 //    && NL2 == 1
 //    && NL3 == 1 )INTT_coincidences = true;
 //   else INTT_coincidences = false;

 //   NL0 = 0;
 //   NL1 = 0;
 //   NL2 = 0;
 //   NL3 = 0;

 //   tree_beam -> Fill();
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   tree_both -> GetEntry( i );
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < nchip_id -> size(); j++ )
 //   {
 //      if(  0 < nchip_id -> at( j ) && nchip_id -> at( j ) < 14 )
 //      {
 //         cell_ids .push_back( nchip_id -> at( j ) );
 //         strip_ids.push_back( nchan_id -> at( j ) );
 //      }
 //      if( 13 < nchip_id -> at( j ) && nchip_id -> at( j ) < 27 )
 //      {
 //         cell_ids .push_back( nchip_id -> at( j ) - 13 );
 //         strip_ids.push_back( abs( nchan_id -> at( j ) - 127 ) + 128 );
 //  		 //cout << nchan_id -> at( j ) << endl;
 //      }
 //   }

 //   tree_beam -> Fill();
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < nadc -> size(); j++ )
 //   {
 //      if( nadc -> at( j ) == 0 )deposits.push_back( ( ( DAC1 + DAC0 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 1 )deposits.push_back( ( ( DAC2 + DAC1 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 2 )deposits.push_back( ( ( DAC3 + DAC2 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 3 )deposits.push_back( ( ( DAC4 + DAC3 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 4 )deposits.push_back( ( ( DAC5 + DAC4 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 5 )deposits.push_back( ( ( DAC6 + DAC5 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 6 )deposits.push_back( ( ( DAC7 + DAC6 ) / 2. ) * 4 + 210 );
 //      if( nadc -> at( j ) == 7 )deposits.push_back( ( ( DAC8 + DAC7 ) / 2. ) * 4 + 210 );
 //   }

 //   tree_beam -> Fill();
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   tree_both -> GetEntry( i );
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < nbco -> size(); j++ )
 //   {
 //      bcos     .push_back( nbco -> at( j ) );
 //      bco_fulls.push_back( nbco_full -> at( j ) );
 //   }
 //      
 //   events++;

 //   tree_beam -> Fill();
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < ncamac_adc -> size(); j++ )
 //   {
 //      camac_adcs.push_back( ncamac_adc -> at( j ) );
 //   }

 //   tree_beam -> Fill();
 //}

 //for( int i = 0; i < nevent; i++ )
 //{
 //   Long64_t tentry = tree_both -> LoadTree( i );
 //   tb -> GetEntry( tentry );

 //   for( UInt_t j = 0; j < ncamac_tdc -> size(); j++ )
 //   {
 //      camac_tdcs.push_back( ncamac_tdc -> at( j ) );
 //   }
 //   
 //   tree_beam -> Fill();
 //}

 TTree *tree_both = tree_both_tmp -> CloneTree();

 tree_both -> Write();
 tree_beam -> Write();

 foroot -> Close();

}

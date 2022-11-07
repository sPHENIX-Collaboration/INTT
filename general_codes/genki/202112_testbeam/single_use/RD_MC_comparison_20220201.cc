#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
#endif

TH1D* GetHist( string data, int module, string tag )
{
  cout << string(50, '-' ) << endl;
  cout << "Data: " << data << endl;
  cout << "module: " << module << endl;
  cout << "tag: " << tag << endl;
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr )
    {
      cerr << data << " is not found" << endl;
      return nullptr;
    }
  
  auto tr = (TTree*)tf->Get( "tree_both" );
  if( tr == nullptr )
    tr = (TTree*)tf->Get( "tree" );

  string hist_name = string( "hist_" ) + tag;
  string hist_title = tag + ";Channel;Entries";
  TH1D* hist = new TH1D( hist_name.c_str(), hist_title.c_str(), 255, 0, 255 );
  
  string cut = "";
  cut = "module==" + to_string( module );
  cut += " && 0<chip_id && chip_id<27";
  cut += " && 0<=chan_id && chan_id<=127";
  cut += " && ampl==0";
  cut += " && (chip_id == 11 || chip_id == 24 )";
  
  string expression = string("int(chip_id / 14) * 255") // it's offsets for y-axis.
   + "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
   + ">> " + hist_name;

  cout << "expression: " << expression << endl;
  cout << "cut: " << cut << endl;
  tr->Draw( expression.c_str(), cut.c_str(), "goff" );

  return (TH1D*)hist->Clone();
}

int RD_MC_comparison_20220201()
{
  string data_RD = "results/ELPH/trim_data/no_clone/run21_no_clone.root";
  auto hist_RD = GetHist( data_RD, 1, "Run21" );
  HistSetting( hist_RD, kBlack );
  hist_RD->SetMarkerStyle( 1 );
  hist_RD->SetFillColorAlpha( kBlack, 0.2 );
  
  string data_MC_mono = "repo/build/e+-1GeV_100k-events_20220116_convert.root";
  auto hist_MC_mono = GetHist( data_MC_mono, 1, "MC_mono" );
  HistSetting( hist_MC_mono, kRed );
  hist_MC_mono->SetMarkerStyle( 1 );
  
  string data_MC_smear = "repo/build/e+-1GeV_1M-events_beam-smeared_20220131.root_convert.root";
  auto hist_MC_smear = GetHist( data_MC_smear, 5, "MC_smear" );
  HistSetting( hist_MC_smear, kAzure  );
  hist_MC_smear->SetMarkerStyle( 1 );

  string output = "results/ELPH/single_use/RD_MC_comaprison_20220201.pdf";
  TCanvas* c = GetCanvas( output, "square", false );
  MultiHist* mh = new MultiHist( "Channel distribution", "Channel distribution;Channel;Entries" );
  mh->Add( hist_RD );
  mh->Add( hist_MC_mono );
  mh->Add( hist_MC_smear );
  mh->NormalizeHist();
  
  c->cd(1);
  mh->Draw( "HISTE" );
  
  c->Print( c->GetName() );
  return 0;
}

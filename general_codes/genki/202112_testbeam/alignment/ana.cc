#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh"
#include "../lib/INTTEvent.hh"

int ana( int run_num = 89, string type = "fitting",
	 string output_dir_arg = "",
	 string cut_arg = "", bool is_MC = false )
{
  gStyle->SetOptStat( 111111 );
  gStyle->SetOptFit( true );
  gErrorIgnoreLevel = kError;
  
  if( !is_MC )
    {
      Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
      db->SetRun( run_num );
      const string kOriginal_data_dir = "data/ELPH/";
      
      if( db->IsBeamRun() == false && run_num <= 89 )
	return 0;
    }
  
  string output_root_dir = "results/ELPH/alignment/" + type + "/";

  if( output_dir_arg == "" )
    output_dir_arg = "normal";

  //string data = output_root_dir + output_dir_arg + "/run" + to_string( run_num );
  string data = output_root_dir + "/run" + to_string( run_num );
  
  if( !is_MC )
    data += + ".root";
  else
    data += + "_MC.root";

  cout << "data: " << data << endl;
  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );
  //  tr->Print();

  string output = output_root_dir + "/ana/";

  if( !is_MC )
    output += output_dir_arg + "/run" + to_string( run_num ) + ".pdf";
  else
    output += output_dir_arg + "/run" + to_string( run_num ) + "_MC.pdf";

  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  vector < pair < TObject*, string > > obj_saved;
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->SetFillColorAlpha( kWhite, 0 );
  c->Print( ((string)c->GetName()+"[").c_str() );

  int num = tr->Draw( "chip_id", cut_arg.c_str(), "goff" );
  auto values = tr->GetV1();
  vector < int > chips;
  for( int i=0; i<num; i++ )
    chips.push_back( values[i] );

  int first_chip = *min_element( chips.begin(), chips.end() );
  int last_chip = *max_element( chips.begin(), chips.end() );

  gPad->SetLogy( true );
  for( int i=first_chip; i<last_chip; i++ )
    {

      string cut_chip = string( "chip_id[0]==" ) + to_string( i );

      double xmin = -256;
      double xmax = 256;
      int bin = xmax - xmin;

      // loop over ladders
      for( int j=0; j<3; j++ )
	{

	  string hist_name = string("ladder") + to_string(j)
	    + "_chip" + to_string( i );
	  string hist_title = hist_name + ";Residual (ch);Entires";
	  
	  TH1D* hist = new TH1D( hist_name.c_str(), hist_title.c_str(), bin, xmin, xmax );
	  HistSetting( hist, kBlack, 3, 1 );
	  hist->SetFillColorAlpha( kBlack, 0.1 );
	  
	  string expression = string("residual[") + to_string(j) + "]>>" + hist_name;

	  string cut = cut_chip;
	  if( cut_arg != "" )
	    cut += " && " + cut_arg;
	  
	  tr->Draw( expression.c_str(), cut.c_str(), "goff" );
	  hist->Draw( "HIST" );

	  double mean = hist->GetMean();
	  double dev = hist->GetStdDev();
	  string formula = "[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]*exp(-0.5*((x-[p1])/[p4])*((x-[p1])/[p4]))";
	  TF1* fit = new TF1( "f", formula.c_str(), xmin, xmax );
	  fit->SetNpx( 1e3 );
	  fit->SetParLimits( 0, 0, hist->GetEntries() * 10 );
	  fit->SetParLimits( 1, mean - 3, mean + 3 );
	  fit->SetParLimits( 2, 0, 3 );
	  fit->SetParLimits( 3, 0, hist->GetEntries() * 0.8 );
	  fit->SetParLimits( 4, 1, 10 );
	  
	  fit->SetParameter( 0, hist->GetEntries() * 0.8 );
	  fit->SetParameter( 1, mean );
	  fit->SetParameter( 2, 0.8 );
	  fit->SetParameter( 3, hist->GetEntries() * 0.2 );
	  fit->SetParameter( 4, 3 );

	  hist->Fit( fit );
	  fit->Draw( "same" );
	  c->Print( c->GetName() );

	  // change scale for MC data
	  if( is_MC )
	    dev = 1;
	  
	  hist->GetXaxis()->SetRangeUser( mean - 5 * dev, mean + 5 * dev );
	  c->Print( c->GetName() );

	  obj_saved.push_back( pair < TObject*, string >(hist, hist->GetName()) );
	}
      
    }

  for( auto& it : obj_saved )
    tf_output->WriteTObject( it.first, it.second.c_str() );
  
  tf_output->Close();
  tf->Close();

  cout << "Input: " << data << endl;
  cout << "Output: " << output << endl;
  c->Print( ((string)c->GetName()+"]").c_str() );
  return 0;
}


//#include "../InttEvent.h"
//#include "../InttEvent.h"
#include "InttEvent.cc"
#include "InttCluster.cc"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"


//int multiplicity_correlation( string file_path = "/bbox/commissioning/INTT/commissioning/hit_files/beam_intt0-00013107-0000_event_base.root" )
int multiplicity_correlation( string file_path = "rootfile/beam_intt5-00020444-0000_event_base.root" )
{
  bool debug = true;
  debug = false;

  //   Find INTT server ID
    string felix_num = file_path.substr( file_path.find_last_of( "/" ), file_path.size() ); // -> /beam_intt6-00009328-0000_event_base.root
    felix_num = felix_num.substr( felix_num.find( "intt" )+0, 14); // -> 6
  
  TFile* tf = new TFile( file_path.c_str(), "READ" );
  if( tf == nullptr )
    {
      cerr << file_path << " is not found." << endl;
      return -1;
    }
  cout << "Data: " << file_path << endl;
  TTree* tr = (TTree*)tf->Get( "tree" ) ;

  InttEvent* ev = new InttEvent();
  tr->SetBranchAddress( "event", &ev );

  InttHit* hit = new InttHit();
  InttHit* hit_prev = new InttHit();
  int hit_counter = 0, clone_counter = 0;
  int bin = 700;
  //  TH2D* hist_inner_outer = new TH2D( "multiplicity_inner_outer",
  //   "INTT Hits clustering Correlation B0 vs B1 Barrels;B0 hits multiplicity;B1 hits multiplicity",
  //				     bin, 0, bin,
  //				     bin, 0, bin
  //				     );
   TH2D *hist_inner_outer = new TH2D("", "", bin, 0, bin, bin, 0, bin);
  for( int i=0; i<tr->GetEntries(); i++ )
    {
      tr->GetEvent( i ) ;
      if( ev->getNHits() == 0 )
	continue;
      
      if( debug ) 
	//cout << i << endl;
	0;
      else
	if( i % 700 == 0 )
	  cout << flush
	       << setw( 9 ) << std::right << i << " / "
	       << setw( 9 ) << std::right << tr->GetEntries() << " = "
	       << setw( 9 ) << setprecision(3) << std::right << 100.0 * i / tr->GetEntries() << "%"
	       << "\r";
      // if (i>0.1*tr->GetEntries())
      // {break;}
      int multiplicity_inner = 0, multiplicity_outer = 0;
      //      auto clusters = Clustering( ev );

      InttClusterList* list_cluster = new InttClusterList();
      Clustering( ev, list_cluster );
      
      /*
      for( int j=0; j<ev->getNHits() - 1 ; j++ )
	{
	  auto hit = ev->getHit( j );	  
	  if( hit->IsEqual( hit_prev ) )
	    {
	      //if( debug )
	      //hit_prev->show();
	      
	      clone_counter++;
	      continue;
	    }
	  hit_counter++;

	  if( hit->barrel == 0 )
	    multiplicity_inner++;
	  else
	    multiplicity_outer++;
	  
	  // set current hit as a previous hit
	  hit_prev = hit;
	  }
      */
      for( int j=0; j<list_cluster->getNhits(); j++ )
	{
	  auto cluster = list_cluster->getCluster( j );
	  //cluster->show();
	  
	  if( cluster->barrel == 0 )
	    multiplicity_inner++;
	  else
	    multiplicity_outer++;
	}
      
      hist_inner_outer->Fill( multiplicity_inner, multiplicity_outer );
      delete list_cluster;
      if( i>1000 && debug )
	break;
    }

  hist_inner_outer->Draw( "colz" );
  //  DrawStats(hist_inner_outer,100,100,101,101);
  /*DrawPaletteAxis(hist_inner_outer,
  		  0.87, 0.1,
		  0.91, 0.9,
		  0.04);*/
  //SetsPhenixStyle();

    TCanvas *c = new TCanvas("c", "c", 685, 630);
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);

    TLegend *leg = new TLegend(.12,.78,.45,.9);
    leg->SetFillStyle(0);
    leg->AddEntry("","#it{#bf{sPHENIX}} INTT","");
    leg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg->SetLineWidth(0);
   
    hist_inner_outer->GetYaxis()->SetNdivisions(405);
    hist_inner_outer->GetXaxis()->SetNdivisions(405);
    hist_inner_outer->SetXTitle("Number of clusters at inner ladders");
    hist_inner_outer->SetYTitle("Number of clusters at outer ladders");
    hist_inner_outer->SetZTitle("Entries");
    hist_inner_outer->Draw("col z");
    leg->Draw("same");
    c->Print(("plot_6_21/"+felix_num+"_HitsClulssteringCorrelation.pdf").c_str());
  
 //    auto legend = new TLegend(0.1,0.7,0.75,0.9);
//   legend->SetHeader("sPHENIX/INTT","C"); // option "C" allows to center the header
//   legend->AddEntry(hist_inner_outer,"Au+Au at 200GeV","");
//   legend->AddEntry("", ("DAC0=15 ADC Threshold in Felix-"+felix_num).c_str() ,"");
//   legend->SetTextSize(0.045);  
// legend->AddEntry("f1","Function abs(#frac{sin(x)}{x})","l");
// legend->AddEntry("gr","Graph with error bars","lep");
//   legend->Draw()

  
       cout << "Clone hit rate: " << clone_counter
       << " / " << hit_counter
       << " = " << 1.0 * clone_counter / hit_counter
       << endl;
  return 0;
}

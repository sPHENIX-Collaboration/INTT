#include "Classifier.hh"
#define Classifier_cc


/////////////////////////////////////////////////
// Constructor                                 //
/////////////////////////////////////////////////
Classifier::Classifier( int run_num )
  : run_num_( run_num )
{

  Init();
};

/////////////////////////////////////////////////
// private                                     //
/////////////////////////////////////////////////
void Classifier::Init()
{

  in_sig_ = 10 * sig_cut_;
  InitPaths();

  tf_hitmap_ = new TFile( hitmap_file_.c_str(), "READ");

  for (int i = 0; i < kFelix_num; i++)
  {
    canA_[i] = new TCanvas(Form("TypeA_Felix_%d", i), Form("TypeA_Felix_%d", i), 1200, 1200);
    canA_[i]->Divide(7, 2);
    
    canB_[i] = new TCanvas(Form("TypeB_Felix_%d", i), Form("TypeB_Felix_%d", i), 1200, 1200);
    canB_[i]->Divide(7, 2);
  }

  InitHists();
}

void Classifier::InitPaths()
{

  //string run_num_str = to_string( run_num_ );
  string run_num_str = GetRunNum8digits( run_num_ );
  //std::string rootfilename = map_input_path + "hitmap_run" + to_string(runnumber) + ".root";
  hitmap_file_ = kIntt_hitmap_dir + "hitmap_run" + run_num_str + ".root";
  
  output_cdb_file_ = output_cdb_dir_ + "cdb_" + run_num_str + ".root";

  // Determination of output file names.
  string output_basename = "InttHotDeadMap_" + run_num_str + "_" + to_string( in_sig_ );
  // output_qa_root_file_ = output_qa_root_path_ + "root/" + output_basename + ".root";
  // output_qa_pdf_file_ = output_qa_pdf_path_ + "plots/" + output_basename + ".pdf";
  // output_qa_txt_file_ = output_qa_pdf_path_ + "txt/" + output_basename + ".txt";  
  output_qa_root_file_ = output_qa_root_path_ + "root/" + to_string( kYear ) + "/" + output_basename + ".root";
  output_qa_pdf_file_ = output_qa_pdf_path_ + "plots/" + to_string( kYear ) + "/" + output_basename + ".pdf";
  output_qa_txt_file_ = output_qa_pdf_path_ + "txt/" + to_string( kYear ) + "/" + output_basename + ".txt";  
}

void Classifier::InitHists()
{
  h1_hist_MPV_ = new TH1D("hist_MPV","hist_MPV", 5000, 1, 5001);
  h1_hist_MPV_->SetTitle( "#hit/ch (non-zero only);#hit;Entries" );
  h1_hist_MPV_ladder_ = new TH1D("NumberOfFiredChannels_ladder","NumberOfFiredChannels_ladder", 1000, 1, 10000);

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 14; j++)
    {

      h1_hist_fit_A_[i][j] = new TH1D( Form("h1_hist_fit_A%d_%d", i, j),
				       Form("h1_hist_fit_A%d_%d", i, j),
					  200, 1, 201);
      h1_hist_fit_B_[i][j] = new TH1D( Form("h1_hist_fit_B%d_%d", i, j),
				       Form("h1_hist_fit_B%d_%d", i, j),
					  200, 1, 201);

      h2_AllMap_[i][j]  = (TH2D*)tf_hitmap_->Get(Form("HitMap_%d_%d", i, j));

      h2_ColdMap_[i][j] = new TH2D( Form("ColdMap_%d_%d", i, j),
				    Form("ColdMap_%d_%d", i, j),
				    128, 0, 128,
				    27, 0, 27);

      h2_HalfMap_[i][j] = new TH2D( Form("HalfMap_%d_%d", i, j),
				    Form("HalfMap_%d_%d", i, j),
				    128, 0, 128,
				    27, 0, 27);

      h2_HotMap_[i][j]  = new TH2D( Form("HotMap_%d_%d", i, j),
				    Form("HotMap_%d_%d", i, j),
				    128, 0, 128,
				    27, 0, 27);

      h2_DeadMap_[i][j] = new TH2D( Form("DeadMap_%d_%d", i, j),
				    Form("DeadMap_%d_%d", i, j),
				    128, 0, 128,
				    27, 0, 27);
    }
  }
};

void Classifier::InitQaTree()
{
  tree_output_ = new TTree("tree", "tree");
  tree_output_->Branch( "felix"		, &felix_	);
  tree_output_->Branch( "module"	, &module_	);
  tree_output_->Branch( "chip_id"	, &chip_id_	);
  tree_output_->Branch( "chan"		, &chan_	);
  tree_output_->Branch( "flag"		, &flag_	);
  tree_output_->Branch( "ch_entry"	, &ch_entry_	);
  tree_output_->Branch( "type"		, &type_	);
  tree_output_->Branch( "mean"		, &mean_gaus_	);
  tree_output_->Branch( "sigma"		, &sigma_gaus_	);
}

void Classifier::PrintLine( string head, string contents, string separator )
{
  printed_contents_.push_back( head + separator + " " + contents );
}

void Classifier::DumpPrintBuffer()
{
  if( printed_contents_.size() == 0 )
  return;

  vector < int > line_lengths;
  for( auto& line : printed_contents_ )
    line_lengths.push_back( line.size() );

  auto max_length = *max_element( line_lengths.begin(), line_lengths.end() );
  cout << "+-" << string( max_length , '-' ) << "-+\n";
  for( auto& line : printed_contents_ )
    {
      cout << "| " + line + string( max_length - line.size(), ' ' ) << " |\n";
    }
  cout << "+-" << string( max_length , '-' ) << "-+\n";
  
  printed_contents_.clear();
}

void Classifier::DrawHists()
{
  gStyle->SetOptFit();
  gStyle->SetOptStat( 1111111 );

  TCanvas* c_pdf = new TCanvas( output_qa_pdf_file_.c_str(), "title", 800, 800 );
  c_pdf->Print( ((string)c_pdf->GetName()+"[").c_str() );
  c_pdf->cd();
  gPad->SetGrid( true, true );

  {
    auto hist = h1_hist_MPV_;
    HistConfig( hist );
    hist->Draw();

    auto line = DrawThreshold( hot_ch_cut_, true );
    
    TLatex* tex = new TLatex();
    tex->SetTextColor( line->GetLineColor() );
    tex->DrawLatex( hot_ch_cut_, line->GetY2() / 2, "#rightarrow hot channels" );
    c_pdf->Print( c_pdf->GetName() );
  }

  {
    auto hist = h1_hist_MPV_ladder_;
    HistConfig( hist );
    hist->Draw();
    gPad->SetLogy( false );

    gPad->Update();
    auto xmax = gPad->GetUxmax();
    if( xmax < hot_ladder_cut_ )
      hist->GetXaxis()->SetRangeUser( 0, hot_ladder_cut_ );

    auto line = DrawThreshold( hot_ladder_cut_ );
    
    TLatex* tex = new TLatex();
    tex->SetTextColor( line->GetLineColor() );
    tex->DrawLatex( hot_ladder_cut_, line->GetY2() / 2, "#rightarrow hot ladders" );
    c_pdf->Print( c_pdf->GetName() );

  }

  c_pdf->Print( ((string)c_pdf->GetName()+"]").c_str() );
}

TLine* Classifier::DrawThreshold( double threshold, bool log_y )
{
  
  gPad->SetLogy( log_y );
  gPad->Update(); // Don't delete it. It's needed for gPad->GetUymax() 
  double ymax = gPad->GetUymax();
  if( log_y )
    ymax = TMath::Power( 10, ymax );
  
  TLine* line = new TLine(threshold, 0, threshold, ymax );
  line->SetLineColor( kRed - 4 );
  line->SetLineWidth( 2 );
  line->DrawLine( threshold, 0, threshold, ymax );

  return line;
}

/////////////////////////////////////////////////
// public                                      //
/////////////////////////////////////////////////
void Classifier::Process()
{

  // loop over all channels of all to fill histograms
  for (int felix = 0; felix < kFelix_num; felix++)
  {
    for (int i = 0; i < kLadders_in_felix_num; i++)
    {
      // loop over all chips in this ladder
      for (int j = 0; j < kChip_num; j++)
      {

	// loop over all channels in this chip
        for (int chan = 0; chan < kCh_num; chan++)
        {
          int NumberOfEntry = h2_AllMap_[felix][i]->GetBinContent(chan+1,j+1);
          if(NumberOfEntry==0)
	    continue;
	  
	  NumberOfFiredChan_[felix][i]++;
	  auto content = h2_AllMap_[felix][i]->GetBinContent(chan + 1, j + 1);

	  if (j < 5 || (j > 12 && j < 18)) // Condition for type B
            {
              h1_hist_fit_B_[felix][i]->Fill( content );
            }
	  else // in the case of type-A
            {
              h1_hist_fit_A_[felix][i]->Fill( content );
            } // if
	  
	  //! @todo Check with Jaein whether it's OK or not. What about normalization?
	  // if( content > 1 )
	  //   cout << content << endl;
	  h1_hist_MPV_->Fill( content );
	  
	} // for( chan )
      } // for( chip )
      
      h1_hist_MPV_ladder_->Fill( NumberOfFiredChan_[felix][i] );
      //std::cout << "CHENCK !! Felix : " << felix << " module : " << i << std::endl;
      
    } // for( ladder )
  } // for( felix )

  // Cut determination
  hot_ch_cut_ = h1_hist_MPV_->GetXaxis()->GetBinCenter( h1_hist_MPV_->GetMaximumBin() ) * 2;
  hot_ladder_cut_ = h1_hist_MPV_ladder_->GetMean() * 5;

  hot_ch_cut_ = 1000;
  //hot_ladder_cut_ = 2000;
  process_done_ = true;
}

void Classifier::SetCdcOupoutDir( string dir )
{
  output_cdb_dir_ = dir;
  InitPaths();
}

void Classifier::SetQaRootOutputDir( string dir )
{
  output_qa_root_path_ = dir;
  InitPaths();
}

void Classifier::SetQaPdfOutputDir( string dir )
{
  output_qa_pdf_path_ = dir;
  InitPaths();
}

void Classifier::Print()
{
  this->PrintLine( "Hitmap", hitmap_file_ );
  this->PrintLine( "CDB output", output_cdb_dir_ );
  this->PrintLine( "QA output (ROOT)", output_qa_root_file_ );
  this->PrintLine( "QA output (PDF)", output_qa_pdf_file_ );
  this->PrintLine( "QA output (txt)", output_qa_txt_file_ );
  
  if( process_done_ )
    {
      this->PrintLine( "Hot ch cut", to_string( hot_ch_cut_) );
      this->PrintLine( "Hot ladder cut", to_string( hot_ladder_cut_ ) );
    }
  
  this->DumpPrintBuffer();
  //  cout << 
}

void Classifier::WriteResults()
{

  tf_qa_ = new TFile(output_qa_root_file_.c_str(), "RECREATE");
  InitQaTree();

  ofstream qa_txt( output_qa_txt_file_.c_str(), ios_base::out | ios_base::trunc );
  qa_txt << "# FELIX\t" << "FELIX_CH\t" << "CHIP\t" << "CHANNEL" << endl;

  DrawHists();
  
  CDBTTree *cdbttree = new CDBTTree( output_cdb_file_.c_str() );

  ///////////////////////////////////
  // Fill the Dead & Cold & HotMap //
  ///////////////////////////////////

  //double mean_first = 0; // not used...
  
  tf_qa_->cd();
  tf_qa_->WriteTObject( h1_hist_MPV_, h1_hist_MPV_->GetName() );
  tf_qa_->WriteTObject( h1_hist_MPV_ladder_, h1_hist_MPV_ladder_->GetName() );
  
  int size = 0;
  TDirectory *dir[ kFelix_num ];
  for (int felix = 0; felix < kFelix_num; felix++)
  {
    
    dir[felix] = tf_qa_->mkdir(Form("Felix_%d", felix));
    
    for (int i = 0; i < kLadders_in_felix_num; i++)
    {

      // ???
      if( NumberOfFiredChan_[felix][i] > hot_ladder_cut_ )
       {
        std::cout<<"HERE IS HOT LADDER"<<std::endl;
        std::cout<< felix<<" "<< i<<std::endl;
       }
      //  cout << "HELLO moudle : " << i << " Type A " << HotChannelCut_A_Fit[felix][i] << endl;
      //  cout << "HELLO moudle : " << i << " Type B " << HotChannelCut_B_Fit[felix][i] << endl;
      
      for (int j = 0; j < kChip_num; j++)
      {
	//    cout << "Felix : " << felix << " moudle : "
	// << i << " Type A and chip : " << j << "  " << HotChannelCut_A_Fit[felix][i] << endl;
       
        for (int chan = 0; chan < kCh_num; chan++)
        {
          // double entry = h1_chip_[i][j]->GetBinContent(chan + 1);
          double entry = h2_AllMap_[felix][i]->GetBinContent(chan + 1, j + 1);
          felix_	= felix;
          ch_entry_	= entry;
          module_	= i;
          chip_id_	= j;
          chan_		= chan;
          flag_		= 0;
	  
          // if (felix_ > 3) // Masking all north side
          // {
          //     flag_ = 8;
          // }

	  // in the case of hot channel
          if (ch_entry_ > hot_ch_cut_ || NumberOfFiredChan_[felix][i] > hot_ladder_cut_ )
          //if (ch_entry_ > hotchannelcut)
          {
	    //cout << "Hot channel: " << felix_ << "\t" << module_ << "\t" << chip_id_ << "\t" << chan_ << endl;
	    //	    stringstream contents;
	    qa_txt << felix_ << "\t" << module_ << "\t" << chip_id_ << "\t" << chan_ << endl;
	    //qa_txt.write( contents.str().c_str() );
	    
            flag_ = 8;
            // std::cout<<felix_<<std::endl;
            // std::cout<<module_<<std::endl;
            // std::cout<<chip_id_<<std::endl;
            // std::cout<<chan_<<std::endl;
            cdbttree->SetIntValue(size, "felix_server", felix);
            cdbttree->SetIntValue(size, "felix_channel", module_);
            cdbttree->SetIntValue(size, "chip", chip_id_);
            cdbttree->SetIntValue(size, "channel", chan_);
            cdbttree->SetIntValue(size, "flag", flag_);
            ++size;
            h2_HotMap_[felix][i]->SetBinContent( chan+1, j+1, ch_entry_ );
          }
          // else if ( (felix==1 && module_ == 10) || (felix==1 && module_<3) )
          // {
          //   flag_ = 8;
          //   cdbttree->SetIntValue(size, "felix_server", felix);
          //   cdbttree->SetIntValue(size, "felix_channel", module_);
          //   cdbttree->SetIntValue(size, "chip", chip_id_);
          //   cdbttree->SetIntValue(size, "channel", chan_);
          //   cdbttree->SetIntValue(size, "flag", flag_);
          //   ++size;
          //   h2_HotMap[felix][i]->SetBinContent(chan + 1, j + 1, ch_entry_);
          // }

          tree_output_->Fill();
        }
      }
   
      dir[felix]->WriteObject( h2_AllMap_[felix][i], h2_AllMap_[felix][i]->GetName() );
      dir[felix]->WriteObject( h2_DeadMap_[felix][i], h2_DeadMap_[felix][i]->GetName() );
      dir[felix]->WriteObject( h2_ColdMap_[felix][i], h2_ColdMap_[felix][i]->GetName() );
      dir[felix]->WriteObject( h2_HalfMap_[felix][i], h2_HalfMap_[felix][i]->GetName() );
      dir[felix]->WriteObject( h2_HotMap_[felix][i], h2_HotMap_[felix][i]->GetName() );
      dir[felix]->WriteObject( h1_hist_fit_A_[felix][i], h1_hist_fit_A_[felix][i]->GetName() );
      dir[felix]->WriteObject( h1_hist_fit_B_[felix][i], h1_hist_fit_B_[felix][i]->GetName() );

    }
    
    tf_qa_->cd();
    tf_qa_->WriteTObject( canA_[felix], canA_[felix]->GetName() );
    tf_qa_->WriteTObject( canB_[felix], canB_[felix]->GetName() );
  }
  
  cdbttree->SetSingleIntValue("size", size);
  cdbttree->Commit();
  cdbttree->CommitSingle();
  cdbttree->WriteCDBTTree();

  tf_qa_->WriteTObject( tree_output_, tree_output_->GetName() );

  // Add content to the end of the file
  //if(Writecsv) csvFile << runnumber << "," << NumOfHot << "\n";

  // Close the file
  //csvFile.close();

  tf_qa_->Write();
  tf_qa_->Close();

  qa_txt.close();
  return;
}
  

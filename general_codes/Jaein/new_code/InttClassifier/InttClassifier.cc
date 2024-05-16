#include "InttClassifier.hh"

// Trick to write like the standard C++ but run as a part of ROOT macro
#define InttClassifier_cc

/////////////////////////////////////////////////
// Constructor                                 //
/////////////////////////////////////////////////
InttClassifier::InttClassifier(int run_num, double sig_cut)
    : run_num_(run_num), sig_cut_(sig_cut)
{
};

/////////////////////////////////////////////////
// private                                     //
/////////////////////////////////////////////////
void InttClassifier::Init()
{

  in_sig_ = 10 * sig_cut_;
  InitPaths();

  tf_hitmap_ = new TFile(hitmap_file_.c_str(), "READ");
  tt_runinfo_ = (TTree *)tf_hitmap_->Get("tree");
  tt_runinfo_->SetBranchAddress("runnumber", &runnumber_);
  tt_runinfo_->SetBranchAddress("total_event", &total_event_);
  for (int i = 0; i < tt_runinfo_->GetEntries(); i++)
  {
    tt_runinfo_->GetEntry(i);
    merged_event_ += total_event_;
  }

  InitHists();

  did_init_ = true;
}

void InttClassifier::InitPaths()
{

  std::string run_num_str = GetRunNum8digits(run_num_);
  if (hitmap_file_.empty())
    hitmap_file_ = hitmap_dir_ + "hitmap_run" + run_num_str + this->GetFileSuffix() + ".root";

  if( output_tag_ != "" )
    output_tag_ = "_" + output_tag_;
  output_cdb_file_ = output_cdb_dir_ + "cdb_" + run_num_str + this->GetFileSuffix() + output_tag_ + ".root";

  // Determination of output file names.
  std::string output_basename = "InttHotDeadMap_" + run_num_str + "_" + std::to_string(in_sig_) + this->GetFileSuffix() + output_tag_;
  // output_qa_root_file_ = output_qa_root_path_ + "root/" + output_basename + ".root";
  // output_qa_pdf_file_ = output_qa_pdf_path_ + "plots/" + output_basename + ".pdf";
  // output_qa_txt_file_ = output_qa_pdf_path_ + "txt/" + output_basename + ".txt";
  output_qa_root_file_ = output_qa_root_dir_ + "root/" + std::to_string(year_) + "/" + output_basename + ".root";
  output_qa_pdf_file_ = output_qa_pdf_dir_ + "plots/" + std::to_string(year_) + "/" + output_basename + ".pdf";
  output_qa_txt_file_ = output_qa_pdf_dir_ + "txt/" + std::to_string(year_) + "/" + output_basename + ".txt";
}

void InttClassifier::InitHists()
{

  string title_x = "Normalized #hit/ch;#hit/ch/event/strip length/(r/r_{inner})";
  string title_y = ";Entries";
  
  h1_hist_MPV_ = new TH1D("hist_MPV", "hist_MPV", 200, 1, 201);

  string title = "Normalized #hit/ch" + title_x + title_y;
  h1_hist_gaus_ = new TH1D("hist_gaus", title.c_str(), bin_num_, xmin_, xmax_ );
  
  for (int i = 0; i < 8; i++)
  {
    string title = string(Form("Normarlized #hit/ch (intt%d)", i)) + ";" + title_x + title_y;
    h1_hist_gaus_felix_[i] = new TH1D(Form("hist_gaus_felix_%d", i),
				      title.c_str(),
				      bin_num_, xmin_, xmax_);
  }
  
  h1_hist_chip_ = new TH1D("hist_chip", "hist_chip", 200, 0, xmax_);
  h1_hist_MPV_->SetTitle("#hit/ch (non-zero only);#hit;Entries");
  h1_hist_MPV_ladder_ = new TH1D("NumberOfFiredChannels_ladder", "NumberOfFiredChannels_ladder", 350, 1, 3501);

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 14; j++)
    {

      h2_AllMap_[i][j] = (TH2D *)tf_hitmap_->Get(Form("HitMap_%d_%d", i, j));

      h2_ColdMap_[i][j] = new TH2D(Form("ColdMap_%d_%d", i, j),
                                   Form("ColdMap_%d_%d", i, j),
                                   128, 0, 128,
                                   27, 0, 27);

      h2_HalfMap_[i][j] = new TH2D(Form("HalfMap_%d_%d", i, j),
                                   Form("HalfMap_%d_%d", i, j),
                                   128, 0, 128,
                                   27, 0, 27);

      h2_HotMap_[i][j] = new TH2D(Form("HotMap_%d_%d", i, j),
                                  Form("HotMap_%d_%d", i, j),
                                  128, 0, 128,
                                  27, 0, 27);

      h2_DeadMap_[i][j] = new TH2D(Form("DeadMap_%d_%d", i, j),
                                   Form("DeadMap_%d_%d", i, j),
                                   128, 0, 128,
                                   27, 0, 27);
    }
  }
};

void InttClassifier::InitQaTree()
{
  tree_output_ = new TTree("tree", "tree");
  tree_output_->Branch("felix", &felix_);
  tree_output_->Branch("module", &module_);
  tree_output_->Branch("chip_id", &chip_id_);
  tree_output_->Branch("chan", &chan_);
  tree_output_->Branch("flag", &flag_);
  tree_output_->Branch("ch_entry", &ch_entry_);
  tree_output_->Branch("type", &type_);
  tree_output_->Branch("mean", &mean_gaus_);
  tree_output_->Branch("sigma", &sigma_gaus_);
}

void InttClassifier::PrintLine(std::string head, std::string contents, std::string separator)
{
  printed_contents_.push_back(head + separator + " " + contents);
}

void InttClassifier::SetHitMapWithPath(std::string fname)
{
  hitmap_file_ = fname;
}

void InttClassifier::DumpPrintBuffer()
{
  if (printed_contents_.size() == 0)
    return;

  vector<int> line_lengths;
  for (auto &line : printed_contents_)
    line_lengths.push_back(line.size());

  auto max_length = *max_element(line_lengths.begin(), line_lengths.end());
  cout << "+-" << std::string(max_length, '-') << "-+\n";
  for (auto &line : printed_contents_)
  {
    cout << "| " + line + std::string(max_length - line.size(), ' ') << " |\n";
  }
  cout << "+-" << std::string(max_length, '-') << "-+\n";

  printed_contents_.clear();
}

void InttClassifier::DrawHists()
{
  gStyle->SetOptFit();
  gStyle->SetOptStat(1111111);

  TCanvas *c_pdf = new TCanvas(output_qa_pdf_file_.c_str(), "title", 1600, 1200);
  c_pdf->Print(((std::string)c_pdf->GetName() + "[").c_str());
  c_pdf->SetMargin( 0.1, 0.05, 0.1, 0.1 );
  gPad->SetGrid(true, true);

  if( isbeam_ == false ) // for cosmics
  {
    auto hist = h1_hist_MPV_;
    HistConfig(hist);
    hist->Draw();
    auto line = DrawThreshold(hot_ch_cut_, true);

    TLatex *tex = new TLatex();
    tex->SetTextColor(line->GetLineColor());
    tex->DrawLatex(hot_ch_cut_, line->GetY2() / 2, "#rightarrow hot channels");
    c_pdf->Print(c_pdf->GetName());
  }

  // #hit / ch / normarization factor
  {
    auto hist = h1_hist_gaus_;
    HistConfig(hist);
    hist->Draw();
    this->DrawStats( hist, 0.7, 0.5, 0.95, 0.9 );
    
    // set x range to show the threshold
    if( hot_ch_cut_ > xmax_ )
      hist->GetXaxis()->SetRangeUser(0, hot_ch_cut_);
    
    this->DrawFittedFunction( hist, hot_ch_cut_ );

    auto line = DrawThreshold(hot_ch_cut_, true);

    TLatex *tex = new TLatex();
    tex->SetTextColor(line->GetLineColor());
    tex->DrawLatex(hot_ch_cut_, line->GetY2() / 2, "#rightarrow hot channels");
    
    c_pdf->Print(c_pdf->GetName());
  }
  
  c_pdf->Print(((std::string)c_pdf->GetName() + "]").c_str());
  return;

  // #hit / ch / felix / normarization factor
  for (int i = 0; i < kFelix_num; i++)
    {
      auto hist = h1_hist_gaus_felix_[i];
      HistConfig(hist);
      
      hist->Draw();
      
      // set x range to show the threshold
      if( hot_ch_cut_gaus_[i] > xmax_ )
	hist->GetXaxis()->SetRangeUser(0, hot_ch_cut_gaus_[i]);
      else
	hist->GetXaxis()->SetRangeUser(0, xmax_);

      hist->GetXaxis()->SetLabelSize( 0.02 );
      gPad->Update();
      bool is_log = false;
      gPad->SetLogy( is_log );
      auto line = DrawThreshold(hot_ch_cut_gaus_[i], is_log );
      
      TLatex *tex = new TLatex();
      tex->SetTextColor(line->GetLineColor());
      tex->DrawLatex(hot_ch_cut_gaus_[i], line->GetY2() / 2, "#rightarrow hot channels");
      c_pdf->Print(c_pdf->GetName());
    }

  if( false )
  {
    auto hist = h1_hist_MPV_ladder_;
    HistConfig(hist);
    hist->Draw();
    gPad->SetLogy(false);

    gPad->Update();
    auto xmax = gPad->GetUxmax();
    if (xmax < hot_ladder_cut_)
      hist->GetXaxis()->SetRangeUser(0, hot_ladder_cut_);

    auto line = DrawThreshold(hot_ladder_cut_);

    TLatex *tex = new TLatex();
    tex->SetTextColor(line->GetLineColor());
    tex->DrawLatex(hot_ladder_cut_, line->GetY2() / 2, "#rightarrow hot ladders");
    c_pdf->Print(c_pdf->GetName());
  }

  c_pdf->Print(((std::string)c_pdf->GetName() + "]").c_str());
}

void InttClassifier::DrawFittedFunction( TH1D* hist, double threshold )
{

  TF1* fit = (TF1*)hist->GetListOfFunctions()->At( 0 );
  
  if( fit == nullptr )
    return;
  
  if( fitting_mode_ == "gaus" )
    {


    }
  else if( fitting_mode_ == "double_gaus" )
    {
      TF1* gaus_main = new TF1( "main_gaus", "gaus", 0, 1 );
      gaus_main->SetNpx( 1e5 ); // needed for Integrate
      for( int i=0; i<gaus_main->GetNpar(); i++ )
	{
	  gaus_main->SetParameter( i, 1.*fit->GetParameter(i) );
	}
      
      gaus_main->SetLineStyle( 7 );
      gaus_main->SetLineColor( kOrange + 1 );
      gaus_main->SetFillColorAlpha( gaus_main->GetLineColor(), 0.25 );
      gaus_main->SetFillStyle( 1001 );
      gaus_main->Draw( "same" );
      
      TF1* gaus_half = new TF1( "main_gaus", "gaus", 0, 1 );
      gaus_half->SetNpx( 1e5 ); // needed for Integrate
      gaus_half->SetParameter( 0, fit->GetParameter(3) );
      gaus_half->SetParameter( 1, fit->GetParameter(1) * 0.5 );
      gaus_half->SetParameter( 2, fit->GetParameter(4) );
      gaus_half->SetLineStyle( 3 );
      gaus_half->SetLineColor( kGreen + 1 );
      gaus_half->SetFillColorAlpha( gaus_half->GetLineColor(), 0.25 );
      gaus_half->SetFillStyle( 1001 );
      gaus_half->Draw( "same" );

      TLatex* tex = new TLatex();
      tex->SetTextSize( 0.03 );
      double pos_x = 0.65, pos_y = 0.425;
      stringstream words;
      words << "Ch with non-zero hit: "
	    << setprecision(3) << 100 * hist->GetEntries() / (128 * 26 * 14 * 8) << "%";
      tex->DrawLatexNDC( pos_x, pos_y, words.str().c_str() );

      words.str( "" );
      words << "Hot ch threshould: "
	    << setprecision(3)
	    << threshold;
      tex->SetTextColor( fit->GetLineColor() );
      pos_y -= 0.05;
      tex->DrawLatexNDC( pos_x, pos_y, words.str().c_str() );

      words.str( "" );
      words << "#Hot ch: "
	    << hot_ch_num_;
      tex->SetTextColor( fit->GetLineColor() );
      pos_y -= 0.05;
      tex->DrawLatexNDC( pos_x, pos_y, words.str().c_str() );

      // lambda function to calcurate #ch in a gaussian
      auto GetNchRatio = [](TH1D* hist, TF1* gaus)
      {
	return  100 * gaus->Integral(0, 1) / hist->GetXaxis()->GetBinWidth( 0 ) / hist->GetEntries();	
      };

      words.str( "" );
      words << "#ch in main gaus: "
	    << setprecision(3)
	    << GetNchRatio( hist, gaus_main ) << "%";
      tex->SetTextColor( gaus_main->GetLineColor() );
      pos_y -= 0.05;
      tex->DrawLatexNDC( pos_x, pos_y, words.str().c_str() );
      
      words.str( "" );
      words << "#ch in half-entry ch: "
	    << setprecision(3)
	    << GetNchRatio( hist, gaus_half ) << "%";
      tex->SetTextColor( gaus_half->GetLineColor() );
      pos_y -= 0.05;
      tex->DrawLatexNDC( pos_x, pos_y, words.str().c_str() );
            
    }
}

TLine *InttClassifier::DrawThreshold(double threshold, bool log_y)
{

  gPad->SetLogy(log_y);
  gPad->Update(); // Don't delete it. It's needed for gPad->GetUymax()
  double ymax = gPad->GetUymax();
  if (log_y)
    ymax = TMath::Power(10, ymax);

  TLine *line = new TLine(threshold, 0, threshold, ymax);
  line->SetLineColor(kRed - 4);
  line->SetLineWidth(2);
  line->DrawLine(threshold, 0, threshold, ymax);

  return line;
}

/////////////////////////////////////////////////
// public                                      //
/////////////////////////////////////////////////

void InttClassifier::ProcessBeam()
{
  if (did_init_ == false)
  {
    cerr << " Please do InttClassiffier::Init() before ProcessBeam." << endl;
    cerr << " Nothing done." << endl;
    return;
  }

  if (isbeam_ != true)
  {
    cerr << " Beam flag is " << isbeam_ << "." << endl;
    cerr << " Why do you run ProcessBeam?" << endl;
    cerr << " Nothing done." << endl;
  }

  for (int felix = 0; felix < kFelix_num; felix++)
  {
    for (int i = 0; i < kLadders_in_felix_num; i++)
    {
      // loop over all chips in this ladder
      for (int j = 0; j < kChip_num; j++)
      {
        double content_chip = 0.;
        int n_fired_ch = 0;

        // loop over all channels in this chip
        for (int chan = 0; chan < kCh_num; chan++)
        {
          double content = h2_AllMap_[felix][i]->GetBinContent(chan + 1, j + 1);
          if (content == 0)
            continue;

          n_fired_ch++;
          content /= merged_event_;
          // if (content != 0)
          //   std::cout << content << std::endl;
          bool ishalf = false;
          for (const auto &chip : half_Chips_)
          {
            if (chip.felix_id_ == felix && chip.module_id_ == i && chip.chip_id_ == j)
            {
              ishalf = true;
              break;
            }
          }
          h1_hist_gaus_->Fill(content);
          if (!ishalf)
            h1_hist_gaus_felix_[felix]->Fill(content);
          content_chip += content;

        } // for( chan )
        h1_hist_chip_->Fill(content_chip / n_fired_ch);
      } // for( chip )
    } // for( ladder )
  }

  auto result = DoGaussianFit(h1_hist_gaus_);
  if (h1_hist_gaus_->GetMean() == 0)
  {
    std::cout << "InttClassifier::Histogram is empty or invalid." << std::endl;
    std::cout << "Skip histogram : " << h1_hist_gaus_->GetName() << std::endl;
    hot_ch_cut_ = 99999;
  }
  else
  {
    auto parameters = result->Parameters();
    hot_ch_cut_ = result->Parameters()[1] + result->Parameters()[2] * sig_cut_;
  }

  for (int i = 0; i < 8; i++)
  {
    TFitResultPtr fitResult = DoGaussianFit(h1_hist_gaus_felix_[i]);
    if (h1_hist_gaus_felix_[i]->GetMean() == 0)
    {
      std::cout << "InttClassifier::Histogram is empty or invalid." << std::endl;
      std::cout << "Skip histogram : " << h1_hist_gaus_felix_[i]->GetName() << std::endl;
      hot_ch_cut_gaus_[i] = 99999;
    }
    else
    {
      std::vector<double> par = fitResult->Parameters();
      hot_ch_cut_gaus_[i] = par[1] + par[2] * sig_cut_;
    }
  }
}

TFitResultPtr InttClassifier::DoGaussianFit(TH1D *hist)
{
  TF1* f;
  if( fitting_mode_ == "gaus" )
    {
      f = new TF1( "func", "gaus", 0, 1 );
      f->SetParLimits( 0, 0, 3e5 ); // constant
      f->SetParLimits( 1, 0, 10 ); // constant
      f->SetParLimits( 2, 0, 1 ); // constant
      f->SetParameter( 1, hist->GetMean() );
      f->SetParameter( 2, hist->GetStdDev() );
      f->SetNpx( 1000 );
    }
  else if( fitting_mode_ == "double_gaus" )
    {
      f = new TF1( "double_gaus", "[0]*exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[1]*0.5)/[4])**2)", 0, 1 );
      f->SetParLimits( 0, 0, 3e5 ); // Main gaus: constant
      f->SetParLimits( 1, 0, 1 );   // Main gaus: mean
      f->SetParLimits( 2, 0, 1 );   // Main gaus: std. dev.
      f->SetParLimits( 3, 0, 3e5 ); // Half-entry gaus: constant
      f->SetParLimits( 4, 0, 1 );   // Half-entry gaus: std. dev.
      f->SetParameter( 1, hist->GetMean() );
      f->SetParameter( 2, hist->GetStdDev() );
      f->SetParameter( 4, 1e-6 );
      f->SetNpx( 1000 );
    }

  f->SetLineWidth( 1 );
  return hist->Fit( f, "S");
}

void InttClassifier::ProcessCosmics()
{

  if( did_init_ == false )
    {
      cerr << " Please do InttClassiffier::Init() before ProcessCosmics." << endl;
      cerr << " Nothing done." << endl;
      return;
    }
  
  if( isbeam_ != false )
    {
      cerr << " Beam flag is " << isbeam_ << "." << endl;
      cerr << " Why do you run ProcessCosmics?" << endl;
      cerr << " Nothing done." << endl;
    }
  
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
          int NumberOfEntry = h2_AllMap_[felix][i]->GetBinContent(chan + 1, j + 1);
          if (NumberOfEntry == 0)
            continue;

          NumberOfFiredChan_[felix][i]++;
          auto content = h2_AllMap_[felix][i]->GetBinContent(chan + 1, j + 1);

          if (j < 5 || (j > 12 && j < 18)) // Condition for type B
          {
            h1_hist_fit_B_[felix][i]->Fill(content);
          }
          else // in the case of type-A
          {
            h1_hist_fit_A_[felix][i]->Fill(content);
          } // if

          h1_hist_MPV_->Fill(content);

        } // for( chan )
      }   // for( chip )

      h1_hist_MPV_ladder_->Fill(NumberOfFiredChan_[felix][i]);
      // std::cout << "CHENCK !! Felix : " << felix << " module : " << i << std::endl;

    } // for( ladder )
  }   // for( felix )

  // Cut determination
  hot_ch_cut_ = h1_hist_MPV_->GetXaxis()->GetBinCenter(h1_hist_MPV_->GetMaximumBin()) * 2;
  // cout << "-----------------------------------------------" << endl;
  // cout << h1_hist_MPV_->GetMaximumBin() << endl;
  // cout << h1_hist_MPV_->GetXaxis()->GetBinCenter(h1_hist_MPV_->GetMaximumBin()) << endl;
  // cout << "-----------------------------------------------" << endl;  

  hot_ladder_cut_ = h1_hist_MPV_ladder_->GetMean() * 5;

  process_done_ = true;
}

string InttClassifier::GetFileSuffix()
{

  if( force_suffix_ == false )
    {
      if( is_official_ == true )
	return "_official";
      else
	return "_special";
    }
  
  return suffix_;
}

void InttClassifier::SetFittingMode( string mode )
{
  assert( mode == "gaus"
	  || mode == "double_gaus" );

  fitting_mode_ = mode;
}

void InttClassifier::Print()
{
  this->PrintLine("Hitmap", hitmap_file_);
  this->PrintLine("CDB output", output_cdb_dir_);
  this->PrintLine("QA output (ROOT)", output_qa_root_file_);
  this->PrintLine("QA output (PDF)", output_qa_pdf_file_);
  this->PrintLine("QA output (txt)", output_qa_txt_file_);

  if (process_done_)
  {
    this->PrintLine("Hot ch cut", std::to_string(hot_ch_cut_));
    this->PrintLine("Hot ladder cut", std::to_string(hot_ladder_cut_));
  }

  this->DumpPrintBuffer();
  //  cout <<
}

void InttClassifier::WriteResults()
{

  if( did_init_ == false )
    {
      cerr << " Please do InttClassiffier::Init() before WriteResults." << endl;
      cerr << " Nothing done." << endl;
      return;
    }
  
  tf_qa_ = new TFile(output_qa_root_file_.c_str(), "RECREATE");
  InitQaTree();

  ofstream qa_txt(output_qa_txt_file_.c_str(), ios_base::out );
  qa_txt << "# FELIX\t"
         << "FELIX_CH\t"
         << "CHIP\t"
         << "CHANNEL" << endl;

  CDBTTree *cdbttree = new CDBTTree(output_cdb_file_.c_str());

  ///////////////////////////////////
  // Fill the Dead & Cold & HotMap //
  ///////////////////////////////////

  // double mean_first = 0; // not used...

  tf_qa_->cd();
  tf_qa_->WriteTObject(h1_hist_MPV_, h1_hist_MPV_->GetName());
  tf_qa_->WriteTObject(h1_hist_gaus_, h1_hist_gaus_->GetName());
  tf_qa_->WriteTObject(h1_hist_chip_, h1_hist_chip_->GetName());
  tf_qa_->WriteTObject(h1_hist_MPV_ladder_, h1_hist_MPV_ladder_->GetName());
  for(int i=0;i<8;i++)
  {
    tf_qa_->WriteTObject(h1_hist_gaus_felix_[i], h1_hist_gaus_felix_[i]->GetName());
  }

  int size = 0;
  TDirectory *dir[kFelix_num];
  for (int felix = 0; felix < kFelix_num; felix++)
  {

    dir[felix] = tf_qa_->mkdir(Form("Felix_%d", felix));

    for (int i = 0; i < kLadders_in_felix_num; i++)
    {

      // ???
      if (NumberOfFiredChan_[felix][i] > hot_ladder_cut_)
      {
        std::cout << "HERE IS HOT LADDER" << std::endl;
        std::cout << felix << " " << i << std::endl;
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
          felix_ = felix;
          ch_entry_ = entry;
          if (isbeam_)
            ch_entry_ /= merged_event_;
          module_ = i;
          chip_id_ = j;
          chan_ = chan;
          flag_ = 0;
          if (chip_id_ < 5 || (chip_id_ > 12 && chip_id_ < 18))
            type_ = 1;
          else
            type_ = 0;
          // if (felix_ > 3) // Masking all north side
          // {
          //     flag_ = 8;
          // }

          // in the case of hot channel
          if (isbeam_)
          {
            if (ch_entry_ > hot_ch_cut_gaus_[felix])
              ishot_ = true;
            else
              ishot_ = false;
          }

          if (!isbeam_)
          {
            if (ch_entry_ > hot_ch_cut_ || NumberOfFiredChan_[felix][i] > hot_ladder_cut_)
              ishot_ = true;
            else
              ishot_ = false;
          }
          // if (ch_entry_ > hotchannelcut)
          if(ishot_)
          {
	    hot_ch_num_++;
            // cout << "Hot channel: " << felix_ << "\t" << module_ << "\t" << chip_id_ << "\t" << chan_ << endl;
            //	    std::stringstream contents;
            qa_txt << felix_ << "\t" << module_ << "\t" << chip_id_ << "\t" << chan_ << endl;
            // qa_txt.write( contents.str().c_str() );

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
            h2_HotMap_[felix][i]->SetBinContent(chan + 1, j + 1, ch_entry_);
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

      dir[felix]->WriteObject(h2_AllMap_[felix][i], h2_AllMap_[felix][i]->GetName());
      dir[felix]->WriteObject(h2_DeadMap_[felix][i], h2_DeadMap_[felix][i]->GetName());
      dir[felix]->WriteObject(h2_ColdMap_[felix][i], h2_ColdMap_[felix][i]->GetName());
      dir[felix]->WriteObject(h2_HalfMap_[felix][i], h2_HalfMap_[felix][i]->GetName());
      dir[felix]->WriteObject(h2_HotMap_[felix][i], h2_HotMap_[felix][i]->GetName());
    }

    tf_qa_->cd();
    // tf_qa_->WriteTObject( canA_[felix], canA_[felix]->GetName() );
    // tf_qa_->WriteTObject( canB_[felix], canB_[felix]->GetName() );
  }

  cdbttree->SetSingleIntValue("size", size);
  cdbttree->Commit();
  cdbttree->CommitSingle();
  cdbttree->WriteCDBTTree();

  tf_qa_->WriteTObject(tree_output_, tree_output_->GetName());

  // Add content to the end of the file
  // if(Writecsv) csvFile << runnumber << "," << NumOfHot << "\n";

  // Close the file
  // csvFile.close();

  tf_qa_->Write();
  tf_qa_->Close();

  qa_txt.close();
  DrawHists();
  return;
}

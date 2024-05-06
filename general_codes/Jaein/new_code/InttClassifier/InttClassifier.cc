#include "InttClassifier.hh"
#define InttClassifier_cc

/////////////////////////////////////////////////
// Constructor                                 //
/////////////////////////////////////////////////
InttClassifier::InttClassifier(int run_num, double sig_cut)
    : run_num_(run_num), sig_cut_(sig_cut)
{

  Init();
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
}

void InttClassifier::InitPaths()
{

  // std::string run_num_str = std::to_string( run_num_ );
  std::string run_num_str = GetRunNum8digits(run_num_);
  // std::std::string rootfilename = map_input_path + "hitmap_run" + std::to_string(runnumber) + ".root";
  // hitmap_file_ = kIntt_hitmap_dir + "hitmap_run" + run_num_str + ".root";
  hitmap_file_ = "hitmap_run" + run_num_str + ".root";

  output_cdb_file_ = output_cdb_dir_ + "cdb_" + run_num_str + ".root";

  // Determination of output file names.
  std::string output_basename = "InttHotDeadMap_" + run_num_str + "_" + std::to_string(in_sig_);
  // output_qa_root_file_ = output_qa_root_path_ + "root/" + output_basename + ".root";
  // output_qa_pdf_file_ = output_qa_pdf_path_ + "plots/" + output_basename + ".pdf";
  // output_qa_txt_file_ = output_qa_pdf_path_ + "txt/" + output_basename + ".txt";
  output_qa_root_file_ = output_qa_root_path_ + "root/" + std::to_string(kYear) + "/" + output_basename + ".root";
  output_qa_pdf_file_ = output_qa_pdf_path_ + "plots/" + std::to_string(kYear) + "/" + output_basename + ".pdf";
  output_qa_txt_file_ = output_qa_pdf_path_ + "txt/" + std::to_string(kYear) + "/" + output_basename + ".txt";
}

void InttClassifier::InitHists()
{
  h1_hist_MPV_ = new TH1D("hist_MPV", "hist_MPV", 200, 1, 201);
  h1_hist_gaus_ = new TH1D("hist_gaus", "hist_gaus", 200, 0, 0.0005);
  for (int i = 0; i < 8; i++)
  {
    h1_hist_gaus_felix_[i] = new TH1D(Form("hist_gaus_felix_%d", i), Form("hist_gaus_felix_%d", i), 200, 0, 0.0005);
  }
  h1_hist_chip_ = new TH1D("hist_chip", "hist_chip", 200, 0, 0.0005);
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

  TCanvas *c_pdf = new TCanvas(output_qa_pdf_file_.c_str(), "title", 800, 800);
  c_pdf->Print(((std::string)c_pdf->GetName() + "[").c_str());
  c_pdf->cd();
  gPad->SetGrid(true, true);

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
  double initial_par[6] = {0.};
  {
    auto hist = h1_hist_gaus_;
    HistConfig(hist);
    hist->Draw();
    auto line = DrawThreshold(hot_ch_cut_, true);

    TLatex *tex = new TLatex();
    tex->SetTextColor(line->GetLineColor());
    tex->DrawLatex(hot_ch_cut_, line->GetY2() / 2, "#rightarrow hot channels");
    c_pdf->Print(c_pdf->GetName());
  }
  for (int i = 0; i < kFelix_num; i++)
  {
    auto hist = h1_hist_gaus_felix_[i];
    HistConfig(hist);
    hist->GetXaxis()->SetRangeUser(0, 0.0005);
    gPad->Update();
    hist->Fit("gaus");
    hist->Draw();

    auto line = DrawThreshold(hot_ch_cut_gaus_[i], true);

    TLatex *tex = new TLatex();
    tex->SetTextColor(line->GetLineColor());
    tex->DrawLatex(hot_ch_cut_, line->GetY2() / 2, "#rightarrow hot channels");
    c_pdf->Print(c_pdf->GetName());
  }
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
  isbeam_ = true;
  std::cout << h2_AllMap_[2][2]->GetName() << std::endl;
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
          h1_hist_gaus_->Fill(content);
          h1_hist_gaus_felix_[felix]->Fill(content);
          content_chip += content;

        } // for( chan )
        h1_hist_chip_->Fill(content_chip / n_fired_ch);
      } // for( chip )
    }   // for( ladder )
  }
  DoGaussianFit(h1_hist_gaus_);
  for (int i = 0; i < 8; i++)
  {
    TFitResultPtr fitResult = DoGaussianFit(h1_hist_gaus_felix_[i]);
    std::vector<double> par = fitResult->Parameters();
    std::cout << par[0] << std::endl;
    std::cout << par[1] << std::endl;
    std::cout << par[2] << std::endl;
    hot_ch_cut_gaus_[i] = par[1] + par[2] * sig_cut_;
  }
}

TFitResultPtr InttClassifier::DoGaussianFit(TH1 *hist)
{
  return hist->Fit("gaus", "S");
}
void InttClassifier::ProcessCosmics()
{
  isbeam_ = false;
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
  hot_ladder_cut_ = h1_hist_MPV_ladder_->GetMean() * 5;

  process_done_ = true;
}

void InttClassifier::SetCdcOupoutDir(std::string dir)
{
  output_cdb_dir_ = dir;
  InitPaths();
}

void InttClassifier::SetQaRootOutputDir(std::string dir)
{
  output_qa_root_path_ = dir;
  InitPaths();
}

void InttClassifier::SetQaPdfOutputDir(std::string dir)
{
  output_qa_pdf_path_ = dir;
  InitPaths();
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

  tf_qa_ = new TFile(output_qa_root_file_.c_str(), "RECREATE");
  InitQaTree();

  ofstream qa_txt(output_qa_txt_file_.c_str(), ios_base::out | ios_base::app);
  qa_txt << "# FELIX\t"
         << "FELIX_CH\t"
         << "CHIP\t"
         << "CHANNEL" << endl;

  DrawHists();

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
          if(isbeam_)
          {
            if(ch_entry_ < hot_ch_cut_gaus_[felix])
            ishot_ = true;
          }
          if (entry_ > hot_ch_cut_ || NumberOfFiredChan_[felix][i] > hot_ladder_cut_)
          // if (ch_entry_ > hotchannelcut)
          if(ishot_)
          {
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
  return;
}

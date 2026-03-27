#include "utilities/sPhenixStyle.C"

#include <iostream>
#include <string>
#include <vector>

namespace
{
  struct IntervalRegion
  {
    std::string hist_name;
    std::string legend_label;
    std::string draw_label;
    int color;
    int line_style;
    double min_interval;
    double max_interval;
    bool include_upper_edge;
  };

  bool IsInRegion(const IntervalRegion& region, const int64_t interval)
  {
    if (interval < region.min_interval) {
      return false;
    }

    if (region.include_upper_edge) {
      return interval >= region.min_interval && interval <= region.max_interval;
    }

    return interval >= region.min_interval && interval < region.max_interval;
  }
}

void MakeCorrectedMultiplicityByInterval()
{
  SetsPhenixStyle();

  const std::string input_filename =
    "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/completed/100files_Criteria2_TTree_0hitHandled_CloneHotRemoved.root";
    // "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/4646files_Criteria2_TTree_0hitHandled_halfladderCutted.root";
  const std::string output_filename =
    "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h1D_CorrectedMultiplicity_interval_by_interval_75519.pdf";

  TFile* file = TFile::Open(input_filename.c_str());
  if (!file || file->IsZombie()) {
    std::cerr << "Error: cannot open file " << input_filename << std::endl;
    return;
  }

  TTree* tree = static_cast<TTree*>(file->Get("tree"));
  if (!tree) {
    std::cerr << "Error: cannot find TTree named \"tree\"" << std::endl;
    file->Close();
    delete file;
    return;
  }

  tree->SetBranchStatus("*", 0);
  tree->SetBranchStatus("n_hit_inner", 1);
  tree->SetBranchStatus("n_hit_outer", 1);
  tree->SetBranchStatus("num_of_carried_hits", 1);
  tree->SetBranchStatus("interval_from_previous", 1);
  tree->SetBranchStatus("is_MBDNS_tight_vtx10cm", 1);

  int n_hit_inner = 0;
  int n_hit_outer = 0;
  int num_of_carried_hits = 0;
  int64_t interval_from_previous = 0;
  bool is_MBDNS_tight_vtx10cm = false;

  tree->SetBranchAddress("n_hit_inner", &n_hit_inner);
  tree->SetBranchAddress("n_hit_outer", &n_hit_outer);
  tree->SetBranchAddress("num_of_carried_hits", &num_of_carried_hits);
  tree->SetBranchAddress("interval_from_previous", &interval_from_previous);
  tree->SetBranchAddress("is_MBDNS_tight_vtx10cm", &is_MBDNS_tight_vtx10cm);

  std::vector<IntervalRegion> regions = {
    {"h_interval_lt23", "interval < 23", "interval < 23", kBlack, 1, 0, 23, false},
    {"h_interval_23to30", "23 #leq interval < 30", "23 #leq interval < 30", kRed + 1, 1, 23, 30, false},
    {"h_interval_30to40", "30 #leq interval < 40", "30 #leq interval < 40", kBlue + 1, 1, 30, 40, false},
    {"h_interval_40to50", "40 #leq interval < 50", "40 #leq interval < 50", kGreen + 2, 1, 40, 50, false},
    {"h_interval_50to60", "50 #leq interval < 60", "50 #leq interval < 60", kOrange + 7, 1, 50, 60, false},
    {"h_interval_60to70", "60 #leq interval < 70", "60 #leq interval < 70", kMagenta + 1, 1, 60, 70, false},
    {"h_interval_70to80", "70 #leq interval < 80", "70 #leq interval < 80", kCyan + 2, 1, 70, 80, false},
    {"h_interval_ge80", "80 #leq interval", "80 #leq interval", kGray + 2, 2, 80, 999999999, true}
  };

  std::vector<TH1D*> histograms;
  histograms.reserve(regions.size());
  std::vector<Long64_t> filled_event_counts(regions.size(), 0);

  for (const auto& region : regions) {
    TH1D* hist = new TH1D(
      region.hist_name.c_str(),
      "Corrected multiplicity by event interval;Corrected multiplicity;Entries (normalized)",
      5000,
      0,
      20000
    );
    hist->SetDirectory(nullptr);
    hist->SetLineColor(region.color);
    hist->SetMarkerColor(region.color);
    hist->SetLineStyle(region.line_style);
    hist->SetLineWidth(2);
    histograms.push_back(hist);
  }

  const Long64_t n_entries = tree->GetEntries();
  std::cout << "Entries = " << n_entries << std::endl;

  for (Long64_t i = 0; i < n_entries; ++i) {
    if (i % 100000 == 0) {
      std::cout << "Processing entry " << i << std::endl;
    }

    tree->GetEntry(i);

    if (!is_MBDNS_tight_vtx10cm) {
      continue;
    }

    const int corrected_multiplicity = n_hit_inner + n_hit_outer + num_of_carried_hits;

    for (std::size_t iregion = 0; iregion < regions.size(); ++iregion) {
      if (!IsInRegion(regions[iregion], interval_from_previous)) {
        continue;
      }

      bool should_fill = true;
      if (iregion <= 4 && num_of_carried_hits <= 0) {
        should_fill = false;
      }

      if (should_fill) {
        histograms[iregion]->Fill(corrected_multiplicity);
        filled_event_counts[iregion]++;
      }
      break;
    }
  }

  std::vector<TH1D*> rebinned_histograms;
  rebinned_histograms.reserve(histograms.size());
  double maximum = 0.0;

  for (std::size_t iregion = 0; iregion < histograms.size(); ++iregion) {
    TH1D* rebinned = static_cast<TH1D*>(
      histograms[iregion]->Rebin(50, Form("%s_rebinned", histograms[iregion]->GetName()))
    );
    rebinned->SetDirectory(nullptr);
    rebinned->SetLineColor(histograms[iregion]->GetLineColor());
    rebinned->SetMarkerColor(histograms[iregion]->GetMarkerColor());
    rebinned->SetLineStyle(histograms[iregion]->GetLineStyle());
    rebinned->SetLineWidth(histograms[iregion]->GetLineWidth());

    const double integral = rebinned->Integral();
    if (integral > 0) {
      rebinned->Scale(1.0 / integral);
    }

    if (rebinned->GetMaximum() > maximum) {
      maximum = rebinned->GetMaximum();
    }

    rebinned_histograms.push_back(rebinned);
  }

  TCanvas* canvas = new TCanvas("c_corrected_multiplicity_by_interval", "Corrected multiplicity by interval", 900, 700);
  canvas->cd();
  canvas->SetLogy();

  TLegend* legend = new TLegend(0.52, 0.55, 0.90, 0.93);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  legend->SetTextSize(0.028);

  for (std::size_t iregion = 0; iregion < rebinned_histograms.size(); ++iregion) {
    rebinned_histograms[iregion]->SetMinimum(1e-5);
    rebinned_histograms[iregion]->SetMaximum(maximum * 50.0);
    rebinned_histograms[iregion]->GetXaxis()->SetRangeUser(0, 20000);
    rebinned_histograms[iregion]->GetXaxis()->SetNdivisions(107);
    rebinned_histograms[iregion]->GetYaxis()->SetTitleOffset(1.4);

    const char* draw_option = (iregion == 0) ? "hist" : "hist same";
    rebinned_histograms[iregion]->Draw(draw_option);

    const std::string legend_text = Form("%s (N=%lld)", regions[iregion].draw_label.c_str(), filled_event_counts[iregion]);
    legend->AddEntry(rebinned_histograms[iregion], legend_text.c_str(), "l");
  }

  legend->Draw();

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.035);
  latex.DrawLatex(0.18, 0.92, "Run 75519, halfladder-cutted tree");
  latex.DrawLatex(0.18, 0.87, "MBDNS tight, |vtx| < 10 cm");
  // latex.SetTextSize(0.028);
  latex.DrawLatex(0.18, 0.77, "For interval < 60, only carryover");
  latex.DrawLatex(0.18, 0.72, " events are filled");

  canvas->Print(output_filename.c_str());

  file->Close();
  delete file;
  delete legend;
  delete canvas;

  for (auto* hist : rebinned_histograms) {
    delete hist;
  }
  for (auto* hist : histograms) {
    delete hist;
  }
}

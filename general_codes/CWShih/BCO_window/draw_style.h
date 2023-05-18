void Characterize_Pad (TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
{
	if (setgrid_bool == true) {pad -> SetGrid (1, 1);}
	pad -> SetLeftMargin   (left);
	pad -> SetRightMargin  (right);
	pad -> SetTopMargin    (top);
	pad -> SetBottomMargin (bottom);
    pad -> SetTicks(1,1);
    if (set_logY == true)
    {
        pad -> SetLogy (1);
    }
	
}

void Characterize_Rate1F (TH1F *hist,  int hcolour)
{
	float ratio = 7.0/3.0;
	hist -> SetTitle       ("");
	hist -> SetLineColor   (hcolour);
	hist -> SetMarkerColor (hcolour);
	hist -> SetMarkerStyle (20);
	hist -> SetMarkerSize  (0.8);
	hist -> SetFillColor   (hcolour);

	hist -> GetXaxis() -> SetTitleSize   (0.052*ratio);
	hist -> GetXaxis() -> SetTitleOffset (0.940);
	hist -> GetXaxis() -> SetLabelSize   (0.042*ratio);
	hist -> GetXaxis() -> SetLabelOffset (0.004*ratio);

	hist -> GetYaxis() -> SetTitle       ("Data/MC");
	hist -> GetYaxis() -> SetTitleSize   (0.052*ratio);
	hist -> GetYaxis() -> SetTitleOffset (0.360);
	hist -> GetYaxis() -> SetLabelSize   (0.042*ratio);
	hist -> GetYaxis() -> SetLabelOffset (0.006);
	hist -> GetYaxis() -> SetRangeUser   (0.4, 3.2);
	hist -> GetYaxis() -> SetNdivisions  (505);

    hist -> SetStats(0);
}

void Characterize_Hist1F (TH1F *hist,  int statsbox = 0, TString color_ID = "#1A3947", bool set_logY = false, bool do_comp = false, TString title = "", TString X_title = "", TString Y_title = "", bool X_label_less = false, bool Y_label_far = false )
{
	float ratio = 1.;
	hist -> SetTitle       ("");

	hist -> SetLineColor   (TColor::GetColor(color_ID));
    hist -> SetLineWidth   (3);

	hist -> SetMarkerColor (TColor::GetColor(color_ID));
	hist -> SetMarkerStyle (20);
	hist -> SetMarkerSize  (0.8);
	// hist -> SetFillColor   (TColor::GetColor(color_ID));

	hist -> GetXaxis() -> SetTitleSize   (0.05);
	hist -> GetXaxis() -> SetTitleOffset (0.8);

	hist -> GetXaxis() -> SetLabelSize   (0.042*ratio);

    double X_label_offset = (do_comp) ? 0.01 : 0.004;
    hist -> GetXaxis() -> SetLabelOffset (X_label_offset*ratio);
    

	// hist -> GetYaxis() -> SetTitle       ("Data/MC");
	hist -> GetYaxis() -> SetTitleSize   (0.05);
    
    // double amount_of_y_offset = (set_logY) ? 1.5 : 0.94;
    double amount_of_y_offset = (set_logY) ? 0.94 : 0.94;
	hist -> GetYaxis() -> SetTitleOffset (amount_of_y_offset);

    if (Y_label_far == true)
    {
        hist -> GetYaxis() -> SetTitleOffset (1.5);
    }

	hist -> GetYaxis() -> SetLabelSize   (0.042*ratio);
	hist -> GetYaxis() -> SetLabelOffset (0.006);
	// hist -> GetYaxis() -> SetRangeUser   (0.4, 3);

    hist -> SetTitle(title);
    hist -> GetXaxis() -> SetTitle(X_title);
    hist -> GetYaxis() -> SetTitle(Y_title);

	hist -> GetYaxis() -> SetNdivisions  (505);

    if (X_label_less == true)
    {
        hist -> GetXaxis() -> SetNdivisions  (505);
    }


    if (statsbox == 0) {hist -> SetStats(0);}
}



// note : for the comparison of data and MC
// note : titles_vec[0] : x
// note : titles_vec[1] : y
void dataMC_comp (TH1F* hist_data, TH1F* hist_MC, TString folder_direction, TString plot_name, vector<TString> titles_vec ,bool linear_or_log = false, bool statsbox_bool = false)
{
    TCanvas * c3 = new TCanvas("c3","c3",850,800);
    c3 -> cd();

    // note : 0) is for SetGrid
    // note : 0,0) is for logY
    TPad *pad_obj = new TPad(Form("pad_obj"), "", 0.0, 0.30, 1.0, 1.0);
    Characterize_Pad(pad_obj, 0.15, 0.1, 0.1, 0.016 , 0, 0);
    pad_obj -> Draw();

    TPad *pad_ratio = new TPad(Form("pad_ratio"), "", 0.0, 0.0, 1.0, 0.30);
    Characterize_Pad(pad_ratio, 0.15, 0.1, 0.02, 0.350, 0, 1);
    pad_ratio -> Draw();

    TLegend *legend1 = new TLegend (0.7, 0.7, 0.85, 0.85);
    legend1 -> SetBorderSize(0);
	legend1 -> SetTextSize (0.050);
	// legend1 -> SetNColumns (4);

    if (linear_or_log == false) // note : linear
    {
        plot_name += "_Linear";
    }
    else if (linear_or_log == true) // note : log
    {   
        pad_obj -> SetLogy();
        plot_name += "_Log";
    }

    Characterize_Hist1F(hist_data,0,"#1A3947",linear_or_log,true ,"",titles_vec[0],titles_vec[1]);
    // hist_data -> GetXaxis() -> SetTitle(titles_vec[0]);
    // hist_data -> GetYaxis() -> SetTitle(titles_vec[1]);
    // hist_data -> GetYaxis() -> SetRangeUser(0,1);

    Characterize_Hist1F(hist_MC,0,"#A08144",linear_or_log,true, "",titles_vec[0],titles_vec[1]);
    // hist_MC -> GetXaxis() -> SetTitle(titles_vec[0]);
    // hist_MC -> GetYaxis() -> SetTitle(titles_vec[1]);
    // hist_MC -> SetTitle(titles_vec[2]);
    // hist_MC -> GetYaxis() -> SetRangeUser(0,1);

    if (statsbox_bool == false)
    {
        hist_MC -> SetStats(0); // note : remove the box
        hist_data -> SetStats(0);
    } 

    // note : normalize
    hist_data->Scale(1./hist_data->Integral(-1,-1));
    hist_MC->Scale(1./hist_MC->Integral(-1,-1));

    legend1 -> AddEntry (hist_data, Form("Data"),  "pl");
    legend1 -> AddEntry (hist_MC, Form("MC"),  "f");
    
     
    double Y_axis_max = (linear_or_log) ? 10 : 1;
    hist_MC -> SetMaximum(Y_axis_max);


    TH1F * hist_ratio = (TH1F*)hist_data -> Clone(); 
    hist_ratio -> Divide (hist_MC);

    Characterize_Rate1F(hist_ratio,1);
    
    pad_obj -> cd();
    hist_MC -> Draw("hist");
    hist_data -> Draw("ep same");
    legend1 -> Draw("same");

    pad_ratio -> cd();
    hist_ratio -> Draw("ep");

    // TString output_plot_name = plot_name.ReplaceAll(" ","_");
    // output_plot_name = output_plot_name.ReplaceAll(",","");

    c3 -> Print( Form("%s/%s.pdf",folder_direction.Data(),plot_name.Data()) );
    c3 -> Clear();
    
}

// note : ADC combination factor cancellation
void binbin_uniform(TH1F * input_hist, vector<double>binbin_factor)
{

    input_hist -> Scale(1. / input_hist -> Integral(-1,-1));

    for (int i = 0; i < input_hist -> GetNbinsX(); i++)
    {
        if (input_hist -> GetBinContent(i+1) == 0 || binbin_factor[i] == 0) continue;

        input_hist -> SetBinContent(i+1, input_hist -> GetBinContent(i+1) / binbin_factor[i]);
    }

    input_hist -> Scale(1. / input_hist -> Integral(-1,-1));
}

// note : multi-plots comparison
// note : titles_vec[0] : x
// note : titles_vec[1] : y
void multi_plot_comp (vector<TH1F*>input_hist_1D, vector<TString>hist_color_array, vector<TString>Legend_array, TString folder_direction, TString plot_name, vector<TString> titles_vec ,bool set_logY = false, bool statsbox_bool = false)
{
    TCanvas * c3 = new TCanvas("c3","c3",850,800);
    c3 -> cd();

    // note : 0) is for SetGrid
    // note : 0,0) is for logY
    TPad *pad_obj = new TPad(Form("pad_obj"), "", 0.0, 0.0, 1.0, 1.0);
    Characterize_Pad(pad_obj, 0.15, 0.1, 0.1, 0.12, 0, 0);
    pad_obj -> Draw();


    TLegend *legend1 = new TLegend (0.6, 0.79-0.03*input_hist_1D.size(), 0.85, 0.85);
    legend1 -> SetBorderSize(0);
	legend1 -> SetTextSize (0.050);
	// legend1 -> SetNColumns (4);

    if (set_logY == false) // note : linear
    {
        plot_name += "_Linear";
    }
    else if (set_logY == true) // note : log
    {   
        pad_obj -> SetLogy();
        plot_name += "_Log";
    }

    for (int i = 0; i < input_hist_1D.size(); i++)
    {
        Characterize_Hist1F(input_hist_1D[i],0,hist_color_array[i],set_logY,false,"",titles_vec[0],titles_vec[1],false,true);    
        // input_hist_1D[i] -> GetXaxis() -> SetTitle(titles_vec[0]);
        // input_hist_1D[i] -> GetYaxis() -> SetTitle(titles_vec[1]);
        // input_hist_1D[i] -> GetYaxis() -> SetRangeUser(0,1);
    }

    // Characterize_Hist1F(hist_data,0,"#1A3947",set_logY);
    // hist_data -> GetXaxis() -> SetTitle(titles_vec[0]);
    // hist_data -> GetYaxis() -> SetTitle(titles_vec[1]);
    // // hist_data -> GetYaxis() -> SetRangeUser(0,1);

    // Characterize_Hist1F(hist_MC,0,"#A08144",set_logY);
    // hist_MC -> GetXaxis() -> SetTitle(titles_vec[0]);
    // hist_MC -> GetYaxis() -> SetTitle(titles_vec[1]);
    // // hist_MC -> SetTitle(titles_vec[2]);
    // // hist_MC -> GetYaxis() -> SetRangeUser(0,1);

    if (statsbox_bool == false)
    {

        for (int i = 0; i < input_hist_1D.size(); i++)
        {
            input_hist_1D[i]-> SetStats(0);
        }
    } 

    // note : normalize
    for (int i = 0; i < input_hist_1D.size(); i++)
    {
        input_hist_1D[i]->Scale(1./input_hist_1D[i]->Integral(-1,-1));
        legend1 -> AddEntry (input_hist_1D[i], Form("%s",Legend_array[i].Data()),  "f");
        // cout<<"get line color : "<<input_hist_1D[i]->GetLineColor()<<endl;
    }
     
    double Y_axis_max = (set_logY) ? 10 : 1;
    input_hist_1D[0] -> SetMaximum(Y_axis_max);


    
    pad_obj -> cd();
    for (int i = 0; i < input_hist_1D.size(); i++)
    {
        if (i == 0)
        {
            input_hist_1D[i] -> Draw("hist");        
        }
        else 
        {
            input_hist_1D[i] -> Draw("hist same");
        }
    }

    legend1 -> Draw("same");


    // TString output_plot_name = plot_name.ReplaceAll(" ","_");
    // output_plot_name = output_plot_name.ReplaceAll(",","");

    c3 -> Print( Form("%s/%s.pdf",folder_direction.Data(),plot_name.Data()) );
    c3 -> Clear();
    
}
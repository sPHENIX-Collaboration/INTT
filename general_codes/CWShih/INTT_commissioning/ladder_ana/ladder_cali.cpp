#include "ladder_cali.h"

// note : if the number of cuts increase, 
// note : modify : gold_profile(0xF), std::bitset<N>, check_profile(0xF)

ladder_cali::ladder_cali(TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree)
    :ladder_name(ladder_name),
    ROC_index(ROC_index),
    ladder_port(ladder_port),
    Felix_ch(Felix_ch),
    threshold(threshold),
    ampl_cut(ampl_cut),
    used_sigma(used_sigma),
    classify_base(classify_base),
    output_basic(output_basic),
    output_dead_channel_tree(output_dead_channel_tree),
    draw_plot_range({1,2,3}),
    gold_profile(0xF), // note : 0xF :  1111
    check_porifle(0xF), // note : 0xF : 1111, the half-entry profile is not tested.
    width_check_mode(0),
    unfit_region(11),
    ch_entry_NC_range(1000) // todo : the noisy channel check
    // N_check(4)
    {
        cout<<"initializatione start"<<endl;

        memset(ampl_cut_count, 0, sizeof(ampl_cut_count));
        bad_ch_list.clear();

        // note : the parameters for the fitting
        ampl_max = 63;
        total_pulses = 640;
        default_true_threshold = threshold + 10;
        default_width = 3;
        max_turn_on_limit = 7.5;

        // note : for the port map
        port_map["A1"] = 0;
        port_map["A2"] = 1;
        port_map["A3"] = 2;

        port_map["B1"] = 3;
        port_map["B2"] = 4;
        port_map["B3"] = 5;

        port_map["C1"] = 6;
        port_map["C2"] = 7;
        port_map["C3"] = 8;
        
        port_map["D1"] = 9;
        port_map["D2"] = 10;
        port_map["D3"] = 11;

        for (int i = 0; i < 26; i++) // note : chip
        {
           for (int j = 0; j < 128; j++) // note : channel
           {
                ch_profile[i][j] = gold_profile;

                ampl_total[i][j] = new TH1F ("","",70,0,70);
                ampl_total[i][j] -> SetTitle(Form("%s, U%i-ch%i", ladder_name.Data(), i+1, j));
                ampl_total[i][j] -> GetXaxis() -> SetTitle("ampl");
                ampl_total[i][j] -> GetYaxis() -> SetTitle("Entry");
                
                // note : the error function
                // todo : set the default parameters before fitting
                ampl_total_fitter[i][j] = new TF1("","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);

                // todo : make the plot looks better
                // todo : TGraphErrors haven't been declared yet.

                for (int k = 0; k < 8; k++) // note : 8 is based on the number of bit of FPHX chip
                {
                    ampl_single[i][j][k] = new TH1F ("","",70,0,70);
                    ampl_single[i][j][k] -> SetTitle(Form("%s, U%i-ch%i, adc%i", ladder_name.Data(), i+1, j, k));
                    ampl_single[i][j][k] -> GetXaxis() -> SetTitle("ampl");
                    ampl_single[i][j][k] -> GetYaxis() -> SetTitle("Entry");

                    // note : the fitter, gaus function
                    // todo : to make things quicker, command out this line 
                    // ampl_single_fitter[i][j][k] = new TF1("","gaus",0,70); 
                }

                // linearity [i][j]; // note : TGraphError
                // width_consistency [i][j]; // note : TGraphError
           } 
        }

        // note : The ROC provides the artificial pulses with the same amplitude 10 times.
        // note : 64 * 10 = 640. so the Maximum of the distribution in X axis should be 640 - threshold * 10.
        ch_entry = new TH1F("",Form("%s, channel entry dist",ladder_name.Data()),100,0,total_pulses - threshold*7);
        ch_entry -> GetXaxis() -> SetTitle("Channel entry");
        ch_entry -> GetYaxis() -> SetTitle("Events");

        // note : the turn on position of the ampl_total hist
        // todo : the minimum is defined by the threshold
        ampl_total_turn_on = new TH1F("",Form("%s, threshold distribution",ladder_name.Data()), 75 - (threshold - 10), threshold - 10, 75);
        ampl_total_turn_on -> GetXaxis() -> SetTitle("ampl");
        ampl_total_turn_on -> GetYaxis() -> SetTitle("Events");

        // note : the width of the turn on position of the ampl_total hist
        ampl_total_width = new TH1F("",Form("%s, Turn ON width",ladder_name.Data()), 100, 0, 20);
        ampl_total_width -> GetXaxis() -> SetTitle("ampl");
        ampl_total_width -> GetYaxis() -> SetTitle("Events");

        ch_entry_ampl_cut = new TH1F("",Form("%s, channel entry distribution (ampl > %i)",ladder_name.Data() ,ampl_cut),100,0,(total_pulses - ampl_cut*8) + 50);
        ch_entry_ampl_cut -> GetXaxis() -> SetTitle("Channel entry");
        ch_entry_ampl_cut -> GetYaxis() -> SetTitle("Events");

        cout<<"initializatione done"<<endl;
           
    }


// note : the constructor with an additional variable. 
ladder_cali::ladder_cali(TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree, int width_check_mode)
    :ladder_name(ladder_name),
    ROC_index(ROC_index),
    ladder_port(ladder_port),
    Felix_ch(Felix_ch),
    threshold(threshold),
    ampl_cut(ampl_cut),
    used_sigma(used_sigma),
    classify_base(classify_base),
    output_basic(output_basic),
    output_dead_channel_tree(output_dead_channel_tree),
    draw_plot_range({1,2,3}),
    gold_profile(0xF), // note : 0x1F : 1111
    check_porifle(0xF), // note : 0xF : 1111, the half-entry profile is not tested.
    width_check_mode(width_check_mode),
    unfit_region(11),
    ch_entry_NC_range(1000) // todo : the noist channel check
    // N_check(4)
    {
        cout<<"initializatione start"<<endl;

        memset(ampl_cut_count, 0, sizeof(ampl_cut_count));
        bad_ch_list.clear();

        // note : the parameters for the fitting
        ampl_max = 63;
        total_pulses = 640;
        default_true_threshold = threshold + 10;
        default_width = 3;
        max_turn_on_limit = 7.5;

        // note : for the port map
        port_map["A1"] = 0;
        port_map["A2"] = 1;
        port_map["A3"] = 2;

        port_map["B1"] = 3;
        port_map["B2"] = 4;
        port_map["B3"] = 5;

        port_map["C1"] = 6;
        port_map["C2"] = 7;
        port_map["C3"] = 8;
        
        port_map["D1"] = 9;
        port_map["D2"] = 10;
        port_map["D3"] = 11;

        for (int i = 0; i < 26; i++) // note : chip
        {
           for (int j = 0; j < 128; j++) // note : channel
           {
                ch_profile[i][j] = gold_profile;

                ampl_total[i][j] = new TH1F("","",70,0,70);
                ampl_total[i][j] -> SetTitle(Form("%s, U%i-ch%i", ladder_name.Data(), i+1, j));
                ampl_total[i][j] -> GetXaxis() -> SetTitle("ampl");
                ampl_total[i][j] -> GetYaxis() -> SetTitle("Entry");
                
                // note : the error function
                // todo : set the default parameters before fitting
                ampl_total_fitter[i][j] = new TF1("","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);

                if (width_check_mode == 1)
                {
                    // note : currently is not used. We only check the width of the single adc dist
                    // ampl_single_0_2[i][j] = new TH1F("","",70,0,70);
                    // ampl_single_0_2[i][j] -> SetTitle(Form("adc0+adc2, %s, U%i-ch%i", ladder_name.Data(), i+1, j));
                    // ampl_single_0_2[i][j] -> GetXaxis() -> SetTitle("ampl");
                    // ampl_single_0_2[i][j] -> GetYaxis() -> SetTitle("Entry");

                    // note : only create the fitter for adc0 and adc2
                    ampl_single_fitter[i][j][0] = new TF1("","gaus",1,69); // note : currently just first avoid the ampl0 bin
                    ampl_single_fitter[i][j][1] = new TF1("","gaus",1,69); // note : currently just first avoid the ampl0 bin
                    ampl_single_fitter[i][j][2] = new TF1("","gaus",1,69); // note : currently just first avoid the ampl0 bin
                }


                // todo : make the plot looks better
                // todo : TGraphErrors haven't been declared yet.

                for (int k = 0; k < 8; k++) // note : 8 is based on the number of bit of FPHX chip
                {
                    ampl_single[i][j][k] = new TH1F("","",70,0,70);
                    ampl_single[i][j][k] -> SetTitle(Form("%s, U%i-ch%i, adc%i", ladder_name.Data(), i+1, j, k));
                    ampl_single[i][j][k] -> GetXaxis() -> SetTitle("ampl");
                    ampl_single[i][j][k] -> GetYaxis() -> SetTitle("Entry");

                    // note : the fitter, gaus function
                    // todo : to make things quicker, command out this line 
                    // ampl_single_fitter[i][j][k] = new TF1("","gaus",0,70); 
                }

                // linearity [i][j]; // note : TGraphError
                // width_consistency [i][j]; // note : TGraphError
           } 
        }

        // note : The ROC provides the artificial pulses with the same amplitude 10 times.
        // note : 64 * 10 = 640. so the Maximum of the distribution in X axis should be 640 - threshold * 10.
        ch_entry = new TH1F("",Form("%s, channel entry dist",ladder_name.Data()),100,0,total_pulses - threshold*7);
        ch_entry -> GetXaxis() -> SetTitle("Channel entry");
        ch_entry -> GetYaxis() -> SetTitle("Events");

        // note : the turn on position of the ampl_total hist
        // todo : the minimum is defined by the threshold
        ampl_total_turn_on = new TH1F("",Form("%s, threshold distribution",ladder_name.Data()), 75 - (threshold - 10), threshold - 10, 75);
        ampl_total_turn_on -> GetXaxis() -> SetTitle("ampl");
        ampl_total_turn_on -> GetYaxis() -> SetTitle("Events");

        // note : the width of the turn on position of the ampl_total hist
        ampl_total_width = new TH1F("",Form("%s, Turn ON width",ladder_name.Data()), 100, 0, 20);
        ampl_total_width -> GetXaxis() -> SetTitle("ampl");
        ampl_total_width -> GetYaxis() -> SetTitle("Events");

        ch_entry_ampl_cut = new TH1F("",Form("%s, channel entry distribution (ampl > %i)",ladder_name.Data() ,ampl_cut),100,0,(total_pulses - ampl_cut*8) + 50);
        ch_entry_ampl_cut -> GetXaxis() -> SetTitle("Channel entry");
        ch_entry_ampl_cut -> GetYaxis() -> SetTitle("Events");

        if (width_check_mode == 1)
        {
            ampl_single_0_width = new TH1F("",Form("%s, width of ampl dist.(adc0)", ladder_name.Data()),100,0,15);
            ampl_single_0_width -> GetXaxis() -> SetTitle("ampl");
            ampl_single_0_width -> GetYaxis() -> SetTitle("Events");   

            ampl_single_1_width = new TH1F("",Form("%s, width of ampl dist.(adc1)", ladder_name.Data()),100,0,15);
            ampl_single_1_width -> GetXaxis() -> SetTitle("ampl");
            ampl_single_1_width -> GetYaxis() -> SetTitle("Events");

            ampl_single_2_width = new TH1F("",Form("%s, width of ampl dist.(adc2)", ladder_name.Data()),100,0,15);
            ampl_single_2_width -> GetXaxis() -> SetTitle("ampl");
            ampl_single_2_width -> GetYaxis() -> SetTitle("Events");   
        }

        cout<<"initializatione done"<<endl;
           
    }




TString ladder_cali::GetLadderName()
{
    return ladder_name; 
}

TString ladder_cali::GetLadderPort()
{
    return ladder_port; 
}

int ladder_cali::GetFelixCh()
{
    return Felix_ch; 
}

int ladder_cali::GetThreshold()
{
    return threshold; 
}

int ladder_cali::GetClassifyBase()
{
    return classify_base; 
}


void ladder_cali::SetAmplCut(int input_ampl_cut)
{
    ampl_cut = input_ampl_cut;
}

int ladder_cali::GetAmplCut()
{
    return ampl_cut;
}

void ladder_cali::SetAdcPickL(int input_adc_pick_l)
{
    adc_pick_l = input_adc_pick_l;
}

int ladder_cali::GetAdcPickL()
{
    return adc_pick_l;
}

void ladder_cali::SetAdcPickR(int input_adc_pick_r)
{   
    adc_pick_r = input_adc_pick_r;
}

int ladder_cali::GetAdcPickR()
{
    return adc_pick_r;
}

void ladder_cali::Fill(int chip_id, int chan_id, int ampl, int adc)
{
    if (chip_id > 0 && chip_id < 27 && chan_id > -1 && chan_id < 128)
    {
        ampl_total[chip_id - 1][chan_id] -> Fill(ampl);
        ampl_single[chip_id - 1][chan_id][adc] -> Fill(ampl);

        if (ampl > ampl_cut)
        {
            ampl_cut_count[chip_id - 1][chan_id] += 1;
        }
    }
    else
    {
        cout << " the weird event : " << " : " << ampl << " : " << adc << " : " << chip_id << " : " << chan_id << endl;
    }
}

void ladder_cali::Fill_adc_0_2()
{
    return 0;
}

void ladder_cali::Fit()
{
    if (width_check_mode == 1)
    {
        for (int i = 0; i < 26; i++) // note : ampl_single
        {
            for (int j = 0; j < 128; j++)
            {   
                //todo : the fit range may have to be tuned
                if(ampl_single[i][j][0] -> GetEntries() < unfit_region) // note : fitting for adc0
                {
                    ampl_single_fitter[i][j][0] -> SetParameter(2,0);
                }
                else 
                {
                    ampl_single[i][j][0] -> Fit(ampl_single_fitter[i][j][0],"NQ","",threshold,69);
                }

                if (ampl_single[i][j][1] -> GetEntries() < unfit_region) // note : fitting for adc1
                {
                    ampl_single_fitter[i][j][1] -> SetParameter(2,0);
                }
                else 
                {
                    ampl_single[i][j][1] -> Fit(ampl_single_fitter[i][j][1],"NQ","",threshold,69);
                }                
                
                if (ampl_single[i][j][2] -> GetEntries() < unfit_region) // note : fitting for adc2
                {
                    ampl_single_fitter[i][j][2] -> SetParameter(2,0);
                }
                else 
                {
                    ampl_single[i][j][2] -> Fit(ampl_single_fitter[i][j][2],"NQ","",threshold,69);
                }

            }
        }    
    }
    
    
    // todo : 640 is the total number of pulses each channel receives.

    // note : ampl_total
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            // todo : the entry of each channel should not be higher than 640, right ? 
            if (ampl_total[i][j] -> GetEntries() < unfit_region || ampl_total[i][j] -> GetEntries() > ch_entry_NC_range)
			{
				ampl_total_fitter[i][j] -> SetParameter(0,5); // note :turn on
				ampl_total_fitter[i][j] -> SetParameter(1,0); // note :slope
				ampl_total_fitter[i][j] -> SetParameter(2,1);// note :height
			}
            // note : full entry, (default_true_threshold + 10) to have more room.
			// else if (ampl_total[i][j] -> GetEntries() > ( total_pulses - ( default_true_threshold + 10 ) * 10 )) 
            // else if (ampl_cut_count[i][j] / (ampl_max - ampl_cut) > 7)
			// {
            //     ampl_total_fitter[i][j] -> SetParameters(default_true_threshold,3.5, 10); 
            //     ampl_total_fitter[i][j] -> SetParLimits(0,threshold,threshold+20);
			// 	   ampl_total_fitter[i][j] -> SetParLimits(1,0.2,10.5);
			// 	   ampl_total_fitter[i][j] -> SetParLimits(2,9.9,10.5);
			// 	   // note : [0] turn_on
			// 	   // note : [1] slope
			// 	   // note : [2] height 	
                
            //     // todo : set the minimum at threshold, to ignore the noise part
            //     ampl_total[i][j]->Fit(ampl_total_fitter[i][j], "NQ", "", threshold, 70);
            // }


            // note : let's try a new thing, define the height based on the entry
            else 
            {
                ampl_total_fitter[i][j] -> SetParameters(default_true_threshold, 3.5, double(ampl_cut_count[i][j]) / double(ampl_max - ampl_cut)); 
                ampl_total_fitter[i][j] -> SetParLimits(0,threshold,threshold+30);
                ampl_total_fitter[i][j] -> SetParLimits(1, 0.15, max_turn_on_limit);
                ampl_total_fitter[i][j] -> SetParLimits(2, double(ampl_cut_count[i][j]) / double(ampl_max - ampl_cut) - 2 ,double(ampl_cut_count[i][j]) / double(ampl_max - ampl_cut) + 2);
                // note : [0] turn_on
                // note : [1] slope
                // note : [2] height 

                ampl_total[i][j]->Fit(ampl_total_fitter[i][j], "NQ", "", threshold, 70);
            }

            
        }
    }
}

void ladder_cali::Overall_info()
{

    ch_entry_fitter = new TF1("","gaus",ch_entry -> GetXaxis() -> GetXmin(), ch_entry -> GetXaxis() -> GetXmax());
    ampl_total_turn_on_fitter = new TF1("","gaus",ampl_total_turn_on -> GetXaxis() -> GetXmin(), ampl_total_turn_on -> GetXaxis() -> GetXmax());
    ampl_total_width_fitter = new TF1("","gaus",ampl_total_width -> GetXaxis() -> GetXmin(), ampl_total_width -> GetXaxis() -> GetXmax());
    ch_entry_fitter -> SetNpx(10000);
    ampl_total_turn_on_fitter -> SetNpx(10000);
    ampl_total_width_fitter -> SetNpx(10000);
    if (width_check_mode == 1) 
    {
        ampl_single_0_width_fitter = new TF1("","gaus",ampl_single_0_width -> GetXaxis() -> GetXmin(), ampl_single_0_width -> GetXaxis() -> GetXmax());
        ampl_single_1_width_fitter = new TF1("","gaus",ampl_single_1_width -> GetXaxis() -> GetXmin(), ampl_single_1_width -> GetXaxis() -> GetXmax());
        ampl_single_2_width_fitter = new TF1("","gaus",ampl_single_2_width -> GetXaxis() -> GetXmin(), ampl_single_2_width -> GetXaxis() -> GetXmax());

        ampl_single_0_width_fitter -> SetNpx(10000);
        ampl_single_1_width_fitter -> SetNpx(10000);
        ampl_single_2_width_fitter -> SetNpx(10000);

    }

    // note : this is judged by the data, I meant the threshold + 10, It maybe due to the pulse amplitude attenuation.
    ch_entry_fitter -> SetParameter(1, total_pulses - default_true_threshold * 10);
    ampl_total_turn_on_fitter -> SetParameter(1, default_true_threshold);
    ampl_total_width_fitter -> SetParameter(1, default_width); // note : the value 3 is determined by the data.
    // if (width_check_mode == 1) {  }



    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            ampl_total_turn_on -> Fill( ampl_total_fitter[i][j] -> GetParameter(0) );
            ampl_total_width   -> Fill( ampl_total_fitter[i][j] -> GetParameter(1) );
            ch_entry           -> Fill( ampl_total[i][j] -> GetEntries() );
            ch_entry_ampl_cut  -> Fill( ampl_cut_count[i][j] );
            if (width_check_mode == 1) 
            { 
                ampl_single_0_width -> Fill(ampl_single_fitter[i][j][0] -> GetParameter(2)); // note : adc 0 
                ampl_single_1_width -> Fill(ampl_single_fitter[i][j][1] -> GetParameter(2)); // note : adc 1
                ampl_single_2_width -> Fill(ampl_single_fitter[i][j][2] -> GetParameter(2)); // note : adc 2
                
                // if (ampl_single_fitter[i][j][2] -> GetParameter(2) > 5.5) {cout<<"ADC2 wide distribution : "<<i+1<<" "<<j<<" "<<ampl_single_fitter[i][j][2] -> GetParameter(2)<<endl;}
            }
        }
    }

    // todo : for ther ampl_total_width, set the Fit minimum at 1 in order to have to conflict with the Fit-parameters setting for the special cases, line 197.
    // todo : tunning the fitting width, the current width is not enough.
    ampl_total_turn_on_fitter -> SetParameter(1, default_true_threshold);
    // note : meaning of the following "if" is to try to classify the possible minority among a empty ladder.
    // note : try to secure the few channel in the empty ladder
    if (ch_entry_ampl_cut -> Integral(3,-1) > 50) 
    {
        ampl_total_turn_on -> Fit(ampl_total_turn_on_fitter,"NQ","",default_true_threshold - 10, default_true_threshold + 25 );
        ampl_total_width   -> Fit(ampl_total_width_fitter,"NQ","", 1, max_turn_on_limit - 0.5); // note : to avoid the conflict with the fitting maximum
        ch_entry           -> Fit(ch_entry_fitter,"NQ","", (total_pulses - ampl_total_turn_on_fitter->GetParameter(1) * 10) - 75, (total_pulses - ampl_total_turn_on_fitter->GetParameter(1) * 10) + 100 );
    }
    else
    {
        ampl_total_turn_on_fitter -> SetParameters(200, default_true_threshold + 1.716785, 1.2636692); // note : (height, mean, width), value from calib_packv5_042523_0143
        ampl_total_width_fitter   -> SetParameters(200, 3.0726692, 0.68432727); // note : (height, mean, width)
        ch_entry_fitter           -> SetParameters(200, (total_pulses - ampl_total_turn_on_fitter->GetParameter(1) * 10), 12.155036); // note : (height, mean, width)
    }
    
    if (width_check_mode == 1) 
    {
        ampl_single_0_width -> Fit(ampl_single_0_width_fitter,"NQ","",1,15);
        ampl_single_1_width -> Fit(ampl_single_1_width_fitter,"NQ","",1,15);
        ampl_single_2_width -> Fit(ampl_single_2_width_fitter,"NQ","",1,15);
    }


    ch_entry_range.first  = ch_entry_fitter -> GetParameter(1) - used_sigma * ch_entry_fitter -> GetParameter(2); 
    ch_entry_range.second = ch_entry_fitter -> GetParameter(1) + used_sigma * ch_entry_fitter -> GetParameter(2);

    // // todo : given by the hist stddev and hist mean
    // ch_entry_ampl_cut_range.first  = ch_entry_ampl_cut -> GetMean() - used_sigma * ch_entry_ampl_cut -> GetStdDev(); 
    // ch_entry_ampl_cut_range.second = ch_entry_ampl_cut -> GetMean() + used_sigma * ch_entry_ampl_cut -> GetStdDev();

    // todo : given by numerical calculation, with the range +- 15
    ch_entry_ampl_cut_range.first  = (ampl_max - ampl_cut) * 10 - 15; 
    ch_entry_ampl_cut_range.second = (ampl_max - ampl_cut) * 10 + 15;
    
    ampl_total_turn_on_range.first  = ampl_total_turn_on_fitter -> GetParameter(1) - used_sigma * ampl_total_turn_on_fitter -> GetParameter(2); 
    ampl_total_turn_on_range.second = ampl_total_turn_on_fitter -> GetParameter(1) + used_sigma * ampl_total_turn_on_fitter -> GetParameter(2);
    
    // ampl_total_width_range.first  = ampl_total_width_fitter -> GetParameter(1) - used_sigma * ampl_total_width_fitter -> GetParameter(2); 
    ampl_total_width_range.first  = 0.1; // note : the steep should be fine, right, which means it has precise ampl measurement.
    ampl_total_width_range.second = ampl_total_width_fitter -> GetParameter(1) + used_sigma * ampl_total_width_fitter -> GetParameter(2);

    // todo : the half entry related cuts are given directly.  
    // note : the cuts for the half-entry checking 
    ch_entry_ampl_cut_HE_range.first  = ((ampl_max - ampl_cut) * 10) / 2. - 15; // note : the method of range determination is same a the ch_entry_ampl_cut_range
    ch_entry_ampl_cut_HE_range.second = ((ampl_max - ampl_cut) * 10) / 2. + 15;

    ampl_total_height_HE_range.first  = 5 - 1.5; // note : the height should be 5, let's try +- 1.5 first
    ampl_total_height_HE_range.second = 5 + 1.5;

    // todo : the DOUBLE entry related cuts are given directly.
    // note : the cuts for the DOUBLE-entry checking 
    ch_entry_ampl_cut_DB_range.first  = ((ampl_max - ampl_cut) * 10) * 2. - 15; // note : the method of range determination is same a the ch_entry_ampl_cut_range
    ch_entry_ampl_cut_DB_range.second = ((ampl_max - ampl_cut) * 10) * 2. + 15;

    ampl_total_height_DB_range.first  = 20 - 1.5; // note : the height should be 20, let's try +- 1.5 first
    ampl_total_height_DB_range.second = 20 + 1.5;

}

void ladder_cali::PrintClassifierPar()
{
    cout<<"============================================== Self calibration mode =================================================="<<endl;
    
    cout<< "ch_entry_range : "<< ch_entry_range.first <<" "<<ch_entry_range.second<<endl;

    cout<< "ch_entry_ampl_cut_range : "<< ch_entry_ampl_cut_range.first <<" "<<ch_entry_ampl_cut_range.second<<endl;

    cout<< "ampl_total_turn_on_range : "<< ampl_total_turn_on_range.first <<" "<<ampl_total_turn_on_range.second<<endl;

    cout<< "ampl_total_width_range : "<< ampl_total_width_range.first <<" "<<ampl_total_width_range.second<<endl;

    cout<< "ampl_total_height_HE_range : "<< ampl_total_height_HE_range.first <<" "<<ampl_total_height_HE_range.second<<endl;

    cout<< "ch_entry_ampl_cut_HE_range : "<< ch_entry_ampl_cut_HE_range.first <<" "<<ch_entry_ampl_cut_HE_range.second<<endl;

    cout<< "ampl_total_height_DB_range : "<< ampl_total_height_DB_range.first <<" "<<ampl_total_height_DB_range.second<<endl;

    cout<< "ch_entry_ampl_cut_DB_range : "<< ch_entry_ampl_cut_DB_range.first <<" "<<ch_entry_ampl_cut_DB_range.second<<endl;

    cout<< "unfit_region (check empty channel) : "<< unfit_region <<endl;

    cout<< "ch_entry_NC_range (check noisy channel) : "<< ch_entry_NC_range <<endl;


}

void ladder_cali::Classifier(TString outside_file_in = "Not used")
{
    
    // note :            1 : 0x01
	// note :           10 : 0x02
	// note :          100 : 0x04
	// note :         1000 : 0x08
	// note :       1 0000 : 0x10
	// note :      10 0000 : 0x20 
	// note :     100 0000 : 0x40 
	// note :    1000 0000 : 0x80 V
    // note :  1 0000 0000 : 0x100
    // note : 10 0000 0000 : 0x200
    
    
    // note :    0 0001 -> ch entry
    // note :    0 0010 -> ch entry ampl cut
    // note :    0 0100 -> turn on
    // note :    0 1000 -> width
    // note :    1 0000 -> half-entry (if the channel has only half entry -> 1, if the channel doesn't have the half-entry issue -> 0)
    // note :   10 0000 -> empty channel, entry < unfit_region. (If the channel is empty -> 1.)
    // note :  100 0000 -> double entry monitor (if the entry of the channel is doubled -> 1.)
    // note : 1000 0000 -> NC, noisy channel (if it's noist -> 1)

    if (classify_base == 0) // note : self-check
    {
        cout<<"classify_base : "<<classify_base<<", self-check"<<endl;
        ladder_cali::PrintClassifierPar();
        cout<<""<<endl;
    }
    else
    {
        cout<<"function not done yet..."<<endl;
        return;
    }

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            
            // note : ch entry
            if ( ampl_total[i][j] -> GetEntries() < ch_entry_range.first || ampl_total[i][j] -> GetEntries() > ch_entry_range.second ) 
            {
                ch_profile[i][j] = ch_profile[i][j]^0x01;
            }
            
            // note : ch entry ampl cut
            if ( ampl_cut_count[i][j] < ch_entry_ampl_cut_range.first || ampl_cut_count[i][j] > ch_entry_ampl_cut_range.second) 
            {
                ch_profile[i][j] = ch_profile[i][j]^0x02;
            }
            else // todo : Set the limit of the Y axis.
            {
                ampl_total[i][j] -> SetMaximum(15);
            }

            // note : turn on
            if ( ampl_total_fitter[i][j] -> GetParameter(0) < ampl_total_turn_on_range.first || ampl_total_fitter[i][j] -> GetParameter(0) > ampl_total_turn_on_range.second ) 
            {
                ch_profile[i][j] = ch_profile[i][j]^0x04;
            }

            // note : width
            if ( ampl_total_fitter[i][j] -> GetParameter(1) < ampl_total_width_range.first || ampl_total_fitter[i][j] -> GetParameter(1) > ampl_total_width_range.second ) 
            {
                ch_profile[i][j] = ch_profile[i][j]^0x08;
            }

            // note : half-entry, check the ch entry < ampl_cut
            // note : half-entry, check the error function fitting height.
            if ( ampl_cut_count[i][j] > ch_entry_ampl_cut_HE_range.first && ampl_cut_count[i][j] < ch_entry_ampl_cut_HE_range.second && ampl_total_fitter[i][j] -> GetParameter(2) > ampl_total_height_HE_range.first && ampl_total_fitter[i][j] -> GetParameter(2) < ampl_total_height_HE_range.second)
            {
                ch_profile[i][j] = ch_profile[i][j] | 0x10; // note : so the golden_profile is 1111, here we use the add method to add the "1 0000", to indicate the confirmation of the half-entry
            }

            // note : check the empty channel
            if (ampl_total[i][j] -> GetEntries() < unfit_region)
            {
                ch_profile[i][j] = ch_profile[i][j] | 0x20; // note : so the golden_profile is 1111, here we use the add method to add the "10 0000", to indicate the confirmation of the "No-entry"
            }

            // note : double-entry, check the ch entry < ampl_cut
            // note : double-entry, check the error function fitting height.
            if ( ampl_cut_count[i][j] > ch_entry_ampl_cut_DB_range.first && ampl_cut_count[i][j] < ch_entry_ampl_cut_DB_range.second && ampl_total_fitter[i][j] -> GetParameter(2) > ampl_total_height_DB_range.first && ampl_total_fitter[i][j] -> GetParameter(2) < ampl_total_height_DB_range.second)
            {
                ch_profile[i][j] = ch_profile[i][j] | 0x40; // note : so the golden_profile is 1111, here we use the add method to add the "100 0000", to indicate the confirmation of the "double-entry"
            }

            // note : noisy channel, check the "ch entry" only
            if( ampl_total[i][j] -> GetEntries() > ch_entry_NC_range)
            {
                ch_profile[i][j] = ch_profile[i][j] | 0x80; // note : so the golden_profile is 1111, here we use the add method to add the "1000 0000", to indicate the confirmation of the "Noisy channel"
            }

        }
    }

    bad_ch_info bad_ch_box;

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            // note : currently, we exclude the half-entry profile (1 0000) for the classification
            // note : currently, we exclude the Zero-entry profile (10 0000) for the classification
            // note : currently, we exclude the double-entry profile (100 0000) for the classification
            if ( (ch_profile[i][j] & check_porifle) != check_porifle ) // note : the "& check_profile" is a way to make sure the size of the data word.
            {
                
                bad_ch_box.chip = i + 1;
                bad_ch_box.chan = j; 
                bad_ch_box.ch_entry = ampl_total[i][j] -> GetEntries(); 
                bad_ch_box.ch_entry_ampl_cut = ampl_cut_count[i][j]; 
                bad_ch_box.turn_on  = ampl_total_fitter[i][j] -> GetParameter(0);
                bad_ch_box.width    = ampl_total_fitter[i][j] -> GetParameter(1);
                bad_ch_box.profile  = ch_profile[i][j];
                
                bad_ch_list.push_back(bad_ch_box);
                
                // todo : the size of the bit length is by a number 
                // todo : the number of bits is five now
                cout<<"bad channel, chip_id "<<bad_ch_box.chip<<" chan_id : "<<bad_ch_box.chan<<" profile : "<<std::bitset<8>(bad_ch_box.profile)<<" ch_entry : "<<bad_ch_box.ch_entry<<" ch_entry_ampl_cut : "<<bad_ch_box.ch_entry_ampl_cut<<" turn on : "<<bad_ch_box.turn_on<<" width : "<<bad_ch_box.width<<endl;
            }
        }
    }
}

void ladder_cali::Draw_bad_ch_plot(TString plot_folder_dire, TString file_name)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);
    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    TString file_name_short = file_name; file_name_short = file_name_short.ReplaceAll(".root","");

    // todo : the size of the bit length is by a number 
    std::bitset<8> profile_string;
     
    c1 -> Print( Form("%s/BadCh_%s_ampl_total.pdf(",plot_folder_dire.Data(),ladder_name.Data()) );

    for (int i = 0; i < bad_ch_list.size(); i++)
    {
        ampl_total[ bad_ch_list[i].chip - 1 ][ bad_ch_list[i].chan ] -> Draw("hist");
        ampl_total_fitter[ bad_ch_list[i].chip - 1 ][ bad_ch_list[i].chan ] -> Draw("lsame");
        
        profile_string = bad_ch_list[i].profile;

        draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, Port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
        draw_text -> DrawLatex(0.12, 0.850, Form("Profile : %s"  , profile_string.to_string().c_str()));
        draw_text -> DrawLatex(0.12, 0.750, Form("Turn on : %.4f", ampl_total_fitter[ bad_ch_list[i].chip - 1 ][ bad_ch_list[i].chan ] -> GetParameter(0)));
        draw_text -> DrawLatex(0.12, 0.720, Form("Width   : %.3f", ampl_total_fitter[ bad_ch_list[i].chip - 1 ][ bad_ch_list[i].chan ] -> GetParameter(1)));
        draw_text -> DrawLatex(0.12, 0.690, Form("Height  : %.3f", ampl_total_fitter[ bad_ch_list[i].chip - 1 ][ bad_ch_list[i].chan ] -> GetParameter(2)));
    
        c1 -> Print( Form("%s/BadCh_%s_ampl_total.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
        c1 -> Clear();
    }

    c1 -> Print( Form("%s/BadCh_%s_ampl_total.pdf)",plot_folder_dire.Data(),ladder_name.Data()) );
    c1 -> Clear();

}

void ladder_cali::Draw_ch_plot(vector<int>draw_plot_range, TString plot_folder_dire, TString file_name, TString draw_what = "ampl_total")
{
    
    //todo : if the plot to be drawn increase, the following has to be modified
    
    //todo : the filled words are strict now.
    unsigned long len_unsigned_long = -1;
    int draw_ampl_single = ( string(draw_what).find("ampl_single") != len_unsigned_long  )? 1 : 0; 
    int draw_ampl_total  = ( string(draw_what).find("ampl_total") != len_unsigned_long  )? 1 : 0;

    if ( (draw_ampl_single + draw_ampl_total) == 0 ) 
    {
        cout<<"Draw nothing, wrong fill or no fill...? !!!"<<endl;
        return;
    }

    TString file_name_short = file_name; file_name_short = file_name_short.ReplaceAll(".root","");

    TCanvas * c1 = new TCanvas("c1","c1",800,800);

    TLatex *draw_text = new TLatex();
	draw_text->SetNDC();
	draw_text->SetTextSize(0.025);

    if (draw_ampl_single == 1)
    {
        // note : ampl_single
        for (int i = 0; i < draw_plot_range.size(); i++)
        {
            c1 -> Print( Form("%s/Detail_%s_ampl_single_U%i.pdf(",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );

            for (int j = 0; j < 128; j++)
            {
                for (int k = 0; k < 8; k++)
                {
                    ampl_single[ draw_plot_range[i] - 1 ][j][k] -> Draw("hist");
                    if (width_check_mode == 1) 
                    { 
                        if ( k == 0 || k == 1 || k == 2 )
                        {
                            ampl_single_fitter[ draw_plot_range[i] - 1 ][j][k] -> Draw("lsame"); 
                            draw_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_single_fitter[ draw_plot_range[i] - 1 ][j][k] -> GetParameter(2)));
                            draw_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_single_fitter[ draw_plot_range[i] - 1 ][j][k] -> GetParameter(1)));
                        }
                        
                    }
                    draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));

                    c1 -> Print( Form("%s/Detail_%s_ampl_single_U%i.pdf",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );
                }
            }
            
            c1 -> Print( Form("%s/Detail_%s_ampl_single_U%i.pdf)",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );
            c1 -> Clear();
        }
    }


    if(draw_ampl_total == 1)
    {

        // note : ampl total
        for (int i = 0; i < draw_plot_range.size(); i++)
        {
            c1 -> Print( Form("%s/Detail_%s_ampl_total_U%i.pdf(",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );

            for (int j = 0; j < 128; j++)
            {
                ampl_total[ draw_plot_range[i] - 1 ][j] -> Draw("hist");
                ampl_total_fitter[draw_plot_range[i] - 1][j] -> Draw("lsame");
                
                draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch, ladder_port.Data()));
                draw_text -> DrawLatex(0.12, 0.750, Form("Turn on : %.4f", ampl_total_fitter[draw_plot_range[i] - 1][j] -> GetParameter(0)));
                draw_text -> DrawLatex(0.12, 0.720, Form("Width   : %.3f", ampl_total_fitter[draw_plot_range[i] - 1][j] -> GetParameter(1)));
                draw_text -> DrawLatex(0.12, 0.690, Form("Height  : %.3f", ampl_total_fitter[draw_plot_range[i] - 1][j] -> GetParameter(2)));
        
                c1 -> Print( Form("%s/Detail_%s_ampl_total_U%i.pdf",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );   

            }
            
            c1 -> Print( Form("%s/Detail_%s_ampl_total_U%i.pdf)",plot_folder_dire.Data(),ladder_name.Data(),draw_plot_range[i]) );
            c1 -> Clear();
        }

    }

}


void ladder_cali::Draw_overall_info(TString plot_folder_dire, TString file_name)
{

    TCanvas * c1 = new TCanvas("c1","c1",800,800);

    TLatex *Guas_fit_text = new TLatex();
	Guas_fit_text->SetNDC();
	Guas_fit_text->SetTextSize(0.025);

    TString file_name_short = file_name; file_name_short = file_name_short.ReplaceAll(".root","");

    ch_entry -> Draw("hist");
    ch_entry_fitter -> Draw("lsame");
    Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
    Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ch_entry_fitter -> GetParameter(1)));
    Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ch_entry_fitter -> GetParameter(2)));
    c1 -> Print( Form("%s/Overall_ch_entry_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
    c1 -> Clear();

    ampl_total_turn_on -> Draw("hist");
    ampl_total_turn_on_fitter -> Draw("lsame");
    Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
    Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_total_turn_on_fitter -> GetParameter(1)));
    Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_total_turn_on_fitter -> GetParameter(2)));
    c1 -> Print( Form("%s/Overall_ampl_total_turnOn_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
    c1 -> Clear();

    ampl_total_width -> Draw("hist");
    ampl_total_width_fitter -> Draw("lsame");
    Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
    Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_total_width_fitter -> GetParameter(1)));
    Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_total_width_fitter -> GetParameter(2)));
    c1 -> Print( Form("%s/Overall_ampl_total_width_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
    c1 -> Clear();

    ch_entry_ampl_cut -> Draw("hist");
    Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
    c1 -> Print( Form("%s/Overall_ch_entry_ampl_%i_cut_%s.pdf",plot_folder_dire.Data(), ampl_cut, ladder_name.Data()) );
    c1 -> Clear();

    if (width_check_mode == 1)
    {
        ampl_single_0_width -> Draw("hist");
        ampl_single_0_width_fitter -> Draw("lsame");
        Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
        Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_single_0_width_fitter -> GetParameter(1)));
        Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_single_0_width_fitter -> GetParameter(2)));
        c1 -> Print( Form("%s/Overall_ampl_single_adc0_width_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
        c1 -> Clear();

        ampl_single_1_width -> Draw("hist");
        ampl_single_1_width_fitter -> Draw("lsame");
        Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
        Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_single_1_width_fitter -> GetParameter(1)));
        Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_single_1_width_fitter -> GetParameter(2)));
        c1 -> Print( Form("%s/Overall_ampl_single_adc1_width_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
        c1 -> Clear();

        ampl_single_2_width -> Draw("hist");
        ampl_single_2_width_fitter -> Draw("lsame");
        Guas_fit_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, port %s",file_name_short.Data(),Felix_ch,ladder_port.Data()));
        Guas_fit_text -> DrawLatex(0.60, 0.750, Form("Gaus mean : %.4f", ampl_single_2_width_fitter -> GetParameter(1)));
        Guas_fit_text -> DrawLatex(0.60, 0.720, Form("Gaus Width: %.4f", ampl_single_2_width_fitter -> GetParameter(2)));
        c1 -> Print( Form("%s/Overall_ampl_single_adc2_width_%s.pdf",plot_folder_dire.Data(),ladder_name.Data()) );
        c1 -> Clear();
    }

}

void ladder_cali::Output_bad_ch_root(TString plot_folder_dire, TString file_name)
{
    // note : file name, ladder_name, module, used sigma, classify based, cut range 
    // note : chip, chan, entry, entry_ampl_cut, turn_on, width
    // todo : if the cut increases, this part has to be updated as well.

    TFile * file_output = new TFile(Form("%s/%s_bad_ch.root", plot_folder_dire.Data(), ladder_name.Data()), "RECREATE");
    file_output -> cd();

    TString file_name_tree;
    TString ladder_name_tree;
    int ROC_index_tree;
    int Felix_ch_tree;
    int port_id_tree;
    int used_sigma_tree;
    int classify_base_tree;
    vector<double> ch_entry_range_tree;
    vector<double> ch_entry_ampl_cut_range_tree;
    vector<double> ampl_total_turn_on_range_tree;
    vector<double> ampl_total_width_range_tree;

    // note : the following cuts are not used for the classification for now, it's the status monitoring. 
    int unfit_region_tree;
    int ch_entry_NC_range_tree;
    vector<double> ch_entry_ampl_cut_HE_range_tree;
    vector<double> ampl_total_height_HE_range_tree;
    vector<double> ch_entry_ampl_cut_DB_range_tree;
    vector<double> ampl_total_height_DB_range_tree;

    TTree * file_tree = new TTree("file_info","The file information");
    file_tree -> Branch("roc_id", &ROC_index_tree);
    file_tree -> Branch("file_name", &file_name_tree);
    file_tree -> Branch("ladder_name", &ladder_name_tree);
    file_tree -> Branch("felix_ch", &Felix_ch_tree);
    file_tree -> Branch("port_id", &port_id_tree);
    file_tree -> Branch("used_sigma", &used_sigma_tree);
    file_tree -> Branch("classify_base", &classify_base_tree);
    file_tree -> Branch("ch_entry_range", &ch_entry_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_range", &ch_entry_ampl_cut_range_tree);
    file_tree -> Branch("ampl_total_turn_on_range", &ampl_total_turn_on_range_tree);
    file_tree -> Branch("ampl_total_width_range", &ampl_total_width_range_tree);

    // note : the following cuts are not used for the classification for now, it's the status monitoring.
    file_tree -> Branch("unfit_region", &unfit_region_tree);
    file_tree -> Branch("ch_entry_NC_range",&ch_entry_NC_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_HE_range", &ch_entry_ampl_cut_HE_range_tree);
    file_tree -> Branch("ampl_total_height_HE_range", &ampl_total_height_HE_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_DB_range", &ch_entry_ampl_cut_DB_range_tree);
    file_tree -> Branch("ampl_total_height_DB_range", &ampl_total_height_DB_range_tree);

    ROC_index_tree = ROC_index;
    file_name_tree = file_name;
    ladder_name_tree = ladder_name;
    Felix_ch_tree = Felix_ch;
    port_id_tree = port_map[ladder_port];
    used_sigma_tree = used_sigma;
    classify_base_tree = classify_base;
    ch_entry_range_tree = {ch_entry_range.first, ch_entry_range.second};
    ch_entry_ampl_cut_range_tree = {ch_entry_ampl_cut_range.first, ch_entry_ampl_cut_range.second};
    ampl_total_turn_on_range_tree = {ampl_total_turn_on_range.first, ampl_total_turn_on_range.second};
    ampl_total_width_range_tree = {ampl_total_width_range.first, ampl_total_width_range.second};

    // note : the following cuts are not used for the classification for now, it's the status monitoring.
    unfit_region_tree = unfit_region;
    ch_entry_NC_range_tree = ch_entry_NC_range;
    ch_entry_ampl_cut_HE_range_tree = {ch_entry_ampl_cut_HE_range.first, ch_entry_ampl_cut_HE_range.second}; 
    ampl_total_height_HE_range_tree = {ampl_total_height_HE_range.first, ampl_total_height_HE_range.second}; 
    ch_entry_ampl_cut_DB_range_tree = {ch_entry_ampl_cut_DB_range.first, ch_entry_ampl_cut_DB_range.second}; 
    ampl_total_height_DB_range_tree = {ampl_total_height_DB_range.first, ampl_total_height_DB_range.second}; 

    file_tree -> Fill();
    
    int chip_tree;
    int chan_tree;
    int ch_entry_tree;
    int ch_entry_ampl_cut_tree;
    double turn_on_tree;
    double width_tree;
    int profile_tree;
    TTree * bad_ch_tree = new TTree("bad_ch","The properties of the bad channels");
    bad_ch_tree -> Branch("roc_id", &ROC_index_tree);
    bad_ch_tree -> Branch("ladder",&ladder_name_tree);
    bad_ch_tree -> Branch("felix_ch",&Felix_ch_tree);
    bad_ch_tree -> Branch("port_id",&port_id_tree);
    bad_ch_tree -> Branch("chip_id",&chip_tree);
    bad_ch_tree -> Branch("chan_id",&chan_tree);
    bad_ch_tree -> Branch("ch_entry",&ch_entry_tree);
    bad_ch_tree -> Branch("ch_entry_ampl_cut",&ch_entry_ampl_cut_tree);
    bad_ch_tree -> Branch("turn_on",&turn_on_tree);
    bad_ch_tree -> Branch("width",&width_tree);
    bad_ch_tree -> Branch("profile",&profile_tree);


    int chip_NC_tree;
    int chan_NC_tree;
    int ch_entry_NC_tree;
    int ch_entry_ampl_cut_NC_tree;
    double turn_on_NC_tree;
    double width_NC_tree;
    int profile_NC_tree;
    TTree * noisy_ch_tree = new TTree("noisy_ch","The properties of the noisy channels");
    noisy_ch_tree -> Branch("roc_id", &ROC_index_tree);
    noisy_ch_tree -> Branch("ladder",&ladder_name_tree);
    noisy_ch_tree -> Branch("felix_ch",&Felix_ch_tree);
    noisy_ch_tree -> Branch("port_id",&port_id_tree);
    noisy_ch_tree -> Branch("chip_id",&chip_NC_tree);
    noisy_ch_tree -> Branch("chan_id",&chan_NC_tree);
    noisy_ch_tree -> Branch("ch_entry",&ch_entry_NC_tree);
    noisy_ch_tree -> Branch("ch_entry_ampl_cut",&ch_entry_ampl_cut_NC_tree);
    noisy_ch_tree -> Branch("turn_on",&turn_on_NC_tree);
    noisy_ch_tree -> Branch("width",&width_NC_tree);
    noisy_ch_tree -> Branch("profile",&profile_NC_tree);


    ROC_index_tree = ROC_index;
    ladder_name_tree = ladder_name;
    Felix_ch_tree = Felix_ch;
    port_id_tree = port_map[ladder_port];

    for (int i = 0; i < bad_ch_list.size(); i++)
    {
        if ((bad_ch_list[i].profile&0x80) == 0x80) // note : To check the noisy channel bit is 1 or 0, if it's 1, we save it to the root file.
        {
            chip_NC_tree = bad_ch_list[i].chip;
            chan_NC_tree = bad_ch_list[i].chan;
            ch_entry_NC_tree = bad_ch_list[i].ch_entry;
            ch_entry_ampl_cut_NC_tree = bad_ch_list[i].ch_entry_ampl_cut;
            turn_on_NC_tree = bad_ch_list[i].turn_on;
            width_NC_tree = bad_ch_list[i].width;
            profile_NC_tree = bad_ch_list[i].profile;

            noisy_ch_tree -> Fill();
        }

        chip_tree = bad_ch_list[i].chip;
        chan_tree = bad_ch_list[i].chan;
        ch_entry_tree = bad_ch_list[i].ch_entry;
        ch_entry_ampl_cut_tree = bad_ch_list[i].ch_entry_ampl_cut;
        turn_on_tree = bad_ch_list[i].turn_on;
        width_tree = bad_ch_list[i].width;
        profile_tree = bad_ch_list[i].profile;

        bad_ch_tree -> Fill();

    }

    file_output   -> cd();
    file_tree     -> Write("", TObject::kOverwrite);
    bad_ch_tree   -> Write("", TObject::kOverwrite);
    noisy_ch_tree -> Write("", TObject::kOverwrite);
    
    file_output -> Close();

}

void ladder_cali::Output_ch_property_root(TString plot_folder_dire, TString file_name)
{
    // note : file name, ladder_name, module, used sigma, classify based, cut range 
    // note : chip, chan, entry, entry_ampl_cut, turn_on, width
    // todo : if the cut increases, this part has to be updated as well.

    TFile * file_output = new TFile(Form("%s/%s_ch_property.root", plot_folder_dire.Data(), ladder_name.Data()), "RECREATE");
    file_output -> cd();

    TString file_name_tree;
    TString ladder_name_tree;
    int ROC_index_tree;
    int Felix_ch_tree;
    int port_id_tree;
    int used_sigma_tree;
    int classify_base_tree;
    vector<double> ch_entry_range_tree;
    vector<double> ch_entry_ampl_cut_range_tree;
    vector<double> ampl_total_turn_on_range_tree;
    vector<double> ampl_total_width_range_tree;

    vector<double> ampl_single_adc0_width_tree; // note : for width and mean, not the range
    vector<double> ampl_single_adc1_width_tree; // note : for width and mean, not the range
    vector<double> ampl_single_adc2_width_tree; // note : for width and mean, not the range

    // note : the following cuts are not used for the classification for now, it's the status monitoring. 
    int unfit_region_tree;
    int ch_entry_NC_range_tree;
    vector<double> ch_entry_ampl_cut_HE_range_tree;
    vector<double> ampl_total_height_HE_range_tree;
    vector<double> ch_entry_ampl_cut_DB_range_tree;
    vector<double> ampl_total_height_DB_range_tree;

    TTree * file_tree = new TTree("file_info","The file information");
    file_tree -> Branch("roc_id", &ROC_index_tree);
    file_tree -> Branch("file_name", &file_name_tree);
    file_tree -> Branch("ladder_name", &ladder_name_tree);
    file_tree -> Branch("felix_ch", &Felix_ch_tree);
    file_tree -> Branch("port_id", &port_id_tree);
    file_tree -> Branch("used_sigma", &used_sigma_tree);
    file_tree -> Branch("classify_base", &classify_base_tree);
    file_tree -> Branch("ch_entry_range", &ch_entry_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_range", &ch_entry_ampl_cut_range_tree);
    file_tree -> Branch("ampl_total_turn_on_range", &ampl_total_turn_on_range_tree);
    file_tree -> Branch("ampl_total_width_range", &ampl_total_width_range_tree);
    
    if (width_check_mode == 1)
    {
        file_tree -> Branch("ampl_single_adc0_width__width_mean", &ampl_single_adc0_width_tree);
        file_tree -> Branch("ampl_single_adc1_width__width_mean", &ampl_single_adc1_width_tree);
        file_tree -> Branch("ampl_single_adc2_width__width_mean", &ampl_single_adc2_width_tree);
    }
    
    // note : the following cuts are not used for the classification for now, it's the status monitoring.
    file_tree -> Branch("unfit_region", &unfit_region_tree);
    file_tree -> Branch("ch_entry_NC_range", &ch_entry_NC_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_HE_range", &ch_entry_ampl_cut_HE_range_tree);
    file_tree -> Branch("ampl_total_height_HE_range", &ampl_total_height_HE_range_tree);
    file_tree -> Branch("ch_entry_ampl_cut_DB_range", &ch_entry_ampl_cut_DB_range_tree);
    file_tree -> Branch("ampl_total_height_DB_range", &ampl_total_height_DB_range_tree);

    ROC_index_tree = ROC_index;
    file_name_tree = file_name;
    ladder_name_tree = ladder_name;
    Felix_ch_tree = Felix_ch;
    port_id_tree = port_map[ladder_port];
    used_sigma_tree = used_sigma;
    classify_base_tree = classify_base;
    ch_entry_range_tree = {ch_entry_range.first, ch_entry_range.second};
    ch_entry_ampl_cut_range_tree = {ch_entry_ampl_cut_range.first, ch_entry_ampl_cut_range.second};
    ampl_total_turn_on_range_tree = {ampl_total_turn_on_range.first, ampl_total_turn_on_range.second};
    ampl_total_width_range_tree = {ampl_total_width_range.first, ampl_total_width_range.second};

    if (width_check_mode == 1)
    {
        ampl_single_adc0_width_tree = {ampl_single_0_width_fitter -> GetParameter(2), ampl_single_0_width_fitter -> GetParameter(1)};
        ampl_single_adc1_width_tree = {ampl_single_1_width_fitter -> GetParameter(2), ampl_single_1_width_fitter -> GetParameter(1)};
        ampl_single_adc2_width_tree = {ampl_single_2_width_fitter -> GetParameter(2), ampl_single_2_width_fitter -> GetParameter(1)};
    }

    // note : the following cuts are not used for the classification for now, it's the status monitoring.
    unfit_region_tree = unfit_region;
    ch_entry_NC_range_tree = ch_entry_NC_range;
    ch_entry_ampl_cut_HE_range_tree = {ch_entry_ampl_cut_HE_range.first, ch_entry_ampl_cut_HE_range.second}; 
    ampl_total_height_HE_range_tree = {ampl_total_height_HE_range.first, ampl_total_height_HE_range.second}; 
    ch_entry_ampl_cut_DB_range_tree = {ch_entry_ampl_cut_DB_range.first, ch_entry_ampl_cut_DB_range.second}; 
    ampl_total_height_DB_range_tree = {ampl_total_height_DB_range.first, ampl_total_height_DB_range.second};

    file_tree -> Fill();
    
    int chip_tree;
    int chan_tree;
    int ch_entry_tree;
    int ch_entry_ampl_cut_tree;
    double turn_on_tree;
    double width_tree;
    int profile_tree;
    
    double adc0_width_tree;
    double adc1_width_tree;
    double adc2_width_tree;
    
    TTree * ch_property_tree = new TTree("ch_property","The properties of the ALL channels");
    ch_property_tree -> Branch("roc_id",&ROC_index_tree);
    ch_property_tree -> Branch("ladder",&ladder_name_tree);
    ch_property_tree -> Branch("felix_ch",&Felix_ch_tree);
    ch_property_tree -> Branch("port_id",&port_id_tree);
    ch_property_tree -> Branch("chip_id",&chip_tree);
    ch_property_tree -> Branch("chan_id",&chan_tree);
    ch_property_tree -> Branch("ch_entry",&ch_entry_tree);
    ch_property_tree -> Branch("ch_entry_ampl_cut",&ch_entry_ampl_cut_tree);
    ch_property_tree -> Branch("turn_on",&turn_on_tree);
    ch_property_tree -> Branch("width",&width_tree);
    ch_property_tree -> Branch("profile",&profile_tree);

    if (width_check_mode == 1)
    {
        ch_property_tree -> Branch("adc0_width",&adc0_width_tree);
        ch_property_tree -> Branch("adc1_width",&adc1_width_tree);
        ch_property_tree -> Branch("adc2_width",&adc2_width_tree);
    }

    ROC_index_tree = ROC_index;
    ladder_name_tree = ladder_name;
    Felix_ch_tree = Felix_ch;
    port_id_tree = port_map[ladder_port];

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            chip_tree = i + 1; 
            chan_tree = j;  
            ch_entry_tree = ampl_total[i][j] -> GetEntries();  
            ch_entry_ampl_cut_tree = ampl_cut_count[i][j];  
            turn_on_tree  = ampl_total_fitter[i][j] -> GetParameter(0); 
            width_tree    = ampl_total_fitter[i][j] -> GetParameter(1); 
            profile_tree  = ch_profile[i][j]; 

            if (width_check_mode == 1)
            {
                adc0_width_tree = ampl_single_fitter[i][j][0] -> GetParameter(2);            
                adc1_width_tree = ampl_single_fitter[i][j][1] -> GetParameter(2);
                adc2_width_tree = ampl_single_fitter[i][j][2] -> GetParameter(2);
            }

            ch_property_tree -> Fill();
        }
    }

    file_output -> cd();
    file_tree   -> Write("", TObject::kOverwrite);
    ch_property_tree -> Write("", TObject::kOverwrite);
    
    file_output -> Close();
}

void ladder_cali::Output_bad_ch_txt(TString plot_folder_dire)
{
    int total_bad = bad_ch_list.size();
    int chip_bad[26] = {0};

    for (int i = 0; i < bad_ch_list.size(); i++)
    {
        chip_bad[ bad_ch_list[i].chip - 1 ] += 1;
    }
    
    ofstream bad_ch_txt (Form("%s/%s_bad_ch.txt", plot_folder_dire.Data(), ladder_name.Data()));
    if (bad_ch_txt.is_open())
    {

        bad_ch_txt << Form("Total %i\n",total_bad);

        // note : for chips 
        for (int i = 0; i < 26; i++)
        {
            if (i != 25)
            {
                bad_ch_txt << Form("U%i %i\n", i+1, chip_bad[i]);
            }
            else 
            {
                bad_ch_txt << Form("U%i %i", i+1, chip_bad[i]);
            }

        }
        bad_ch_txt.close();
    }
    else 
    {
        cout << "Unable to open file : "<<Form("%s/%s_bad_ch.txt", plot_folder_dire.Data(), ladder_name.Data())<<endl;
        return;
    }
         

    


}
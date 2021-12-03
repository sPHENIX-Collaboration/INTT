//----------------------------------------------------------------------------------------------------------------
// this is version 7 the last update date : 2021/11/30 09:54 pm
//
// Note by Cheng-Wei Shih @ NCU 2021/11/19
// for being more familiar with this tracking macro, please take a look the slides in the link below : 
// https://indico.bnl.gov/event/13849/#1-the-tracking-macro-update
//
// Based on current tracking macro, there are 3 bases
// 1. No magnetic field -> the trajectory should be a straight line
// 2. The beam direction should be perpendicular to the ladder. 
// 3. only one track in one event. Multi-tracks is not allowed yet. I am working on it now
// 4. the file should run with the filter macro before running the analysis
//
//
// !!!!!!!! the new features of version 7 comparing with version 6 : !!!!!!!!
//
// 1. the chip sequence of the adc plot is same as the hitmap 
// 2. 3 hit alignment algorithm is added 
// 3. for 3 hit case, the way to pick up the second bad hit is by checking the (2 points abs. slope), not the abs difference now
// 4. the residual histogram is available for 3 hit layer case now
// 5. the alignment histogram is available for 3 hit layer case now
// 6. for the 4 hit second outlier pick up, is no by checking the (2 points abs. slope), not the abs. difference now.
// 7. the alignment root file output is available for hit3 case now
// 9. for hit4 alignment the correction order is based on the 3 points fitting r-chi2 
// 10.for the event that its profile not perfect, its event display will be printed.
// 11. full analysis option
//
// !!!!!!!! the new features of version 7 comparing with version 6 : !!!!!!!!
//
// the new features of version 6 comparing with version 5 : 
// 1. the setbranchaddress is used
// 2. the consideration of 4 points case is up to 0000
// 3. the consideration of 3 points case is up to 0000 
// 4. the hit map, the adc distribution, the event hit distribution
// 5. more powerful alignment output root file 
//
// a lot of options you can change, listed below
// 1. camac_tdc6 range_cut         (mandatory)
// 2. INTT_event                   (mandatory)
// 3. event_length                 (mandatory)
// 3. camac_adc cut                (optional)
// 4. double saving event included (optional)
// 5. multi-cluster                (optional)
// 6. 1 hit-cluster(pure hit track)(optional)
// 7. alignment                    (optional)
// 
// 8. 3 or 4 layers 
// 9. the size of the cut window
//
//
//
//
//----------------------------------------------------------------------------------------------------------------

// this function decides the study chip (slot)
// the coordinate is trnasformed already. For example, chip 5 and chip 18 are both chip 5 now
// it calculates the number of vote each chip (slot) has
// then it picks up the one with highest voted chip (slot)
// of course you can decide the study chip by yourself
// output_array[0] = the ID of the winner chip (slot)
// output_array[1] = the number of vote the winner chip (slot) has 
// for the case that output_array[1] = 3 or 2,
// starts from output_array[2], the array first saves the layer ID with no hit at winner chip (slot), and then save the layer ID that with hit at winer chip
vector<int> majority_chip (vector<vector<int>> event_chip_to_function, bool major_chip_on, int study_chip )
{
	vector<int>layer_chip_hit[4][26];
	int chip_hit[13]; 
	memset(chip_hit, 0, sizeof(chip_hit));

	int layer_missing_hit=999;
	vector<int>layer_missing_hit_2hit; layer_missing_hit_2hit.clear();
	int winner_slot; //the variable for majory_chip_on and off

	vector<int> output_array; output_array.clear();

	for (int i=0; i<4; i++)
	{
		for (int i1=0; i1<26; i1++)
		{
			layer_chip_hit[i][i1].clear();
		}
	}

	for (int i1=0; i1<4; i1++)
	{
		for (int i2=0; i2<event_chip_to_function[i1].size(); i2++)
		{
			layer_chip_hit[i1][ event_chip_to_function[i1][i2]-1 ].push_back(1);
		}
	}

	for (int i=0; i<4; i++)
	{
		for (int i1=0; i1<13; i1++) // i1 = "chip_id+1" + "chip_id+1+13"
		{
			if (layer_chip_hit[i][i1].size()>0 || layer_chip_hit[i][i1+13].size()>0)
			{
				chip_hit[i1]+=1;
			}

		}
	}

	if (major_chip_on == true) // true -> we use this function to find the winner chip
	{
		int size = 13;
		int sort_chip_hit_index[size];
		TMath::Sort(size, chip_hit, sort_chip_hit_index);
		winner_slot = sort_chip_hit_index[0];
	}
	else // false -> we define the winner chip
	{
		winner_slot = study_chip - 1;
	}

	output_array.push_back(winner_slot+1);
	output_array.push_back(chip_hit[winner_slot]);

	if (chip_hit[winner_slot] == 4)
	{
		output_array.push_back(layer_missing_hit);
	}
	else // winner chip has 3 or 2 votes
	{
		// first we save the ID of the layer with no hit
		for (int i=0; i<4; i++)
		{
			if ( layer_chip_hit[i][winner_slot].size()<1 && layer_chip_hit[i][winner_slot+13].size()<1)
			{		
				output_array.push_back(i);
			}
		}

		// then we save the ID of the layer with hits
		for (int i=0; i<4; i++)
		{
			if ( layer_chip_hit[i][winner_slot].size()>0 || layer_chip_hit[i][winner_slot+13].size()>0)
			{		
				output_array.push_back(i);
			}
		}

	}

	// cout<<"----------- function test : "<<winner_slot<<endl;
	
	
	return output_array;
}

// this function calculates the clusters and their positions
vector<vector<double>> data_all_edep_weight (vector<int> data_all[4][26],vector<int>most_hit_info, double adc_convert[8],double alignment_array[4] )
{
	double INTT_strip_width = 0.078;
	double lower_section_initial = -9.961;
	double upper_section_initial = 0.055;

	vector<double> chan_master; chan_master.clear();
	
	double chan_truck=0;
	double chan_truck_denominator=0;
	int standard_channel;
	int distance_meter=0;
	vector<vector<double>> final_output;
	final_output.clear();

	vector <int> data_all_new[4][256]; // layer and channel, the chip was selected already. 
	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<256; i6++)	
			data_all_new[i5][i6].clear();


	vector <int> data_all_new1[4][26]; //layer and chip
	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<26; i6++)	
			data_all_new1[i5][i6].clear();


	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<data_all[l1][most_hit_info[0]-1].size(); l2++)	
		{
			data_all_new[l1][ (data_all[l1][most_hit_info[0]-1][l2]%1000) ].push_back( data_all[l1][most_hit_info[0]-1][l2] );
		}
	}

	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<256; l2++)
		{
			if (data_all_new[l1][l2].size()>0)
			{
				data_all_new1[l1][most_hit_info[0]-1].push_back(data_all_new[l1][l2][0]);
			}
		}
	}


	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<data_all_new1[l1][most_hit_info[0]-1].size(); l2++)
		{
			standard_channel = data_all_new1[l1][most_hit_info[0]-1][l2]%1000;

			if ( (data_all_new1[l1][most_hit_info[0]-1][l2]%1000) <128  )
			{
				chan_truck += (lower_section_initial + INTT_strip_width * (data_all_new1[l1][most_hit_info[0]-1][l2]%1000)) * adc_convert[ int(data_all_new1[l1][most_hit_info[0]-1][l2]/1000) ];
			}	
			else
			{
				chan_truck += ( upper_section_initial + INTT_strip_width * ( (data_all_new1[l1][most_hit_info[0]-1][l2]%1000) -128 ) ) * adc_convert[ int(data_all_new1[l1][most_hit_info[0]-1][l2]/1000) ];
			}
			chan_truck_denominator+=adc_convert[ int(data_all_new1[l1][most_hit_info[0]-1][l2]/1000) ];
			if (data_all_new1[l1][most_hit_info[0]-1].size()-l2==1) // for the single hit layer 
			{
				chan_master.push_back(chan_truck/chan_truck_denominator );
			}

			for (int l3=0; l3<data_all_new1[l1][most_hit_info[0]-1].size()-(l2+1); l3++)
			{
				if ( fabs(standard_channel - (data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]%1000))==l3+1 )	
				{						
					if ( (data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]%1000) <128  )
					{
						chan_truck += (lower_section_initial + INTT_strip_width * (data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]%1000)) * adc_convert[int(data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]/1000)];
					}	
					else
					{
						chan_truck += ( upper_section_initial + INTT_strip_width * ( (data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]%1000) -128 ) ) * adc_convert[int(data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]/1000)];
					}

					chan_truck_denominator+=adc_convert[ int(data_all_new1[l1][most_hit_info[0]-1][l3+l2+1]/1000) ];
					distance_meter+=1;

					if ((data_all_new1[l1][most_hit_info[0]-1].size()-(l2+1)-l3) == 1) // for the last hit
					{
						chan_master.push_back(chan_truck/chan_truck_denominator );	
					}

					// channel_indicator = (data_all_new1[l1][most_hit_info[0]-1][l2+channel_indicator_distance]%1000);	
				}
				else 
				{
					chan_master.push_back(chan_truck/  chan_truck_denominator);
					break; 
				}
			}
			
			l2+=distance_meter;
			distance_meter=0;
			chan_truck=0;
			chan_truck_denominator=0;
		}
		for (int l4=0; l4<chan_master.size(); l4++) 
		{
			chan_master[l4]=chan_master[l4]+alignment_array[l1];
			// the line below is to check the cluster position
			// printf("cluster test, layer %d, %.4f \n",l1,chan_master[l4] );
		}
		final_output.push_back(chan_master);
		chan_master.clear();
	}//end of l1 loop

	return final_output;


	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<256; i6++)	
			data_all_new[i5][i6].clear();

	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<26; i6++)	
			data_all_new1[i5][i6].clear();


}

// the alignment function only works for 4 layer Testbeam case 
vector <double> align_func (vector <double> hit_position, double correction_rate, bool ploting, TString folder_name,TString file_name,double Legend_Y_offset, double offset_tolerance, TCanvas * c2,int event_id,int info_sampling,int eID)
{
	// int plot_Y_min = 10;
	// int plot_Y_max = -10;
	int plot_Y_min = (hit_position[0]+hit_position[1]+hit_position[2]+hit_position[3])/4.-3.;
	int plot_Y_max = (hit_position[0]+hit_position[1]+hit_position[2]+hit_position[3])/4.+3.;

	vector <double> hit_3_y_axis[4]; for (int i=0; i<4; i++) hit_3_y_axis[i].clear();
	// vector <double> hit_3_y_axis_0; hit_3_y_axis_0.clear(); 
	// vector <double> hit_3_y_axis_1; hit_3_y_axis_1.clear(); 
	// vector <double> hit_3_y_axis_2; hit_3_y_axis_2.clear(); 
	// vector <double> hit_3_y_axis_3; hit_3_y_axis_3.clear(); 
	vector <double> hit_fit_rchi;  hit_fit_rchi.clear(); // {rchi of hit3 miss0, rchi of hit3 miss1, rchi of hit3 miss2, rchi of hit3 miss3, rchi of all considered}
 	vector <double> hit_fit_rchi_new;  hit_fit_rchi_new.clear();
 	double x_axis[4]     = {0,1,2,3};
	double x_axis_3[4][3] = {{1,2,3}, {0,2,3}, {0,1,3}, {0,1,2}};
	// double x_axis_3_1[3] = {0,2,3};
	// double x_axis_3_2[3] = {0,1,3};
	// double x_axis_3_3[3] = {0,1,2};

	double layer_residual[4];
	double abs_layer_residual[4];
	double layer_residual_new[4];
	int residual_order[4];
	double amount_of_calibrate = 0;
	double calibrate_sum = 0;
	double final_calibrate[6]={0,0,0,0,0,0};
	double self_outlier_check[4];
	vector<double> rchi_wihtno_test; rchi_wihtno_test.clear();
	vector<double> rchi_wihtno_test_new; rchi_wihtno_test_new.clear();

	//global fit, 4 points are considered
	TF1 *hit4_fit_align = new TF1("hit4_fit_align","pol1",-1,10);

	//local fit, 3 points are considered. So there are 4 local fits in total
	TF1 *hit3_fit_0 = new TF1("hit3_fit_0","pol1",-1,10);//green
	hit3_fit_0->SetLineColor(3);

	TF1 *hit3_fit_1 = new TF1("hit3_fit_1","pol1",-1,10);//blue
	hit3_fit_1->SetLineColor(4);
	
	TF1 *hit3_fit_2 = new TF1("hit3_fit_2","pol1",-1,10);//purple
	hit3_fit_2->SetLineColor(6);
	
	TF1 *hit3_fit_3 = new TF1("hit3_fit_3","pol1",-1,10);//sky blue
	hit3_fit_3->SetLineColor(7);

	// to calculate the difference between the rest 3 points
	// so far there are 3 methods to find out the sequence of layer requires correction 
	// 1. to check the self position difference
	// 2. to check the residual against the 4 points fitting line 
	// 3. to check the r-chi2 of 3 points fitting line 
	// the method 3 seems to be better

	// self_outlier_check[0]  = fabs(hit_position[0]-(hit_position[1]+hit_position[2]+hit_position[3])/3.);
	// self_outlier_check[1]  = fabs(hit_position[1]-(hit_position[0]+hit_position[2]+hit_position[3])/3.);
	// self_outlier_check[2]  = fabs(hit_position[2]-(hit_position[1]+hit_position[0]+hit_position[3])/3.);
	// self_outlier_check[3]  = fabs(hit_position[3]-(hit_position[1]+hit_position[2]+hit_position[0])/3.);

	//hit4 case
	TGraph * hit4_grr_align = new TGraph (4,x_axis,&hit_position[0]);
	hit4_grr_align ->Fit("hit4_fit_align","NQ");
	
	//to calculate the original residual of each layer
	for (int i=0; i<4; i++) 
	{
		layer_residual[i] = ( hit_position[i] - (hit4_fit_align->GetParameter(1)* double(x_axis[i]) + hit4_fit_align->GetParameter(0)) );  
		// abs_layer_residual[i] = fabs( hit_position[i] - (hit4_fit_align->GetParameter(1)* double(x_axis[i]) + hit4_fit_align->GetParameter(0)) );
	}
	//to find out the order from big "self_outlier_check" to small "self_outlier_check" by using sort
	
	
	//hit3 case
	for (int i2=0; i2<4; i2++) { for (int i3=0; i3<3; i3++) { hit_3_y_axis[i2].push_back( hit_position[ int(x_axis_3[i2][i3]) ] ); } }
	TGraph * hit3_grr_0 = new TGraph (3,x_axis_3[0],&hit_3_y_axis[0][0]);
	TGraph * hit3_grr_1 = new TGraph (3,x_axis_3[1],&hit_3_y_axis[1][0]);
	TGraph * hit3_grr_2 = new TGraph (3,x_axis_3[2],&hit_3_y_axis[2][0]);
	TGraph * hit3_grr_3 = new TGraph (3,x_axis_3[3],&hit_3_y_axis[3][0]);
	hit3_grr_0->Fit("hit3_fit_0","NQ");
	hit3_grr_1->Fit("hit3_fit_1","NQ");
	hit3_grr_2->Fit("hit3_fit_2","NQ");
	hit3_grr_3->Fit("hit3_fit_3","NQ");

	hit_fit_rchi.push_back( hit3_fit_0->GetChisquare()/double(hit3_fit_0->GetNDF()) ); //exclude l0
	hit_fit_rchi.push_back( hit3_fit_1->GetChisquare()/double(hit3_fit_1->GetNDF()) ); //exclude l1
	hit_fit_rchi.push_back( hit3_fit_2->GetChisquare()/double(hit3_fit_2->GetNDF()) ); //exclude l2
	hit_fit_rchi.push_back( hit3_fit_3->GetChisquare()/double(hit3_fit_3->GetNDF()) ); //exclude l3 

	// "false" means the ID of the order start from small to large
	TMath::Sort(4, &hit_fit_rchi[0], residual_order,false); 
	// for (int i=0; i<4; i++) cout<<"residual order : "<<residual_order[i] <<" "<< self_outlier_check[residual_order[i]]<<endl;

	hit_fit_rchi.push_back( hit4_fit_align->GetChisquare()  /double(hit4_fit_align->GetNDF())   );


	if (ploting == true && event_id%info_sampling==0 )
	{	
		c2->cd();
		hit4_grr_align -> SetTitle("first fit");
		hit4_grr_align -> SetMarkerStyle(20);
		hit4_grr_align -> SetMarkerSize(2);
		hit4_grr_align -> GetXaxis()->SetLimits(-1,5);
		hit4_grr_align -> GetXaxis()->SetTitle("Layer ID");
		hit4_grr_align -> GetYaxis()->SetRangeUser(plot_Y_min,plot_Y_max);
		hit4_grr_align -> GetYaxis()->SetTitle("Y position (Unit : mm)");

		hit4_grr_align->Draw("apl");
		hit4_fit_align->Draw("lsame");

		hit3_fit_0->Draw("lsame");
		hit3_fit_1->Draw("lsame");
		hit3_fit_2->Draw("lsame");
		hit3_fit_3->Draw("lsame");

		TLatex *hit4_fit_latex = new TLatex();
		hit4_fit_latex -> SetNDC();
		hit4_fit_latex -> SetTextSize(0.028);	

		hit4_fit_latex -> DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit_align->GetParameter(1),hit4_fit_align->GetParameter(0)));
		hit4_fit_latex -> DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit_align->GetChisquare(),hit4_fit_align->GetNDF(),hit4_fit_align->GetChisquare()/double(hit4_fit_align->GetNDF())));
		hit4_fit_latex -> DrawLatex(0.12, 0.690-Legend_Y_offset,Form("l0-residual : %.5f",layer_residual[0]));
		hit4_fit_latex -> DrawLatex(0.12, 0.660-Legend_Y_offset,Form("l1-residual : %.5f",layer_residual[1]));

		hit4_fit_latex->DrawLatex(0.12, 0.750, Form("#chi^{2}/NDF : %.4f, exclude l0, G", hit_fit_rchi[0]));
		hit4_fit_latex->DrawLatex(0.12, 0.720, Form("#chi^{2}/NDF : %.4f, exclude l1, B", hit_fit_rchi[1]));
		hit4_fit_latex->DrawLatex(0.12, 0.690, Form("#chi^{2}/NDF : %.4f, exclude l2, P", hit_fit_rchi[2]));
		hit4_fit_latex->DrawLatex(0.12, 0.660, Form("#chi^{2}/NDF : %.4f, exclude l3, SB",hit_fit_rchi[3]));

		c2->Print(Form ("%s/folder_%s/alignment_plot/alignment_%i.pdf(",folder_name.Data(),file_name.Data(),eID));
		c2->Clear();
	}

	for (int i=0; i<4; i++)	{	hit_3_y_axis[i].clear();	}

	for (int i=0; i<4; i++) //only 2 layers will be calibrate
	{
		// printf("!!!! we are now working on layer %i \n",residual_order[i]);

		for (int i1=0; i1<100; i1++ )
		{
			amount_of_calibrate = (layer_residual[residual_order[i]]*correction_rate);
			hit_position [residual_order[i]] -= amount_of_calibrate;

			TGraph * hit4_grr_align = new TGraph (4,x_axis,&hit_position[0]);
			hit4_grr_align ->Fit("hit4_fit_align","NQ");

			for (int i2=0; i2<4; i2++) { for (int i3=0; i3<3; i3++) { hit_3_y_axis[i2].push_back( hit_position[ int(x_axis_3[i2][i3]) ] ); /*cout<<"aaa : "<<i2<<" "<<i3<<" "<<hit_position[ int(x_axis_3[i2][i3]) ]<<endl;*/ } }
			TGraph * hit3_grr_0 = new TGraph (3,x_axis_3[0],&hit_3_y_axis[0][0]);
			TGraph * hit3_grr_1 = new TGraph (3,x_axis_3[1],&hit_3_y_axis[1][0]);
			TGraph * hit3_grr_2 = new TGraph (3,x_axis_3[2],&hit_3_y_axis[2][0]);
			TGraph * hit3_grr_3 = new TGraph (3,x_axis_3[3],&hit_3_y_axis[3][0]);
			hit3_grr_0->Fit("hit3_fit_0","NQ");
			hit3_grr_1->Fit("hit3_fit_1","NQ");
			hit3_grr_2->Fit("hit3_fit_2","NQ");
			hit3_grr_3->Fit("hit3_fit_3","NQ");

			hit_fit_rchi_new.push_back( hit3_fit_0->GetChisquare()/double(hit3_fit_0->GetNDF()) );
			hit_fit_rchi_new.push_back( hit3_fit_1->GetChisquare()/double(hit3_fit_1->GetNDF()) );
			hit_fit_rchi_new.push_back( hit3_fit_2->GetChisquare()/double(hit3_fit_2->GetNDF()) );
			hit_fit_rchi_new.push_back( hit3_fit_3->GetChisquare()/double(hit3_fit_3->GetNDF()) );
			hit_fit_rchi_new.push_back( hit4_fit_align->GetChisquare()  /double(hit4_fit_align->GetNDF())   );

			for (int i2=0; i2<4; i2++) {layer_residual_new[i2] = ( hit_position[i2] - (hit4_fit_align->GetParameter(1)* double(x_axis[i2]) + hit4_fit_align->GetParameter(0)) );  }//the residual of each layer after new fit
			// printf("test test %.4f, %.4f \n",hit_fit_rchi_new[2], hit_fit_rchi[2]);
			// cout<<"now_pre chi2 test : "<< (hit_fit_rchi_new[0] - hit_fit_rchi[0]) <<"  "<< (hit_fit_rchi_new[1] - hit_fit_rchi[1]) <<"  "<< (hit_fit_rchi_new[2] - hit_fit_rchi[2]) <<"  "<< (hit_fit_rchi_new[3] - hit_fit_rchi[3]) <<"  "<< (hit_fit_rchi_new[4] - hit_fit_rchi[4]) <<endl;

			//only focus on the event without the align
			for (int i2=0; i2<4; i2++) 
			{
				if ( i2 != residual_order[i])
				{
					rchi_wihtno_test.push_back(hit_fit_rchi[i2]) ;
					rchi_wihtno_test_new.push_back(hit_fit_rchi_new[i2]);
				}
			}			

			if (  fabs(layer_residual_new[residual_order[i]]) > offset_tolerance && (rchi_wihtno_test_new[0] - rchi_wihtno_test[0])<0 && (rchi_wihtno_test_new[1] - rchi_wihtno_test[1])<0  && (rchi_wihtno_test_new[2] - rchi_wihtno_test[2])<0 && (hit_fit_rchi_new[4] - hit_fit_rchi[4])<0 )
			{
				// cout<<"good"<<endl;
				hit_fit_rchi.clear();
				hit_fit_rchi = hit_fit_rchi_new;
				hit_fit_rchi_new.clear();
				calibrate_sum -= amount_of_calibrate;

				if (ploting == true && event_id%info_sampling==0)
				{
					hit4_grr_align -> SetTitle(Form("%i",i));
					hit4_grr_align -> SetMarkerStyle(20);
					hit4_grr_align -> SetMarkerSize(2);
					hit4_grr_align -> GetXaxis()->SetLimits(-1,5);
					hit4_grr_align -> GetXaxis()->SetTitle("Layer ID");
					hit4_grr_align -> GetYaxis()->SetRangeUser(plot_Y_min,plot_Y_max);
					hit4_grr_align -> GetYaxis()->SetTitle("Y position (Unit : mm)");

					hit4_grr_align->Draw("apl");
					hit4_fit_align->Draw("lsame");

					hit3_fit_0->Draw("lsame");
					hit3_fit_1->Draw("lsame");
					hit3_fit_2->Draw("lsame");
					hit3_fit_3->Draw("lsame");

					TLatex *hit4_fit_latex = new TLatex();
					hit4_fit_latex -> SetNDC();
					hit4_fit_latex -> SetTextSize(0.028);	

					hit4_fit_latex -> DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit_align->GetParameter(1),hit4_fit_align->GetParameter(0)));
					hit4_fit_latex -> DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit_align->GetChisquare(),hit4_fit_align->GetNDF(),hit4_fit_align->GetChisquare()/double(hit4_fit_align->GetNDF())));
					hit4_fit_latex -> DrawLatex(0.12, 0.690-Legend_Y_offset,Form("l0-residual : %.5f",layer_residual[0]));
					hit4_fit_latex -> DrawLatex(0.12, 0.660-Legend_Y_offset,Form("l3-residual : %.5f",layer_residual[1]));

					hit4_fit_latex->DrawLatex(0.12, 0.750, Form("#chi^{2}/NDF : %.4f, exclude l0, G", hit_fit_rchi[0]));
					hit4_fit_latex->DrawLatex(0.12, 0.720, Form("#chi^{2}/NDF : %.4f, exclude l1, B", hit_fit_rchi[1]));
					hit4_fit_latex->DrawLatex(0.12, 0.690, Form("#chi^{2}/NDF : %.4f, exclude l2, P", hit_fit_rchi[2]));
					hit4_fit_latex->DrawLatex(0.12, 0.660, Form("#chi^{2}/NDF : %.4f, exclude l3, SB", hit_fit_rchi[3]));

					c2->Print(Form ("%s/folder_%s/alignment_plot/alignment_%i.pdf",folder_name.Data(),file_name.Data(),eID));
					c2->Clear();
				}

				for (int i2=0; i2<4; i2++) {layer_residual[i2] = layer_residual_new[i2];}

			}
			else 
			{
				// cout<<"bad"<<endl;
				if (ploting == true && event_id%info_sampling==0 )
				{
					hit4_grr_align -> SetTitle(Form("%i bad",i));
					hit4_grr_align -> SetMarkerStyle(20);
					hit4_grr_align -> SetMarkerSize(2);
					hit4_grr_align -> GetXaxis()->SetLimits(-1,5);
					hit4_grr_align -> GetXaxis()->SetTitle("Layer ID");
					hit4_grr_align -> GetYaxis()->SetRangeUser(plot_Y_min,plot_Y_max);
					hit4_grr_align -> GetYaxis()->SetTitle("Y position (Unit : mm)");

					hit4_grr_align->Draw("apl");
					hit4_fit_align->Draw("lsame");

					hit3_fit_0->Draw("lsame");
					hit3_fit_1->Draw("lsame");
					hit3_fit_2->Draw("lsame");
					hit3_fit_3->Draw("lsame");

					TLatex *hit4_fit_latex = new TLatex();
					hit4_fit_latex -> SetNDC();
					hit4_fit_latex -> SetTextSize(0.028);	

					hit4_fit_latex -> DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit_align->GetParameter(1),hit4_fit_align->GetParameter(0)));
					hit4_fit_latex -> DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit_align->GetChisquare(),hit4_fit_align->GetNDF(),hit4_fit_align->GetChisquare()/double(hit4_fit_align->GetNDF())));
					hit4_fit_latex -> DrawLatex(0.12, 0.690-Legend_Y_offset,Form("l0-residual : %.5f",layer_residual[0]));
					hit4_fit_latex -> DrawLatex(0.12, 0.660-Legend_Y_offset,Form("l3-residual : %.5f",layer_residual[1]));

					hit4_fit_latex->DrawLatex(0.12, 0.750, Form("#chi^{2}/NDF : %.4f, exclude l0, G", hit_fit_rchi_new[0]));
					hit4_fit_latex->DrawLatex(0.12, 0.720, Form("#chi^{2}/NDF : %.4f, exclude l1, B", hit_fit_rchi_new[1]));
					hit4_fit_latex->DrawLatex(0.12, 0.690, Form("#chi^{2}/NDF : %.4f, exclude l2, P", hit_fit_rchi_new[2]));
					hit4_fit_latex->DrawLatex(0.12, 0.660, Form("#chi^{2}/NDF : %.4f, exclude l3, SB", hit_fit_rchi_new[3]));

					c2->Print(Form ("%s/folder_%s/alignment_plot/alignment_%i.pdf",folder_name.Data(),file_name.Data(),eID));
					c2->Clear();
				}

				hit_position [residual_order[i]] += amount_of_calibrate;
				final_calibrate[residual_order[i]] = calibrate_sum;

				calibrate_sum = 0;
				hit_fit_rchi_new.clear();
				for (int i2=0; i2<4; i2++) hit_3_y_axis[i2].clear();
				rchi_wihtno_test.clear();
				rchi_wihtno_test_new.clear();
				break;
			}
			


			// for (int i=0; i<4; i++) {layer_residual[i] = ( hit_position[i] - (hit4_fit_align->GetParameter(1)* double(x_axis[i]) + hit4_fit_align->GetParameter(0)) );  }//the residual of each layer
			rchi_wihtno_test.clear();
			rchi_wihtno_test_new.clear();
			hit_fit_rchi_new.clear();
			for (int i2=0; i2<4; i2++) hit_3_y_axis[i2].clear();
		}

	}
	// cout<<"alignment_func_test"<<endl;
	if (ploting == true && event_id%info_sampling==0) {c2->Print(Form ("%s/folder_%s/alignment_plot/alignment_%i.pdf)",folder_name.Data(),file_name.Data(),eID));}
	c2->Clear();

	TGraph * hit4_grr_align_final = new TGraph (4,x_axis,&hit_position[0]);
	hit4_grr_align_final ->Fit("hit4_fit_align","NQ");

	vector<double> func_final_out; func_final_out.clear();
	final_calibrate[4] = hit4_fit_align->GetParameter(1); // final slope
	final_calibrate[5] = hit4_fit_align->GetParameter(0); // final offset,
	for (int i=0; i<6; i++) {func_final_out.push_back(final_calibrate[i]);}
	return func_final_out;

}

// it works for the 3 layers testbeam setup. 
// NOT 4 layers but one layer missing hit
// for the 3 points alignment, the idea is simple,
// 3 points can have 3 combinations of 2 points 
// we pick up the one with smallest slope, and correct the rest point back to this slope. 
vector <double> align_func_hit3 (vector <double> hit_position, double actual_xpos[4] )
{
	double x_axis[3];
	x_axis[0] = actual_xpos [0];
	x_axis[1] = actual_xpos [1];
	x_axis[2] = actual_xpos [2];

	double self_difference[3] = { hit_position[3], hit_position[4], hit_position[5] };//to save the self_difference

	int sort_self_difference_index[3];
	TMath::Sort(3, self_difference, sort_self_difference_index);
	int the_smallest_diff_index = sort_self_difference_index[2]; //the index with the smallest difference. it should be 0 or 1 or 2
	int aligned_layer;

	vector<double> hit2_xaixs;
	vector<double> hit2_yaixs;

	double final_correction[3];
	
	if      (the_smallest_diff_index == 0) aligned_layer = 2;
	else if (the_smallest_diff_index == 1) aligned_layer = 1;
	else if (the_smallest_diff_index == 2) aligned_layer = 0;
	
	// if index is 0 -> 2 is the outlier
 	// if index is 1 -> 1 is the outlier
 	// if index is 2 -> 0 is the outlier

 	// hit3_best_fit_picker_layer[6] = fabs(hit3_fit_Y[0]-hit3_fit_Y[1]);
	// hit3_best_fit_picker_layer[7] = fabs(hit3_fit_Y[0]-hit3_fit_Y[2]);
	// hit3_best_fit_picker_layer[8] = fabs(hit3_fit_Y[1]-hit3_fit_Y[2]);
	
	TF1 * hit2_fit = new TF1 ("hit2_fit","pol1",0,4); //MAYBE if we change to use the actual position, the fitting range has to be modified.

	for (int i=0; i<3; i++)
	{
		if (i == aligned_layer) {}
		else
		{
			hit2_xaixs.push_back(x_axis [i]);
			hit2_yaixs.push_back(hit_position [i]);
		}
	}

	TGraph * hit2_grr = new TGraph (2,&hit2_xaixs[0],&hit2_yaixs[0]);
	hit2_grr->Fit("hit2_fit","NQ");

	for (int i=0; i<3; i++)
	{
		if (i == aligned_layer) 
		{
			final_correction[i] = hit_position[i] - (hit2_fit->GetParameter(1)* x_axis[i] + hit2_fit->GetParameter(0));
		}
		else 
		{
			final_correction[i] = 0;	
		}
	}

	vector<double> array_output; array_output.clear();

	array_output.push_back(final_correction[0]); //the amount of correction of l0
	array_output.push_back(final_correction[1]); //the amount of correction of l1
	array_output.push_back(final_correction[2]); //the amount of correction of l2

	array_output.push_back(hit2_fit->GetParameter(1)); //2 points slope
	array_output.push_back(hit2_fit->GetParameter(0)); //2 points offset

	array_output.push_back(aligned_layer); // the layer to be aligned.

	return array_output;

	hit2_xaixs.clear();
	hit2_yaixs.clear();

}

//1 <-> 14
//2 <-> 15
//3 <-> 16
//4 <-> 17
//5 <-> 18
//6 <-> 19
//7 <-> 20
//8 <-> 21
//9 <-> 22
//10 <-> 23
//11 <-> 24
//12 <-> 25
//13 <-> 26
//1 to 13 : upper

//=======================================================================================================================
// detector geometry on G4:                                    
//                                                             (the position of first strip of upper chip in Y aixs : 0.055) 
//         -------------------------------------------  chan 0              /|\
//         | U3    U4     U5  |   U6     U7   U8      |                      |   Y axis
//         |                  |                       | chan 127             |  
// o  o o  |-----typeB-----------X-----typeA----------| o o o o o o    <-----|
//         |                  |                       | chan 127        x axis
//         | U16   U17    U18 |   U19    U20  U21     |
//         -------------------------------------------  chan 0 (the position of first strip in Y aixs :-9.961 mm )
//
//  (X : origin)
//========================================================================================================================

// the stopping line

void tracking_single () // by analyzing the tree_both
{
	//the name of the file you want to study, remember to remove the ".root"
	TString file_name = "electron_100W_1_convert_3p_l0_70_l2offset_filter";
	//the direction of the data 
	TString folder_direction = "/data4/chengwei/Geant4/INTT_simulation/G4/for_CW/data";
	//this line creates a folder to save all the informations and plot of this root file 
	system(Form("mkdir -p %s/folder_%s",folder_direction.Data(),file_name.Data()));

	TCanvas * c2 = new TCanvas ("c2","c2",1800,1800); // the canvas for alignment
	TCanvas * c1 = new TCanvas ("c1","c1",1800,1800); // the canvas for the rest plot

	TFile *f1 = new TFile (Form("%s/%s.root", folder_direction.Data(), file_name.Data()), "read");
	TTree *tree_in = (TTree *)f1->Get("tree");
	TTree *tree_both_in = (TTree *)f1->Get("tree_both");

	long event_N = tree_both_in -> GetEntriesFast();
	printf("# of event in file %s.root : %i \n",file_name.Data(),event_N);

	//====================================================================================================================================	
	
	int decoder = 0x0F;    //1111, the default setting of the event profile
 	
	int l4_miss_l0 = 0x07; //0111
	int l4_miss_l1 = 0x0b; //1011
	int l4_miss_l2 = 0x0d; //1101
	int l4_miss_l3 = 0x0e; //1110

	int l3_miss_l0 = 0x06; //0110 we still use 4 bits to contain the 3 layers case
	int l3_miss_l1 = 0x0a; //1010
	int l3_miss_l2 = 0x0c; //1100 

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	const int number_of_layer = 3; // number of layers of the INTT ladders in the Testbeam experiment. 
 	bool raw_plot_print = true; //whether you want to draw the event_display or not 
 	bool full_analysis = true;

 	int module_ID [4];
	module_ID[0] = 6; // module ID setting, if there are only 3 layers in the Testbeam, you don't need to change "l3", leave it alone
	module_ID[1] = 5;
	module_ID[2] = 8;
	module_ID[3] = 2;

	int adc_setting[9] = {15,30,60,90,120,150,180,210,240}; // adc setting 
	double adc_convert[8];
	for (int i=0; i<8; i++) {adc_convert[i] = (adc_setting[i]+adc_setting[i+1])/2.;}

	double actual_xpos [4] = {0,1,2,3}; // the actual ladder position (unit : mm), now I use layer index
	
	int camac_tdc_cutL = 550; //the camac_tdc cut,  left-hand side cut, 510 in principle
	int camac_tdc_cutR = 1200;//the camac_tdc cut, right-hand side cut

	bool DSE_on = false;// false -> no dse, true -> include double saving event 
	bool multi_cluster_on = true; // true -> consider the multi-cluster, false -> just the golden track 
	bool allow_cluster = true; //allow more than 1 hit forms a cluster. true-> allow. false -> not allow (only consider pure hit track)

	bool major_chip_on = true; // true -> the func. "majority_chip" is used, false -> the chip is choosed. 
	int  study_chip = 5; // (1 ~ 13) if major_chip_check is false, you have to give a chip_ID that you want to study
	
	bool camac_adc_on = true; // to use camac_adc cut or not, true -> camac_adc is a cut
	int camac_adc_cut = 1200; // the camac_adc cut
	
	int analysis_size = 2000000; // the # of event you want to study

	bool alignment_on = true; // the alignment setting, true -> we do the alignment study. It takes time 
	double correction_rate = 0.3; // the amount of position correction = current residual * correction_rate
	double offset_tolerance = 0.003; // If the residual < offset_tolerance, stop doing the correction
	bool ploting = true; // true : Print the detail correction plot

	double Legend_Y_offset = 0.5; // the value to tune the position of TLatex position in the event display plot
	int info_sampling = 5000; //to print the information per "info_sampling" event.
	bool show_bad_plot  = true; // to show the event display whose event profile is not perfect

	bool draw_hitmap = true; // to run the full analysis or not, or just check the overall plot

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	double alignment_array [4]={0,0,0,0};  //offset matrix of 4 layers, Y axis; ** if the target sensor type is changed, the alignment needs to be re-studied 
	
	// memset(alignment_array, 0, sizeof(alignment_array));
	
	double INTT_strip_width = 0.078;
	double lower_section_initial = -9.961;
	double upper_section_initial = 0.055;

	double noise_hit_distance = 5*INTT_strip_width; // the windows cut
	//====================================================================================================================================

	if (alignment_on == true && number_of_layer == 4 ) // the folder for all alignment plots
	{
		system(Form("mkdir -p %s/folder_%s/alignment_plot/",folder_direction.Data(),file_name.Data()));
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||hitmap|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	if (draw_hitmap == true)
	{
		TCanvas *c4 = new TCanvas("c4", "hit_map", 0, 700, 1625, 250);
		TH2D* hitmap = new TH2D("hitmap", "", 13, 0, 13, 257, 0, 257);
	
		c4->cd();
		c4->Print( Form("%s/folder_%s/hit_map.pdf(",folder_direction.Data(),file_name.Data()) );
	
		for (int i1=0; i1 < number_of_layer; i1++)
		{
			hitmap->SetTitle(Form("Hit map, module ID : %i; chip_id; chan_id ",module_ID[i1]));
			stringstream cut_base; // useful class to generate string using non-string and char parameters
			cut_base << "ampl<70 " << "&& module ==" << module_ID[i1] << Form(" && -1 < chan_id && chan_id < 128 && 0 < chip_id && chip_id<27 ");
	
			string expression = string("int(chip_id / 14) * 256") // it's offsets for y-axis.
				+ "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
				+ ": -chip_id"                                   // Basically, x value increases as chip_id decreases (chip13 at x=0, chip12 at x=-1, ...)
				+ "+ (1 + int(chip_id / 14)) * 13"               // In addition to above, offset is needed. 
				+ ">> hitmap";
	
			tree_in->Draw(expression.c_str(), cut_base.str().c_str(), "goff");
			hitmap->Draw("colz");
		
			hitmap->GetXaxis()->SetNdivisions(15);
			hitmap->GetXaxis()->SetLabelSize(0.08);
			hitmap->GetXaxis()->SetTitleSize(0.08);
			hitmap->GetYaxis()->SetLabelSize(0.08);
			hitmap->SetStats(0);
			// draw chip_id on the hist
			for (int i = 0; i < 26; i++) {
				TLatex* tex = new TLatex();
				double x = i < 13 ? 12.25 - i : 12.25 - i + 13;
				double y = i < 13 ? 64 : 192;
				tex->DrawLatex(x, y, Form("Chip%d", i + 1));
			}
	
	
			c4->Print( Form("%s/folder_%s/hit_map.pdf",folder_direction.Data(),file_name.Data()) );
			
			hitmap->Reset();
			c4->Clear();
		}
	
		c4->cd();
		c4->Print( Form("%s/folder_%s/hit_map.pdf)",folder_direction.Data(),file_name.Data()) );
	}
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||hitmap|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	
	// the histogram for number of hit of each event 
	TH1F * event_hit_distribution = new TH1F ("","event hit distribution",21,0,21);
	event_hit_distribution->GetXaxis()->SetTitle("# of hits in this event");
	event_hit_distribution->GetYaxis()->SetTitle("entry");

	TH1F * adc_distribution [4][26];
	for (int i = 0; i < number_of_layer; i++)
	{
		for (int i1=0; i1<26; i1++)
		{
			adc_distribution [i][i1]= new TH1F ("",Form("%i-%i",module_ID[i],i1+1),8,0,8);
		}
	}

	//1000 = 0x08 layer=0;
	//0100 = 0x04 layer=1;
	//0010 = 0x02 layer=2;
	//0001 = 0x01 layer=3;
	//1111 = 0x0f
	

	// In the beginning of the macro (until the line "data loading finish" @ ~ line 966),
	// All the data has to pass the l1 selection,
	// If the event passes the l1 selection, it can be stored in the 3-dimensional vectors. 
	// the 3-dimensional vector contains all the data [event][layer][info.]
	vector<vector<vector<int>>> event_chip; 
	vector<vector<vector<int>>> event_chan;
	vector<vector<vector<int>>> event_adc;
	vector<vector<vector<int>>> event_event;
	vector<int> eID_array; eID_array.clear();

	vector<vector<int>> event_chip_to_function;

	vector<int> hit_chip_0;
	vector<int> hit_chan_0;
	vector<int> hit_adc_0;
	vector<int> hit_event_0;

	vector<int> hit_chip_1;
	vector<int> hit_chan_1;
	vector<int> hit_adc_1;
	vector<int> hit_event_1;

	vector<int> hit_chip_2;
	vector<int> hit_chan_2;
	vector<int> hit_adc_2;
	vector<int> hit_event_2;

	vector<int> hit_chip_3;
	vector<int> hit_chan_3;
	vector<int> hit_adc_3;
	vector<int> hit_event_3;

	vector<vector<int>> layer_chip;
	vector<vector<int>> layer_chan;
	vector<vector<int>> layer_adc;
	vector<vector<int>> layer_event;

	hit_chip_0.clear();
	hit_chan_0.clear();
	hit_adc_0.clear();
	hit_event_0.clear();

	hit_chip_1.clear();
	hit_chan_1.clear();
	hit_adc_1.clear();
	hit_event_1.clear();

	hit_chip_2.clear();
	hit_chan_2.clear();
	hit_adc_2.clear();
	hit_event_2.clear();

	hit_chip_3.clear();
	hit_chan_3.clear();
	hit_adc_3.clear();
	hit_event_3.clear();

	layer_chip.clear();
	layer_event.clear();
	layer_adc.clear();
	layer_chan.clear();

	event_chip.clear();
	event_chan.clear();
	event_adc.clear();
	event_event.clear();

	event_chip_to_function.clear();


	int event_length;
	vector<int> * camac_tdc = new vector<int>();
	vector<int> * camac_adc = new vector<int>();
	bool INTT_event;
	vector<int> * adc = new vector<int>();
	vector<int> * chip_id = new vector<int>();
	vector<int> * module = new vector<int>();
	vector<int> * chan_id = new vector<int>();
	vector<int> * event = new vector<int>();
	bool DSE;
	int eID;

	tree_both_in -> SetBranchAddress ("nele",&event_length);

	tree_both_in -> SetBranchAddress ("camac_adc",&camac_adc);
	tree_both_in -> SetBranchAddress ("camac_tdc",&camac_tdc);
	tree_both_in -> SetBranchAddress ("INTT_event",&INTT_event);

	tree_both_in -> SetBranchAddress ("adc",&adc);
	tree_both_in -> SetBranchAddress ("chip_id",&chip_id);
	tree_both_in -> SetBranchAddress ("module",&module);
	tree_both_in -> SetBranchAddress ("chan_id",&chan_id);
	tree_both_in -> SetBranchAddress ("event",&event);

	tree_both_in -> SetBranchAddress ("DSE",&DSE);
	tree_both_in -> SetBranchAddress ("eID",&eID);

	printf("------------------------- data loading start -------------------------\n");
	for (int i=0; i<event_N; i++)
	{
		if (i%info_sampling==0) printf("loading data : %i \n",i);
		tree_both_in -> GetEntry (i);

		// the level-1 selection is shown below
		if (INTT_event == 1 && camac_tdc->at(5)>camac_tdc_cutL && camac_tdc->at(5)<camac_tdc_cutR) //INTT_event, camac_tdc range cut
		{
			if (camac_adc_on == true ) // true -> the consider camac_adc as a cut as well
			{
				if (camac_adc->at(0) > camac_adc_cut || camac_adc->at(1) > camac_adc_cut || camac_adc->at(2) > camac_adc_cut)
				{
					cout<<"----WWW---- the event is skipped due to some large camac_adc, eID : "<<eID<<" ----WWW----"<<endl;
					continue;
				}
			}

			if (DSE_on == false && DSE == 1) // false -> we don't want the double-saving event
			{
				cout<<"----WWW---- the event is skipped due to DSE, eID : "<<eID<<" ----WWW----"<<endl;
				continue;
			}

			event_hit_distribution->Fill(event_length);

			if (event_length<2) // # of hit is not enough to form a track
			{
				cout<<"----WWW---- the event is skipped due to only "<<event_length<< " hit, eID : "<<eID<<" ----WWW----"<<endl;
				continue;
			}

			// if (allow_cluster == false && event_length > number_of_layer) // the cut for the tighter selection, 3 or 4 hits, no cluster
			// {
			// 	cout<<"----WWW---- the event is skipped due to not pure hit track candidate, hit : "<<event_length<< " eID : "<<eID<<" ----WWW----"<<endl;
			// 	continue;
			// }

			for (int i1=0; i1<event_length; i1++)
			{
				if      (module->at(i1) == module_ID[0])
				{
					hit_adc_0.push_back(adc->at(i1));
					hit_event_0.push_back(event->at(i1));
					hit_chan_0.push_back(chan_id->at(i1));
					hit_chip_0.push_back(chip_id->at(i1));
					adc_distribution[0][chip_id->at(i1)-1]->Fill(adc->at(i1));
				}
				else if (module->at(i1) == module_ID[1])
				{
					hit_adc_1.push_back(adc->at(i1));
					hit_event_1.push_back(event->at(i1));
					hit_chan_1.push_back(chan_id->at(i1));
					hit_chip_1.push_back(chip_id->at(i1));
					adc_distribution[1][chip_id->at(i1)-1]->Fill(adc->at(i1));
				}
				else if (module->at(i1) == module_ID[2])
				{
					hit_adc_2.push_back(adc->at(i1));
					hit_event_2.push_back(event->at(i1));
					hit_chan_2.push_back(chan_id->at(i1));
					hit_chip_2.push_back(chip_id->at(i1));
					adc_distribution[2][chip_id->at(i1)-1]->Fill(adc->at(i1));
				}
				else if (module->at(i1) == module_ID[3])
				{
					hit_adc_3.push_back(adc->at(i1));
					hit_event_3.push_back(event->at(i1));
					hit_chan_3.push_back(chan_id->at(i1));
					hit_chip_3.push_back(chip_id->at(i1));
					adc_distribution[3][chip_id->at(i1)-1]->Fill(adc->at(i1));
				}
			}

			layer_chan.push_back(hit_chan_0);
			layer_chan.push_back(hit_chan_1);
			layer_chan.push_back(hit_chan_2);
			layer_chan.push_back(hit_chan_3);

			layer_adc.push_back(hit_adc_0);
			layer_adc.push_back(hit_adc_1);
			layer_adc.push_back(hit_adc_2);
			layer_adc.push_back(hit_adc_3);

			layer_chip.push_back(hit_chip_0);
			layer_chip.push_back(hit_chip_1);
			layer_chip.push_back(hit_chip_2);
			layer_chip.push_back(hit_chip_3);

			layer_event.push_back(hit_event_0);
			layer_event.push_back(hit_event_1);
			layer_event.push_back(hit_event_2);
			layer_event.push_back(hit_event_3);

			event_event.push_back(layer_event);
			event_adc.push_back(layer_adc);
			event_chan.push_back(layer_chan);
			event_chip.push_back(layer_chip);

			eID_array.push_back(eID); // to save the eID of the event pass all cuts

			hit_chip_0.clear();
			hit_chan_0.clear();
			hit_adc_0.clear();
			hit_event_0.clear();

			hit_chip_1.clear();
			hit_chan_1.clear();
			hit_adc_1.clear();
			hit_event_1.clear();

			hit_chip_2.clear();
			hit_chan_2.clear();
			hit_adc_2.clear();
			hit_event_2.clear();

			hit_chip_3.clear();
			hit_chan_3.clear();
			hit_adc_3.clear();
			hit_event_3.clear();

			layer_chip.clear();
			layer_event.clear();
			layer_adc.clear();
			layer_chan.clear();

		}
		if (i==analysis_size)break;
	}

	// data.Close();
	printf("------------------------- data loading finish ------------------------\n");
	printf("\n");

	

	TCanvas * c3 = new TCanvas ("c3","c3",1800,1800); // the canvas for event_hit_distribution
	c3->cd();
	c3->SetLogy();
	event_hit_distribution->Draw("hist");
	c3->Print( Form("%s/folder_%s/event_hit_distribution.pdf",folder_direction.Data(),file_name.Data()) );
	c3->Clear();

	// the cancas for the chip adc distribution 
	TCanvas *c5 = new TCanvas("c5", "channel to adc", 1625, 250);
	c5->Divide(13, 2);

	c5->Print( Form("%s/folder_%s/chip_adc_distribution.pdf(",folder_direction.Data(),file_name.Data()) );		
	for (int i=0; i<number_of_layer; i++)
	{
		if (i!=0)c5->Divide(13, 2);
		for (int i1=0; i1< 26; i1++)
		{
			c5->cd( 26 - i1 );
			adc_distribution[i][i1]->Draw("hist");
		}
		c5->Print( Form("%s/folder_%s/chip_adc_distribution.pdf",folder_direction.Data(),file_name.Data()) );
		c5->Clear();
	}

	c5->Print( Form("%s/folder_%s/chip_adc_distribution.pdf)",folder_direction.Data(),file_name.Data()) );


	// delete the vector and close the root file to save the memory
	delete camac_tdc;
	delete camac_adc;
	delete adc;
	delete chip_id;
	delete module;
	delete chan_id;
	delete event;
	f1 -> Close();

	c1->cd();

	// for (int i=0; i<event_event.size(); i++)//event
	// {
	// 	for (int j=0; j<event_event[i].size(); j++)//layer
	// 	{
	// 		for (int k=0; k<event_event[i][j].size(); k++)//hits on this layer
	// 		{
	// 			cout<<event_event[i][j][k]<<" "<<event_chip[i][j][k]<<" "<<event_chan[i][j][k]<<" "<<event_adc[i][j][k]<<endl;
	// 		}

	// 	}
	// 	printf("\n");
	// }
	if (full_analysis == true)
	{
		//data_all[layer][chip],it saves one event per loop, and saves the data in different format
		vector<int> data_all[4][26]; 
		for (int i=0; i<4; i++)
		{
			for (int i1=0; i1<13; i1++)
			{
				data_all[i][i1].clear();
			}
		}
		vector<vector<double>> data_all_MC_EW; data_all_MC_EW.clear(); //data_all Most hit Chip, Edep Weighted. -> to find the positions of clusters 
		TGraph* grr;

		vector<int>most_hit_info; most_hit_info.clear(); //this vector associates with the function "majority_chip", it provide the vote winner chip
		int pass_l1_counting =0; // the counter to count the number of event pass the l2 selection
		double hit3_layer[3];
		double hit3_fit_Y[3];
		int first_layer_chan;
		int second_layer_chan;
		int third_layer_chan;
		int fourth_layer_chan;
		double hit3_best_fit_picker=100000000000;
		double hit3_best_fit_picker_layer[14];
		double hit3_worse_hit_picker=0;
		int hit3_worse_hit_picker_layer;
		int hit3_fit_loop=0;
		double hit3_second_bad_picker_big=100000;
		int hit3_second_bad_picker;
		vector<double>hit3_final_small_X; hit3_final_small_X.clear();
		vector<vector<double>>hit3_final_X; hit3_final_X.clear();
		vector<double>hit3_final_small_Y; hit3_final_small_Y.clear();
		vector<vector<double>>hit3_final_Y; hit3_final_Y.clear();
		
		vector<double> for_final_alignment; for_final_alignment.clear(); 
		vector <double> get_align_result; get_align_result.clear();

		int hit3_final_pickup_ID;
		double hit3_actual_xpos[3];
		vector<TGraph*> hit3_grr; hit3_grr.clear();

		vector<TGraph*> hit4_grr; hit4_grr.clear();
		double hit4_layer[4]={0,1,2,3};
		double hit4_fit_Y[4];
		double hit4_best_fit_picker=100000000000;
		double hit4_best_fit_picker_layer[14];
		double hit4_worse_hit_picker=0;
		double hit4_first_bad_picker_big=0;
		double hit4_first_bad_picker;
		int hit4_worse_hit_picker_layer;
		int hit4_fit_loop=0;
		vector<double>hit4_final_small_X; hit4_final_small_X.clear();
		vector<vector<double>>hit4_final_X; hit4_final_X.clear();
		vector<double>hit4_final_small_Y; hit4_final_small_Y.clear();
		vector<vector<double>>hit4_final_Y; hit4_final_Y.clear();
		// vector<double> hit4_final_pickup; hit4_final_pickup.clear(); 
		int hit4_final_pickup_ID;
		double hit4_second_residual[3];
		double hit4_second_bad_pciker[6];
		int hit4_second_residual_counter=0;
		double hit4_second_bad_pciker_big=10000000;
		int hit4_second_bad_pciker_id;

		double hit2_point_difference;
		double hit2_point_comparator=100000000;

		// the vector final_pickup is a checking vector
		// for the 3 and 4 hits cases, it can have have more than 1 combination for the fitting, if some of layers have multi-clusters
		// I use fit to find out the best track (minimal r-chi2)
		// this vector saves the ID of the nth trial that has minimal r-chi2
		vector<double> final_pickup; final_pickup.clear();

		vector <double> hit4_secondfit_X; hit4_secondfit_X.clear();
		vector <double> hit4_secondfit_X_ID; hit4_secondfit_X_ID.clear();
		vector <double> hit4_secondfit_Y; hit4_secondfit_Y.clear();

		vector<int> event_quality; event_quality.clear(); // to save the profile of each event 
		vector<int> event_quality_ID; event_quality_ID.clear(); // to save the ID of the event pass the l2 selection 

		int hit_layer_counting = 0;

		double alignment_l0;
		double alignment_l1;
		double alignment_l2;
		double alignment_l3;
		double fit_slope;
		double fit_offset;
		double chosen_layer;

		double origin_l0;
		double origin_l1;
		double origin_l2;
		double origin_l3;
		double origin_fit_slope;
		double origin_fit_offset;

		double origin_slope_2; // this is for hit3 case, to check the 2 points slope
		double origin_slope_1;
		double origin_slope_0;

		int    align_eID;

		// the Histogram of Y axis residual of each layer, without the  alignment
		TH1F * TH1_hit4_layer0_Y_residual = new TH1F ("","layer0 Y axis residual",50,-0.5,0.5);
		TH1_hit4_layer0_Y_residual->GetXaxis()->SetTitle("residual (mm)");

		TH1F * TH1_hit4_layer1_Y_residual = new TH1F ("","layer1 Y axis residual",50,-0.5,0.5);
		TH1_hit4_layer1_Y_residual->GetXaxis()->SetTitle("residual (mm)");

		TH1F * TH1_hit4_layer2_Y_residual = new TH1F ("","layer2 Y axis residual",50,-0.5,0.5);
		TH1_hit4_layer2_Y_residual->GetXaxis()->SetTitle("residual (mm)");

		TH1F * TH1_hit4_layer3_Y_residual = new TH1F ("","layer3 Y axis residual",50,-0.5,0.5);
		TH1_hit4_layer3_Y_residual->GetXaxis()->SetTitle("residual (mm)");

		//the histogram for Y axis alignment study
		TH1F * TH1_hit4_layer0_Y_alignment = new TH1F ("","layer0 Y axis alignment",50,-0.5,0.5);
		TH1_hit4_layer0_Y_alignment->GetXaxis()->SetTitle("correction (mm)");

		TH1F * TH1_hit4_layer1_Y_alignment = new TH1F ("","layer1 Y axis alignment",50,-0.5,0.5);
		TH1_hit4_layer1_Y_alignment->GetXaxis()->SetTitle("correction (mm)");

		TH1F * TH1_hit4_layer2_Y_alignment = new TH1F ("","layer2 Y axis alignment",50,-0.5,0.5);
		TH1_hit4_layer2_Y_alignment->GetXaxis()->SetTitle("correction (mm)");

		TH1F * TH1_hit4_layer3_Y_alignment = new TH1F ("","layer3 Y axis alignment",50,-0.5,0.5);
		TH1_hit4_layer3_Y_alignment->GetXaxis()->SetTitle("correction (mm)");

		TH1F * TH1_hit4_aligned_slope = new TH1F ("","post-alignment fitting slope",50,-0.5,0.5);
		TH1_hit4_aligned_slope->GetXaxis()->SetTitle("slope");

		// the fitting function for 3 hits case, for the 2 hits we don't need to fit 
		TF1 * fit_basis;
		vector<TF1*> hit3_fit; hit3_fit.clear(); 

		TLatex *hit3_fit_latex = new TLatex();
		hit3_fit_latex->SetNDC();
		hit3_fit_latex->SetTextSize(0.028);

		//the fitting function for 4 hits case
		vector<TF1*> hit4_fit; hit4_fit.clear();

		TLatex *hit4_fit_latex = new TLatex();
		hit4_fit_latex->SetNDC();
		hit4_fit_latex->SetTextSize(0.028);

		//the output root file of the alignment result 
		TFile * file_output;
		TTree *tree_output;
		if (alignment_on == true && number_of_layer == 4) 
		{
			file_output = new TFile(Form("%s/folder_%s/alignment_result_data.root",folder_direction.Data(),file_name.Data()), "RECREATE");
			tree_output = new TTree("alignment_Y", "alignment_Y");
			tree_output->Branch("l0"   ,&alignment_l0); //alignment result
			tree_output->Branch("l1"   ,&alignment_l1);
			tree_output->Branch("l2"   ,&alignment_l2);
			tree_output->Branch("l3"   ,&alignment_l3);
			tree_output->Branch("fit_slope"   ,&fit_slope); // slope after position correction
			tree_output->Branch("fit_offset"   ,&fit_offset); // offset after position correction

			tree_output->Branch("origin_l0"   ,&origin_l0); // the original positions
			tree_output->Branch("origin_l1"   ,&origin_l1);
			tree_output->Branch("origin_l2"   ,&origin_l2);
			tree_output->Branch("origin_l3"   ,&origin_l3);
			tree_output->Branch("origin_fit_slope"    ,&origin_fit_slope); // the slope before position correction
			tree_output->Branch("origin_fit_offset"   ,&origin_fit_offset);	// the offset before position correction

			tree_output->Branch("eID"  ,&align_eID);
		}
		else if (alignment_on == true && number_of_layer == 3) // the branch setting of the 3 layer testbeam 
		{
			file_output = new TFile(Form("%s/folder_%s/alignment_result_data.root",folder_direction.Data(),file_name.Data()), "RECREATE");
			tree_output = new TTree("alignment_Y", "alignment_Y");
			tree_output->Branch("l0"   ,&alignment_l0); //alignment result
			tree_output->Branch("l1"   ,&alignment_l1);
			tree_output->Branch("l2"   ,&alignment_l2);

			tree_output->Branch("fit_slope"   ,&fit_slope); // slope after position correction
			tree_output->Branch("fit_offset"   ,&fit_offset); // offset after position correction

			tree_output->Branch("origin_l0"   ,&origin_l0); // the original positions
			tree_output->Branch("origin_l1"   ,&origin_l1);
			tree_output->Branch("origin_l2"   ,&origin_l2);

			tree_output->Branch("chosen_layer"   ,&chosen_layer); // the layer to be corrected. 

			tree_output->Branch("origin_slope_2"   ,&origin_slope_2); // the original slope
			tree_output->Branch("origin_slope_1"   ,&origin_slope_1);
			tree_output->Branch("origin_slope_0"   ,&origin_slope_0);		

			tree_output->Branch("origin_fit_slope"    ,&origin_fit_slope); // the slope before position correction
			tree_output->Branch("origin_fit_offset"   ,&origin_fit_offset);	// the offset before position correction

			tree_output->Branch("eID"  ,&align_eID);
		}

		// here the setting of the event selection is shown 
		cout<<" "<<endl;
		cout<<"/=/=/=/=/=/=/=/=/=/=  the selection setting  "<<"/=/=/=/=/=/=/=/=/=/=/"<<endl;
		cout<<"||"<<endl;
		cout<<"|| number of layer          : "<<number_of_layer<<endl;
		cout<<"||"<<endl;
		cout<<"|| l1, INTT_event           : true "<<"	(true : this cut is applied)"<<endl;
		cout<<"|| l1, camac_tdc6 range cut : true "<<"	(true : this cut is applied)"<<endl;
		cout<<"|| l1, hit cut (# of hit <2): true "<<endl;
		cout<<"|| l1, camac_adc max cut    : "<<camac_adc_on<<"	(1 : adc cut is applied) "<<endl;
		cout<<"|| l1, double saving event  : "<<DSE_on<<"	(0 : DSE is excluded) "<<endl;
		cout<<"|| "<<endl;
		cout<<"|| l2, multi-cluster event  : "<<multi_cluster_on<<" (0 : only golden track are considered) "<<endl;
		cout<<"|| l2, cluster cut          : true  (# of cluster < 2)"<<endl;
		cout<<"|| l2, pure hit track       : "<<allow_cluster<<" (0 : consider only pure hit track) "<<endl;
		cout<<"||"<<endl;
		cout<<"/=/=/=/=/=/=/=/=/=/=  the selection setting  "<<"/=/=/=/=/=/=/=/=/=/=/"<<endl;
		cout<<""<<endl;
		cout<<"/=/=/=/=/=/=/=/= number of event pass l1 selection : "<<event_event.size()<<" /=/=/=/=/=/=/=/=/"<<endl;
		cout<<""<<endl;
		
		//to print the event display plot
		if (raw_plot_print==true){c1->cd (); c1->Print( Form("%s/folder_%s/event_display.pdf(",folder_direction.Data(),file_name.Data()) );}
		

		// here we start to do the event analysis 
		// the level-1 selection is a overall selection, 
		// the level-2 selection can check the event quality in a more detail level 
		for (int i=0; i<event_event.size(); i++)
		{
			if (i%info_sampling==0) {printf("~~~~~~~ we are working on loop_i : %i, eID : %i, PLC candidate : %i \n",i,eID_array[i],pass_l1_counting );}

			for (int i1=0; i1<4; i1++) // to save the data in data_all[layer][chip], it makes us easier to do the analysis
			{
				for (int i2=0; i2<event_event[i][i1].size(); i2++) //the # of hit of this layer of this event
				{
					// I use a new coordinate to store the data
					// Example : U18 is now set to be Slot5 but with channel ID 0 ~ 127, U5 is still Slot5 but with channel ID 128 ~ 255
					// Because U5 is the upper chip in the G4 testbeam setup, if the setup is different, the coordinate has to be changed. 
					// Example : U5 chan_id 127 -> Slot5 chan_id 128, U5 chan_id 10 -> Slot5 chan_id 245
					// Example : U18 chan_id 0 -> Slot5 chan_id 0
					// I combine the adc information and channel information together by the function adc*1000+channel_ID-> it's a integer
					// Example adc 3, channel ID 47 -> 3047
					// By doing so, I don't need to add one more dimension
					// if the orientation is different with the G4 model the line below has to be modified.
					if (event_chip[i][i1][i2]-1<13)//0 ~ 12
					{
						data_all[i1][ event_chip[i][i1][i2]-1 ].push_back(event_adc[i][i1][i2]*1000 + (255-event_chan[i][i1][i2]) ); //adc *1000 + chan info
					}
					else if (event_chip[i][i1][i2]-1>12 && event_chip[i][i1][i2]-1 < 26) //13 ~ 25
					{
						data_all[i1][ event_chip[i][i1][i2]-1 - 13 ].push_back(event_adc[i][i1][i2]*1000 + event_chan[i][i1][i2] ); //adc *1000 + chan info
					}
					
				}
			}
			
			event_chip_to_function = event_chip [i]; //to save the chip information of a single event in a 2D vector, to reduce the required memory. 
			// the function "majority_chip" can find out the slot with most votes
			most_hit_info = majority_chip(event_chip_to_function,major_chip_on,study_chip);
			if (i%info_sampling==0){cout<<"~~~~~~~ before selection loop_i  : "<<i<<", eID : "<<eID_array[i]<<" most hit chip : "<<most_hit_info[0]<<" number : "<<most_hit_info[1]<<" missing_hitL : "<<most_hit_info[2]<<""<<endl;}
			//the function "data_all_edep_weight" can find out the cluster and also the position of each cluster
			data_all_MC_EW = data_all_edep_weight(data_all,most_hit_info,adc_convert,alignment_array);
			
			// the first level-2 selection : golden track. It is optional
			if (multi_cluster_on == false) // false -> we only consider the golden tracks
			{
				if (data_all_MC_EW[0].size() > 1 || data_all_MC_EW[1].size()>1 || data_all_MC_EW[2].size()>1 || data_all_MC_EW[3].size()>1)
				{
					cout<<"----WWW---- the event is skipped due to multi cluster, event ID : "<<eID_array[i]<<" ----WWW----"<<endl;
					
					for (int i10=0; i10<4; i10++)
					{
						for (int i11=0; i11<26; i11++)
						{
							data_all[i10][i11].clear();
						}
					}
					event_chip_to_function.clear();
					most_hit_info.clear();
					data_all_MC_EW.clear();
					continue;
				}
			}
			//the pure hit track selection, the second level-2 selection
			if (allow_cluster == false && (data_all_MC_EW[0].size()+data_all_MC_EW[1].size()+data_all_MC_EW[2].size()+data_all_MC_EW[3].size()) != (data_all[0][most_hit_info[0]-1].size()+data_all[1][most_hit_info[0]-1].size()+data_all[2][most_hit_info[0]-1].size()+data_all[3][most_hit_info[0]-1].size()) )
			{
				cout<<"----WWW---- the event is skipped due to not pure hit track, event ID : "<<eID_array[i]<<" ----WWW----"<<endl;

				for (int i10=0; i10<4; i10++)
				{
					for (int i11=0; i11<26; i11++)
					{
						data_all[i10][i11].clear();
					}
				}
				event_chip_to_function.clear();
				most_hit_info.clear();
				data_all_MC_EW.clear();
				continue;
			}

			// the third level-2 selection : the event with 1 or 0 cluster in total will be skipped.
			for (int i10=0; i10<4; i10++) {if (data_all_MC_EW[i10].size()>0){hit_layer_counting+=1;}}
			if (hit_layer_counting<2)
			{
				cout<<"----WWW---- the event is skipped due to only 0 or 1 cluster, event ID : "<<eID_array[i]<<" ----WWW----"<<endl;
				
				for (int i10=0; i10<4; i10++)
				{
					for (int i11=0; i11<26; i11++)
					{
						data_all[i10][i11].clear();
					}
				}
				event_chip_to_function.clear();
				most_hit_info.clear();
				data_all_MC_EW.clear();
				hit_layer_counting=0;
				continue;
			}
			

			/* =================================================== ana prepare end =================================================== */

			if (most_hit_info[1] < 2)// the fourth level-2 selection : only 0 or 1 cluster in the sutdy chip (slot).  
			{
				cout<<"----WWW---- the event is skipped due to no enough vote in major chip, event ID : "<<eID_array[i]<<" ----WWW----"<<endl;
			}
			else // for the case passes all l1 and l2 selection -> 2 or 3 or 4 layers have clusters in the study chip (slot). 
			{
				event_quality.push_back(decoder);// for the event pass l1 & l2 selection it will get a event default profile, 1111. 
				event_quality_ID.push_back(eID_array[i]); // this vector helps me to check the raw data
				if (pass_l1_counting%info_sampling==0){cout<<"~~~~~~~ after l2 selection loop_i: "<<i<<", eID : "<<eID_array[i]<<" PLC "<<pass_l1_counting<<" most hit chip : "<<most_hit_info[0]<<" number : "<<most_hit_info[1]<<" missing_hitL : "<<most_hit_info[2]<<""<<endl;}
				// cout<<"--------- test : decoder"<<std::bitset<4>(event_quality[pass_l1_counting])<<endl;
				
				// now we have 3 possible cases, 2 or 3 or 4 layers have hits at the study chip (slot)
				// so I separate them into 3 different case.
				// "most_hit_info[1]" this tells us how many layers have cluster at the study chip (slot)
				if      (most_hit_info[1]==2)// suitable for 3 or 4 layer case -> 2
				{
					// for the 2 layers have clusters case, fitting doesn't work. 
					// because the r-chi2 is always 0 for the fitting of 2 points case. 
					// so here I use the numerical way to calculate the difference of the positions of the cluster at different layer
					// the one with minimal difference will be picked up. 
					// if the difference > the setting cut -> the event profile : 0000

					// the most_hit_info[2] and most_hit_info[3] can tell us the ID of the layer that has no hit at study chip (slot)
					// then we change the layer profile of those layers by 0 (the function of loop below)
					for (int i7=0; i7<2; i7++)
					{
						if      (most_hit_info[i7+2]==0)// no major chip hit on layer0
						{
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
						}
						else if (most_hit_info[i7+2]==1)// no major chip hit on layer1
						{
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
						} 
						else if (most_hit_info[i7+2]==2)// no major chip hit on layer2
						{
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
						}
						else if (most_hit_info[i7+2]==3)// no major chip hit on layer3
						{
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
						}
					}

					// the loop below calculates the difference of the cluster position at different layers
					// the combination with minimal difference will be picked up
					for (int i7=0; i7 < data_all_MC_EW[ most_hit_info[4] ].size(); i7++ )
					{
						for (int i8=0; i8 < data_all_MC_EW[ most_hit_info[5] ].size(); i8++)
						{
							hit2_point_difference = fabs( data_all_MC_EW[ most_hit_info[4] ][i7] - data_all_MC_EW[ most_hit_info[5] ][i8] );
							if (hit2_point_difference < hit2_point_comparator)
							{
								hit2_point_comparator = hit2_point_difference;
							}
						}
					}

					// then we check whether the difference is smaller than the setting cut
					if (hit2_point_comparator > noise_hit_distance) 
					{
						event_quality[pass_l1_counting]=0x00;
					}
					hit2_point_comparator = 100000000;

					//becase for the 2 hits case, it doesn't require fitting. the final_pickup number is 2222
					final_pickup.push_back(2222);
				}
				else if (most_hit_info[1]==3)// suitable for 3 or 4 layer case -> 3
				{
					//for the 3 hits case, we first find out the layer with no hit
					//for the layer with no hit, the profile of that layer has to be 0
					if      (most_hit_info[2]==0)// no major chip hit on layer0
					{
						event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
						hit3_layer[0]=1;
						hit3_layer[1]=2;
						hit3_layer[2]=3;
						
					}
					else if (most_hit_info[2]==1)// no major chip hit on layer1
					{
						event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
						hit3_layer[0]=0;
						hit3_layer[1]=2;
						hit3_layer[2]=3;
					} 
					else if (most_hit_info[2]==2)// no major chip hit on layer2
					{
						event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
						hit3_layer[0]=0;
						hit3_layer[1]=1;
						hit3_layer[2]=3;
					}
					else if (most_hit_info[2]==3)// no major chip hit on layer3
					{
						event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
						hit3_layer[0]=0;
						hit3_layer[1]=1;
						hit3_layer[2]=2;
					}
					
					hit3_actual_xpos[0] = actual_xpos[ int (hit3_layer[0]) ]; // the actual position of the ladders in x axis
					hit3_actual_xpos[1] = actual_xpos[ int (hit3_layer[1]) ]; // to avoid the problem of non- isometric layer gap
					hit3_actual_xpos[2] = actual_xpos[ int (hit3_layer[2]) ];

					//the nasted for loop fits all the combinations and only the one with minimal r-chi2 is picked up
					for (int l1=0; l1<data_all_MC_EW[ int(hit3_layer[0]) ].size(); l1++)
					{
						for (int l2=0; l2<data_all_MC_EW[ int(hit3_layer[1]) ].size(); l2++)
						{
							for (int l3=0; l3<data_all_MC_EW[ int (hit3_layer[2]) ].size(); l3++)
							{
								hit3_fit_Y[0] = data_all_MC_EW[ int(hit3_layer[0]) ][l1];
								hit3_fit_Y[1] = data_all_MC_EW[ int(hit3_layer[1]) ][l2];
								hit3_fit_Y[2] = data_all_MC_EW[ int(hit3_layer[2]) ][l3];

								hit3_fit.push_back(fit_basis);
								hit3_fit[ int (hit3_fit.size())-1 ] = new TF1(Form("hit3_fit_%i",int (hit3_fit.size())-1),"pol1",-1,10);

								hit3_grr.push_back(grr);
								hit3_grr[ int (hit3_grr.size())-1 ] = new TGraph (3,hit3_actual_xpos,hit3_fit_Y); // MAYBE the hit3_layer has to be replaced by hit3_actual_xpos. then the x axis range, fit range has to be modified as well.
								hit3_grr[ int (hit3_grr.size())-1 ] ->Fit(Form("hit3_fit_%i",int (hit3_fit.size())-1),"NQ");
								
								// the plot of each trial 
								// if (raw_plot_print==true && pass_l1_counting%info_sampling==0)
								if (raw_plot_print == true)
								{
									hit3_grr[ int (hit3_grr.size())-1 ] ->SetTitle( Form("3 hit PLC : %d-%d, eID : %d ,chip : %d, position :%.3f, %.3f, %.3f",pass_l1_counting,hit3_fit_loop,eID_array[i],most_hit_info[0],hit3_fit_Y[0],hit3_fit_Y[1],hit3_fit_Y[2]) );
									hit3_grr[ int (hit3_grr.size())-1 ] ->SetMarkerStyle(20);
									hit3_grr[ int (hit3_grr.size())-1 ] ->SetMarkerSize(2);
									hit3_grr[ int (hit3_grr.size())-1 ] ->GetXaxis()->SetLimits(-1,5);
									hit3_grr[ int (hit3_grr.size())-1 ] ->GetXaxis()->SetTitle("Layer ID");
									hit3_grr[ int (hit3_grr.size())-1 ] ->GetYaxis()->SetRangeUser(-10,10);
									hit3_grr[ int (hit3_grr.size())-1 ] ->GetYaxis()->SetTitle("Y position (Unit : mm)");
									// c1->cd();
									// hit3_grr[ int (hit3_grr.size())-1 ]->Draw("apl");
									// hit3_fit->Draw("l same");
									// c1->cd();
									// hit3_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit3_fit->GetParameter(1),hit3_fit->GetParameter(0)));
									// c1->cd();
									// hit3_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit3_fit->GetChisquare(),hit3_fit->GetNDF(),hit3_fit->GetChisquare()/double(hit3_fit->GetNDF())));
									
									// c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
								}
								
								// c1->Clear();

								//some important parameters are saved to the "hit3_best_fit_picker_layer" for further analysis or checking 
								//1. the chosen cluster position at layer 0 1 2 (0 ~ 2) 
								//2. the residual of each chosen cluster against the fitting line (3 ~ 5)
								//3. the position difference of the clusters divided by the layer distance against each other  (6 ~ 8)
								//4. the position difference of the clusters against each other  (9 ~ 11)
								//5. the original best fit slope and offset (12~13)
								if (  ( hit3_fit[ int (hit3_fit.size())-1 ] ->GetChisquare()/double( hit3_fit[ int (hit3_fit.size())-1 ]->GetNDF() ) ) < hit3_best_fit_picker)
								{
									hit3_best_fit_picker =( hit3_fit[ int (hit3_fit.size())-1 ]->GetChisquare()/double(hit3_fit[ int (hit3_fit.size())-1 ]->GetNDF()) );
									hit3_best_fit_picker_layer[0]=hit3_fit_Y[0];
									hit3_best_fit_picker_layer[1]=hit3_fit_Y[1];
									hit3_best_fit_picker_layer[2]=hit3_fit_Y[2];

									hit3_best_fit_picker_layer[3] = ( hit3_fit_Y[0] - (hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(1)* hit3_layer[0] + hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(0)) );//deviation from fit firstlayer
									hit3_best_fit_picker_layer[4] = ( hit3_fit_Y[1] - (hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(1)* hit3_layer[1] + hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(0)) );//deviation from fit secondlayer
									hit3_best_fit_picker_layer[5] = ( hit3_fit_Y[2] - (hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(1)* hit3_layer[2] + hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(0)) );//deviation from fit thirdlayer
								
									hit3_best_fit_picker_layer[6] = fabs(hit3_fit_Y[0]-hit3_fit_Y[1]) / fabs(hit3_actual_xpos[0]-hit3_actual_xpos[1]); // because now we check the slope, 
									hit3_best_fit_picker_layer[7] = fabs(hit3_fit_Y[0]-hit3_fit_Y[2]) / fabs(hit3_actual_xpos[0]-hit3_actual_xpos[2]); // the reason of using "hit3_actual_xpos" instead of hit3_layer is to make it compatible with non-isometric layer gap 
									hit3_best_fit_picker_layer[8] = fabs(hit3_fit_Y[1]-hit3_fit_Y[2]) / fabs(hit3_actual_xpos[1]-hit3_actual_xpos[2]);

									hit3_best_fit_picker_layer[9]  = fabs(hit3_fit_Y[0]-hit3_fit_Y[1]) ; // these three is for the final check -> 0000
									hit3_best_fit_picker_layer[10] = fabs(hit3_fit_Y[0]-hit3_fit_Y[2]) ; // we check the absolute position difference
									hit3_best_fit_picker_layer[11] = fabs(hit3_fit_Y[1]-hit3_fit_Y[2]) ; 

									hit3_best_fit_picker_layer[12] = hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(1); // original slope
									hit3_best_fit_picker_layer[13] = hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(0); // original offset

									hit3_final_pickup_ID = hit3_fit_loop;
								}

								// hit3_grr->Clear();						
								memset(hit3_fit_Y, 0, sizeof(hit3_fit_Y));
								hit3_fit_loop+=1;
							}
						}
					}//hit3 end of all the fit 
					final_pickup.push_back(hit3_final_pickup_ID);
					if (pass_l1_counting%info_sampling==0) {cout<<"---------hit3, PLC "<<pass_l1_counting<<" final pick : "<<hit3_final_pickup_ID<<endl;}
					hit3_fit_loop=0;

					if (alignment_on == true && number_of_layer == 3)
					{
						for_final_alignment.push_back(hit3_best_fit_picker_layer[0]); // to save the best fit positions
						for_final_alignment.push_back(hit3_best_fit_picker_layer[1]);
						for_final_alignment.push_back(hit3_best_fit_picker_layer[2]);

						for_final_alignment.push_back(hit3_best_fit_picker_layer[6]); //0-1
						for_final_alignment.push_back(hit3_best_fit_picker_layer[7]); //0-2
						for_final_alignment.push_back(hit3_best_fit_picker_layer[8]); //1-2

						get_align_result = align_func_hit3 (for_final_alignment,actual_xpos);

						TH1_hit4_layer0_Y_alignment->Fill(get_align_result[0]); // to fill the alignment to the histogram 
						TH1_hit4_layer1_Y_alignment->Fill(get_align_result[1]);
						TH1_hit4_layer2_Y_alignment->Fill(get_align_result[2]);

						//the required data for further alignment check is saved in root file 
						alignment_l0 = get_align_result[0]; // the amount of the correction of layer 0
						alignment_l1 = get_align_result[1]; // the amount of the correction of layer 1
						alignment_l2 = get_align_result[2]; // the amount of the correction of layer 2
						
						fit_slope    = get_align_result[3]; // the slope of the 2 fixed points
						fit_offset   = get_align_result[4]; // the offset of the 2 fixed points 
						
						chosen_layer = get_align_result[5]; // the chosen layer

						origin_l0     = hit3_best_fit_picker_layer[0]; // the original position at l0
						origin_l1     = hit3_best_fit_picker_layer[1]; // the original position at l1
						origin_l2     = hit3_best_fit_picker_layer[2]; // the original position at l2

						origin_slope_2 = hit3_best_fit_picker_layer[6]; // the original slope, exclude layer 2
						origin_slope_1 = hit3_best_fit_picker_layer[7];
						origin_slope_0 = hit3_best_fit_picker_layer[8];

						origin_fit_slope  = hit3_best_fit_picker_layer[12]; // the original best fit slope before correction 
						origin_fit_offset = hit3_best_fit_picker_layer[13]; // the original best fit offset before correction

						align_eID    = eID_array[i];

						tree_output->Fill();

					}

					if (number_of_layer == 3)
					{
						TH1_hit4_layer0_Y_residual->Fill(hit3_best_fit_picker_layer[3]); // fill the residual to the histogram withno correction
						TH1_hit4_layer1_Y_residual->Fill(hit3_best_fit_picker_layer[4]);
						TH1_hit4_layer2_Y_residual->Fill(hit3_best_fit_picker_layer[5]);
					}

					// hit3_final_small_X.push_back(hit3_layer[0]);
					// hit3_final_small_X.push_back(hit3_layer[1]);
					// hit3_final_small_X.push_back(hit3_layer[2]);
					// hit3_final_small_Y.push_back(hit3_best_fit_picker_layer[0]);
					// hit3_final_small_Y.push_back(hit3_best_fit_picker_layer[1]);
					// hit3_final_small_Y.push_back(hit3_best_fit_picker_layer[2]);

					// here we check whether the residual of each layer is larger than the setting cut "noise_hit_distance"
					// if the residual of one of the layer is larger than the setting cut, the further analysis is applied 
					// we check hit3_best_fit_picker_layer[6] ~ hit3_best_fit_picker_layer[8]
					// the one with largest value is the outlier, and its profile has to be 0
					if (fabs(hit3_best_fit_picker_layer[3])>noise_hit_distance || fabs(hit3_best_fit_picker_layer[4])>noise_hit_distance || fabs(hit3_best_fit_picker_layer[5])>noise_hit_distance)
					{
						for (int i3=6; i3<9; i3++)
						{						
							if (hit3_second_bad_picker_big > hit3_best_fit_picker_layer[i3] )
							{
								hit3_second_bad_picker_big = hit3_best_fit_picker_layer[i3];
								hit3_second_bad_picker = i3;
							}
						}
						if (hit3_second_bad_picker == 6 )
						{
							if      (hit3_layer[2]==0) 
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
							else if (hit3_layer[2]==1)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
							else if (hit3_layer[2]==2)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
							else if (hit3_layer[2]==3)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
						}
						else if (hit3_second_bad_picker == 7)
						{
							if      (hit3_layer[1]==0) 
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
							else if (hit3_layer[1]==1)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
							else if (hit3_layer[1]==2)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
							else if (hit3_layer[1]==3)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
						}
						else if (hit3_second_bad_picker == 8)
						{
							if      (hit3_layer[0]==0) 
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
							else if (hit3_layer[0]==1)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
							else if (hit3_layer[0]==2)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
							else if (hit3_layer[0]==3)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
						}

						// to check the position difference of the rest 2 points
						//hit3_second_bad_picker : 6 ~ 8
						// we check the absolute position difference
						if ( fabs(hit3_best_fit_picker_layer[hit3_second_bad_picker+3]) > noise_hit_distance ) 
						{
							event_quality[pass_l1_counting] = 0x00;
						}
					}

					if (raw_plot_print == true )
					{
						if (number_of_layer == 4)
						{
							if ( pass_l1_counting%info_sampling == 0 || (event_quality[pass_l1_counting] != decoder && show_bad_plot == true) ) //1111
							{
								for (int i9 = 0; i9 < hit3_grr.size(); i9 ++)
								{
									c1->cd();
									hit3_grr[ i9 ] -> Draw("apl");
									hit3_fit[ i9 ] -> Draw("l same");
									c1->cd();
									hit3_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(1),hit3_fit[ int (hit3_fit.size())-1 ]->GetParameter(0)));
									c1->cd();
									hit3_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit3_fit[ int (hit3_fit.size())-1 ]->GetChisquare(),hit3_fit[ int (hit3_fit.size())-1 ]->GetNDF(),hit3_fit[ int (hit3_fit.size())-1 ]->GetChisquare()/double(hit3_fit[ int (hit3_fit.size())-1 ]->GetNDF())));
									
									c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
									c1->Clear();
								}
							}
						}
						else if (number_of_layer == 3)
						{
							if ( pass_l1_counting%info_sampling == 0 || (event_quality[pass_l1_counting] != l4_miss_l3 && show_bad_plot == true) ) //1110
							{
								for (int i9 = 0; i9 < hit3_grr.size(); i9 ++)
								{
									c1->cd();
									hit3_grr[ i9 ] -> Draw("apl");
									hit3_fit[ i9 ] -> Draw("l same");
									c1->cd();
									hit3_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit3_fit[ i9 ]->GetParameter(1),hit3_fit[ i9 ]->GetParameter(0)));
									c1->cd();
									hit3_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit3_fit[ i9 ]->GetChisquare(),hit3_fit[ i9 ]->GetNDF(),hit3_fit[ i9 ]->GetChisquare()/double(hit3_fit[ i9 ]->GetNDF())));
									
									c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
									c1->Clear();
								}
							}
						}
					}

					hit3_grr.clear();
					hit3_fit.clear();
					// hit3_final_X.push_back(hit3_final_small_X);
					// hit3_final_Y.push_back(hit3_final_small_Y);
					// hit3_final_small_X.clear();
					// hit3_final_small_Y.clear();

				}//end of the bracket "most_hit_info[1]==3"

				// here we work on the 4 hits case, finally.
				// Again, we first fit with all the combinations, and then pick up the one with minimal r-chi2
				// then we check its residuals. If one of the residuals is larger than the setting cut, the further analysis is applied
			 	else if (most_hit_info[1]==4)//4 layer, all layers have hits ->4 
			 	{
			 		for (int l1=0; l1<data_all_MC_EW[ 0 ].size(); l1++)
					{
						for (int l2=0; l2<data_all_MC_EW[ 1 ].size(); l2++)
						{
							for (int l3=0; l3<data_all_MC_EW[ 2 ].size(); l3++)
							{
								for (int l4=0; l4<data_all_MC_EW[ 3 ].size(); l4++)	
								{
									
									hit4_fit_Y[0] = data_all_MC_EW [0][l1];
									hit4_fit_Y[1] = data_all_MC_EW [1][l2];
									hit4_fit_Y[2] = data_all_MC_EW [2][l3];
									hit4_fit_Y[3] = data_all_MC_EW [3][l4];

									hit4_fit.push_back(fit_basis);
									hit4_fit[ int (hit4_fit.size())-1 ] = new TF1(Form("hit4_fit_%i",int (hit4_fit.size())-1),"pol1",-1,10);

									hit4_grr.push_back(grr);
									hit4_grr[ int (hit4_grr.size())-1 ] = new TGraph (4,hit4_layer,hit4_fit_Y); //MAYBE the "hit4_layer" has to be modified in the future, fit range and the plot x axis range as well. 
									hit4_grr[ int (hit4_grr.size())-1 ] -> Fit(Form("hit4_fit_%i",int (hit4_fit.size())-1),"NQ");

									// if (raw_plot_print==true && pass_l1_counting%info_sampling==0)
									if (raw_plot_print==true)
									{
										hit4_grr[ int (hit4_grr.size())-1 ] -> SetTitle( Form("4 hit PLC : %d-%d, eID : %d, chip : %d, chan :%.3f, %.3f, %.3f,%.3f",pass_l1_counting,hit4_fit_loop,eID_array[i],most_hit_info[0],hit4_fit_Y[0],hit4_fit_Y[1],hit4_fit_Y[2],hit4_fit_Y[3]) );
										hit4_grr[ int (hit4_grr.size())-1 ] -> SetMarkerStyle(20);
										hit4_grr[ int (hit4_grr.size())-1 ] -> SetMarkerSize(2);
										hit4_grr[ int (hit4_grr.size())-1 ] -> GetXaxis()->SetLimits(-1,5); //MAYBE if we change the xaxis to the actuall position, we have to modify this line
										hit4_grr[ int (hit4_grr.size())-1 ] -> GetXaxis()->SetTitle("Layer ID");
										hit4_grr[ int (hit4_grr.size())-1 ] -> GetYaxis()->SetRangeUser(-10,10);
										hit4_grr[ int (hit4_grr.size())-1 ] -> GetYaxis()->SetTitle("Y position (Unit : mm)");
										
										// c1->cd();
										// hit4_grr->Draw("apl");
										// hit4_fit->Draw("l same");
										// c1->cd();
										// hit4_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit->GetParameter(1),hit4_fit->GetParameter(0)));
										// c1->cd();
										// hit4_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit->GetChisquare(),hit4_fit->GetNDF(),hit4_fit->GetChisquare()/double(hit4_fit->GetNDF())));
										// c1->cd();
										// c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
									}
									// c1->Clear();

									// cout<<"~~~~~~~~~~~~~~~~what is outside : "<<setprecision(5)<<hit4_fit_Y[0]<<endl;
									// cout<<"fit on first layer  : "<< (hit4_fit->GetParameter(1)* double(hit4_layer[0]) + hit4_fit->GetParameter(0)) <<endl;
									
									//again, some important parameters are saved to the "hit4_best_fit_picker_layer" for further analysis or checking 
									//1. the chosen cluster position at different layer (0 ~ 3) 
									//2. the residual of each chosen cluster against the fitting line (4 ~ 7)
									//3. the position difference of the clusters against each other  (8 ~ 11)
									//4. the original fitting slope and offset (12, 13)
									if (  ( hit4_fit[ int (hit4_fit.size())-1 ]->GetChisquare()/double(hit4_fit[ int (hit4_fit.size())-1 ]->GetNDF()) ) < hit4_best_fit_picker)
									{
										hit4_best_fit_picker =( hit4_fit[ int (hit4_fit.size())-1 ]->GetChisquare()/double(hit4_fit[ int (hit4_fit.size())-1 ]->GetNDF()) );
										hit4_best_fit_picker_layer[0] = hit4_fit_Y[0];
										hit4_best_fit_picker_layer[1] = hit4_fit_Y[1];
										hit4_best_fit_picker_layer[2] = hit4_fit_Y[2];
										hit4_best_fit_picker_layer[3] = hit4_fit_Y[3];

										hit4_best_fit_picker_layer[4] = ( hit4_fit_Y[0] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_layer[0]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0)) );//deviation from fit firstlayer
										hit4_best_fit_picker_layer[5] = ( hit4_fit_Y[1] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_layer[1]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0)) );//deviation from fit secondlayer
										hit4_best_fit_picker_layer[6] = ( hit4_fit_Y[2] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_layer[2]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0)) );//deviation from fit thirdlayer
										hit4_best_fit_picker_layer[7] = ( hit4_fit_Y[3] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_layer[3]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0)) );//deviation from fit fourthlayer
										
										hit4_best_fit_picker_layer[8]  = fabs(hit4_fit_Y[0]-(hit4_fit_Y[1]+hit4_fit_Y[2]+hit4_fit_Y[3])/3.); // MAYBE this way to find out the outlier is not the best 
										hit4_best_fit_picker_layer[9]  = fabs(hit4_fit_Y[1]-(hit4_fit_Y[0]+hit4_fit_Y[2]+hit4_fit_Y[3])/3.); // but sofar it works, the easy solution is to first make the window larger
										hit4_best_fit_picker_layer[10] = fabs(hit4_fit_Y[2]-(hit4_fit_Y[1]+hit4_fit_Y[0]+hit4_fit_Y[3])/3.); // calculate the rotation and correct it back
										hit4_best_fit_picker_layer[11] = fabs(hit4_fit_Y[3]-(hit4_fit_Y[1]+hit4_fit_Y[2]+hit4_fit_Y[0])/3.);
										
										hit4_best_fit_picker_layer[12] = hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1);
										hit4_best_fit_picker_layer[13] = hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0);

										hit4_final_pickup_ID = hit4_fit_loop;
										// cout<<setprecision(5)<<"--------- what is inside : "<<hit4_best_fit_picker_layer[4]<<" "<<endl;
									}

									// hit4_grr->Clear();						
									memset(hit4_fit_Y, 0, sizeof(hit4_fit_Y));
									hit4_fit_loop+=1;


								}
							}
						}
					}//end of 4 hit fit
					if (pass_l1_counting%info_sampling==0) {cout<<"---------hit4, PLC "<<pass_l1_counting<<" final pick : "<<hit4_final_pickup_ID<<endl;}
					final_pickup.push_back(hit4_final_pickup_ID);
					hit4_fit_loop=0;

					// for the 4 hits case, there is one more thing we can do, the alignment test 
					if (alignment_on == true && number_of_layer==4)
					{
						for_final_alignment.push_back(hit4_best_fit_picker_layer[0]);
						for_final_alignment.push_back(hit4_best_fit_picker_layer[1]);
						for_final_alignment.push_back(hit4_best_fit_picker_layer[2]);
						for_final_alignment.push_back(hit4_best_fit_picker_layer[3]);

						// the alignment is done by the function "align_func"
						get_align_result = align_func(for_final_alignment,correction_rate,ploting,folder_direction,file_name,Legend_Y_offset,offset_tolerance,c2,pass_l1_counting,info_sampling,eID_array[i]);
						// cout<<"alignment_func_test"<<endl;
						TH1_hit4_layer0_Y_alignment->Fill(get_align_result[0]);
						TH1_hit4_layer1_Y_alignment->Fill(get_align_result[1]);
						TH1_hit4_layer2_Y_alignment->Fill(get_align_result[2]);
						TH1_hit4_layer3_Y_alignment->Fill(get_align_result[3]);
						
						//the required data for further alignment check is saved in root file 
						alignment_l0 = get_align_result[0]; // the amount of the correction of layer 0
						alignment_l1 = get_align_result[1]; // the amount of the correction of layer 1
						alignment_l2 = get_align_result[2]; // the amount of the correction of layer 2
						alignment_l3 = get_align_result[3]; // the amount of the correction of layer 3
						fit_slope    = get_align_result[4]; // the final fitting slope after the correction
						fit_offset   = get_align_result[5]; // the final fitting offset after the correction 
						
						origin_l0     = hit4_best_fit_picker_layer[0]; // the original position at l0
						origin_l1     = hit4_best_fit_picker_layer[1]; // the original position at l1
						origin_l2     = hit4_best_fit_picker_layer[2]; // the original position at l2
						origin_l3     = hit4_best_fit_picker_layer[3]; // the original position at l3

						origin_fit_slope  = hit4_best_fit_picker_layer[12]; // the original best fit slope before correction 
						origin_fit_offset = hit4_best_fit_picker_layer[13]; // the original best fit offset before correction

						align_eID    = eID_array[i];

						tree_output->Fill();
					}

					// cout<<"alignment_func_test"<<endl;

					// the following 4 histograms is for checking the residual before alignment 
					// if all the layers are aligned well, the peak of the distributions should all be at 0
					if (number_of_layer == 4)
					{
						TH1_hit4_layer0_Y_residual->Fill(hit4_best_fit_picker_layer[4]);
						TH1_hit4_layer1_Y_residual->Fill(hit4_best_fit_picker_layer[5]);
						TH1_hit4_layer2_Y_residual->Fill(hit4_best_fit_picker_layer[6]);
						TH1_hit4_layer3_Y_residual->Fill(hit4_best_fit_picker_layer[7]);
					}
					
					// cout<<"test 11111"<<endl;

					// If one of the residuals is larger than the setting cut, the further analysis is applied 
					// the procedures are shown below 
					// 1. find out the one with largest residual (the outlier)
					// 2. remove that cluster, and fit with the rest 3 points 
					// 3. the profile of the removed cluster is changed to be 0
					// 4. check the residual of the rest 3 points against the new fitting line 
					// 5. if one of the residual is larger than the setting cut, the numerical method is applied to pick up the second outlier
					if (fabs(hit4_best_fit_picker_layer[4])>noise_hit_distance || fabs(hit4_best_fit_picker_layer[5])>noise_hit_distance || fabs(hit4_best_fit_picker_layer[6])>noise_hit_distance || fabs(hit4_best_fit_picker_layer[7])>noise_hit_distance)
					{
						// procedure 1
						for (int i4=8; i4<12; i4++)
						{
							if (hit4_first_bad_picker_big<hit4_best_fit_picker_layer[i4])
							{
								hit4_first_bad_picker_big=hit4_best_fit_picker_layer[i4];
								hit4_first_bad_picker=i4;	
							}
						}
						// procedure 2
						for (int i4=0; i4<4; i4++)
						{
							if (i4 == hit4_first_bad_picker-8) // hit4_first_bad_picker : 8 ~ 11
							{}
							else 
							{
								hit4_secondfit_X.push_back(hit4_layer[i4]); //MAYBE if we change the x axis to the actual layer gap this has to be modified.
								hit4_secondfit_X_ID.push_back(i4);
								hit4_secondfit_Y.push_back(hit4_best_fit_picker_layer[i4]);
							}

						}
						
						
						hit4_fit.push_back(fit_basis);
						hit4_fit[ int (hit4_fit.size())-1 ] = new TF1(Form("hit4_fit_%i",int (hit4_fit.size())-1),"pol1",-1,10);
						hit4_grr.push_back(grr);
						hit4_grr [ int (hit4_grr.size()) -1 ] = new TGraph (3,&hit4_secondfit_X[0],&hit4_secondfit_Y[0]);
						hit4_grr [ int (hit4_grr.size()) -1 ] ->Fit(Form("hit4_fit_%i",int (hit4_fit.size())-1),"NQ");

						if (raw_plot_print==true)
						{
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetTitle( Form("4 hit PLC : %d, eID : %d, chip : %d, chan :%.3f, %.3f, %.3f,%.3f",pass_l1_counting,eID_array[i],most_hit_info[0],hit4_best_fit_picker_layer[0],hit4_best_fit_picker_layer[1],hit4_best_fit_picker_layer[2],hit4_best_fit_picker_layer[3]) );
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetMarkerStyle(20);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetMarkerStyle(20);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetMarkerSize(2);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->GetXaxis()->SetLimits(-1,5);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->GetXaxis()->SetTitle("Layer ID");
							hit4_grr [ int (hit4_grr.size()) -1 ] ->GetYaxis()->SetRangeUser(-10,10);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->GetYaxis()->SetTitle("Y position (Unit : mm)");
						}
						
						
						
						
						// c1->cd();
						// hit4_grr->Draw("apl");
						// hit4_fit->Draw("l same");


						hit4_grr.push_back(grr);
						hit4_grr [ int (hit4_grr.size()) -1 ] = new TGraph (4,hit4_layer,hit4_best_fit_picker_layer);
						if (raw_plot_print==true)
						{
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetMarkerStyle(20);
							hit4_grr [ int (hit4_grr.size()) -1 ] ->SetMarkerSize(2);	
						}
						
						// hit4_grr_all->Draw("plsame");
						
						final_pickup.pop_back();
						final_pickup.push_back(99999);

						if (pass_l1_counting%info_sampling==0) {cout<<"---------hit4, PLC "<<pass_l1_counting<<" final pick second fit : "<<99999<<endl;}

						// c1->cd();
						// hit4_fit_latex->DrawLatex(0.12, 0.800-Legend_Y_offset,Form("hit 4 second fit, isolated layer %d",int(hit4_first_bad_picker-8)));
						// c1->cd();
						// hit4_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit->GetParameter(1),hit4_fit->GetParameter(0)));
						// c1->cd();
						// hit4_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit->GetChisquare(),hit4_fit->GetNDF(),hit4_fit->GetChisquare()/double(hit4_fit->GetNDF())));
						// if (raw_plot_print==true && pass_l1_counting%info_sampling==0){c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );}
						// c1->Clear();
						// hit4_grr->Clear();
						// hit4_grr_all->Clear();

						if      (hit4_first_bad_picker-8==0) 
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
						else if (hit4_first_bad_picker-8==1)
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
						else if (hit4_first_bad_picker-8==2)
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
						else if (hit4_first_bad_picker-8==3)
							event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;

						//procedure 3
						hit4_second_residual[0] = hit4_secondfit_Y[0] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_secondfit_X[0]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0));
						hit4_second_residual[1] = hit4_secondfit_Y[1] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_secondfit_X[1]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0));
						hit4_second_residual[2] = hit4_secondfit_Y[2] - (hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1)* double(hit4_secondfit_X[2]) + hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0));
						// calculate the outlier numerically 
						hit4_second_bad_pciker[0] = fabs( hit4_secondfit_Y[1]-hit4_secondfit_Y[2] )/fabs(hit4_secondfit_X[1]-hit4_secondfit_X[2]); /*bad in 0      /|\ */
						hit4_second_bad_pciker[1] = fabs( hit4_secondfit_Y[0]-hit4_secondfit_Y[2] )/fabs(hit4_secondfit_X[0]-hit4_secondfit_X[2]); //bad in 1       |
						hit4_second_bad_pciker[2] = fabs( hit4_secondfit_Y[1]-hit4_secondfit_Y[0] )/fabs(hit4_secondfit_X[1]-hit4_secondfit_X[0]); //bad in 2 of ---|

						hit4_second_bad_pciker[3] = fabs( hit4_secondfit_Y[1]-hit4_secondfit_Y[2] );
						hit4_second_bad_pciker[4] = fabs( hit4_secondfit_Y[0]-hit4_secondfit_Y[2] );
						hit4_second_bad_pciker[5] = fabs( hit4_secondfit_Y[1]-hit4_secondfit_Y[0] );

						//procedure 4
						if (fabs(hit4_second_residual[0]) > noise_hit_distance || fabs(hit4_second_residual[1]) > noise_hit_distance || fabs(hit4_second_residual[2]) > noise_hit_distance)
						{
							// procedure 5
							for (int i4=0; i4<3; i4++)
							{
								if (hit4_second_bad_pciker[i4]<hit4_second_bad_pciker_big)
								{
									hit4_second_bad_pciker_big = hit4_second_bad_pciker[i4];
									hit4_second_bad_pciker_id = i4;
								}
							}
							if      (hit4_secondfit_X_ID[hit4_second_bad_pciker_id]==0)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
							else if (hit4_secondfit_X_ID[hit4_second_bad_pciker_id]==1)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
							else if (hit4_secondfit_X_ID[hit4_second_bad_pciker_id]==2)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
							else if (hit4_secondfit_X_ID[hit4_second_bad_pciker_id]==3)
								event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;

							// to check the position difference of the rest 2 points
							// hit4_second_bad_pciker_id 0 ~ 2
							if ( fabs( hit4_second_bad_pciker [hit4_second_bad_pciker_id+3] ) > noise_hit_distance) 
							{
								event_quality[pass_l1_counting] = 0x00;
							}
						}

						
					
					}//end of if the residual > than window

					if ( raw_plot_print == true )
					{
						if ( pass_l1_counting%info_sampling == 0 || (event_quality[pass_l1_counting] != decoder && show_bad_plot == true ) ) //1111
						{
							if ( hit4_grr.size() == hit4_fit.size() )
							{
								for (int i9 = 0; i9 < hit4_fit.size(); i9 ++)
								{
									c1->cd();
									hit4_grr[i9]->Draw("apl");
									hit4_fit[i9]->Draw("lsame");
									
									c1->cd();
									hit4_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit[i9]->GetParameter(1),hit4_fit[i9]->GetParameter(0)));
									c1->cd();
									hit4_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit[i9]->GetChisquare(),hit4_fit[i9]->GetNDF(),hit4_fit[i9]->GetChisquare()/double(hit4_fit[i9]->GetNDF())));
									c1->cd();
									c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
									c1->Clear();
									
								}
							}
							else 
							{
								for (int i9 = 0; i9 < hit4_fit.size(); i9 ++)
								{
									c1->cd();
									hit4_grr[i9]->Draw("apl");
									hit4_fit[i9]->Draw("lsame");
									
									if (i9 != hit4_fit.size()-1) 
									{
										c1->cd();
										hit4_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit[i9]->GetParameter(1),hit4_fit[i9]->GetParameter(0)));
										c1->cd();
										hit4_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit[i9]->GetChisquare(),hit4_fit[i9]->GetNDF(),hit4_fit[i9]->GetChisquare()/double(hit4_fit[i9]->GetNDF())));
										c1->cd();
										c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
										c1->Clear();
									}
								}
								hit4_grr[ int (hit4_grr.size())-1 ]->Draw("plsame");
								c1->cd();
								hit4_fit_latex->DrawLatex(0.12, 0.800-Legend_Y_offset,Form("hit 4 second fit, isolated layer %d",int(hit4_first_bad_picker-8)));
								c1->cd();
								hit4_fit_latex->DrawLatex(0.12, 0.750-Legend_Y_offset, Form("fit result : Y = %.4f X + %.4f", hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(1),hit4_fit[ int (hit4_fit.size())-1 ]->GetParameter(0)));
								c1->cd();
								hit4_fit_latex->DrawLatex(0.12, 0.720-Legend_Y_offset, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", hit4_fit[ int (hit4_fit.size())-1 ]->GetChisquare(),hit4_fit[ int (hit4_fit.size())-1 ]->GetNDF(),hit4_fit[ int (hit4_fit.size())-1 ]->GetChisquare()/double(hit4_fit[ int (hit4_fit.size())-1 ]->GetNDF())));
								c1->Print( Form("%s/folder_%s/event_display.pdf",folder_direction.Data(),file_name.Data()) );
								c1->Clear();
							}
							
						}
					}

					hit4_grr.clear();
					hit4_fit.clear();

					// for (int i3=4; i3<8; i3++)
					// {
					// 	hit4_worse_hit_picker_layer = hit4_layer[i3-4];
					// 	// cout<<setprecision(5)<<"residual : "<<hit4_best_fit_picker_layer[i3]<<" criteria "<<noise_hit_distance<<endl;
					// 	if (fabs(hit4_best_fit_picker_layer[i3])>noise_hit_distance)
					// 	{
					// 		// cout<<"-------- test bad fit ? : "<<pass_l1_counting<<endl;
					// 		if      (hit4_worse_hit_picker_layer==0) 
					// 			event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x08;
					// 		else if (hit4_worse_hit_picker_layer==1)
					// 			event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x04;
					// 		else if (hit4_worse_hit_picker_layer==2)
					// 			event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x02;
					// 		else if (hit4_worse_hit_picker_layer==3)
					// 			event_quality[pass_l1_counting]=event_quality[pass_l1_counting]^0x01;
					// 	}
					// }		 		

			 			
			 	}//end of if "most_hit_info[1]==4"

			 	
				pass_l1_counting+=1;
			}//end of the bracket that pass l1 and l2 selection (2, 3, 4 hits)

		
			//==============================================about to end the loop==============================================
			most_hit_info.clear();
			for (int i10=0; i10<4; i10++)
			{
				for (int i11=0; i11<26; i11++)
				{
					data_all[i10][i11].clear();
				}
			}
			hit_layer_counting=0;
			hit3_best_fit_picker=100000000000;
			hit3_worse_hit_picker=0;
			hit4_best_fit_picker=100000000000;
			hit4_worse_hit_picker=0;
			data_all_MC_EW.clear();
			hit3_second_bad_picker_big=100000000;
			hit4_first_bad_picker_big=0;
			hit4_second_residual_counter=0;
			hit4_second_bad_pciker_big=10000000;

			for_final_alignment.clear();
			get_align_result.clear();

			hit4_secondfit_Y.clear();
			hit4_secondfit_X.clear();
			hit4_secondfit_X_ID.clear();
			event_chip_to_function.clear();
			if (pass_l1_counting%info_sampling==0) {cout<<" "<<endl;}
		}//end of event loop "i"

		c1->cd();
		
		if (raw_plot_print==true ){c1->Print( Form("%s/folder_%s/event_display.pdf)",folder_direction.Data(),file_name.Data()) );}

		printf("\n");	
		printf("\n");
		
		//here the profile of all event is saved in the event_profile_out.txt
		ofstream event_profile_out;
		event_profile_out.open(Form("%s/folder_%s/event_profile_out.txt", folder_direction.Data(),file_name.Data()), ios::out);
		
		// here we calculate the efficiency
		// we separate into 2 cases 
		// 4 layers of INTT ladders of or 3 layers of INTT ladders 
		if (number_of_layer == 4)
		{
			double N_HHHH=0; // all layers have qualified hit
			double N_LHHH=0; // l0 no qualified hit
			double N_HLHH=0; // l1 no qualified hit
			double N_HHLH=0; // l2 no qualified hit
			double N_HHHL=0; // l3 no qualified hit

			for (int i=0; i<event_quality.size(); i++)
			{
				if      (event_quality[i] == decoder)    {N_HHHH+=1;}
				else if (event_quality[i] == l4_miss_l0) {N_LHHH+=1;}
				else if (event_quality[i] == l4_miss_l1) {N_HLHH+=1;}
				else if (event_quality[i] == l4_miss_l2) {N_HHLH+=1;}
				else if (event_quality[i] == l4_miss_l3) {N_HHHL+=1;}
				event_profile_out<<" event ID :	"<<i<<"	eID : "<<event_quality_ID[i]<<"	"<<std::bitset<4>(event_quality[i])<<" final pickup : "<<final_pickup[i]<<"\r"<<endl;
			}
			cout<<"layer 4 final counting : "<<N_HHHH<<" "<<N_LHHH<<" "<<N_HLHH<<" "<<N_HHLH<<" "<<N_HHHL<<endl;

			printf("=====4-layers=====efficiency results===========\n");
			printf("|| l0 : %.5f % \n", N_HHHH/(N_HHHH+N_LHHH)*100.);
			printf("|| l1 : %.5f % \n", N_HHHH/(N_HHHH+N_HLHH)*100.);
			printf("|| l2 : %.5f % \n", N_HHHH/(N_HHHH+N_HHLH)*100.);
			printf("|| l3 : %.5f % \n", N_HHHH/(N_HHHH+N_HHHL)*100.);
			printf("=====4-layers=====efficiency results===========\n");
			printf("\n");

		}
		else if (number_of_layer==3)
		{
			double N_HHH=0; // all layers have qualified hit
			double N_LHH=0; // l0 no qualified hit
			double N_HLH=0; // l1 no qualified hit
			double N_HHL=0; // l2 no qualified hit

			double N_LLL=0;

			for (int i=0; i<event_quality.size(); i++)
			{
				if      (event_quality[i] == l4_miss_l3) {N_HHH+=1;}
				else if (event_quality[i] == l3_miss_l0) {N_LHH+=1;}
				else if (event_quality[i] == l3_miss_l1) {N_HLH+=1;}
				else if (event_quality[i] == l3_miss_l2) {N_HHL+=1;}
				else if (event_quality[i] == 0x00)       {N_LLL+=1;}
				event_profile_out<<" event ID :	"<<i<<"	eID : "<<event_quality_ID[i]<<"	"<<std::bitset<4>(event_quality[i])<<" final pickup : "<<final_pickup[i]<<"\r"<<endl;
			}

			cout<<"layer 3 final counting : "<<N_HHH<<" "<<N_LHH<<" "<<N_HLH<<" "<<N_HHL<<" "<<N_LLL<<endl;
			
			printf("=====3-layers=====efficiency results===========\n");
			printf("|| l0 : %.5f % \n", N_HHH/(N_HHH+N_LHH)*100.);
			printf("|| l1 : %.5f % \n", N_HHH/(N_HHH+N_HLH)*100.);
			printf("|| l2 : %.5f % \n", N_HHH/(N_HHH+N_HHL)*100.);
			printf("=====3-layers=====efficiency results===========\n");
			printf("\n");
		}
		else 
		{
			for (int i=0; i<event_quality.size(); i++)
			{
				event_profile_out<<" event ID :	"<<i<<"	eID : "<<event_quality_ID[i]<<"	"<<std::bitset<4>(event_quality[i])<<" final pickup : "<<final_pickup[i]<<"\r"<<endl;
			}
		}

		event_profile_out.close();

		// to print all plots

		if (number_of_layer > 2)
		{
			c1->Clear();
			c1->cd();
			TH1_hit4_layer0_Y_residual->Draw("hist");
			c1->Print( Form("%s/folder_%s/Y_residual.pdf(",folder_direction.Data(),file_name.Data()) );
			c1->Clear();
		
			c1->cd();
			TH1_hit4_layer1_Y_residual->Draw("hist");
			c1->Print( Form("%s/folder_%s/Y_residual.pdf",folder_direction.Data(),file_name.Data()) );
			c1->Clear();
			if (number_of_layer == 4)
			{
				c1->cd();
				TH1_hit4_layer2_Y_residual->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_residual.pdf",folder_direction.Data(),file_name.Data()) );
				c1->Clear();
		
				c1->cd();
				TH1_hit4_layer3_Y_residual->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_residual.pdf)",folder_direction.Data(),file_name.Data()) );
				c1->Clear();	
			}
			else if (number_of_layer == 3)
			{
				c1->cd();
				TH1_hit4_layer2_Y_residual->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_residual.pdf)",folder_direction.Data(),file_name.Data()) );
				c1->Clear();
			}
		}

		if (alignment_on == true && number_of_layer > 2)
		{
			c1->Clear();
			c1->cd();
			TH1_hit4_layer0_Y_alignment->Draw("hist");
			c1->Print( Form("%s/folder_%s/Y_alignment.pdf(",folder_direction.Data(),file_name.Data()) );
			c1->Clear();

			c1->cd();
			TH1_hit4_layer1_Y_alignment->Draw("hist");
			c1->Print( Form("%s/folder_%s/Y_alignment.pdf",folder_direction.Data(),file_name.Data()) );
			c1->Clear();
			if (number_of_layer == 4)
			{
				c1->cd();
				TH1_hit4_layer2_Y_alignment->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_alignment.pdf",folder_direction.Data(),file_name.Data()) );
				c1->Clear();

				c1->cd();
				TH1_hit4_layer3_Y_alignment->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_alignment.pdf)",folder_direction.Data(),file_name.Data()) );
				c1->Clear();	
			}
			else if (number_of_layer == 3)
			{
				c1->cd();
				TH1_hit4_layer2_Y_alignment->Draw("hist");
				c1->Print( Form("%s/folder_%s/Y_alignment.pdf)",folder_direction.Data(),file_name.Data()) );
				c1->Clear();
			}
			

			file_output->cd ();
			tree_output->Write("", TObject::kOverwrite);
			file_output->Close();
			printf("alignment data saving done\n");
		}
	} //end of the "full analysis bool"
		
}
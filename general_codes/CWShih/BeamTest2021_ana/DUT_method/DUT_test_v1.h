// note : this code is for the test of the DUT method

// note : include the parameter information
#include "par_init.h"
#include "double_gaus.h"

// note : the structure for event profile
struct profile_str {
    int profile_eID;
    int profile_chip;
    int profile;
    int final_pickup;
    int clusterID_0;
    int clusterID_1;
    int clusterID_2;
};

// note : the structure of cluster info.
struct cluster_str {
    
    int cluster_eID;
    vector<int> cluster_layer;
    vector<int> cluster_chip;
    vector<int> cluster_hit;
    vector<double> cluster_position;
    vector<double> cluster_adc;
};

// note : the structure for vector<double>[13][3]
struct cluster_reformat_str {
    int eID;
    vector<int> cluster_hit[13][3];
    vector<double> cluster_pos[13][3];
    vector<double> cluster_adc[13][3];
};

struct noise_hit_str{
    int noise_chip;
    int noise_layer;
    double noise_pos;
};

struct noise_event_str{
    int eID;
    vector<noise_hit_str> noise_event;
};

struct multi_track_str{
    // note : in order to output two things.

    // note : in order to take out the araay of the # of track of each event.
    vector<noise_event_str> noise_vec;
    vector<int> N_track;

    vector<int> track_chip_count;
    
};

struct DUT_str{
    int track_111_count;
    int track_101_count;
    vector<double> middle_layer_residual; 
    vector<double> good_combination_slope_hit3;
    vector<double> good_combination_slope_hit2;
    
    // note : mostly for the study of the satellite peak of run89 
    vector<double> good_combination_l0_pos_hit3;
    vector<double> good_combination_l1_pos_hit3;
    vector<double> good_combination_l2_pos_hit3;
};


vector<cluster_str> cluster_read_and_build (TString folder_direction, TString file_name, TString cluster_file_name, int study_chip)
{
    // note : the root file of the cluster information, chip, layer, Nhits, position.
    // note : it is in different file, no "_test"
    // TFile* cluster_file = TFile::Open (Form("%s/folder_%s_chip_%i/%s.root", folder_direction.Data(), file_name.Data(),study_chip,cluster_file_name.Data()), "read");
    TFile* cluster_file = TFile::Open (Form("%s/%s.root", folder_direction.Data(),cluster_file_name.Data()), "read");
    TTree *cluster_tree = (TTree *)cluster_file->Get("cluster_info");
    int cluster_tree_size = cluster_tree->GetEntriesFast();
    cout<<"Tree cluster size : "<<cluster_tree_size<<endl;

    int cluster_eID;
    vector<int> * cluster_layer = new vector<int>();
    vector<int> * cluster_chip = new vector<int>();
    vector<int> * cluster_hit = new vector<int>();
    vector<double> * cluster_position = new vector<double>();
    vector<double> * cluster_adc = new vector<double>();

    cluster_tree -> SetBranchAddress("eID",&cluster_eID);
    cluster_tree -> SetBranchAddress("layer",&cluster_layer);
    cluster_tree -> SetBranchAddress("chip",&cluster_chip);
    cluster_tree -> SetBranchAddress("Nhit",&cluster_hit);
    cluster_tree -> SetBranchAddress("position",&cluster_position);
    cluster_tree -> SetBranchAddress("cluster_adc",&cluster_adc);

    vector<cluster_str> output_vec; output_vec.clear();
    cluster_str fill_unit;

    fill_unit.cluster_layer.clear();
    fill_unit.cluster_chip.clear();
    fill_unit.cluster_hit.clear();
    fill_unit.cluster_position.clear();
    fill_unit.cluster_adc.clear();

    for (int i = 0; i < cluster_tree_size; i++)
    {
        cluster_tree -> GetEntry(i);

        output_vec.push_back(fill_unit);

        output_vec[output_vec.size()-1].cluster_eID = cluster_eID;
        for ( int i1 = 0; i1 < cluster_layer->size(); i1++ )
        {
            output_vec[output_vec.size()-1].cluster_layer.push_back(cluster_layer->at(i1));
            output_vec[output_vec.size()-1].cluster_chip.push_back(cluster_chip->at(i1));
            output_vec[output_vec.size()-1].cluster_hit.push_back(cluster_hit->at(i1));
            output_vec[output_vec.size()-1].cluster_position.push_back(cluster_position->at(i1));
            output_vec[output_vec.size()-1].cluster_adc.push_back(cluster_adc->at(i1));
        }


        // cout<<"-------------- -------------- -------------- -------------- --------------"<<endl;
        // if (1==1) {cout<<i<<" event check : "<<cluster_chip->size()<<endl;}
        // if (1==1) {cout<<i<<" event check : "<< output_vec[0].cluster_chip.size()<< " "<<output_vec[0].cluster_position[0] <<endl;}

    }

    
    // cout<<"event check out but in : "<< output_vec[0].cluster_chip.size() <<endl;
    cout<<"cluster size check : "<<output_vec.size()<<endl;
    return output_vec;
}

vector<cluster_reformat_str> cluster_reformat ( vector<cluster_str> input_vec )
{

    vector<cluster_reformat_str> output_vec; output_vec.clear();
    cluster_reformat_str fill_unit;

    for (int i = 0; i < 13; i++)
    {
        fill_unit.cluster_hit[0][i].clear(); fill_unit.cluster_hit[1][i].clear(); fill_unit.cluster_hit[2][i].clear();
        fill_unit.cluster_pos[0][i].clear(); fill_unit.cluster_pos[1][i].clear(); fill_unit.cluster_pos[2][i].clear();
        fill_unit.cluster_adc[0][i].clear(); fill_unit.cluster_adc[1][i].clear(); fill_unit.cluster_adc[2][i].clear();
    }

    for (int i = 0; i < input_vec.size(); i++ )
    {
        output_vec.push_back(fill_unit);
        
        // note : fill the eID
        output_vec[ output_vec.size() - 1 ].eID = input_vec[i].cluster_eID;
        
        for (int i1 = 0; i1 < input_vec[i].cluster_layer.size(); i1++)
        {
            output_vec[ output_vec.size() - 1 ].cluster_hit[ input_vec[i].cluster_chip[i1] - 1 ][ input_vec[i].cluster_layer[i1] ].push_back( input_vec[i].cluster_hit[i1] );
            output_vec[ output_vec.size() - 1 ].cluster_pos[ input_vec[i].cluster_chip[i1] - 1 ][ input_vec[i].cluster_layer[i1] ].push_back( input_vec[i].cluster_position[i1] );
            output_vec[ output_vec.size() - 1 ].cluster_adc[ input_vec[i].cluster_chip[i1] - 1 ][ input_vec[i].cluster_layer[i1] ].push_back( input_vec[i].cluster_adc[i1] );
        }
    }

    cout<<"reformat output check : "<<output_vec.size()<<endl;
    return output_vec;
    
}


DUT_str efficiency_DUT_method (vector<cluster_reformat_str> input_cluster_vec, int study_chip)
{ 
	double edge_exclusion_bottom = (lower_section_initial - INTT_strip_width / 2.) + INTT_strip_width * double(boundary_cut);
	double edge_exclusion_upper = ( INTT_strip_width * 128. ) - INTT_strip_width * double(boundary_cut);

    int layer_hit_count = 0;

    vector<int> receiver_unit_clu[13][3]; // note : for abbreviation, for hit 
    vector<double> receiver_unit_clu_pos[13][3];
    vector<double> receiver_unit_clu_adc[13][3];

    // note : the clusters that pass the pre-selections (cluster cut / event cut) will be saved here.
    vector<int> receiver_unit_clu_size_filter[3]; 
    vector<double> receiver_unit_clu_pos_size_filter[3]; 
    vector<double> receiver_unit_clu_adc_size_filter[3];

    for (int i = 0; i < 3; i++)
    {
        receiver_unit_clu_size_filter[i].clear();
        receiver_unit_clu_pos_size_filter[i].clear(); 
        receiver_unit_clu_adc_size_filter[i].clear(); 
    }

    TF1 * linear_fit;
    TGraph * grr;

    double chi2_register = 0; 
    double cluster_register_l0 = 0;
    double cluster_register_l1 = 0;
    double cluster_register_l2 = 0;
    double hit3_best_fit_picker_info[7]; // note : first 3 : residual by fittting, second 3 : the selected positions, the 7th : the middle-layer residual by the DUT method
    double hit2_best_fit_picker_info[2]; // note : first 2 : the selected positions of the first and last layers.

    int multi_track_count = 0; // note : this is for finding the multi_track, if we find a new good track, then +=1;

    double hit2_slope;

    // note : for the output 
    int track_111_count = 0;
    int track_101_count = 0;
    vector<double> middle_layer_residual; middle_layer_residual.clear();
    vector<double> good_combination_slope_hit3; good_combination_slope_hit3.clear();
    vector<double> good_combination_slope_hit2; good_combination_slope_hit2.clear();

    vector<double> good_combination_l0_pos_hit3; good_combination_l0_pos_hit3.clear();
    vector<double> good_combination_l1_pos_hit3; good_combination_l1_pos_hit3.clear();
    vector<double> good_combination_l2_pos_hit3; good_combination_l2_pos_hit3.clear();


    for (int i = 0; i < input_cluster_vec.size(); i++)
    {
        if (i % 1000 == 0){ cout<<"Doing the DUT test, eID : "<<input_cluster_vec[i].eID<<endl;}

        // note : abbreviation
        for (int i1 = 0; i1 < 13; i1++)
        {
            for (int i2 = 0; i2 < 3; i2++)
            {
                receiver_unit_clu[i1][i2] = input_cluster_vec[i].cluster_hit[ i1 ][i2];
                receiver_unit_clu_pos[i1][i2] = input_cluster_vec[i].cluster_pos[ i1 ][i2];
                receiver_unit_clu_adc[i1][i2] = input_cluster_vec[i].cluster_adc[ i1 ][i2];

            }
        }

        // todo : the LoE
        // todo :       -> right now, what I try is to make sure there is no cluster in the adjacent chips for all three layers (chip 8 and 10, for example)
        // todo :       -> to avoid any suspicious events. 
        // todo :       -> works well !!! 2022/11/23
        // todo : the adc cut
        // todo : cluster size 
        // todo : cluster size combined
        // todo : N cluster in layer 0 and layer 2
        // todo : slope cut
        // todo : the edge exclusion

        // todo : residual cut
        

        // note : no cluster in selected_chip-1 and selected_chip+1, "fixed cut"
        if ( (receiver_unit_clu[study_chip-1-1][0].size()+receiver_unit_clu[study_chip-1-1][1].size()+receiver_unit_clu[study_chip-1-1][2].size()+receiver_unit_clu[study_chip-1+1][0].size()+receiver_unit_clu[study_chip-1+1][1].size()+receiver_unit_clu[study_chip-1+1][2].size()) == 0 )
        {   
            // note : we require the first and the last layers have clusters
            // note : and the total numbr of cluster in single layer (l0 and l2) has to be 1

            // note : in the begining, the cut below is at the next if structure, but it will allow the event that has 2 clusters in single layer, but only one cluster pass the following cut. Not good, not tight.
            // note : therefore, the cut is moved here.
            if (receiver_unit_clu[study_chip-1][0].size() == hit_allowed_in_adjacent_layers && receiver_unit_clu[study_chip-1][2].size() == hit_allowed_in_adjacent_layers )
            {
                // note : filter, reject some clusters or events
                for (int i1 = 0; i1 < 3; i1++)
                {
                    for (int i2 = 0; i2 < receiver_unit_clu[study_chip-1][i1].size(); i2++ )
                    {
                        if (i1 == 1) // note : middle layer, all clusters are included
                        {
                            receiver_unit_clu_size_filter[i1].push_back(receiver_unit_clu[study_chip-1][i1][i2]);
                            receiver_unit_clu_pos_size_filter[i1].push_back(receiver_unit_clu_pos[study_chip-1][i1][i2]); 
                            receiver_unit_clu_adc_size_filter[i1].push_back(receiver_unit_clu_adc[study_chip-1][i1][i2]);
                        }
                        else // note : layer 0 and layer 1
                        {
                            if ( receiver_unit_clu[study_chip-1][i1][i2] > cluster_size_requirement ) // note : cluster size cut
                            {
                                if ( receiver_unit_clu_adc[study_chip-1][i1][i2] > cluster_adc_value_requirement ) // note : cluster adc cut
                                {
                                    receiver_unit_clu_size_filter[i1].push_back(receiver_unit_clu[study_chip-1][i1][i2]);
                                    receiver_unit_clu_pos_size_filter[i1].push_back(receiver_unit_clu_pos[study_chip-1][i1][i2]);
                                    receiver_unit_clu_adc_size_filter[i1].push_back(receiver_unit_clu_adc[study_chip-1][i1][i2]);
                                }
                                
                            }
                        }

                    }
                }
            }   
            


            // note : to check whether the first and the last layers have hits or not
            if (receiver_unit_clu_size_filter[0].size() > 0 && receiver_unit_clu_size_filter[2].size() > 0)
            {
                // todo : the following cut is set to be the first index, if the # of allowed cluster is changed, then the following cut has to be modified
                if (receiver_unit_clu_size_filter[0][0]+receiver_unit_clu_size_filter[2][0] > cluster_size_requirement_combine)
                {
                    if ( receiver_unit_clu_size_filter[1].size() > 0 ) // note : the middle layer has at least one cluster
                    {
                        for (int l0 = 0; l0 < receiver_unit_clu_size_filter[0].size(); l0 ++ ) // note : # of clusters of the chip of the layer 0
                        {
                            for (int l1 = 0; l1 < receiver_unit_clu_size_filter[1].size(); l1 ++ ) // note : # of clusters of the chip of the layer 1
                            {
                                for (int l2 = 0; l2 < receiver_unit_clu_size_filter[2].size(); l2 ++ ) // note : # of clusters of the chip of the layer 2
                                {
                                    double hit3_Y_data[3] = {receiver_unit_clu_pos_size_filter[0][l0], receiver_unit_clu_pos_size_filter[1][l1], receiver_unit_clu_pos_size_filter[2][l2]};

                                    grr = new TGraph(3,actual_xpos,hit3_Y_data);
                                    linear_fit = new TF1("linear_fit","pol1",-1,actual_xpos[2]+2);
                                    grr -> Fit("linear_fit","NQ");

                                    if (l0 + l1 + l2 == 0) // note : the first one
                                    {
                                        chi2_register = ( linear_fit->GetChisquare()/double (linear_fit->GetNDF()) );
                                        cluster_register_l0 = l0;
                                        cluster_register_l1 = l1;
                                        cluster_register_l2 = l2;

                                        hit3_best_fit_picker_info[0] = ( hit3_Y_data[0] - ( linear_fit -> GetParameter(1) * actual_xpos[0] + linear_fit -> GetParameter(0) ) );
                                        hit3_best_fit_picker_info[1] = ( hit3_Y_data[1] - ( linear_fit -> GetParameter(1) * actual_xpos[1] + linear_fit -> GetParameter(0) ) );
                                        hit3_best_fit_picker_info[2] = ( hit3_Y_data[2] - ( linear_fit -> GetParameter(1) * actual_xpos[2] + linear_fit -> GetParameter(0) ) );
                                        
                                        hit3_best_fit_picker_info[3] = hit3_Y_data[0];
                                        hit3_best_fit_picker_info[4] = hit3_Y_data[1];
                                        hit3_best_fit_picker_info[5] = hit3_Y_data[2];

                                        // note : the middle-layer residual of the DUT method
                                        hit3_best_fit_picker_info[6] = hit3_Y_data[1] - (hit3_Y_data[0]+hit3_Y_data[2])/2.;

                                    }
                                    else 
                                    {
                                        if ( linear_fit->GetChisquare()/double (linear_fit->GetNDF()) < chi2_register )
                                        {
                                            chi2_register = ( linear_fit->GetChisquare()/double (linear_fit->GetNDF()) );
                                            cluster_register_l0 = l0;
                                            cluster_register_l1 = l1;
                                            cluster_register_l2 = l2;

                                            hit3_best_fit_picker_info[0] = ( hit3_Y_data[0] - ( linear_fit -> GetParameter(1) * actual_xpos[0] + linear_fit -> GetParameter(0) ) );
                                            hit3_best_fit_picker_info[1] = ( hit3_Y_data[1] - ( linear_fit -> GetParameter(1) * actual_xpos[1] + linear_fit -> GetParameter(0) ) );
                                            hit3_best_fit_picker_info[2] = ( hit3_Y_data[2] - ( linear_fit -> GetParameter(1) * actual_xpos[2] + linear_fit -> GetParameter(0) ) );
                                            
                                            hit3_best_fit_picker_info[3] = hit3_Y_data[0];
                                            hit3_best_fit_picker_info[4] = hit3_Y_data[1];
                                            hit3_best_fit_picker_info[5] = hit3_Y_data[2];

                                            // note : the middle-layer residual of the DUT method
                                            hit3_best_fit_picker_info[6] = hit3_Y_data[1] - (hit3_Y_data[0]+hit3_Y_data[2])/2.;
                                            

                                        }
                                    }
                                }
                            }
                        } // note : end of 3 layers track finding and fitting

                        // note : for saving the position of the selected clusters
                        good_combination_l0_pos_hit3.push_back(hit3_best_fit_picker_info[3]);
                        good_combination_l1_pos_hit3.push_back(hit3_best_fit_picker_info[4]);
                        good_combination_l2_pos_hit3.push_back(hit3_best_fit_picker_info[5]);

                        // note : save the residual at the middle layer.
                        middle_layer_residual.push_back( hit3_best_fit_picker_info[4] - ((hit3_best_fit_picker_info[3]+hit3_best_fit_picker_info[5])/2.) );
                        // note : save the slope of the good combination
                        good_combination_slope_hit3.push_back( (hit3_best_fit_picker_info[5]-hit3_best_fit_picker_info[3])/actual_xpos[2] + slope_correction);

                        // note : check the slope
                        if ( fabs( (hit3_best_fit_picker_info[5]-hit3_best_fit_picker_info[3])/actual_xpos[2] + slope_correction ) < slope_cut )
                        {
                            // note : the edge exclusion of the first and last layers
                            if ( hit3_best_fit_picker_info[3] > edge_exclusion_bottom && hit3_best_fit_picker_info[3] < edge_exclusion_upper && hit3_best_fit_picker_info[5] > edge_exclusion_bottom && hit3_best_fit_picker_info[5] < edge_exclusion_upper )
                            {
                                // note : after finding the best combination, let's check it, check the residual. 
                                if ( fabs( hit3_best_fit_picker_info[4] - ((hit3_best_fit_picker_info[3]+hit3_best_fit_picker_info[5])/2.) ) < noise_hit_distance ) // todo : noise_hit_distance may need to be changed
                                {
                                    track_111_count += 1;
                                }
                                else 
                                {
                                    track_101_count += 1;
                                    cout<<"101 event, eID : "<<input_cluster_vec[i].eID<<" -> middle has cluster"<<endl;
                                }
                            }
                                
                        }

                    }   
                    else // note : the middle layer has no cluster 
                    {
                        for (int l0 = 0; l0 < receiver_unit_clu_size_filter[0].size(); l0 ++ ) // note : layer 0
                        {
                            for (int l1 = 0; l1 < receiver_unit_clu_size_filter[2].size(); l1 ++ ) // note : layer 2
                            {
                                
                                if (l0 + l1 == 0) // note : the first one
                                {
                                    hit2_slope = fabs( (receiver_unit_clu_pos_size_filter[2][l1] - receiver_unit_clu_pos_size_filter[0][l0]) / actual_xpos[2] + slope_correction );

                                    hit2_best_fit_picker_info[0] = receiver_unit_clu_pos_size_filter[0][l1]; // note : the first layer
                                    hit2_best_fit_picker_info[1] = receiver_unit_clu_pos_size_filter[2][l1]; // note : the last layer
                                }
                                else 
                                {
                                    if ( fabs( (receiver_unit_clu_pos_size_filter[2][l1] - receiver_unit_clu_pos_size_filter[0][l0]) / actual_xpos[2] + slope_correction ) < hit2_slope )
                                    {
                                        hit2_slope = fabs( (receiver_unit_clu_pos_size_filter[2][l1] - receiver_unit_clu_pos_size_filter[0][l0]) / actual_xpos[2] + slope_correction );

                                        hit2_best_fit_picker_info[0] = receiver_unit_clu_pos_size_filter[0][l1]; // note : the first layer
                                        hit2_best_fit_picker_info[1] = receiver_unit_clu_pos_size_filter[2][l1]; // note : the last layer
                                    }
                                }
            
                            }
                        } // note : end of 3 layers track finding and fitting

                        // note : save the slope of the good combination
                        good_combination_slope_hit2.push_back( (hit2_best_fit_picker_info[1]-hit2_best_fit_picker_info[0])/actual_xpos[2] + slope_correction);

                        if ( hit2_slope < slope_cut )
                        {
                            // note : the edge exclusion of the first and last layers
                            if ( hit2_best_fit_picker_info[0] > edge_exclusion_bottom && hit2_best_fit_picker_info[0] < edge_exclusion_upper && hit2_best_fit_picker_info[1] > edge_exclusion_bottom && hit2_best_fit_picker_info[1] < edge_exclusion_upper )
                            {
                                track_101_count += 1;
                                cout<<"101 event, eID : "<<input_cluster_vec[i].eID<<" -> no cluster"<<endl;
                            }
                            
                        }


                    }
                }
            }
        } // note : end of if, No cluster in adjacent chips


        // note : start clean

        // layer_hit_count = 0;
        for (int i1 = 0; i1 < 3; i1++)
        {
            receiver_unit_clu_size_filter[i1].clear();
            receiver_unit_clu_pos_size_filter[i1].clear();  
            receiver_unit_clu_adc_size_filter[i1].clear();
        }    

    } // note : end of for loop, event


    DUT_str output_space;
    output_space.track_111_count = track_111_count;
    output_space.track_101_count = track_101_count;
    output_space.middle_layer_residual = middle_layer_residual;
    output_space.good_combination_slope_hit3 = good_combination_slope_hit3;
    output_space.good_combination_slope_hit2 = good_combination_slope_hit2;
    output_space.good_combination_l0_pos_hit3 = good_combination_l0_pos_hit3;
    output_space.good_combination_l1_pos_hit3 = good_combination_l1_pos_hit3;
    output_space.good_combination_l2_pos_hit3 = good_combination_l2_pos_hit3;
    
    return output_space;

}

void plot_residual (vector<double> input_vec, TString folder_direction)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);
    c1 -> cd();
    c1 -> SetLogy();

    TH1F * N_track_hist = new TH1F("","",100,-10,10);
    for (int i = 0; i < input_vec.size(); i++)
    {
        N_track_hist -> Fill(input_vec[i]);
    }

    N_track_hist -> SetTitle("DUT layer 1 residual");
    N_track_hist -> GetXaxis() -> SetTitle("Unit : mm");
    N_track_hist -> GetYaxis() -> SetTitle("Entry");

    N_track_hist -> Draw("hist");

    c1 -> Print( Form("%s/DUT_residual_full_range.pdf",folder_direction.Data()) );
    c1 -> Clear();
}

// note : plot and fit, narrow range
void plot_residual_narrow (vector<double> input_vec, TString folder_direction)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);
    c1 -> cd();
    c1 -> SetLogy();

    TF1 * gaus_fit = new TF1 ("gaus_fit","gaus",-0.5,0.5);
		
    TF1 * D_gaus_fit = new TF1 ("D_gaus_fit",double_gaus,-10,10,5);
    D_gaus_fit -> SetParameters(40000,0.987,0,0.03,0.1);
    // D_gaus_fit -> SetParLimits  (1, 0.5,      1); // fraction
    D_gaus_fit -> SetParLimits  (3, 0.025,      1);
    D_gaus_fit -> SetParLimits  (1, 0.97,      1);
    D_gaus_fit -> SetLineColor(TColor::GetColor("#F5751D"));

    TH1F * N_track_hist = new TH1F("","",100,-1.5,1.5);
    for (int i = 0; i < input_vec.size(); i++)
    {
        N_track_hist -> Fill(input_vec[i]);
    }

    N_track_hist -> SetTitle("DUT layer 1 residual");
    N_track_hist -> GetXaxis() -> SetTitle("Unit : mm");
    N_track_hist -> GetYaxis() -> SetTitle("Entry");

    N_track_hist -> Draw("hist");

    TLatex *gaus_fit_latex = new TLatex();
    gaus_fit_latex -> SetNDC();
    gaus_fit_latex -> SetTextSize(0.028);

    double D_gaus_xmin = -10;
    double D_gaus_xmax =  10;
    double the_portion = 0.9973;

    N_track_hist->Fit("gaus_fit","NQ");
    N_track_hist->Fit(D_gaus_fit,"N","",-0.3,0.3);
    bool run_fit_effisig = double_gaus_getsigmarange (D_gaus_fit,the_portion,D_gaus_xmin,D_gaus_xmax);
    
    gaus_fit -> SetNpx(10000);
    D_gaus_fit -> SetNpx(10000);

    gaus_fit->Draw("lsame");	
    D_gaus_fit->Draw("lsame");	
    
    gaus_fit_latex -> DrawLatex(0.12, 0.750, Form("fit gaus mean :  %.4f, fit gaus width %.4f", gaus_fit->GetParameter(1),gaus_fit->GetParameter(2)));
    gaus_fit_latex -> DrawLatex(0.12, 0.720, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", gaus_fit->GetChisquare(),gaus_fit->GetNDF(),gaus_fit->GetChisquare()/double(gaus_fit->GetNDF())));
    
    gaus_fit_latex -> DrawLatex(0.12, 0.660, Form("double gaussian fit"));
    gaus_fit_latex -> DrawLatex(0.12, 0.630, Form("mean : %.4f, fraction : %.3f",D_gaus_fit->GetParameter(2),D_gaus_fit->GetParameter(1)));
    gaus_fit_latex -> DrawLatex(0.12, 0.600, Form("first width : %.4f, second width : %.4f",D_gaus_fit->GetParameter(3),D_gaus_fit->GetParameter(4)));
    gaus_fit_latex -> DrawLatex(0.12, 0.570, Form("3 sigma width : %.4f %.4f = %.4f",D_gaus_xmax,D_gaus_xmin,D_gaus_xmax-D_gaus_xmin));
    gaus_fit_latex -> DrawLatex(0.12, 0.540, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", D_gaus_fit->GetChisquare(),D_gaus_fit->GetNDF(),D_gaus_fit->GetChisquare()/double(D_gaus_fit->GetNDF())));
    
    gaus_fit_latex -> Draw("same");

    c1 -> Print( Form("%s/DUT_residual_narrow.pdf",folder_direction.Data()) );
    c1 -> Clear();
}


void plot_angle (vector<double> input_vec_hit3, vector<double> input_vec_hit2, TString folder_direction)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);
    c1 -> cd();
    // c1 -> SetLogy();

    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    cout<<"----- Now we are doing the plot_angle "<<endl;
    cout<<"input_vec_hit3 size : "<<input_vec_hit3.size()<<endl;
    cout<<"input_vec_hit2 size : "<<input_vec_hit2.size()<<endl;
    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;

    TH1F * N_track_hist = new TH1F("","",50,-0.05,0.05);
    for (int i = 0; i < input_vec_hit3.size(); i++)
    {
        N_track_hist -> Fill(input_vec_hit3[i]);
    }

    for (int i = 0; i < input_vec_hit2.size(); i++)
    {
        N_track_hist -> Fill(input_vec_hit2[i]);
    }

    N_track_hist -> SetTitle("DUT candidate track slope");
    N_track_hist -> GetXaxis() -> SetTitle("(l2-l1)/distance");
    N_track_hist -> GetYaxis() -> SetTitle("Entry");

    N_track_hist -> Draw("hist");

    TF1 * gaus_fit = new TF1 ("gaus_fit","gaus",-0.1,0.1);
    gaus_fit -> SetNpx(10000);

    N_track_hist -> Fit("gaus_fit","NQ");

    gaus_fit->Draw("lsame");

    TLatex *gaus_fit_latex = new TLatex();
    gaus_fit_latex -> SetNDC();
    gaus_fit_latex -> SetTextSize(0.028);

    gaus_fit_latex -> DrawLatex(0.12, 0.750, Form("fit gaus mean :  %.4f, fit gaus width %.4f", gaus_fit->GetParameter(1),gaus_fit->GetParameter(2)));
    gaus_fit_latex -> DrawLatex(0.12, 0.720, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.4f", gaus_fit->GetChisquare(),gaus_fit->GetNDF(),gaus_fit->GetChisquare()/double(gaus_fit->GetNDF())));
    gaus_fit_latex -> Draw("same");

    c1 -> Print( Form("%s/DUT_candidate_track_slope.pdf",folder_direction.Data()) );
    c1 -> Clear();
}

void hit3_plot_residual_slope_2D (vector<double> input_vec_1, vector<double> input_vec_2, TString folder_direction)
{
    TCanvas * c3 = new TCanvas("c3","c3",1000,800);
    // c3 -> cd();

    TPad *pad = new TPad(Form("pad1"), "", 0.0, 0.0, 1.0, 1.0);
	//pad->SetTopMargin(0.12);
	//pad->SetBottomMargin(0.120);
	pad->SetLeftMargin(0.15);
	pad->SetRightMargin(0.15);
	//pad->SetGrid(1, 1);
	pad->Draw("same");
	pad->cd();

    // c3 -> SetLogy();

    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    cout<<"----- Now we are doing the hit3_plot_residual_slope_2D "<<endl;
    // cout<<"----- Don't forget to turn it on"<<endl;
    cout<<"input_vec_1 size : "<<input_vec_1.size()<<endl;
    cout<<"input_vec_2 size : "<<input_vec_2.size()<<endl;
    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;

    TH2F * residual_slope_2Dhist = new TH2F("","",100,-10,10,100,-0.1,0.1);
    residual_slope_2Dhist -> SetStats(0);
    for (int i = 0; i < input_vec_1.size(); i++)
    {
        residual_slope_2Dhist -> Fill(input_vec_1[i],input_vec_2[i]);
    }

    residual_slope_2Dhist -> SetTitle("Correlation of residual and slope");
    residual_slope_2Dhist -> GetXaxis() -> SetTitle("Residual, unit : mm");
    residual_slope_2Dhist -> GetYaxis() -> SetTitle("(l2-l1)/distance");

    residual_slope_2Dhist -> Draw("colz0");

    c3 -> Print( Form("%s/DUT_residual_slope_2Dhist.pdf",folder_direction.Data()) );
    c3 -> Clear();
}

void hit3_plot_residual_position_2D (vector<double> input_vec_1, vector<double> input_vec_2, TString folder_direction)
{
    TCanvas * c3 = new TCanvas("c3","c3",1000,800);
    // c3 -> cd();
    TPad *pad = new TPad(Form("pad1"), "", 0.0, 0.0, 1.0, 1.0);
	//pad->SetTopMargin(0.12);
	//pad->SetBottomMargin(0.120);
	pad->SetLeftMargin(0.15);
	pad->SetRightMargin(0.15);
	//pad->SetGrid(1, 1);
	pad->Draw("same");
	pad->cd();
    // c3 -> SetLogy();

    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    cout<<"----- Now we are doing the hit3_plot_residual_position_2D "<<endl;
    // cout<<"----- Don't forget to turn it on"<<endl;
    cout<<"input_vec_1 size : "<<input_vec_1.size()<<endl;
    cout<<"input_vec_2 size : "<<input_vec_2.size()<<endl;
    cout<<"--------------------------------------------------------------------------------------------------------"<<endl;

    TH2F * residual_slope_2Dhist = new TH2F("","",100,-10,10,100,-10,10);
    residual_slope_2Dhist -> SetStats(0);
    for (int i = 0; i < input_vec_1.size(); i++)
    {
        residual_slope_2Dhist -> Fill(input_vec_1[i],input_vec_2[i]);
    }

    residual_slope_2Dhist -> SetTitle("Correlation of residual and l0 position");
    residual_slope_2Dhist -> GetXaxis() -> SetTitle("Residual, unit : mm");
    residual_slope_2Dhist -> GetYaxis() -> SetTitle("L0 cluster pos, unit : mm");

    residual_slope_2Dhist -> Draw("colz0");

    c3 -> Print( Form("%s/DUT_residual_l0pos_2Dhist.pdf",folder_direction.Data()) );
    c3 -> Clear();
}

void print_used_par ()
{
    
    cout<<"=========================== used parameters =============================="<<endl;
    cout<<"the distance of 1st and 3rd ladder :       "<<actual_xpos[2]<<endl;
    cout<<"the INTT_strip_width :                     "<<INTT_strip_width<<endl;
    cout<<"the lower_initial :                        "<<lower_section_initial<<endl;
    cout<<"the upper_initial :                        "<<upper_section_initial<<endl;
    cout<<" "<<endl;
    cout<<"the residual tolerance :                   "<<noise_hit_distance<<endl;
    cout<<"tge slope cut :                            "<<slope_cut<<endl;
    cout<<"the slope correction :                     "<<slope_correction<<endl;
    cout<<"the cluster size requirement :             "<<cluster_size_requirement<<endl;
    cout<<"the cluster size requirement combined :    "<<cluster_size_requirement_combine<<endl;
    cout<<"the cluster adc value requirement :        "<<cluster_adc_value_requirement<<endl;
    cout<<"the clusters allowed in adjacent layers :  "<<hit_allowed_in_adjacent_layers<<endl;
    cout<<"the edge exclusion :                       "<<boundary_cut<<endl;
    cout<<"=========================== used parameters =============================="<<endl;
}

// void DUT_test_v1 ()
// {
//     // note : the setting pars
//     TString folder_direction = "/Users/chengweishi/Downloads/BeamTest_2021/noise_ana_data/folder_DUT";
//     TString file_name = "run52_no_clone_filter_all_int_1000"; // note : not used
//     TString cluster_file_name = "cluster_information_offset-0.2908_adcinfo";
//     int study_chip = 8;

//     // vector<profile_str> profile_str_vec; profile_str_vec.clear();
//     // profile_str_vec = profile_read_and_build(folder_direction, file_name, cluster_file_name, study_chip);

//     vector<cluster_str> cluster_str_vec; cluster_str_vec.clear();
//     cluster_str_vec = cluster_read_and_build(folder_direction, file_name, cluster_file_name, study_chip);   

//     vector<cluster_reformat_str> cluster_reformat_str_vec; cluster_reformat_str_vec.clear();
//     cluster_reformat_str_vec = cluster_reformat(cluster_str_vec);

//     DUT_str DUT_data = efficiency_DUT_method(cluster_reformat_str_vec,study_chip);

//     // note : make plot
//     plot_residual( DUT_data.middle_layer_residual, folder_direction );
//     plot_angle( DUT_data.good_combination_slope, folder_direction );

//     cout<<"# of 111 : "<<DUT_data.track_111_count<<endl;
//     cout<<"# of 101 : "<<DUT_data.track_101_count<<endl;
//     cout<<"Efficiency : "<<double(DUT_data.track_111_count) / double(DUT_data.track_111_count + DUT_data.track_101_count)<<endl;


//     TH1F * total_hist = new TH1F ("","",1,0,1);
//     TH1F * pass_hist = new TH1F ("","",1,0,1);
    
//     // note : layer - 1 efficiency
//     total_hist->SetBinContent(1,(DUT_data.track_111_count + DUT_data.track_101_count)); 
//     pass_hist ->SetBinContent(1,DUT_data.track_111_count);

//     TEfficiency * detection_effi = new TEfficiency (*pass_hist,*total_hist);
//     printf("Efficiency by TEfficiency \n");
//     printf("%.5f\t+%.5f\t-%.5f \n", detection_effi->GetEfficiency(1)*100.,detection_effi->GetEfficiencyErrorUp(1)*100.,detection_effi->GetEfficiencyErrorLow(1)*100.);


// }
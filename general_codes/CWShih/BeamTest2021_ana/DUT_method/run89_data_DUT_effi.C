#include "DUT_test_v1.h"

void run89_data_DUT_effi ()
{
    // note : the setting pars
    TString folder_direction = "/Users/chengweishi/Downloads/BeamTest_2021/DUT_efficiency/run89_data";
    TString file_name = "not_used"; // note : not used
    TString cluster_file_name = "cluster_information_offset-0.2850_adcinfo";
    int study_chip = 10;

    // noise_hit_distance = 0.234; // note : the tolerance of the residual
    // double slope_cut = 0.015; // note : the parameter for the DUT
    // double slope_correction = 0.002725; // note : to correct the slope distribution. 
    // int hit_allowed_in_adjacent_layers = 1;
    // int cluster_size_requirement = 0; // note : the cluster size of single cluster
    // int cluster_size_requirement_combine = 2;
    // double cluster_adc_value_requirement = 22.5;
    // double INTT_strip_width = 0.078;
    // double lower_section_initial = -9.945;
    // double upper_section_initial = 0.039;
    // int boundary_cut = 5;

    // note : change the parameter : 
    // change_noise_hit_distance(0.1872);
    change_slope_cut(0.01);
    change_slope_correction(0.005269);
    // change_cluster_size_requirement(1); // note : " > the_value" 
    change_cluster_size_requirement_combine(1); // note : " > the_value" "to allow the combination to be 2+1"
    // change_cluster_adc_value_requirement(); // note : " > the_value"
    // change_hit_allowed_in_adjacent_layers(); // note : " == the_value "
    // change_INTT_strip_width();
    // change_lower_section_initial();
    // change_upper_section_initial();
    change_boundary_cut(5);


    // vector<profile_str> profile_str_vec; profile_str_vec.clear();
    // profile_str_vec = profile_read_and_build(folder_direction, file_name, cluster_file_name, study_chip);

    vector<cluster_str> cluster_str_vec; cluster_str_vec.clear();
    cluster_str_vec = cluster_read_and_build(folder_direction, file_name, cluster_file_name, study_chip);   

    vector<cluster_reformat_str> cluster_reformat_str_vec; cluster_reformat_str_vec.clear();
    cluster_reformat_str_vec = cluster_reformat(cluster_str_vec);

    DUT_str DUT_data = efficiency_DUT_method(cluster_reformat_str_vec,study_chip);

    // note : make plot
    plot_residual( DUT_data.middle_layer_residual, folder_direction );
    plot_residual_narrow( DUT_data.middle_layer_residual, folder_direction );
    plot_angle( DUT_data.good_combination_slope_hit3, DUT_data.good_combination_slope_hit2, folder_direction );
    hit3_plot_residual_slope_2D(DUT_data.middle_layer_residual,DUT_data.good_combination_slope_hit3, folder_direction);
    hit3_plot_residual_position_2D(DUT_data.middle_layer_residual,DUT_data.good_combination_l0_pos_hit3, folder_direction);

    // note : calculate the efficiency, roughly
    cout<<"# of 111 : "<<DUT_data.track_111_count<<endl;
    cout<<"# of 101 : "<<DUT_data.track_101_count<<endl;
    cout<<"Efficiency : "<<double(DUT_data.track_111_count) / double(DUT_data.track_111_count + DUT_data.track_101_count)<<endl;

    // note : Efficiency calculation by the TEfficiency (layer-1 efficiency)
    TH1F * total_hist = new TH1F ("","",1,0,1);
    TH1F * pass_hist = new TH1F ("","",1,0,1);
    
    total_hist->SetBinContent(1,(DUT_data.track_111_count + DUT_data.track_101_count)); 
    pass_hist ->SetBinContent(1,DUT_data.track_111_count);

    TEfficiency * detection_effi = new TEfficiency (*pass_hist,*total_hist);
    printf("Efficiency by TEfficiency \n");
    printf("%.5f\t+%.5f\t-%.5f \n", detection_effi->GetEfficiency(1)*100.,detection_effi->GetEfficiencyErrorUp(1)*100.,detection_effi->GetEfficiencyErrorLow(1)*100.);
    
    // note : print the used parameters
    print_used_par();


} 
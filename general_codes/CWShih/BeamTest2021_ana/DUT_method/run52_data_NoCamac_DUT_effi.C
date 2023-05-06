#include "DUT_test_v1.h"

void run52_data_NoCamac_DUT_effi ()
{
    // note : the setting pars
    TString folder_direction = "/Users/chengweishi/Downloads/BeamTest_2021/DUT_efficiency/run52_data_NoCamac";
    TString file_name = "not_used"; // note : not used
    TString cluster_file_name = "cluster_information_offset-0.2908_adcinfo_NoCamac";
    int study_chip = 8;

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
    // change_noise_hit_distance(0.1964);
    change_slope_cut(0.01);
    change_slope_correction(0.002725);
    // change_cluster_size_requirement(1); // note : " > the_value" 
    change_cluster_size_requirement_combine(1); // note : " > the_value" "to allow the combination to be 2+1"
    // change_cluster_adc_value_requirement(); // note : " > the_value"
    // change_hit_allowed_in_adjacent_layers(); // note : " == the_value "
    // change_INTT_strip_width();
    // change_lower_section_initial();
    // change_upper_section_initial();
    change_boundary_cut(5);

    vector<cluster_str> cluster_str_vec; cluster_str_vec.clear();
    cluster_str_vec = cluster_read_and_build(folder_direction, file_name, cluster_file_name, study_chip);   

    vector<cluster_reformat_str> cluster_reformat_str_vec; cluster_reformat_str_vec.clear();
    cluster_reformat_str_vec = cluster_reformat(cluster_str_vec);

    // note : it's the V1 version, more flexible, but for the residual distribution, it can be more correct.
    // DUT_str DUT_data = efficiency_DUT_method(cluster_reformat_str_vec,study_chip);


    // note : it's the v2 version, not flexible, but for the residual dsitribution, it is the correct one, right after the application of the residual cut.
    DUT_str DUT_data_v2 = efficiency_DUT_method_v2(cluster_reformat_str_vec,study_chip);

    // note : make plot
    plot_residual( DUT_data_v2.middle_layer_residual, folder_direction );
    plot_residual_narrow( DUT_data_v2.middle_layer_residual, folder_direction );
    plot_angle( DUT_data_v2.good_combination_slope_hit3, DUT_data_v2.good_combination_slope_hit2, folder_direction );

    // note : print the efficiency
    // print_effi(DUT_data);
    print_effi(DUT_data_v2);
    
    // note : print the used parameters
    print_used_par();


} 
#include "InttConversion_new.h"


InttConversion::InttConversion()
{}

InttConversion::InttConversion(string conversion_mode, double peek)
:conversion_mode(conversion_mode), peek(peek)
{
    // todo : check the file you use
    TFile * file_in = TFile::Open(Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/geo_folder/survey_data_all_3.32.root"));
    TTree * tree = (TTree *)file_in->Get("t_ch_pos");
    int pid;
    int module;
    int chip;
    int chan;
    double ch_x;
    double ch_y;
    double ch_z;
    double ch_phi;

    tree -> SetBranchAddress("pid",&pid);
    tree -> SetBranchAddress("module",&module);
    tree -> SetBranchAddress("chip",&chip);
    tree -> SetBranchAddress("chan",&chan);
    tree -> SetBranchAddress("x",&ch_x);
    tree -> SetBranchAddress("y",&ch_y);
    tree -> SetBranchAddress("z",&ch_z);
    tree -> SetBranchAddress("phi",&ch_phi);

    // pos_3D all_ch_pos[8][14][26][128];
    
    for (int i = 0; i < tree -> GetEntries(); i++){
        tree -> GetEntry(i);
        all_ch_pos[pid][module][chip-1][chan] = {float(ch_x),float(ch_y),float(ch_z)};
    }

    file_in -> Close();
}


pair<double,double> InttConversion::Get_dummy_ladder_XY(int chip_id, int chan_id, int ladder_SN)
{
    // note : chip_id 1 ~ 13 -> 0 
    // note : chip_id 14 ~ 26 -> 1
    int row_index = (chip_id > 0 && chip_id < 14) ? 0 : 1;
    double dummy_X = 0; 
    double dummy_Y;
    
    // note : north
    if (ladder_SN == 1) {
        dummy_Y = (row_index == 0) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;
    }
    // note : south
    else if (ladder_SN == 0){
        dummy_Y = (row_index == 1) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;
    }
    else{
        cout<<" wrong ladder_SN ID "<<endl;
        exit(1);
    }

    // note : original, only works for north
    // // note : chip_id 1 ~ 13, 14 ~ 26
    // int row_index = (chip_id > 0 && chip_id < 14) ? 0 : 1;

    // double dummy_X = 0;
    // double dummy_Y = (row_index == 0) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;


    return {dummy_X,dummy_Y};
}

pair<double,double> InttConversion::Get_self_rotation( pair<double,double> chan_pos, double angle)
{
    return {chan_pos.second * cos(angle / (180/TMath::Pi())), chan_pos.second * sin(angle / (180/TMath::Pi()))};
}

pos_str InttConversion::Get_XY_all(/*string ladder_name*/string server, int module, int chip_id, int chan_id)
{

    vector<string> conversion_mode_DB = {"ideal","nominal","survey_1_XY","survey_1_XYAlpha","survey_1_XYAlpha_Peek","full_survey_3.32"};

    // string ladder_pos_str = ladder_name;
    string ladder_pos_str = string(serverFC_toinfo_map[Form("%s_%i",server.c_str(),module)].Ladder);
    
    int layer_index; 
    if      (ladder_pos_str.substr(0,4) == "B0L0") layer_index = 0;
    else if (ladder_pos_str.substr(0,4) == "B0L1") layer_index = 1;
    else if (ladder_pos_str.substr(0,4) == "B1L0") layer_index = 2;
    else if (ladder_pos_str.substr(0,4) == "B1L1") layer_index = 3;
    else 
    {
        cout<<"error happens, wrong name"<<endl;
        exit(1);
    }

    int ladder_index = stoi(ladder_pos_str.substr(4,2));

    // note : 0, 1, 2, 3 : south, id 0
    // note : 4, 5, 6, 7 : north, id 1
    int ladder_SN = ( stoi(server.substr(4,1)) < 4 ) ? 0 : 1;
    // cout<<"test : ladder_SN : "<<ladder_SN<<endl;

    
    double ladder_location_angle = ladder_location_angle_ini_correction[layer_index] -  (angle_increment[layer_index] * ladder_index);

    double ring_pos_x = layer_raduis[layer_index] * cos(ladder_location_angle / (180/TMath::Pi()));
    double ring_pos_y = layer_raduis[layer_index] * sin(ladder_location_angle / (180/TMath::Pi()));

    double ladder_self_pos_X;
    double ladder_self_pos_Y;
    double final_pos_X;
    double final_pos_Y;
    double final_pos_Z;

    if (conversion_mode == conversion_mode_DB[0] || conversion_mode == conversion_mode_DB[1]) // note : ideal, nominal
    {
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second;
        final_pos_X = ring_pos_x + ladder_self_pos_X;
        final_pos_Y = ring_pos_y + ladder_self_pos_Y;
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[2] ) // note : survey_1_XY (based on survey data typeB south)
    {
        // note : ideal ladder rotation
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second;
        
        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y;
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[3] ) // note : survey_1_XYAlpha (based on survey data typeB south)
    {
        // note : here is an attempt, try to import the survey data, but only the center position of ladders

        // note : 
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second;
        
        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y;    
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[4] ) // note : survey_1_XYAlpha_Peek
    {
        // note : 
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second;
        
        double peek_correction_x = peek * cos( ( survey_ladder_rotate[layer_index][ladder_index] + 90. + 90.) / (180./M_PI) );
        double peek_correction_y = peek * sin( ( survey_ladder_rotate[layer_index][ladder_index] + 90. + 90.) / (180./M_PI) );
        // cout<<" test : "<<sqrt(pow(peek_correction_x,2)+pow(peek_correction_y,2))<<endl;
        // if (chip_id == 1)
        // { 
        //     if (chan_id == 0)
        //     {
        //         cout<<"test : "<<ladder_pos_str<<" "<<survey_ladder_rotate[layer_index][ladder_index] + 180.<<" "<<peek_correction_x<<" "<<peek_correction_y<<endl;
        //     }
        // }
        

        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X + peek_correction_x;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y + peek_correction_y;    
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if (conversion_mode == conversion_mode_DB[5]) // note : full survey data with the 3.32 mm correction inward. 2023_10_11
    {
        // trial_channel.felix_server = server[4] - '0'; // note : from 0 to 7
        // trial_channel.felix_channel = module;
        // trial_channel.chip = chip_id - 1; // note : from 0 to 25
        // trial_channel.channel = chan_id;   // note : chan should be from 0 to 127

        // Eigen::Affine3d sensor_trans = Intt::GetTransform(survey_tree, ToOffline(trial_channel));
        // Eigen::Vector4d local_trans = Intt::GetLocalPos(ToOffline(trial_channel));
        // final_pos_X = (sensor_trans * local_trans)[0];
        // final_pos_Y = (sensor_trans * local_trans)[1];
        // final_pos_Z = (sensor_trans * local_trans)[2];

        pos_3D single_ch_pos = all_ch_pos[(server[4] - '0')][module][chip_id - 1][chan_id];

        final_pos_X = single_ch_pos.x;
        final_pos_Y = single_ch_pos.y;
        final_pos_Z = single_ch_pos.z;
    }
    else 
    {
        cout<<"InttConversion_new.h wrong mode input"<<endl;
        cout<<"you have filled : "<<conversion_mode<<endl;
        exit(1);
    }    

    int    final_pos_layer = (layer_index == 0 || layer_index == 1) ? 0 : 1;
    double final_pos_phi = (final_pos_Y < 0) ? atan2(final_pos_Y,final_pos_X) * (180./TMath::Pi()) + 360 : atan2(final_pos_Y,final_pos_X) * (180./TMath::Pi());

    return {final_pos_X,final_pos_Y,final_pos_Z,final_pos_layer,final_pos_phi};
}

string InttConversion::GetGeoMode()
{
    return conversion_mode;
}

string InttConversion::GetLadderName(string server_module)
{
    return (serverFC_toinfo_map[server_module].Ladder).Data();
}


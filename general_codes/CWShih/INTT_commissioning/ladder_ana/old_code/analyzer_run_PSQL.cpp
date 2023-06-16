struct ladder_map_format{
    int module_id;
    TString port;
    TString ladder;
};

vector<ladder_map_format> ReadMap( TString DataIn_directory )
{
    // TString map_name = input_file; //map_name = map_name.ReplaceAll(".root","_map.txt");
    vector<ladder_map_format> map_info;
    ladder_map_format map_info_unit;

    unsigned long len_unsigned_long = -1;

    fstream map_file;
    map_file.open(Form("%s", DataIn_directory.Data()), ios::in);
    if (map_file.is_open()) 
    {
        string data;
        while (getline(map_file, data)) 
        {
            if ( TString(data[0]) == "#"  )  { continue; }
            if ( TString(data[0]) == " "  )  { continue; }
            if ( TString(data[0]) == ""   )  { continue; }
            if ( TString(data[0]) == "  " )  { continue; }
            if (TString(data).Length() < 2)  { continue; }
            if ( data.find("B")   == len_unsigned_long)     { continue; }

            TString data_TString(data);
            data_TString.ReplaceAll("\t"," "); // note : to replace the "tab, \t" by the space
            TObjArray * data_split = data_TString.Tokenize(" ");

            map_info_unit.module_id = (( (TObjString *) data_split->At(0) ) -> String()).Atoi();
            map_info_unit.port      = ( (TObjString *) data_split->At(1) ) -> String();
            map_info_unit.ladder    = ( (TObjString *) data_split->At(2) ) -> String();
            map_info.push_back(map_info_unit);
        }
        map_file.close();
    }
    
    
    cout<<"-----------------------Map content----------------------------"<<endl;
    for (int i = 0; i < map_info.size(); i++)
    {
        cout<<map_info[i].module_id<<" "<<map_info[i].port<<" "<<map_info[i].ladder<<" "<<endl;
    }

    return map_info;

}

// note : read the map of the maps
vector<TString> read_map_map_file(TString main_folder_directory, TString south_map_file)
{

    vector<TString> output_file; output_file.clear();

    fstream read_map_file;
    read_map_file.open(Form("%s/%s", main_folder_directory.Data(),south_map_file.Data()), ios::in);
    if (read_map_file.is_open()) 
    {
        string data;
        while (getline(read_map_file, data)) 
        {
            if (TString(data).Length() < 2) {continue;}
            // cout<<data<<endl;
            output_file.push_back(data);
        }
        read_map_file.close();
    }

    return output_file;
    
}

vector<TString> Get_directory(vector<TString> input_vec)
{
    vector<TString> output_vec; output_vec.clear();
    for (int i = 0; i < input_vec.size(); i++)
    {
        output_vec.push_back( string(input_vec[i]).substr(0, string(input_vec[i]).find_last_of("/")) );
    }

    return output_vec;
}

vector<TString> Get_filename(vector<TString> input_vec)
{
    vector<TString> output_vec; output_vec.clear();
    for (int i = 0; i < input_vec.size(); i++)
    {
        output_vec.push_back(string(input_vec[i]).substr(string(input_vec[i]).find_last_of("/")+1));
    }

    return output_vec;
}

void print_map_info (vector<TString> map_map_directory, vector<TString> map_map_filename)
{
    for (int i = 0; i < map_map_directory.size(); i++)
    {
        cout<<map_map_directory[i]<<endl;
    }

    for (int i = 0; i < map_map_filename.size(); i++)
    {
        cout<<map_map_filename[i]<<endl;
    }
}

void analyzer_run_PSQL (TString full_data_directory, TString full_map_directory, int threshold, int ROC_index)
{
    
    TString DataIn_directory = string(full_data_directory).substr(0, string(full_data_directory).find_last_of("/"));
    TString DataIn_filename = string(full_data_directory).substr(string(full_data_directory).find_last_of("/")+1);
    TString DataOut_directory = "/home/inttdev/data/IR/folder_cali_ana";

    cout<<"= = = = = = = = = = = = = = = = = = = = working on file : "<<DataIn_filename<<" = = = = = = = = = = = = = = = = = = = = = = "<<endl;
    vector<ladder_map_format> ladder_map = ReadMap( full_map_directory );
    
    for (int i1 = 0; i1 < ladder_map.size(); i1++ )
    {
        cout<<" "<<endl;
        cout<<"------->> "<<ladder_map[i1].module_id<<" "<<ladder_map[i1].port<<" "<<ladder_map[i1].ladder<<" "<<endl;

        TString directory_in_word = Form("\\\"%s\\\"",DataIn_directory.Data());

        TString full_map_in_word = Form("\\\"%s\\\"",full_map_directory.Data());
        
        TString directory_out_word = Form("\\\"%s\\\"",DataOut_directory.Data());
        
        TString file_name_word = Form("\\\"%s\\\"",DataIn_filename.Data());
        
        int ROC_index_word = ROC_index;

        // TString output_log_word = Form("%s/%s/run_%i.out",main_folder_directory.Data(), string(map_map_directory[i]).substr(string(map_map_directory[i]).find_last_of("/")+1).c_str(),ladder_map[i1].module_id);
        
        system(Form("nohup root -l -b -q /home/inttdev/work/cwshih/calib_db/file_logger_PSQL/ladder_ana/analyzer_multi.cpp\\(%i,%s,%s,%s,%s,%i,%i\\)&>nohup.out&",ladder_map[i1].module_id, directory_in_word.Data(), full_map_in_word.Data(), directory_out_word.Data(), file_name_word.Data(), ROC_index_word, threshold));

        sleep(1);
    }

}
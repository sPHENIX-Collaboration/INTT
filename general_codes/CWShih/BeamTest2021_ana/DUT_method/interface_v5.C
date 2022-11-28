// note : this code is for converting the MC data into INTT BeamTest format.
// note : this code is more powerful, the idea is to form the sets of sci part and INTT_silicon part
// note : and do the match, therefore, it's ok some of the event that failed in the matching, we just lose some events. 
// note : But we can make sure that the content of the event is correct

// note : update 2022/11/25 : sort the eID of sci_MC and INTT_MC, then it should work
// note : when run the code, check the "todo" first, 2022/11/10, CW



//note : the structure for INTT ladder, MC
struct INTT_event_MC
{
    int eID_MC;
    vector<int> UpandDown_MC;
    vector<int> Xpos_MC;
    vector<int> Ypos_MC;
    vector<int> Zpos_MC;
    vector<int> silicon_type_MC;
    vector<double> edep_MC;
};

INTT_event_MC INTT_MC_clear()
{
    INTT_event_MC nothing;
    nothing.eID_MC = 0;

    nothing.UpandDown_MC.clear();
    nothing.Xpos_MC.clear();
    nothing.Ypos_MC.clear();
    nothing.Zpos_MC.clear();
    nothing.silicon_type_MC.clear();
    nothing.edep_MC.clear();

    return nothing;
}
//note : the structure for scintillator, MC
struct sci_event_MC
{
    int eID_MC;
    vector<int>edep_MC;
    vector<int>sci_ID_MC;
};

sci_event_MC sci_MC_clear()
{
    sci_event_MC nothing;
    nothing.eID_MC = 0;
    nothing.edep_MC.clear();
    nothing.sci_ID_MC.clear();

    return nothing;
}

//note : the final output structure
struct INTT_event_final_s
{
    vector<int> camac_adc_final;
    vector<int> camac_tdc_final;
    bool INTT_event_final;
    vector<int> adc_array_final;
    vector<int> ampl_array_final;
    vector<int> chip_id_array_final;
    vector<int> fpga_id_array_final;
    vector<int> module_array_final;
    vector<int> chan_id_array_final;
    vector<int> fem_id_array_final;
    vector<UInt_t> bco_array_final;
    vector<UInt_t> bco_full_array_final;
    vector<int> event_array_final;

    int MC_eID;

};

INTT_event_final_s INTT_final_initialize(/*double INTT_event_ratio, TRandom *rand_INTT_event ,TRandom *rand_bcofull*/)
{
    INTT_event_final_s nothing;

    nothing.camac_adc_final.clear();
    nothing.camac_tdc_final.clear();
    
    nothing.INTT_event_final = 0;

    nothing.adc_array_final.clear();
    nothing.ampl_array_final.clear();
    nothing.chip_id_array_final.clear();
    nothing.fpga_id_array_final.clear();
    nothing.module_array_final.clear();
    nothing.chan_id_array_final.clear();
    nothing.fem_id_array_final.clear();
    nothing.bco_array_final.clear();
    nothing.bco_full_array_final.clear();
    nothing.event_array_final.clear();

    nothing.MC_eID = 0;

    // Frand_INTT_event = rand_INTT_event -> Uniform(0,1);
    // if (Frand_INTT_event<INTT_event_ratio) // the setting of INTT_event efficiency is here. Now is set to be 90%
    // {
    //     nothing.INTT_event_final = 1;
    // }
    // else
    // {
    //     nothing.INTT_event_final = 0;
    // }

    // // random generator for bco_full
    // // for bco, it is the lowest 7 bits of bco_full, so the bco_full and bco are not fully independent.
	// UInt_t Frand_bcofull = int(rand_bcofull->Uniform(0.,65536.)); //0 ~ 65535
	// UInt_t Frand_bco = Frand_bcofull & UInt_t( 255 ); // take the lowest 7 bits from the BCO full

    return nothing;
}

INTT_event_final_s Noise_hit_add (int sci_loop, int sci_eID,INTT_event_final_s single_event, TRandom *rand_noise, int module_id_l1,int module_id_l2, int module_id_l3, TH1F * hist_1D[7], TH2F * hist_2D[2])
{

    // keep the the numbers that will be used but fixed.    
    // int keep_camac_adc   = single_event.camac_adc_final[0];
    // int keep_camac_tdc   = single_event.camac_tdc_final[0];
    // bool keep_INTT_event = single_event.INTT_event_final;
    int keep_ampl        = single_event.ampl_array_final[0];
    int keep_fpga_id     = single_event.fpga_id_array_final[0];
    int keep_fem_id      = single_event.fem_id_array_final[0];
    int keep_bco         = single_event.bco_array_final[0];
    int keep_bco_full    = single_event.bco_full_array_final[0];
    int keep_event       = single_event.event_array_final[0];
    
    
    // the distribution of # of the noise hit
    double noise_hit_dis_mean = 0;
    double noise_hit_dis_sig =  1.5;
    
    // int dis_N_noise_hit = int(round(fabs(rand_noise->Gaus(noise_hit_dis_mean,noise_hit_dis_sig))));

    // note : new method to predict noise level
    // note : this method is based on the data (run52), extract the noise from the data. 
    int dis_N_noise_hit;
    if ( rand_noise -> Uniform(0,1) <0.783 ){dis_N_noise_hit = 0;} // note : no noise hit to be added.
    else // note : if the noise hit is decided to be added (by random), the # of the noise hit is defined by the following random dist..
    {  
        // note : setting parameters are based on the slide "INTT_2022_08_24", page 6.
        dis_N_noise_hit = int(rand_noise -> Exp(1.01819e+00) +1);
    }


    hist_1D[1] -> Fill(dis_N_noise_hit);

    // int dis_N_noise_hit = 5;

    //the chip chan adc
    int noise_hit_chip;
    int noise_hit_chan;
    int noise_hit_layer; int rand_for_layer;
    int noise_hit_adc;

    int check_double_count = 0;

    for (int i=0; i<dis_N_noise_hit; i++)
    {
        noise_hit_chip  = int(rand_noise -> Uniform(1,27));
        noise_hit_chan  = int(rand_noise -> Uniform(0,128));
        rand_for_layer  = int(rand_noise -> Uniform(1,4)); // title : follow the G4 convention. layer ID 1 ~ 3
        noise_hit_adc   = int(rand_noise -> Uniform(0,8));

        if      (rand_for_layer == 1) { noise_hit_layer = module_id_l1; }
        else if (rand_for_layer == 2) { noise_hit_layer = module_id_l2; }
        else if (rand_for_layer == 3) { noise_hit_layer = module_id_l3; }

        // to check whether the generated noise hit already exists or not. 
        for (int i1 = 0; i1 < single_event.chip_id_array_final.size(); i1++)
        {
            if ( single_event.chip_id_array_final[i1] == noise_hit_chip && single_event.chan_id_array_final[i1] == noise_hit_chan && single_event.module_array_final[i1] == noise_hit_layer )
            {
                cout<<"sci_loop : "<<sci_loop<<" sci_eID : "<<sci_eID<<" hit order : "<<i1<<" hit is duplicate ! module : "<<noise_hit_layer<<" chip : "<<noise_hit_chip<<" chan : "<<noise_hit_chan<<endl;
                check_double_count = 1;
                break;
            }
        }

        if ( check_double_count == 1 ) { i -= 1; } // if the noise hit already exists, re-generate the hit. 
        else
        {
            
            // single_event.camac_adc_final.push_back(keep_camac_adc);
            // single_event.camac_tdc_final.push_back(keep_camac_tdc);
            // single_event.INTT_event_final.push_back(keep_INTT_event);
            single_event.adc_array_final.push_back(noise_hit_adc); // for random, new hit
            single_event.ampl_array_final.push_back(keep_ampl);
            single_event.chip_id_array_final.push_back(noise_hit_chip); // for random, new hit
            single_event.fpga_id_array_final.push_back(keep_fpga_id);
            single_event.module_array_final.push_back(noise_hit_layer); // for random, new hit
            single_event.chan_id_array_final.push_back(noise_hit_chan); // for random, new hit
            single_event.fem_id_array_final.push_back(keep_fem_id);
            single_event.bco_array_final.push_back(keep_bco);
            single_event.bco_full_array_final.push_back(keep_bco_full);
            single_event.event_array_final.push_back(keep_event);

            hist_1D[0] -> Fill(noise_hit_adc);

            hist_2D[0] -> Fill(noise_hit_chip,noise_hit_chan);

            hist_1D[6] -> Fill(rand_for_layer);

        }

        check_double_count = 0;
    }

    return single_event;
}

//the histograms for noise hit monitoring
int hist_define ( TH1F * hist_1D[7], TH2F * hist_2D[2] )
{

    // 1D histogram
    hist_1D [0] = new TH1F ("","Noise, adc distribution",8,0,8);
    // hist_1D [0] -> GetYaxis() -> SetRangeUser(0,300);
    hist_1D [0] -> SetMinimum(0);
    hist_1D [0] -> GetXaxis() -> SetTitle("adc");

    hist_1D [1] = new TH1F ("","Noise, # of hit per event",15,0,15);
    hist_1D [1] -> GetXaxis() -> SetTitle("# of noise hit");


    hist_1D [2] = new TH1F ("","Data, adc distribution",8,0,8);
    hist_1D [2] -> GetXaxis() -> SetTitle("adc");

    hist_1D [3] = new TH1F ("","Data, # of hit per event",15,0,15);
    hist_1D [3] -> GetXaxis() -> SetTitle("# of hit");
   

    hist_1D [4] = new TH1F ("","Combine, adc distribution",8,0,8);
    hist_1D [4] -> GetXaxis() -> SetTitle("adc");
     
    hist_1D [5] = new TH1F ("","Combine, # of hit per event",15,0,15);
    hist_1D [5] -> GetXaxis() -> SetTitle("# of hit");

    hist_1D [6] = new TH1F ("","Noise, layer distribution",4,0,4);
    // hist_1D [6] -> GetYaxis() -> SetRangeUser(0,1000);
    hist_1D [6] -> SetMinimum(0);
    hist_1D [6] -> GetXaxis() -> SetTitle("layer");


    // 2D histogram
    hist_2D [0] = new TH2F ("","Hit map, noise only",26,1,27,128,0,128);
    hist_2D [0] -> SetStats(0);
    hist_2D [0] -> GetXaxis() -> SetTitle("Chip");
    hist_2D [0] -> GetYaxis() -> SetTitle("Channel");

    hist_2D [1] = new TH2F ("","Hit map, data only",26,1,27,128,0,128);
    hist_2D [1] -> SetStats(0);
    hist_2D [1] -> GetXaxis() -> SetTitle("Chip");
    hist_2D [1] -> GetYaxis() -> SetTitle("Channel");

    hist_2D [2] = new TH2F ("","Hit map, noise + data combine",26,1,27,128,0,128);
    hist_2D [2] -> SetStats(0);
    hist_2D [2] -> GetXaxis() -> SetTitle("Chip");
    hist_2D [2] -> GetYaxis() -> SetTitle("Channel");

    return 0;
}

void interface_v5 (TString file_name, int random_seed = 200)
{
    // todo : reduce the event 
    bool reduce_effi = true;
    double target_l1_effi = 0.95;
    //the direction of the data
	TString folder_name = "/data4/chengwei/Geant4/INTT_simulation/G4/for_CW/data/run52_MC_2";
	//use TFile, TTree to open the tree
	TFile *f1 = TFile::Open(Form("%s/%s.root", folder_name.Data(), file_name.Data()));
 	// Chamber1 -> the tree to save the information of INTT ladders in  G4
	TTree *Gettree = (TTree *)f1->Get("Chamber1");
	// sci_trigger -> the tree to save the information of scintillators in G4
	TTree *Gettree_sci = (TTree *)f1->Get("sci_trigger");

    cout<<"we are now working on : "<<file_name<<".root"<<endl;

    int N_converted_event = 500000;

	int Event_ID, UpandDown, Xpos, Ypos, Zpos, silicon_type;
	double Edep;
	double sci_edep;
	int Event_ID_sci,sci_ID;


	// total channel hits of the whole run, it should be ~ 4 times # of event you set in G4, because the G4 model has 4 ladders 
	int total_size = Gettree->GetEntriesFast();
	cout<<"total entry INTT   : "<<total_size<<endl;

	//The branch in Chamber1, 
	//I divide the INTT sensitive area into 4 region ("Up and down of sensor" * "type A and B") 
	//-> UpandDown & silicon_type -> 1,1   1,0  0,1   0,0
	//Event_ID is the ID to separate the event in G4
	Gettree->SetBranchAddress("Event_ID", &Event_ID);
	Gettree->SetBranchAddress("UpandDown", &UpandDown);
	Gettree->SetBranchAddress("Xpos", &Xpos);
	Gettree->SetBranchAddress("Ypos", &Ypos);
	Gettree->SetBranchAddress("Zpos", &Zpos);
	Gettree->SetBranchAddress("silicon_type", &silicon_type);
	Gettree->SetBranchAddress("Edep", &Edep);//in MeV

    //to get the first event ID of the root file. If you run the G4 with multi-thread, the order of event ID can be disordered, 
	//0 may not be the first one.  
	Gettree->GetEntry(0);
	int event_buffer;
	event_buffer=Event_ID;
	cout<<"First INTT EventID : "<<event_buffer<<endl;

	// total hits of the whole run, it should be ~ 3 times # of event you set in G4, because the G4 model has 3 layers of scintillator 
	int total_size_sci = Gettree_sci->GetEntriesFast();
	cout<<"total entry SCI    : "<<total_size_sci<<endl;

	//the branch in sci_trigger
	//sci_ID from 0 to 2 (3 scintillators)
	Gettree_sci->SetBranchAddress("Event_ID",&Event_ID_sci);
	Gettree_sci->SetBranchAddress("sci_ID",&sci_ID);
	Gettree_sci->SetBranchAddress("sci_edep",&sci_edep);

	//to get the first event ID of the root file. If you run the G4 with multi-thread, the order of event ID can be disordered, 
	//0 may not be the first one.  
	//it can be different with INTT event_ID, if the first sci or INTT get nothing.   
	Gettree_sci->GetEntry(0); 
	int event_buffer_sci = Event_ID_sci;
	cout<<"First SCI EventID  : "<<event_buffer_sci<<endl;

	//the setting of Gain, offset, ADC setting and module ID is here
	int adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event;
	double Gain=100;
	double offset=200;
	int set_DAC0=15;
	int set_DAC1=30;
	int set_DAC2=60;
	int set_DAC3=90;
	int set_DAC4=120;
	int set_DAC5=150;
	int set_DAC6=180;
	int set_DAC7=210;

    // title : note that, in G4, the Zpos is from 1 ~ 3. 
    // title : but in testbeam, the final layer ID is from 0 ~ 2.

	int module_id_l0=1000; // title not used
	int module_id_l1=1;
	int module_id_l2=6;
	int module_id_l3=5;
	double ev_DAC_convert;

	int sci_require = 0;
    double INTT_event_ratio = 0.98;
    int show_rate = 1000;
    bool add_noise_bool = false;
    bool fix_rand_seed = true;

    TH1F * hist_1D[7];
    TH2F * hist_2D[3];
    hist_define(hist_1D,hist_2D);

    // cout<<"hist test : "<<hist_1D[0] -> GetBinCenter(3)<<endl;

	// the random generator for bco and bco_full
	TRandom *rand_bcofull = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_bcofull -> SetSeed(random_seed); }
	UInt_t Frand_bcofull;
	//int Frand_bco =  int(rand_bco->Uniform(0.,128.)); //0 ~ 127
	UInt_t Frand_bco;
	
	int event_count=0;
	vector<int> camac_adc; camac_adc.clear();
	vector<int> camac_tdc; camac_tdc.clear();
	bool INTT_event;

	// the random generator for camac_tdc
	// the range I use is beased on the cosmic test result from NWU
    TRandom *rand_effi = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_effi -> SetSeed(random_seed); }
    
	TRandom *rand_tdc1 = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_tdc1 -> SetSeed(random_seed); }
	TRandom *rand_tdc2 = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_tdc2 -> SetSeed(random_seed); }
	TRandom *rand_tdc3 = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_tdc3 -> SetSeed(random_seed); }
	TRandom *rand_tdc6 = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_tdc6 -> SetSeed(random_seed); }
	TRandom *rand_INTT_event = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_INTT_event -> SetSeed(random_seed); }
	int Frand_tdc1 = int (rand_tdc1->Gaus(148,12.5));
	int Frand_tdc2 = int (rand_tdc2->Gaus(139.7,0.87));
	int Frand_tdc3 = int (rand_tdc3->Gaus(128,12.3));
	int Frand_tdc6 = int (rand_tdc6->Uniform(520.,1080.));

    TRandom *rand_noise = new TRandom3 ( 0 ); if (fix_rand_seed == true) { rand_noise -> SetSeed(random_seed); }

	//the random generator for INTT_event 
	double Frand_INTT_event = rand_INTT_event -> Uniform(0,1); 
	int tree_counting=0;
	int first_sci_with_3_index = -1;
	int first_sci_INTT_match_index;
	int match_finder_check=0;
	int loop_match_finder=0;
	int loop_match_finder_check=0;
    int MC_eID;

	vector<int> adc_array;      adc_array.clear();
	vector<int> ampl_array;     ampl_array.clear();
	vector<int> chip_id_array;  chip_id_array.clear();
	vector<int> fpga_id_array;  fpga_id_array.clear();
	vector<int> module_array;   module_array.clear();
	vector<int> chan_id_array;  chan_id_array.clear();
	vector<int> fem_id_array;   fem_id_array.clear();
	vector<UInt_t>	bco_array;      bco_array.clear();
	vector<UInt_t> bco_full_array; bco_full_array.clear();
	vector<int> event_array;    event_array.clear();

	// vector<vector<int>> adc_array_all;      adc_array_all.clear();
	// vector<vector<int>> ampl_array_all;     ampl_array_all.clear();
	// vector<vector<int>> chip_id_array_all;  chip_id_array_all.clear();
	// vector<vector<int>> fpga_id_array_all;  fpga_id_array_all.clear();
	// vector<vector<int>> module_array_all;   module_array_all.clear();
	// vector<vector<int>> chan_id_array_all;  chan_id_array_all.clear();
	// vector<vector<int>> fem_id_array_all;   fem_id_array_all.clear();
	// vector<vector<int>>	bco_array_all;      bco_array_all.clear();
	// vector<vector<int>> bco_full_array_all; bco_full_array_all.clear();
	// vector<vector<int>> event_array_all;    event_array_all.clear();

	//the name of the output file
	//here is the part to create the trees, branches of the output file
	TFile *file_output;
    if (add_noise_bool == false)
    {
        if (reduce_effi == true)
        {
            file_output = new TFile(Form("%s/%s_convert_test_L1EffiReduce_%.2f.root", folder_name.Data(), file_name.Data(),target_l1_effi), "RECREATE");
        }
        else if (reduce_effi == false)
        {
            file_output = new TFile(Form("%s/%s_convert_test.root", folder_name.Data(), file_name.Data()), "RECREATE");
        }
        
    } 
	else if (add_noise_bool == true) 
    {
        file_output = new TFile(Form("%s/%s_convert_noise.root", folder_name.Data(), file_name.Data()), "RECREATE");
    } 

    TTree *tree_output1 = new TTree("tree", "tree");
	TTree *tree_output2 = new TTree("tree_camac", "tree_camac");
	TTree *tree_output3 = new TTree("tree_both", "tree_both");
	
	tree_output1->Branch("adc"     , &adc);
	tree_output1->Branch("ampl"    , &ampl); 
	tree_output1->Branch("chip_id" , &chip_id);
	tree_output1->Branch("fpga_id" , &fpga_id); //0
	tree_output1->Branch("module"  , &module);
	//assume we use 
	//C-1 for layer-0 (6)
	//C-2 for layer-1 (5)
	//C-3 for layer-2 (8)
	//A-1 for layer-3 (2)
	tree_output1->Branch("chan_id" , &chan_id);
	tree_output1->Branch("fem_id"  , &fem_id); //0
	tree_output1->Branch("bco"     , &bco);
	tree_output1->Branch("bco_full", &bco_full);
	tree_output1->Branch("event"   , &event); //fed by event_count, to avoid the case that edep<adc0
	
	tree_output2->Branch("camac_adc",&camac_adc); //edep*120 directly, I don't know the conversion function
	tree_output2->Branch("camac_tdc",&camac_tdc);
	tree_output2->Branch("INTT_event",&INTT_event); //0 or 1, generated randomly. 

	tree_output3->Branch("camac_adc" ,&camac_adc);
	tree_output3->Branch("camac_tdc" ,&camac_tdc);
	tree_output3->Branch("INTT_event",&INTT_event);
	tree_output3->Branch("adc"       ,&adc_array);
	tree_output3->Branch("ampl"      ,&ampl_array);
	tree_output3->Branch("chip_id"   ,&chip_id_array);
	tree_output3->Branch("fpga_id"   ,&fpga_id_array);
	tree_output3->Branch("module"    ,&module_array);
	tree_output3->Branch("chan_id"   ,&chan_id_array);
	tree_output3->Branch("fem_id"    ,&fem_id_array);
	tree_output3->Branch("bco"       ,&bco_array);
	tree_output3->Branch("bco_full"  ,&bco_full_array);
	tree_output3->Branch("event"     ,&event_array); //-1 for always
    tree_output3->Branch("MC_eID"     ,&MC_eID);

    TH1F * sci_hit = new TH1F ("sci_hit","sci_hit",10,0,10);

	// here is the part to convert the sci information of same event into a vector
	// The event_ID_sci is use to separate event
	// sci_array is a 2D vector[eventID][the hit information]

    vector<sci_event_MC> sci_MC; sci_MC.clear();
    sci_event_MC sci_event_basis; sci_event_basis = sci_MC_clear();
    sci_MC.push_back(sci_event_basis);

	for (int i=0; i<total_size_sci; i++)
	{
		Gettree_sci->GetEntry(i);

		if (Event_ID_sci == event_buffer_sci)
		{
			sci_MC[sci_MC.size()-1].eID_MC = Event_ID_sci;
            sci_MC[sci_MC.size()-1].edep_MC.push_back(int(sci_edep*120.));
            sci_MC[sci_MC.size()-1].sci_ID_MC.push_back(sci_ID);
		}
		else
		{
			event_buffer_sci = Event_ID_sci;
            sci_MC.push_back(sci_event_basis);

            sci_MC[sci_MC.size()-1].eID_MC = Event_ID_sci;
            sci_MC[sci_MC.size()-1].edep_MC.push_back(int(sci_edep*120.));
            sci_MC[sci_MC.size()-1].sci_ID_MC.push_back(sci_ID);
		}
	}

	cout<<"size of sci_MC : "<<sci_MC.size()<<endl;

    vector<int> sci_MC_eID_spinout; sci_MC_eID_spinout.clear();
    for (int i = 0; i < N_converted_event /*sci_MC.size()*/; i++)
    {
        sci_MC_eID_spinout.push_back(sci_MC[i].eID_MC);
    }
    int size_sci_MC = N_converted_event /*sci_MC.size()*/;
    int sort_sci_MC_eID[size_sci_MC];
    TMath::Sort(size_sci_MC, &sci_MC_eID_spinout[0], sort_sci_MC_eID,false);
    cout<<"sci_MC sort done !"<<endl;


    vector<INTT_event_MC> INTT_MC; INTT_MC.clear();
    INTT_event_MC INTT_event_basis; INTT_event_basis = INTT_MC_clear();
    INTT_MC.push_back(INTT_event_basis);

    INTT_event_final_s INTT_event_buffer;

    for (int i=0; i<total_size; i++)
    {
        Gettree -> GetEntry(i);

        if (Event_ID == event_buffer)
        {
            INTT_MC[INTT_MC.size()-1].eID_MC = Event_ID;
            INTT_MC[INTT_MC.size()-1].UpandDown_MC.push_back(UpandDown);
            INTT_MC[INTT_MC.size()-1].Xpos_MC.push_back(Xpos);
            INTT_MC[INTT_MC.size()-1].Ypos_MC.push_back(Ypos);
            INTT_MC[INTT_MC.size()-1].Zpos_MC.push_back(Zpos);
            INTT_MC[INTT_MC.size()-1].silicon_type_MC.push_back(silicon_type);
            INTT_MC[INTT_MC.size()-1].edep_MC.push_back(Edep);
        }
        else 
        {

            event_buffer = Event_ID;
            INTT_MC.push_back(INTT_event_basis);

            INTT_MC[INTT_MC.size()-1].eID_MC = Event_ID;
            INTT_MC[INTT_MC.size()-1].UpandDown_MC.push_back(UpandDown);
            INTT_MC[INTT_MC.size()-1].Xpos_MC.push_back(Xpos);
            INTT_MC[INTT_MC.size()-1].Ypos_MC.push_back(Ypos);
            INTT_MC[INTT_MC.size()-1].Zpos_MC.push_back(Zpos);
            INTT_MC[INTT_MC.size()-1].silicon_type_MC.push_back(silicon_type);
            INTT_MC[INTT_MC.size()-1].edep_MC.push_back(Edep);
        }
    }

    cout<<"size of INTT_MC : "<<INTT_MC.size()<<endl;


    vector<int> INTT_MC_eID_spinout; INTT_MC_eID_spinout.clear();
    for (int i = 0; i < N_converted_event /*INTT_MC.size()*/; i++)
    {
        INTT_MC_eID_spinout.push_back(INTT_MC[i].eID_MC);
    }
    int size_INTT_MC = N_converted_event /*INTT_MC.size()*/;
    int sort_INTT_MC_eID[size_INTT_MC];
    TMath::Sort(size_INTT_MC, &INTT_MC_eID_spinout[0], sort_INTT_MC_eID,false);
    cout<<"INTT_MC sort done !"<<endl;

    for (int i = 0; i < 2000; i++)
    {
        cout<<i<<" "<<INTT_MC[ sort_INTT_MC_eID[i] ].eID_MC<<" "<<sci_MC[ sort_sci_MC_eID[i] ].eID_MC<<endl;
    }
    
    

    vector<INTT_event_final_s> INTT_final; INTT_final.clear();
    INTT_event_final_s INTT_final_basis; // note : structure
    INTT_final_basis = INTT_final_initialize();

    int finding_match_initialization; // note : try to make the code quicker.
    int finding_match_end;// note : also, to make the code run faster.

    // note :  we start to do the match here, and convert the info. to the final output data formate
    for (int i = 0; i < N_converted_event /*sci_MC.size()*/; i++) // todo : here I change the maximum size of the file, not the sci_MC.size()
    {

        if (i%show_rate == 0) cout<<"======================================================================== " <<endl;
        if (i%show_rate == 0) cout<<"we are working on sci event (loop) : "<<i<<", eID : "<<sci_MC[ sort_sci_MC_eID [i] ].eID_MC<<endl;

        if (sci_MC[ sort_sci_MC_eID [i] ].edep_MC.size() == 2)
        {
            if ( (sci_MC[ sort_sci_MC_eID [i] ].sci_ID_MC[0] + sci_MC[ sort_sci_MC_eID [i] ].sci_ID_MC[1]) == 1)
            {
                if ( sci_MC[ sort_sci_MC_eID [i] ].edep_MC[0] > 77 && sci_MC[ sort_sci_MC_eID [i] ].edep_MC[0] < 600 )
                {
                    if ( sci_MC[ sort_sci_MC_eID [i] ].edep_MC[1] > 77 && sci_MC[ sort_sci_MC_eID [i] ].edep_MC[1] < 600 )
                    {

                        
                        if (i%show_rate == 0) cout<<"sci count : OK"<<endl;
                        if (i%show_rate == 0) cout<<"start searching matching "<<endl;

                        INTT_final.push_back(INTT_final_basis);//note : we add one empty sturcture in it.

                        INTT_final[INTT_final.size()-1].MC_eID = sci_MC[ sort_sci_MC_eID [i] ].eID_MC;

                        // note : camac_tdc
                        Frand_tdc1 = int (rand_tdc1->Gaus(148,12.5));
                        Frand_tdc2 = int (rand_tdc2->Gaus(139.7,0.87));
                        Frand_tdc3 = int (rand_tdc3->Gaus(128,12.3));
                        Frand_tdc6 = int (rand_tdc6->Uniform(520.,1080.));

                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc1);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc2);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc3);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(0);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(0);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc6);

                        
                        //note : camac_adc
                        INTT_final[INTT_final.size()-1].camac_adc_final = sci_MC[ sort_sci_MC_eID [i] ].edep_MC;

                        // note : the INTT_event
                        Frand_INTT_event = rand_INTT_event -> Uniform(0,1);
                        if (Frand_INTT_event<INTT_event_ratio) // note : the setting of INTT_event efficiency is here. Now is set to be 90%
                        {
                            INTT_final[INTT_final.size()-1].INTT_event_final = 1;
                        }
                        else
                        {
                            INTT_final[INTT_final.size()-1].INTT_event_final = 0;
                        }

                        
                        // note : the float i1 initialitor can make the code quicker, in principle.
                        if (i < 50000)
                        {
                            finding_match_initialization = 0;
                        }
                        else
                        {
                            finding_match_initialization = int(i * 0.4);
                        }

                        // todo : the search range is just roughly selected.
                        for (int i1 = finding_match_initialization; i1 < i+200000; i1++) // note : start to find the match, by checking the eID_MC
                        {
                            
                            if (i%show_rate == 0 && i1%500 == 0) cout<<"sci ID (loop) : "<<i<<" searching INTT (loop) : "<<i1<<endl;

                            if (sci_MC[ sort_sci_MC_eID [i] ].eID_MC == INTT_MC[ sort_INTT_MC_eID[i1] ].eID_MC) // note : once we found the match.
                            {
                                match_finder_check = 1;
                                
                                if (i%show_rate == 0) cout<<"the eID matched !, sci_ID : "<< sci_MC[ sort_sci_MC_eID [i] ].eID_MC << endl;
                                

                                // note : random generator for bco_full
                                // note : for bco, it is the lowest 7 bits of bco_full, so the bco_full and bco are not fully independent.
                                Frand_bcofull = int(rand_bcofull->Uniform(0.,65536.)); // note : 0 ~ 65535
                                Frand_bco = Frand_bcofull & UInt_t( 255 ); // note : take the lowest 7 bits from the BCO full

                                // cout<<"test INTT_MC size : "<<INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size()<<endl;

                                // note : we start to work on hits
                                for (int i2 = 0; i2 < INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size(); i2++)
                                {
                                    ev_DAC_convert = (INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC[i2]*100*1.6*Gain/3.6+offset-210.)/4.;

                                    if ( ev_DAC_convert <= set_DAC0 ) { continue; }
                                    if ( reduce_effi == true && rand_INTT_event -> Uniform(0,1) > target_l1_effi ){ continue; }

                                    // bco_full and bco        
                                    INTT_final[INTT_final.size()-1].bco_full_array_final.push_back(Frand_bcofull);
                                    INTT_final[INTT_final.size()-1].bco_array_final.push_back(Frand_bco);

                                    //ampl
                                    INTT_final[INTT_final.size()-1].ampl_array_final.push_back(0);

                                    //event 
                                    INTT_final[INTT_final.size()-1].event_array_final.push_back(-1);

                                    //fem_id
                                    INTT_final[INTT_final.size()-1].fem_id_array_final.push_back(0);

                                    //fpga_id  
                                    INTT_final[INTT_final.size()-1].fpga_id_array_final.push_back(0);

                                    //hit adc
                                    if (ev_DAC_convert<=set_DAC0) {continue;}
                                    else if (ev_DAC_convert>set_DAC0 && ev_DAC_convert<=set_DAC1){adc=0; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC1 && ev_DAC_convert<=set_DAC2){adc=1; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC2 && ev_DAC_convert<=set_DAC3){adc=2; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC3 && ev_DAC_convert<=set_DAC4){adc=3; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC4 && ev_DAC_convert<=set_DAC5){adc=4; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC5 && ev_DAC_convert<=set_DAC6){adc=5; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC6 && ev_DAC_convert<=set_DAC7){adc=6; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else{adc=7; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}

                                    //chip_id and chan_id
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].UpandDown_MC[i2] == 1) // upper
                                    {
                                        chan_id = 127-INTT_MC[ sort_INTT_MC_eID[i1] ].Ypos_MC[i2];
                                        chip_id = 13-INTT_MC[ sort_INTT_MC_eID[i1] ].Xpos_MC[i2];
                                        INTT_final[INTT_final.size()-1].chip_id_array_final.push_back(chip_id);
                                        INTT_final[INTT_final.size()-1].chan_id_array_final.push_back(chan_id);
                                    }
                                    else //down
                                    {
                                        chan_id = INTT_MC[ sort_INTT_MC_eID[i1] ].Ypos_MC[i2];
                                        chip_id = 26-INTT_MC[ sort_INTT_MC_eID[i1] ].Xpos_MC[i2];	
                                        INTT_final[INTT_final.size()-1].chip_id_array_final.push_back(chip_id);
                                        INTT_final[INTT_final.size()-1].chan_id_array_final.push_back(chan_id);
                                    }

                                    //module, (layer)
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 0) // title : this layer will not be used. 
                                    {
                                        module = module_id_l0;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 1)
                                    {
                                        module = module_id_l1;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 2)
                                    {
                                        module = module_id_l2;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 3)
                                    {
                                        module = module_id_l3;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }

                                } // note : end of event hit, i2
                                
                                // cout<<"test INTT_MC end loop : "<<INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size()<<endl;
                                //INTT_final[INTT_final.size()-1].clear(); // this is useful, to reduce the memory usage (probably)
                                
                                // todo : the erase method somehow is slow when dealing with the big vector (level of million)
                                // INTT_MC.erase(INTT_MC.begin()+i1);//we delete this event as it was already used. We try to make the macro run quickly.
                                break;
                            } // note : end of if event match
                        } //note : end of for loop, INTT_MC, i1

                        if ( add_noise_bool == true && INTT_final[INTT_final.size()-1].adc_array_final.size() != 0 )
                        {
                            INTT_event_buffer = INTT_final[INTT_final.size()-1];
                            INTT_final[INTT_final.size()-1] = Noise_hit_add (i,sci_MC[ sort_sci_MC_eID [i] ].eID_MC ,INTT_event_buffer, rand_noise, module_id_l1, module_id_l2, module_id_l3,hist_1D,hist_2D);
                        }

                        
                        // cout<<"test save begin "<<endl;
                        camac_adc = INTT_final[INTT_final.size()-1].camac_adc_final;
                        camac_tdc = INTT_final[INTT_final.size()-1].camac_tdc_final;
                        INTT_event = INTT_final[INTT_final.size()-1].INTT_event_final;
                        adc_array = INTT_final[INTT_final.size()-1].adc_array_final;
                        ampl_array = INTT_final[INTT_final.size()-1].ampl_array_final;
                        chip_id_array = INTT_final[INTT_final.size()-1].chip_id_array_final;
                        fpga_id_array = INTT_final[INTT_final.size()-1].fpga_id_array_final;
                        module_array = INTT_final[INTT_final.size()-1].module_array_final;
                        chan_id_array = INTT_final[INTT_final.size()-1].chan_id_array_final;
                        fem_id_array = INTT_final[INTT_final.size()-1].fem_id_array_final;
                        bco_array = INTT_final[INTT_final.size()-1].bco_array_final;
                        bco_full_array = INTT_final[INTT_final.size()-1].bco_full_array_final;
                        event_array = INTT_final[INTT_final.size()-1].event_array_final;

                        MC_eID = INTT_final[INTT_final.size()-1].MC_eID;
                        // cout<<"test save end 1 "<<endl;
                        tree_output3->Fill();
                        // cout<<"test save end 2 "<<endl;

                        if (match_finder_check == 0) 
                        {
                            // if (i%show_rate == 0) cout<<"event doesn't match !!!, sci_ID : "<<sci_MC[ sort_sci_MC_eID [i] ].eID_MC<<endl;
                            // todo : here the (i%show_rate == 0) was canceled
                            if (i%show_rate == 0) cout<<"event doesn't match !!!, sci_ID : "<<sci_MC[ sort_sci_MC_eID [i] ].eID_MC<<endl;
                        }


                    }
                }
            }
        }
        else if (sci_MC[ sort_sci_MC_eID [i] ].edep_MC.size() == 3)
        {
            if ( sci_MC[ sort_sci_MC_eID [i] ].edep_MC[0] > 77 && sci_MC[ sort_sci_MC_eID [i] ].edep_MC[0] < 600)
            {
                if ( sci_MC[ sort_sci_MC_eID [i] ].edep_MC[1] > 77 && sci_MC[ sort_sci_MC_eID [i] ].edep_MC[1] < 600 )
                {
                    if ( sci_MC[ sort_sci_MC_eID [i] ].edep_MC[2] > 77 && sci_MC[ sort_sci_MC_eID [i] ].edep_MC[2] < 600 )
                    {

                        
                        if (i%show_rate == 0) cout<<"sci count : OK"<<endl;
                        if (i%show_rate == 0) cout<<"start searching matching "<<endl;

                        INTT_final.push_back(INTT_final_basis);//note : we add one empty sturcture in it.

                        INTT_final[INTT_final.size()-1].MC_eID = sci_MC[ sort_sci_MC_eID [i] ].eID_MC;

                        // note : camac_tdc
                        Frand_tdc1 = int (rand_tdc1->Gaus(148,12.5));
                        Frand_tdc2 = int (rand_tdc2->Gaus(139.7,0.87));
                        Frand_tdc3 = int (rand_tdc3->Gaus(128,12.3));
                        Frand_tdc6 = int (rand_tdc6->Uniform(520.,1080.));

                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc1);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc2);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc3);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(0);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(0);
                        INTT_final[INTT_final.size()-1].camac_tdc_final.push_back(Frand_tdc6);

                        
                        //note : camac_adc
                        INTT_final[INTT_final.size()-1].camac_adc_final = sci_MC[ sort_sci_MC_eID [i] ].edep_MC;

                        // note : the INTT_event
                        Frand_INTT_event = rand_INTT_event -> Uniform(0,1);
                        if (Frand_INTT_event<INTT_event_ratio) // note : the setting of INTT_event efficiency is here. Now is set to be 90%
                        {
                            INTT_final[INTT_final.size()-1].INTT_event_final = 1;
                        }
                        else
                        {
                            INTT_final[INTT_final.size()-1].INTT_event_final = 0;
                        }

                        
                        // note : the float i1 initialitor can make the code quicker, in principle.
                        if (i < 50000)
                        {
                            finding_match_initialization = 0;
                        }
                        else
                        {
                            finding_match_initialization = int(i*0.4);
                        }

                        // todo : the search range is just roughly selected.
                        for (int i1 = finding_match_initialization; i1 < i+200000; i1++) // note : start to find the match, by checking the eID_MC
                        {
                            
                            if (i%show_rate == 0 && i1%500 == 0) cout<<"sci ID (loop) : "<<i<<" searching INTT (loop) : "<<i1<<endl;

                            if (sci_MC[ sort_sci_MC_eID [i] ].eID_MC == INTT_MC[ sort_INTT_MC_eID[i1] ].eID_MC) // note : once we found the match.
                            {
                                match_finder_check = 1;
                                
                                if (i%show_rate == 0) cout<<"the eID matched !, sci_ID : "<< sci_MC[ sort_sci_MC_eID [i] ].eID_MC << endl;
                                

                                // note : random generator for bco_full
                                // note : for bco, it is the lowest 7 bits of bco_full, so the bco_full and bco are not fully independent.
                                Frand_bcofull = int(rand_bcofull->Uniform(0.,65536.)); // note : 0 ~ 65535
                                Frand_bco = Frand_bcofull & UInt_t( 255 ); // note : take the lowest 7 bits from the BCO full

                                // cout<<"test INTT_MC size : "<<INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size()<<endl;

                                // note : we start to work on hits
                                for (int i2 = 0; i2 < INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size(); i2++)
                                {
                                    ev_DAC_convert = (INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC[i2]*100*1.6*Gain/3.6+offset-210.)/4.;

                                    if ( ev_DAC_convert <= set_DAC0 ) { continue; }
                                    if ( reduce_effi == true && rand_INTT_event -> Uniform(0,1) > target_l1_effi ){ continue; }

                                    // bco_full and bco        
                                    INTT_final[INTT_final.size()-1].bco_full_array_final.push_back(Frand_bcofull);
                                    INTT_final[INTT_final.size()-1].bco_array_final.push_back(Frand_bco);

                                    //ampl
                                    INTT_final[INTT_final.size()-1].ampl_array_final.push_back(0);

                                    //event 
                                    INTT_final[INTT_final.size()-1].event_array_final.push_back(-1);

                                    //fem_id
                                    INTT_final[INTT_final.size()-1].fem_id_array_final.push_back(0);

                                    //fpga_id  
                                    INTT_final[INTT_final.size()-1].fpga_id_array_final.push_back(0);

                                    //hit adc
                                    if (ev_DAC_convert<=set_DAC0) {continue;}
                                    else if (ev_DAC_convert>set_DAC0 && ev_DAC_convert<=set_DAC1){adc=0; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC1 && ev_DAC_convert<=set_DAC2){adc=1; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC2 && ev_DAC_convert<=set_DAC3){adc=2; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC3 && ev_DAC_convert<=set_DAC4){adc=3; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC4 && ev_DAC_convert<=set_DAC5){adc=4; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC5 && ev_DAC_convert<=set_DAC6){adc=5; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else if (ev_DAC_convert>set_DAC6 && ev_DAC_convert<=set_DAC7){adc=6; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}
                                    else{adc=7; INTT_final[INTT_final.size()-1].adc_array_final.push_back(adc);}

                                    //chip_id and chan_id
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].UpandDown_MC[i2] == 1) // upper
                                    {
                                        chan_id = 127-INTT_MC[ sort_INTT_MC_eID[i1] ].Ypos_MC[i2];
                                        chip_id = 13-INTT_MC[ sort_INTT_MC_eID[i1] ].Xpos_MC[i2];
                                        INTT_final[INTT_final.size()-1].chip_id_array_final.push_back(chip_id);
                                        INTT_final[INTT_final.size()-1].chan_id_array_final.push_back(chan_id);
                                    }
                                    else //down
                                    {
                                        chan_id = INTT_MC[ sort_INTT_MC_eID[i1] ].Ypos_MC[i2];
                                        chip_id = 26-INTT_MC[ sort_INTT_MC_eID[i1] ].Xpos_MC[i2];	
                                        INTT_final[INTT_final.size()-1].chip_id_array_final.push_back(chip_id);
                                        INTT_final[INTT_final.size()-1].chan_id_array_final.push_back(chan_id);
                                    }

                                    //module, (layer)
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 0) // title : this layer will not be used. 
                                    {
                                        module = module_id_l0;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 1)
                                    {
                                        module = module_id_l1;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 2)
                                    {
                                        module = module_id_l2;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }
                                    if (INTT_MC[ sort_INTT_MC_eID[i1] ].Zpos_MC[i2] == 3)
                                    {
                                        module = module_id_l3;
                                        INTT_final[INTT_final.size()-1].module_array_final.push_back(module);
                                    }

                                } // note : end of event hit, i2
                                
                                // cout<<"test INTT_MC end loop : "<<INTT_MC[ sort_INTT_MC_eID[i1] ].edep_MC.size()<<endl;
                                //INTT_final[INTT_final.size()-1].clear(); // this is useful, to reduce the memory usage (probably)
                                
                                // todo : the erase method somehow is slow when dealing with the big vector (level of million)
                                // INTT_MC.erase(INTT_MC.begin()+i1);//we delete this event as it was already used. We try to make the macro run quickly.
                                break;
                            } // note : end of if event match
                        } //note : end of for loop, INTT_MC, i1

                        if ( add_noise_bool == true && INTT_final[INTT_final.size()-1].adc_array_final.size() != 0 )
                        {
                            INTT_event_buffer = INTT_final[INTT_final.size()-1];
                            INTT_final[INTT_final.size()-1] = Noise_hit_add (i,sci_MC[ sort_sci_MC_eID [i] ].eID_MC ,INTT_event_buffer, rand_noise, module_id_l1, module_id_l2, module_id_l3,hist_1D,hist_2D);
                        }

                        
                        // cout<<"test save begin "<<endl;
                        camac_adc = INTT_final[INTT_final.size()-1].camac_adc_final;
                        camac_tdc = INTT_final[INTT_final.size()-1].camac_tdc_final;
                        INTT_event = INTT_final[INTT_final.size()-1].INTT_event_final;
                        adc_array = INTT_final[INTT_final.size()-1].adc_array_final;
                        ampl_array = INTT_final[INTT_final.size()-1].ampl_array_final;
                        chip_id_array = INTT_final[INTT_final.size()-1].chip_id_array_final;
                        fpga_id_array = INTT_final[INTT_final.size()-1].fpga_id_array_final;
                        module_array = INTT_final[INTT_final.size()-1].module_array_final;
                        chan_id_array = INTT_final[INTT_final.size()-1].chan_id_array_final;
                        fem_id_array = INTT_final[INTT_final.size()-1].fem_id_array_final;
                        bco_array = INTT_final[INTT_final.size()-1].bco_array_final;
                        bco_full_array = INTT_final[INTT_final.size()-1].bco_full_array_final;
                        event_array = INTT_final[INTT_final.size()-1].event_array_final;

                        MC_eID = INTT_final[INTT_final.size()-1].MC_eID;
                        // cout<<"test save end 1 "<<endl;
                        tree_output3->Fill();
                        // cout<<"test save end 2 "<<endl;

                        if (match_finder_check == 0) 
                        {
                            // if (i%show_rate == 0) cout<<"event doesn't match !!!, sci_ID : "<<sci_MC[ sort_sci_MC_eID [i] ].eID_MC<<endl;
                            // todo : here the (i%show_rate == 0) was canceled
                            if (i%show_rate == 0) cout<<"event doesn't match !!!, sci_ID : "<<sci_MC[ sort_sci_MC_eID [i] ].eID_MC<<endl;
                        }


                    }
                }
            }
        }

        match_finder_check = 0;

    } // note : end of for loop, sci_MC, i

    if (add_noise_bool == true)
    {
        TCanvas * c1 = new TCanvas("c1","c1",2400,600);
        c1 -> Divide(4,1);
        
        // note : noise adc dist..
        c1 -> cd(1);
        hist_1D[0] -> Draw("hist");

        // note : # of noise hit per event.    
        c1 -> cd(2);
        c1 -> SetLogy(1);
        hist_1D[1] -> Draw("hist");
        
        // note : noise hit, hit map
        c1 -> cd(3);
        hist_2D[0] -> Draw("colz0");
        
        // note : noise hit, layer dist..
        c1 -> cd(4);
        hist_1D[6] -> Draw("hist");
        
        c1 -> Print(Form("%s/noise_hit/%s_noise_only_monitor.pdf",folder_name.Data(),file_name.Data()));
    }



    // cout<<" too_small_adc_counting : "<<too_small_adc_counting<<endl;
	file_output->cd();
	//cout<<"testtest"<<endl;
	// tree_output1->Write("", TObject::kOverwrite);
	// tree_output2->Write("", TObject::kOverwrite);
	tree_output3->Write("", TObject::kOverwrite);
	cout<<"conversion done "<<endl;
	file_output->Close();



    // int INTT_temp_hit_counting = 0;

    // for (int i=0; i<INTT_MC.size(); i++)
    // {

    //     INTT_temp_hit_counting += INTT_MC[i].edep_MC.size();

    //     cout<<"============================================================"<<endl;
    //     for (int i1=0; i1<INTT_MC[i].edep_MC.size(); i1++)
    //     {
    //         cout<<INTT_MC[i].eID_MC<<" "<<INTT_MC[i].UpandDown_MC[i1]<<" "<<INTT_MC[i].Xpos_MC[i1]<<" "<<INTT_MC[i].Ypos_MC[i1]<<" "<<INTT_MC[i].Zpos_MC[i1]<<" "<<INTT_MC[i].silicon_type_MC[i1]<<" "<<INTT_MC[i].edep_MC[i1]<<endl;
    //     }
    // }

    // cout<<"INTT total hit : "<<INTT_temp_hit_counting<<endl;

    // int temp_sci_counting = 0;

    // for (int i=0; i<sci_MC.size(); i++)
    // {
    //     if (sci_MC[i].edep_MC.size() >2)
    //     {
    //         temp_sci_counting+=1;
    //     }
    // }

    // cout<<"size of sci_MC passed : "<<temp_sci_counting<<endl;

}

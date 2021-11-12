//----------------------------------------------------------------------------------------------------------------
// Note by Cheng-Wei Shih @ NCU 2021/11/11
//
// 1.The purpose of the macro : to filter the "double saving event" in one event, in principle it should not happen. 
//   You can check the slide in the link : https://indico.bnl.gov/event/13810/#1-the-progress-for-testbeam-20 
//
// 2. The definition of double saving 
//   A : filter for tree_both : at same event (within same camac_tdc, iow. in same vector), there can only be one hit 
//       with same module, same chip & same channel.  
//   B : filter for tree : at one bco and bco full, there can only be one hit with same module, same chip & same channel.    
//
// 3.The idea is to create a new root file, so the original root file will not be affected.  
//
// 4.Base on the current macro, the filter only applies on tree_both in order to speed up the running time. 
//   Of course the "tree" also has the double saving problem, but for the analysis we focus on "tree_both"
//
// 5. based on current macro : the last double-saving event will be saved. The example is shown below.
//
//    module  chip  channel adc 
//      5       7      24    7
//      5       7      24    2
//      5       7      24    5  -> this hit will be saved.
// 
// 6. The idea of filtering the double saving event is to use a 3 dimensional array, for example : "int vec_adc_filter[4][26][128];"
//    the first  bracket : layer
//    the second bracket : chip
//    the thirs  bracket : channel
//    
//    the information of the first double-saving event will be overweitten by the second double-saving event;
//
// 7. the parameter you may want to tune : 
//    A : file_name @ line 41  -> the root file you would like filter.  !!! Caution : remember to remove ".root"
//    B : path_in   @ line 42  -> the direction of file
//    C : the output file name after filtering @ line 181
//    D : the module ID for each layer @ line 131
//----------------------------------------------------------------------------------------------------------------

void filter_N_v2 ()
{
	//===================================option===========================================
	TString file_name = "electron_100W_1_converttest";
	TString path_in = "/data4/chengwei/Geant4/INTT_simulation/G4/for_CW/data/";
	//===================================option===========================================
	
	// use TFile, TTree to open tree_camac, tree and tree_both from the orignal file
	TFile *file_in = new TFile (path_in+file_name+".root", "read");
	TTree *tree_camac_in = (TTree*)file_in -> Get ("tree_camac");
	TTree *tree_in = (TTree*)file_in -> Get ("tree");
	TTree *tree_both_in = (TTree*)file_in -> Get ("tree_both");

	//the variables for reading the data by using SetBranchAddress, tree
	int adc_in, ampl_in, chip_id_in, fpga_id_in, module_in, chan_id_in, fem_id_in, bco_in, bco_full_in, event_in;

	//the branch in "tree"
	tree_in -> SetBranchAddress ("adc", &adc_in);
	tree_in -> SetBranchAddress ("ampl", &ampl_in);
	tree_in -> SetBranchAddress ("chip_id", &chip_id_in);
	tree_in -> SetBranchAddress ("fpga_id", &fpga_id_in);
	tree_in -> SetBranchAddress ("module", &module_in);
	tree_in -> SetBranchAddress ("chan_id", &chan_id_in);
	tree_in -> SetBranchAddress ("fem_id", &fem_id_in);
	tree_in -> SetBranchAddress ("bco", &bco_in);
	tree_in -> SetBranchAddress ("bco_full", &bco_full_in);
	tree_in -> SetBranchAddress ("event", &event_in);

	// # of the hits in "tree"
	long nEvent = tree_in -> GetEntriesFast();

	Printf("tree,	# of event in %s.root : [%li]\n",file_name.Data(),nEvent);
	

	// the variables for reading the data by using SetBranchAddress, tree_camac
	vector<int> *vec_camacadc_ca = 0;
	vector<int> *vec_camactdc_ca = 0;
	bool INTT_event_ca;
	tree_camac_in -> SetBranchAddress ("camac_adc", &vec_camacadc_ca);
	tree_camac_in -> SetBranchAddress ("camac_tdc", &vec_camactdc_ca);
	tree_camac_in -> SetBranchAddress ("INTT_event", &INTT_event_ca);

	// # of the event in "tree_camac"
	long nEvent_ca = tree_camac_in -> GetEntriesFast();

	Printf("tree_camac,	# of event in %s.root : [%li]\n",file_name.Data(),nEvent_ca);


	// the variables for reading the data by using SetBranchAddress, tree_both
	vector<int> *vec_camacadc_bo = 0;
	vector<int> *vec_camactdc_bo = 0;
	bool INTT_event_bo;

	vector<int> * vec_adc=0;
	vector<int> * vec_ampl=0;
	vector<int> * vec_chip_id=0;
	vector<int> * vec_fpga_id=0;
	vector<int> * vec_module=0;
	vector<int> * vec_chan_id=0;
	vector<int> * vec_fem_id=0;
	vector<int> * vec_bco=0;
	vector<int> * vec_bco_full=0;
	vector<int> * vec_event=0;
	

	tree_both_in -> SetBranchAddress ("camac_adc", &vec_camacadc_bo);
	tree_both_in -> SetBranchAddress ("camac_tdc", &vec_camactdc_bo);
	tree_both_in -> SetBranchAddress ("INTT_event", &INTT_event_bo);

	tree_both_in -> SetBranchAddress ("adc", &vec_adc);
	tree_both_in -> SetBranchAddress ("ampl", &vec_ampl);
	tree_both_in -> SetBranchAddress ("chip_id", &vec_chip_id);
	tree_both_in -> SetBranchAddress ("fpga_id", &vec_fpga_id);
	tree_both_in -> SetBranchAddress ("module", &vec_module);
	tree_both_in -> SetBranchAddress ("chan_id", &vec_chan_id);
	tree_both_in -> SetBranchAddress ("fem_id", &vec_fem_id);
	tree_both_in -> SetBranchAddress ("bco", &vec_bco);
	tree_both_in -> SetBranchAddress ("bco_full", &vec_bco_full);
	tree_both_in -> SetBranchAddress ("event", &vec_event);

	// # of the event in "tree_both"
	long nEvent_both = tree_both_in -> GetEntriesFast();

	Printf("tree_both,	# of event in %s.root : [%li]\n",file_name.Data(),nEvent_both);

	



	

	//======================preparation of recreate================================
	// the module ID of each layer
	int module_l0 =6;
	int module_l1 =5;
	int module_l2 =8;
	int module_l3 =2; 

	int adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event;

	vector<int> * camac_adc_out; //camac_adc_out.clear();
	vector<int> * camac_tdc_out; //camac_tdc_out.clear();
	bool INTT_event_out;

	vector<int> adc_array;      adc_array.clear();
	vector<int> ampl_array;     ampl_array.clear();
	vector<int> chip_id_array;  chip_id_array.clear();
	vector<int> fpga_id_array;  fpga_id_array.clear();
	vector<int> module_array;   module_array.clear();
	vector<int> chan_id_array;  chan_id_array.clear();
	vector<int> fem_id_array;   fem_id_array.clear();
	vector<int>	bco_array;      bco_array.clear();
	vector<int> bco_full_array; bco_full_array.clear();
	vector<int> event_array;    event_array.clear();
	int vec_element;

	//the 3 dimensional array to filter the double-saving event
	int vec_adc_filter[4][26][128];
	int vec_ampl_filter[4][26][128];
	int vec_chip_id_filter[4][26][128];
	int vec_fpga_id_filter[4][26][128];
	int vec_module_filter[4][26][128];
	int vec_chan_id_filter[4][26][128];
	int vec_fem_id_filter[4][26][128];
	int vec_bco_filter[4][26][128];
	int vec_bco_full_filter[4][26][128];
	int vec_event_filter[4][26][128];

	//the way to reset these 3 dimensional arraies, very quick.
	memset (vec_adc_filter     ,-1,sizeof(vec_adc_filter));
	memset (vec_ampl_filter    ,-1,sizeof(vec_ampl_filter));
	memset (vec_chip_id_filter ,-1,sizeof(vec_chip_id_filter));
	memset (vec_fpga_id_filter ,-1,sizeof(vec_fpga_id_filter));
	memset (vec_module_filter  ,-1,sizeof(vec_module_filter));
	memset (vec_chan_id_filter ,-1,sizeof(vec_chan_id_filter));
	memset (vec_fem_id_filter  ,-1,sizeof(vec_fem_id_filter));
	memset (vec_bco_filter     ,-1,sizeof(vec_bco_filter));
	memset (vec_bco_full_filter,-1,sizeof(vec_bco_full_filter));
	memset (vec_event_filter   ,-1,sizeof(vec_event_filter));

	//the file name of the output root file
	//based on current macro, we don't need to define the tree and tree_camac in advance. 
	//these 2 trees will be cloned with the orignal tree
	TFile *file_output = new TFile(path_in+file_name+"_filter_test.root", "RECREATE");
	// TTree * tree_output1 = new TTree("tree", "tree");
	// TTree * tree_output2;
	TTree *tree_output3 = new TTree("tree_both", "tree_both");

	

	// tree_output2->Branch("camac_adc",&camac_adc_out);
	// tree_output2->Branch("camac_tdc",&camac_tdc_out);
	// tree_output2->Branch("INTT_event",&INTT_event_out);

	//create the branches in the tree_both + nele (# of element in each vector)
	tree_output3->Branch("camac_adc" ,&camac_adc_out);
	tree_output3->Branch("camac_tdc" ,&camac_tdc_out);
	tree_output3->Branch("INTT_event",&INTT_event_out);
	tree_output3->Branch("adc"       ,&adc_array);
	tree_output3->Branch("ampl"      ,&ampl_array);
	tree_output3->Branch("chip_id"   ,&chip_id_array);
	tree_output3->Branch("fpga_id"   ,&fpga_id_array);
	tree_output3->Branch("module"    ,&module_array);
	tree_output3->Branch("chan_id"   ,&chan_id_array);
	tree_output3->Branch("fem_id"    ,&fem_id_array);
	tree_output3->Branch("bco"       ,&bco_array);
	tree_output3->Branch("bco_full"  ,&bco_full_array);
	tree_output3->Branch("event"     ,&event_array);
	tree_output3->Branch("nele"      ,&vec_element);
	//======================preparation of recreate================================

	//=====start=================the part of tree filter, not used now================================
	vector <int> event_chan_temp[4][26][128]; 
	vector <int> event_module_temp[4][26][128];
	vector <int> event_chip_temp[4][26][128];
	vector <int> event_ampl_temp[4][26][128];
	vector <int> event_fpga_temp[4][26][128];
	vector <int> event_bco_temp[4][26][128];
	vector <int> event_bco_full_temp[4][26][128];
	vector <int> event_adc_temp[4][26][128];
	vector <int> event_fem_temp[4][26][128];
	vector <int> event_event_temp[4][26][128];

	for (int j=0; j< 4; j++)
	{
		for (int i=0; i< 26; i++)
		{
			for (int i1=0; i1<128; i1++)
			{
				event_chan_temp     [j][i][i1].clear(); 
				event_module_temp   [j][i][i1].clear();
				event_chip_temp     [j][i][i1].clear();
				event_ampl_temp     [j][i][i1].clear();
				event_fpga_temp     [j][i][i1].clear();
				event_bco_temp      [j][i][i1].clear();
				event_bco_full_temp [j][i][i1].clear();
				event_adc_temp      [j][i][i1].clear();
				event_fem_temp      [j][i][i1].clear();
				event_event_temp    [j][i][i1].clear();
			}
		}
	}

	tree_in->GetEntry(0);
	int tree_bco_buffer = bco_in;
	int tree_bco_full_buffer = bco_full_in;

	//====end==================the part of tree filter, not used now================================

	// if (opt_tree_filter == true)
	// {
	// 	tree_output1->Branch("adc"     , &adc);
	// 	tree_output1->Branch("ampl"    , &ampl); 
	// 	tree_output1->Branch("chip_id" , &chip_id);
	// 	tree_output1->Branch("fpga_id" , &fpga_id); 
	// 	tree_output1->Branch("module"  , &module);
	// 	tree_output1->Branch("chan_id" , &chan_id);
	// 	tree_output1->Branch("fem_id"  , &fem_id);
	// 	tree_output1->Branch("bco"     , &bco);
	// 	tree_output1->Branch("bco_full", &bco_full);
	// 	tree_output1->Branch("event"   , &event);

	// 	for (int i=0; i< nEvent; i++)//tree filter
	// 	{
	// 		if (i%3000==0){cout<<"we are working on tree filter : "<<i<<endl;}
	// 		tree_in->GetEntry(i);
			
	// 		if (bco_in == tree_bco_buffer && bco_full_in == tree_bco_full_buffer)
	// 		{
	// 			if      (module_in == module_l0)
	// 			{
	// 				event_module_temp  [0][chip_id_in-1][chan_id_in].push_back(module_in);
	// 				event_chip_temp    [0][chip_id_in-1][chan_id_in].push_back(chip_id_in);
	// 				event_chan_temp    [0][chip_id_in-1][chan_id_in].push_back(chan_id_in);
	// 				event_ampl_temp    [0][chip_id_in-1][chan_id_in].push_back(ampl_in);
	// 				event_event_temp   [0][chip_id_in-1][chan_id_in].push_back(event_in);
	// 				event_bco_temp     [0][chip_id_in-1][chan_id_in].push_back(bco_in);
	// 				event_bco_full_temp[0][chip_id_in-1][chan_id_in].push_back(bco_full_in);
	// 				event_fem_temp     [0][chip_id_in-1][chan_id_in].push_back(fem_id_in);
	// 				event_fpga_temp    [0][chip_id_in-1][chan_id_in].push_back(fpga_id_in);
	// 				event_adc_temp     [0][chip_id_in-1][chan_id_in].push_back(adc_in);
	// 			}
	// 			else if (module_in == module_l1)
	// 			{
	// 				event_module_temp  [1][chip_id_in-1][chan_id_in].push_back(module_in);
	// 				event_chip_temp    [1][chip_id_in-1][chan_id_in].push_back(chip_id_in);
	// 				event_chan_temp    [1][chip_id_in-1][chan_id_in].push_back(chan_id_in);
	// 				event_ampl_temp    [1][chip_id_in-1][chan_id_in].push_back(ampl_in);
	// 				event_event_temp   [1][chip_id_in-1][chan_id_in].push_back(event_in);
	// 				event_bco_temp     [1][chip_id_in-1][chan_id_in].push_back(bco_in);
	// 				event_bco_full_temp[1][chip_id_in-1][chan_id_in].push_back(bco_full_in);
	// 				event_fem_temp     [1][chip_id_in-1][chan_id_in].push_back(fem_id_in);
	// 				event_fpga_temp    [1][chip_id_in-1][chan_id_in].push_back(fpga_id_in);
	// 				event_adc_temp     [1][chip_id_in-1][chan_id_in].push_back(adc_in);
	// 			}
	// 			else if (module_in == module_l2)
	// 			{
	// 				event_module_temp  [2][chip_id_in-1][chan_id_in].push_back(module_in);
	// 				event_chip_temp    [2][chip_id_in-1][chan_id_in].push_back(chip_id_in);
	// 				event_chan_temp    [2][chip_id_in-1][chan_id_in].push_back(chan_id_in);
	// 				event_ampl_temp    [2][chip_id_in-1][chan_id_in].push_back(ampl_in);
	// 				event_event_temp   [2][chip_id_in-1][chan_id_in].push_back(event_in);
	// 				event_bco_temp     [2][chip_id_in-1][chan_id_in].push_back(bco_in);
	// 				event_bco_full_temp[2][chip_id_in-1][chan_id_in].push_back(bco_full_in);
	// 				event_fem_temp     [2][chip_id_in-1][chan_id_in].push_back(fem_id_in);
	// 				event_fpga_temp    [2][chip_id_in-1][chan_id_in].push_back(fpga_id_in);
	// 				event_adc_temp     [2][chip_id_in-1][chan_id_in].push_back(adc_in);
	// 			}
	// 			else if (module_in == module_l3)
	// 			{
	// 				event_module_temp  [3][chip_id_in-1][chan_id_in].push_back(module_in);
	// 				event_chip_temp    [3][chip_id_in-1][chan_id_in].push_back(chip_id_in);
	// 				event_chan_temp    [3][chip_id_in-1][chan_id_in].push_back(chan_id_in);
	// 				event_ampl_temp    [3][chip_id_in-1][chan_id_in].push_back(ampl_in);
	// 				event_event_temp   [3][chip_id_in-1][chan_id_in].push_back(event_in);
	// 				event_bco_temp     [3][chip_id_in-1][chan_id_in].push_back(bco_in);
	// 				event_bco_full_temp[3][chip_id_in-1][chan_id_in].push_back(bco_full_in);
	// 				event_fem_temp     [3][chip_id_in-1][chan_id_in].push_back(fem_id_in);
	// 				event_fpga_temp    [3][chip_id_in-1][chan_id_in].push_back(fpga_id_in);
	// 				event_adc_temp     [3][chip_id_in-1][chan_id_in].push_back(adc_in);
	// 			}
				

	// 			if (i == nEvent-1)//final event fill
	// 			{
	// 				for (int j=0; j< 4; j++)
	// 				{
	// 					for (int i2=0; i2< 26; i2++)
	// 					{
	// 						for (int i1=0; i1<128; i1++)
	// 						{
	// 							if (event_adc_temp[j][i2][i1].size()>0)
	// 							{
	// 								adc       = event_adc_temp     [j][i2][i1][0];
	// 								ampl      = event_ampl_temp    [j][i2][i1][0];
	// 								chip_id   = event_chip_temp    [j][i2][i1][0];
	// 								fpga_id   = event_fpga_temp    [j][i2][i1][0];
	// 								module    = event_module_temp  [j][i2][i1][0];
	// 								chan_id   = event_chan_temp    [j][i2][i1][0];
	// 								fem_id    = event_fem_temp     [j][i2][i1][0];
	// 								bco       = event_bco_temp     [j][i2][i1][0];
	// 								bco_full  = event_bco_full_temp[j][i2][i1][0];
	// 								event     = event_event_temp   [j][i2][i1][0];
	// 								tree_output1->Fill();
	// 							}
	// 							event_chan_temp     [j][i2][i1].clear(); 
	// 							event_module_temp   [j][i2][i1].clear();
	// 							event_chip_temp     [j][i2][i1].clear();
	// 							event_ampl_temp     [j][i2][i1].clear();
	// 							event_fpga_temp     [j][i2][i1].clear();
	// 							event_bco_temp      [j][i2][i1].clear();
	// 							event_bco_full_temp [j][i2][i1].clear();
	// 							event_adc_temp      [j][i2][i1].clear();
	// 							event_fem_temp      [j][i2][i1].clear();
	// 							event_event_temp    [j][i2][i1].clear();

	// 						}
	// 					}
	// 				}
	// 			}

	// 		}
	// 		else 
	// 		{
	// 			tree_bco_buffer=bco_in;
	// 			tree_bco_full_buffer = bco_full_in;
	// 			for (int j=0; j< 4; j++)
	// 			{
	// 				for (int i2=0; i2< 26; i2++)
	// 				{
	// 					for (int i1=0; i1<128; i1++)
	// 					{
	// 						if (event_adc_temp[j][i2][i1].size()>0)
	// 						{
	// 							adc       = event_adc_temp     [j][i2][i1][0];
	// 							ampl      = event_ampl_temp    [j][i2][i1][0];
	// 							chip_id   = event_chip_temp    [j][i2][i1][0];
	// 							fpga_id   = event_fpga_temp    [j][i2][i1][0];
	// 							module    = event_module_temp  [j][i2][i1][0];
	// 							chan_id   = event_chan_temp    [j][i2][i1][0];
	// 							fem_id    = event_fem_temp     [j][i2][i1][0];
	// 							bco       = event_bco_temp     [j][i2][i1][0];
	// 							bco_full  = event_bco_full_temp[j][i2][i1][0];
	// 							event     = event_event_temp   [j][i2][i1][0];
	// 							tree_output1->Fill();
	// 						}
	// 						event_chan_temp     [j][i2][i1].clear(); 
	// 						event_module_temp   [j][i2][i1].clear();
	// 						event_chip_temp     [j][i2][i1].clear();
	// 						event_ampl_temp     [j][i2][i1].clear();
	// 						event_fpga_temp     [j][i2][i1].clear();
	// 						event_bco_temp      [j][i2][i1].clear();
	// 						event_bco_full_temp [j][i2][i1].clear();
	// 						event_adc_temp      [j][i2][i1].clear();
	// 						event_fem_temp      [j][i2][i1].clear();
	// 						event_event_temp    [j][i2][i1].clear();

	// 					}
	// 				}
	// 			}

	// 			i-=1;

	// 		}

			

	// 	}
	// }
	// else 
	// {
	// 	TTree * tree_output1 = tree_in->CloneTree();
	// }


	// these 2 lines clone the tree and tree_camac from the original root file
	auto tree_output1 = tree_in->CloneTree();
	auto tree_output2 = tree_camac_in->CloneTree();

	
	// for (int i=0; i< nEvent_ca; i++)//tree_camac no filter
	// {
	// 	if (i%3000==0){cout<<"we are working on tree_camac filter : "<<i<<endl;}
	// 	tree_camac_in->GetEntry(i);
	// 	camac_adc_out  = vec_camacadc_ca;
	// 	camac_tdc_out  = vec_camactdc_ca;
	// 	INTT_event_out = INTT_event_ca;
	// 	tree_output2->Fill();
	// }

	// here we start to filter the tree_both
	for (int i=0; i<nEvent_both; i++) // # of total events
	{
		if (i%3000==0){cout<<"we are working on tree_both filter : "<<i<<endl;}

		tree_both_in -> GetEntry (i);
		unsigned int lenvec = vec_adc->size(); // the # of element in each event

		// here we clone the information from camac_tdc, camac_adc, INTT_evnet of tree_both (they don't need to be checked)
		camac_adc_out = vec_camacadc_bo;
		camac_tdc_out = vec_camactdc_bo;
		INTT_event_out = INTT_event_bo;

		//here we start to filter the double saving event, we check it event by event
		//the idea of filtering double-saving event is mentioned in the note in the begining of the code
		//central thought : the size of array is fixed, the new event with same module, chipID, chanID will replace the previous one.
		for (int j=0; j<lenvec; j++)
		{
			//the first selection is module ID
			if (vec_module->at(j) == module_l0)
			{
				//                layer     chip                channel
				vec_adc_filter     [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_adc->at(j));
				vec_ampl_filter    [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_ampl->at(j));
				vec_chip_id_filter [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_chip_id->at(j));
				vec_fpga_id_filter [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_fpga_id->at(j));
				vec_module_filter  [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_module->at(j));
				vec_chan_id_filter [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_chan_id->at(j));
				vec_fem_id_filter  [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_fem_id->at(j));
				vec_bco_filter     [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_bco->at(j));
				vec_bco_full_filter[0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_bco_full->at(j));
				vec_event_filter   [0][vec_chip_id->at(j)-1][vec_chan_id->at(j)] =  (vec_event->at(j));	
			}
			else if (vec_module->at(j) == module_l1)
			{
				vec_adc_filter     [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_adc->at(j));
				vec_ampl_filter    [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_ampl->at(j));
				vec_chip_id_filter [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chip_id->at(j));
				vec_fpga_id_filter [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fpga_id->at(j));
				vec_module_filter  [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_module->at(j));
				vec_chan_id_filter [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chan_id->at(j));
				vec_fem_id_filter  [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fem_id->at(j));
				vec_bco_filter     [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco->at(j));
				vec_bco_full_filter[1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco_full->at(j));
				vec_event_filter   [1][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_event->at(j));	
			}

			else if (vec_module->at(j) == module_l2)
			{
				vec_adc_filter     [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_adc->at(j));
				vec_ampl_filter    [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_ampl->at(j));
				vec_chip_id_filter [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chip_id->at(j));
				vec_fpga_id_filter [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fpga_id->at(j));
				vec_module_filter  [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_module->at(j));
				vec_chan_id_filter [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chan_id->at(j));
				vec_fem_id_filter  [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fem_id->at(j));
				vec_bco_filter     [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco->at(j));
				vec_bco_full_filter[2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco_full->at(j));
				vec_event_filter   [2][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_event->at(j));	
			}

			else if (vec_module->at(j) == module_l3)
			{
				vec_adc_filter     [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_adc->at(j));
				vec_ampl_filter    [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_ampl->at(j));
				vec_chip_id_filter [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chip_id->at(j));
				vec_fpga_id_filter [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fpga_id->at(j));
				vec_module_filter  [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_module->at(j));
				vec_chan_id_filter [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_chan_id->at(j));
				vec_fem_id_filter  [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_fem_id->at(j));
				vec_bco_filter     [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco->at(j));
				vec_bco_full_filter[3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_bco_full->at(j));
				vec_event_filter   [3][vec_chip_id->at(j)-1][vec_chan_id->at(j)] = (vec_event->at(j));	
			}

			
		}

		// By checking whether the value is -1 or not, we can know whether we save something there.
		// the original setting of each element is -1
		// if the element is not -1 that means we do write something at that element
		// then we save the real event in new vector
		for (int j=0; j<4; j++)
		{
			for (int i2=0; i2< 26; i2++)
			{
				for (int i1=0; i1<128; i1++)
				{
					// cout<<"!!!!!!test7!!!!!! "<<i2<<" "<<i1<<endl;
					if (vec_adc_filter[j][i2][i1] != -1)
					{
						adc_array.push_back(vec_adc_filter[j][i2][i1]);
						ampl_array.push_back(vec_ampl_filter[j][i2][i1]);
						chip_id_array.push_back(vec_chip_id_filter[j][i2][i1]);
						fpga_id_array.push_back(vec_fpga_id_filter[j][i2][i1]);
						module_array.push_back(vec_module_filter[j][i2][i1]);
						chan_id_array.push_back(vec_chan_id_filter[j][i2][i1]);
						fem_id_array.push_back(vec_fem_id_filter[j][i2][i1]);
						bco_array.push_back(vec_bco_filter[j][i2][i1]);
						bco_full_array.push_back(vec_bco_full_filter[j][i2][i1]);
						event_array.push_back(vec_event_filter[j][i2][i1]);
					}
				}
			}	
		}
		
		// to get the # of elements in each new event
		vec_element = adc_array.size();

		tree_output3->Fill();

		adc_array.clear();
		ampl_array.clear();
		chip_id_array.clear();
		fpga_id_array.clear();
		module_array.clear();
		chan_id_array.clear();
		fem_id_array.clear();
		bco_array.clear();
		bco_full_array.clear();
		event_array.clear();


		//the way to reset these 3 dimensional arraies, very quick.
		memset (vec_adc_filter     ,-1,sizeof(vec_adc_filter));
		memset (vec_ampl_filter    ,-1,sizeof(vec_ampl_filter));
		memset (vec_chip_id_filter ,-1,sizeof(vec_chip_id_filter));
		memset (vec_fpga_id_filter ,-1,sizeof(vec_fpga_id_filter));
		memset (vec_module_filter  ,-1,sizeof(vec_module_filter));
		memset (vec_chan_id_filter ,-1,sizeof(vec_chan_id_filter));
		memset (vec_fem_id_filter  ,-1,sizeof(vec_fem_id_filter));
		memset (vec_bco_filter     ,-1,sizeof(vec_bco_filter));
		memset (vec_bco_full_filter,-1,sizeof(vec_bco_full_filter));
		memset (vec_event_filter   ,-1,sizeof(vec_event_filter));



		// printf ("[+] the vector at [%d] contain:\n", i);

		// if (lenvec < 6)
		// {
		// 	lenvec = 6 ; 
		// }

		// for (int j=0; j<lenvec; j++)
		// {
		// 	// cout<<<vec_adc->at(j);
			
		// 	printf (" |-- module : [%d] chip : [%d] chan : [%d] adc : [%d] bco : [%d] bco_full : [%d] \n",vec_module->at(j),vec_chip_id->at(j),vec_chan_id->at(j),vec_adc->at(j),vec_bco->at(j),vec_bco_full->at(j));
		// }

	}

	file_output->cd();

	// tree_output1->Print();
	// tree_output2->Print();

	tree_output1->Write("", TObject::kOverwrite);
	tree_output2->Write("", TObject::kOverwrite);
	tree_output3->Write("", TObject::kOverwrite);
	file_output->Close();
	printf("filter done\n");
	file_in->Close();


}

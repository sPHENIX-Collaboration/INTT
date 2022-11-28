// note : output the cluster
// note : new update, output the adc information. 2022/11/05
// note : v1 : only applies the alignment correction at l1, single chip.
// note : only single chip (or you can call it slot) correction
// note : chip 8 for run 52.

#include "sigmaEff.h"
#include "double_gaus.h"
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

struct cluster_str {
    vector<vector<double>> cluster_hit_pos_vec;
	vector<vector<double>> cluster_adc_vec;
};

// this function calculates the clusters and their positions
// note : [4][13][cluster ID] = _ _ _ 0 _ _ _._ _ _
// note : first three digits : # of cluster
// note : second three digits : the position of this cluster
// vector<vector<vector<double>>> data_all_edep_weight (vector<int> data_all[4][26],vector<int>most_hit_info, double adc_convert[8],double alignment_array[4] )
cluster_str data_all_edep_weight (vector<int> data_all[4][26],int selected_chip, double adc_convert[8],double alignment_array[4], int chip_to_apply_align_correction )
{

	// cout<<"something1"<<endl;

	double INTT_strip_width = 0.078;
	double lower_section_initial = -9.945;
	double upper_section_initial = 0.039;
    
    // note : new added parameters
    double num_hit = 0;
    vector<double> chan_master_Nhit; chan_master_Nhit.clear();

	double edep_sum = 0; // note : for saving the adc edep
    vector<double> chan_master_edep_sum; chan_master_edep_sum.clear();

	vector<double> chan_master; chan_master.clear();

	vector<double> combine_posi; combine_posi.clear(); // note : to combine the hit and pos information.
	
	
	double chan_truck=0;
	double chan_truck_denominator=0;
	int standard_channel;
	int distance_meter=0;
	vector<vector<double>> final_output;
	final_output.clear();

	vector<vector<double>> final_output_adc;
	final_output_adc.clear();

	vector <int> data_all_new[4][256]; // layer and channel, the chip was selected already. 
	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<256; i6++)	
			data_all_new[i5][i6].clear();


	vector <int> data_all_new1[4][26]; //layer and chip
	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<26; i6++)	
			data_all_new1[i5][i6].clear();

	// cout<<"something2 "<<selected_chip-1<<endl;

	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<data_all[l1][selected_chip-1].size(); l2++)	
		{
			// cout<<"test : "<<l1<<" "<<(data_all[l1][selected_chip-1][l2]%1000)<<" "<<data_all[l1][selected_chip-1][l2]<<endl;
			data_all_new[l1][ (data_all[l1][selected_chip-1][l2]%1000) ].push_back( data_all[l1][selected_chip-1][l2] );
		}
	}

	// cout<<"something3"<<endl;

	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<256; l2++)
		{
			if (data_all_new[l1][l2].size()>0)
			{
				data_all_new1[l1][selected_chip-1].push_back(data_all_new[l1][l2][0]);
			}
		}
	}

	// cout<<"something4"<<endl;

	for (int l1=0; l1<4; l1++)
	{
		for (int l2=0; l2<data_all_new1[l1][selected_chip-1].size(); l2++)
		{
			standard_channel = data_all_new1[l1][selected_chip-1][l2]%1000;

			if ( (data_all_new1[l1][selected_chip-1][l2]%1000) <128  )
			{
				chan_truck += (lower_section_initial + INTT_strip_width * (data_all_new1[l1][selected_chip-1][l2]%1000)) * adc_convert[ int(data_all_new1[l1][selected_chip-1][l2]/1000) ];
			}	
			else
			{
				chan_truck += ( upper_section_initial + INTT_strip_width * ( (data_all_new1[l1][selected_chip-1][l2]%1000) -128 ) ) * adc_convert[ int(data_all_new1[l1][selected_chip-1][l2]/1000) ];
			}
			chan_truck_denominator+=adc_convert[ int(data_all_new1[l1][selected_chip-1][l2]/1000) ];
            num_hit += 1; 
			if (data_all_new1[l1][selected_chip-1].size()-l2==1) // for the single hit layer 
			{
				chan_master.push_back(chan_truck/chan_truck_denominator );
                chan_master_Nhit.push_back(num_hit);
				chan_master_edep_sum.push_back(chan_truck_denominator);
			}

			for (int l3=0; l3<data_all_new1[l1][selected_chip-1].size()-(l2+1); l3++)
			{
				if ( fabs(standard_channel - (data_all_new1[l1][selected_chip-1][l3+l2+1]%1000))==l3+1 )	
				{						
					if ( (data_all_new1[l1][selected_chip-1][l3+l2+1]%1000) <128  )
					{
						chan_truck += (lower_section_initial + INTT_strip_width * (data_all_new1[l1][selected_chip-1][l3+l2+1]%1000)) * adc_convert[int(data_all_new1[l1][selected_chip-1][l3+l2+1]/1000)];
					}	
					else
					{
						chan_truck += ( upper_section_initial + INTT_strip_width * ( (data_all_new1[l1][selected_chip-1][l3+l2+1]%1000) -128 ) ) * adc_convert[int(data_all_new1[l1][selected_chip-1][l3+l2+1]/1000)];
					}

					chan_truck_denominator+=adc_convert[ int(data_all_new1[l1][selected_chip-1][l3+l2+1]/1000) ];
                    num_hit += 1;
					distance_meter+=1;

					if ((data_all_new1[l1][selected_chip-1].size()-(l2+1)-l3) == 1) // for the last hit
					{
						chan_master.push_back(chan_truck/chan_truck_denominator );	
                        chan_master_Nhit.push_back(num_hit);
						chan_master_edep_sum.push_back(chan_truck_denominator);
					}

					// channel_indicator = (data_all_new1[l1][selected_chip-1][l2+channel_indicator_distance]%1000);	
				}
				else 
				{
					chan_master.push_back(chan_truck/  chan_truck_denominator);
                    chan_master_Nhit.push_back(num_hit);
					chan_master_edep_sum.push_back(chan_truck_denominator);
					break; 
				}
			}
			
			l2+=distance_meter;
			distance_meter=0;
			chan_truck=0;
			chan_truck_denominator=0;
            num_hit = 0;
		}
		for (int l4=0; l4<chan_master.size(); l4++) 
		{
			if (selected_chip == chip_to_apply_align_correction)// note : only the specific chip is applied with the alignment correction.
			{
				chan_master[l4]=chan_master[l4]+alignment_array[l1]; // note : apply the alignment correction
			}
			

			if (chan_master[l4] >= 0 ){ combine_posi.push_back(chan_master_Nhit[l4] * 1000 + chan_master[l4]); }
			else if (chan_master[l4] < 0) { combine_posi.push_back(-1 * chan_master_Nhit[l4] * 1000 + chan_master[l4]); }

			// the line below is to check the cluster position
			// printf("cluster test, layer %d, chip : %i ,cluster of hit : %.0f, %.4f, combine : %.4f \n",l1,selected_chip,chan_master_Nhit[l4],chan_master[l4],combine_posi );
		}
		final_output.push_back(combine_posi);
		final_output_adc.push_back(chan_master_edep_sum);

		// note : start clean
		combine_posi.clear();
		chan_master.clear();
		chan_master_Nhit.clear();
		chan_master_edep_sum.clear();

	}//end of l1 loop


	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<256; i6++)	
			data_all_new[i5][i6].clear();

	for (int i5=0; i5<4; i5++)	
		for (int i6=0; i6<26; i6++)	
			data_all_new1[i5][i6].clear();

	cluster_str final_output_str_vec;

	final_output_str_vec.cluster_hit_pos_vec = final_output; 
	final_output_str_vec.cluster_adc_vec = final_output_adc;

	return final_output_str_vec;

	
}

// it works for the 3 layers testbeam setup. 
// NOT 4 layers but one layer missing hit
// for the 3 points alignment, the idea is simple,
// 3 points can have 3 combinations of 2 points 
// we pick up the one with smallest slope, and correct the rest point back to this slope. 

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

//title =======================================================================================================================
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
//title ========================================================================================================================

// the stopping line
//title ========================================================================================================================
//title ========================================================================================================================
//title ========================================================================================================================
//title ========================================================================================================================
//title ========================================================================================================================


void track_single_cluster_v1 (TString file_name) // by analyzing the tree_both
{
	//the name of the file you want to study, remember to remove the ".root"
	// TString file_name = "BeamData_20211210-2043_0_filter";
	//the direction of the data 
	//todo : for different runs, change here
	TString folder_direction = "/data4/chengwei/Geant4/INTT_simulation/G4/for_CW/data/run52_MC_2";
	//this line creates a folder to save all the informations and plot of this root file 

	TFile *f1 = new TFile (Form("%s/%s.root", folder_direction.Data(), file_name.Data()), "read");
	TTree *tree_both_in = (TTree *)f1->Get("tree_both");

	long event_N = tree_both_in -> GetEntriesFast();
	printf("# of event in file %s.root : %li \n",file_name.Data(),event_N);

	double INTT_strip_width = 0.078;
	double lower_section_initial = -9.945;
	double upper_section_initial = 0.039;

	//title =====================================================================================================================================	

	//title for setting ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	const int number_of_layer = 3; // number of layers of the INTT ladders in the Testbeam experiment. 
 	bool full_analysis = true;

 	int module_ID [4];
	module_ID[0] = 1; // module ID setting, if there are only 3 layers in the Testbeam, you don't need to change "l3", leave it alone
	module_ID[1] = 6;
	module_ID[2] = 5;
	module_ID[3] = 2;

	int adc_setting[9] = {15,30,60,90,120,150,180,210,240}; // adc setting
	int mV_setting[9];
	for (int i=0; i<9; i++) {mV_setting[i] = adc_setting[i]*4+210;}
	// int adc_setting[9] = {15,23,60,98,135,173,210,248,286}; // adc setting 
	double adc_convert[8];
	for (int i=0; i<8; i++) {adc_convert[i] = (adc_setting[i]+adc_setting[i+1])/2.;}

	double actual_xpos [4] = {0,26.1,52.2,78.3}; // the actual ladder position (unit : mm), now I use layer index
	//todo : for different runs, change here
	int camac_tdc_id = 5;	
	int camac_tdc_cutL = 400; //the camac_tdc cut,  left-hand side cut, 510 in principle
	int camac_tdc_cutR = 1200;//the camac_tdc cut, right-hand side cut


	bool DSE_on = false;// false -> no dse, true -> include double saving event 
	bool multi_cluster_on = true; // true -> consider the multi-cluster, false -> just the golden track 
	bool allow_cluster = true; //allow more than 1 hit forms a cluster. true-> allow. false -> not allow (only consider pure hit track)

	bool adc_0_cut = false; // true : if there is one hit that its adc value is 0, the event is skipped.
	
	//todo : for different runs, change here
	bool major_chip_on = false; // true -> the func. "majority_chip" is used, false -> the chip is choosed. 
	int  study_chip = 11; // (1 ~ 13) if major_chip_check is false, you have to give a chip_ID that you want to study
	
	
	bool camac_adc_on = true; // to use camac_adc cut or not, true -> camac_adc is a cut
	//todo : for different runs, change here
	int camac_adc_cut_0L = 77; // the camac_adc cut l0 left
	int camac_adc_cut_0R = 600; // the camac_adc cut l0 riight

	int camac_adc_cut_1L = 77; // the camac_adc cut l1 left 
	int camac_adc_cut_1R = 600; // the camac_adc cut l1 right
	
	int camac_adc_cut_2L = 77; // the camac_adc cut l2 left 
	int camac_adc_cut_2R = 600; // the camac_adc cut l2 right
	
	
	int analysis_size = 3000000; // the # of event you want to study


	int info_sampling = 1000; //to print the information per "info_sampling" event.
	bool show_bad_plot  = false; // to show the event display whose event profile is not perfect


	//title for setting ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	//todo : for different runs, change here
	double alignment_array [4]={0, -0.,0,0};  //offset matrix of 4 layers, Y axis; ** if the target sensor type is changed, the alignment needs to be re-studied 
	//    TColor::GetColor("#CB8262")
	// memset(alignment_array, 0, sizeof(alignment_array));
	

	TString output_folder_name = Form("%s_chip_%s",file_name.Data(),std::to_string(study_chip).c_str());
	system(Form("mkdir -p %s/folder_%s",folder_direction.Data(),output_folder_name.Data()));
	//title ====================================================================================================================================


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
	// note the 3-dimensional vector contains all the data [event][layer][info.]
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

	int adc_0_counter = 0;

	int event_length;
	vector<int> * camac_tdc = new vector<int>();
	vector<int> * camac_adc = new vector<int>();
	bool INTT_event;
	vector<int> * adc = new vector<int>();
	vector<int> * chip_id = new vector<int>();
	vector<int> * module = new vector<int>();
	vector<int> * chan_id = new vector<int>();
	vector<int> * event = new vector<int>();
	int DSE;
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
		if (INTT_event == 1 && camac_tdc->at(camac_tdc_id)>camac_tdc_cutL && camac_tdc->at(camac_tdc_id)<camac_tdc_cutR) //INTT_event, camac_tdc range cut
		{

			//todo : for different runs (data/MC), change here
			if (camac_adc_on == true ) // true -> the consider camac_adc as a cut as well
			{
				if (camac_adc->size() < 2) // note : requrie at least two scintillators fired (The cut is made for MC)
				{
					cout<<"----WWW---- the event is skipped due to only one camac_adc, eID : "<<eID<<" ----WWW----"<<endl;
					continue;
				}
				if ( camac_adc->at(0) < camac_adc_cut_0L || camac_adc->at(0) > camac_adc_cut_0R )
				{
					cout<<"----WWW---- the event is skipped due to l0 camac_adc outlier, eID : "<<eID<<" ----WWW----"<<endl;
					continue;
				}

				if ( camac_adc->at(1) < camac_adc_cut_1L || camac_adc->at(1) > camac_adc_cut_1R )
				{
					cout<<"----WWW---- the event is skipped due to l1 camac_adc outlier, eID : "<<eID<<" ----WWW----"<<endl;
					continue;
				}

				// if ( camac_adc->at(2) < camac_adc_cut_2L || camac_adc->at(2) > camac_adc_cut_2R )
				// {
				// 	cout<<"----WWW---- the event is skipped due to l2 camac_adc outlier, eID : "<<eID<<" ----WWW----"<<endl;
				// 	continue;
				// }

			}

			if (DSE_on == false && DSE != 0) // false -> we don't want the double-saving event
			{
				cout<<"----WWW---- the event is skipped due to DSE, eID : "<<eID<<" ----WWW----"<<endl;
				continue;
			}

			if ( event_length < 2 ) // # of hit is not enough to form a track
			{
				cout<<"----WWW---- the event is skipped due to only "<<event_length<< " hit, eID : "<<eID<<" ----WWW----"<<endl;
				continue;
			}

			if ( adc_0_cut == true) //true : adc 0 is excluded
			{
				for (int i1=0; i1<event_length; i1++)
				{
					if ( adc->at(i1) == 0 )
					{
						adc_0_counter = 1;
						break;
					} 
				}

				if (adc_0_counter == 1)
				{
					cout<<"----WWW---- the event is skipped due to the adc of some hits is 0"<< ", eID : "<<eID<<" ----WWW----"<<endl;
					adc_0_counter = 0;
					continue;
				}
			}

			// it's not a perfect cut, therefore command out 
			// if (allow_cluster == false && event_length > number_of_layer) // the cut for the tighter selection, 3 or 4 hits, no cluster
			// {
			// 	cout<<"----WWW---- the event is skipped due to not pure hit track candidate, hit : "<<event_length<< " eID : "<<eID<<" ----WWW----"<<endl;
			// 	continue;
			// }

			for (int i1=0; i1<event_length; i1++)
			{
				if ( chip_id->at(i1) > 0 && chip_id->at(i1) < 27 && chan_id->at(i1) > -1 && chan_id->at(i1) < 128 && adc->at(i1) > -1 && adc->at(i1) < 8)
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
				else
				{
					cout<<"----WWW---- the event is has some wrong data word "<<"eID : "<<eID<<" ----WWW----"<<endl;
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

	// delete the vector and close the root file to save the memory
	delete camac_tdc;
	delete camac_adc;
	delete adc;
	delete chip_id;
	delete module;
	delete chan_id;
	delete event;
	f1 -> Close();


	if (full_analysis == true)
	{
		//note data_all[layer][chip], it saves one event per loop, and saves the data in different format
		vector<int> data_all[4][26]; 
		vector<int> data_all_LoE[2][4][26]; 

		for (int i=0; i<4; i++)
		{
			for (int i1=0; i1<13; i1++)
			{
				data_all[i][i1].clear();
				data_all_LoE[0][i][i1].clear();
				data_all_LoE[1][i][i1].clear();
			}
		}
		
		// note : new added, for all the chips
		vector<vector<double>> data_all_MC_EW_chip[13];
		for (int i = 0; i < 13; i++)
		{
			data_all_MC_EW_chip[i].clear();
		}

		vector<vector<double>> data_all_MC_EW_chip_adcinfo; data_all_MC_EW_chip_adcinfo.clear();

        vector<vector<vector<int>>> data_all_EW_id; data_all_EW_id.clear(); // note : new implemented (new function), data_all_EW_id[4][13][cluster ID]

		vector<vector<double>> data_all_MC_EW; data_all_MC_EW.clear(); //note : data_all Most hit Chip, Edep Weighted. -> to find the positions of clusters 
		
		vector<vector<double>> data_all_MC_EW_LoE; data_all_MC_EW_LoE.clear(); //note : data_all Most hit Chip, Edep Weighted. this is for the LoE, the +- 1 chips is considered (larger longitudinal axis tolerance) 
		// vector<vector<double>> data_all_MC_EW_LoEM; data_all_MC_EW_LoEM.clear(); //data_all Most hit Chip, Edep Weighted. this is for the LoE, the +- 1 chips is considered (larger longitudinal axis tolerance) 
		int LoE_hit_check = 0;
		bool LoE_final_check = false;
		
		double hit2_best_fit_picker_layer[2];

		double hit3_actual_xpos_LoE[3];
		TGraph * hit3_grr_LoE;
		double hit3_fit_Y_LoE[3];
		TF1 * hit3_fit_LoE ;
		double hit3_best_fit_picker_layer_LoE [6];
		double hit3_best_fit_picker_LoE = 100000;


		TGraph* grr;

		vector<int>most_hit_info; most_hit_info.clear(); //this vector associates with the function "majority_chip", it provide the vote winner chip
		int pass_l1_counting =0; // the counter to count the number of event pass the l2 selection
		double hit3_layer[3];
		double hit3_fit_Y[3];
		double hit3_best_fit_picker=100000000000;
		double hit3_best_fit_picker_layer[14];
		double hit3_worse_hit_picker=0;
		int hit3_worse_hit_picker_layer;
		int hit3_fit_loop=0;
		double hit3_second_bad_picker_big=100000;
		int hit3_second_bad_picker;

		
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
		
		double hit4_second_residual[3];
		double hit4_second_bad_pciker[6];
		int hit4_second_residual_counter=0;
		double hit4_second_bad_pciker_big=10000000;

		

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
		vector<int> event_quality_chip; event_quality_chip.clear(); // to save the winner chip


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

		// the fitting function for 3 hits case, for the 2 hits we don't need to fit 
		TF1 * fit_basis;
		vector<TF1*> hit3_fit; hit3_fit.clear(); 

		TLatex *hit3_fit_latex = new TLatex();
		hit3_fit_latex->SetNDC();
		hit3_fit_latex->SetTextSize(0.028);

		//the fitting function for 4 hits case
		vector<TF1*> hit4_fit; hit4_fit.clear();


		double cluster_l0;
		double cluster_l1;
		double cluster_l2;
		double cluster_l3;
		int cluster_eID;
		int cluster_event_profile;
		int cluster_final_pickup;

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
		
		// note : new added, the save the cluster information into the root file
		int cluster_info_eID;
		vector<int> cluster_info_layer;
		vector<int> cluster_info_chip;
		vector<int> cluster_info_hit;
		vector<double> cluster_info_position;
		vector<double> cluster_info_raw;
		vector<double> cluster_info_adc;

		TFile * cluster_info_output_file = new TFile(Form("%s/folder_%s/cluster_information_offset%.4f_adcinfo.root",folder_direction.Data(),output_folder_name.Data(),alignment_array[1]), "RECREATE");
		cluster_info_output_file -> cd();
		TTree * cluster_info_output_tree = new TTree("cluster_info", "cluster_info");
		cluster_info_output_tree -> Branch("eID",&cluster_info_eID);
		cluster_info_output_tree -> Branch("layer",&cluster_info_layer);
		cluster_info_output_tree -> Branch("chip",&cluster_info_chip);
		cluster_info_output_tree -> Branch("Nhit",&cluster_info_hit);
		cluster_info_output_tree -> Branch("position",&cluster_info_position);
		cluster_info_output_tree -> Branch("raw_data",&cluster_info_raw);
		cluster_info_output_tree -> Branch("cluster_adc",&cluster_info_adc);

		// note : the memory to keep all the cluster information
		cluster_str all_cluster_info;

		// here we start to do the event analysis 
		// the level-1 selection is a overall selection, 
		// the level-2 selection can check the event quality in a more detail level 
		for (int i=0; i<event_event.size(); i++)
		{
			if (i%info_sampling==0) {printf("~~~~~~~ we are working on loop_i : %i, eID : %i, PLC candidate : %i \n",i,eID_array[i],pass_l1_counting );}

			for (int i1=0; i1<4; i1++) // note : to save the data in data_all[layer][chip], it makes us easier to do the analysis
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
					//note : if the orientation is different with the G4 model the line below has to be modified.
					if (event_chip[i][i1][i2]-1<13)//0 ~ 12
					{
						data_all[i1][ event_chip[i][i1][i2]-1 ].push_back(event_adc[i][i1][i2]*1000 + (event_chan[i][i1][i2]) ); //adc *1000 + chan info
					}
					else if (event_chip[i][i1][i2]-1>12 && event_chip[i][i1][i2]-1 < 26) //13 ~ 25
					{
						data_all[i1][ event_chip[i][i1][i2]-1 - 13 ].push_back(event_adc[i][i1][i2]*1000 + (255-event_chan[i][i1][i2]) ); //adc *1000 + chan info
					}
					
				}
			} // note : end of single event.

            // note : do something
            if (i%info_sampling==0) cout<<"============================== eID : "<<eID_array[i]<<", cluster information =============================="<<endl;
            event_chip_to_function = event_chip [i];
			most_hit_info = majority_chip(event_chip_to_function,major_chip_on,study_chip);
			

			cluster_info_eID = eID_array[i];
			for (int i10 = 0; i10 < 13; i10 ++) // note : chip, or you can call it slot.
			{
				if ( data_all[0][i10].size() + data_all[1][i10].size() + data_all[2][i10].size() + data_all[3][i10].size() > 0)
				{
					all_cluster_info = data_all_edep_weight(data_all,i10+1,adc_convert,alignment_array,study_chip);
					data_all_MC_EW_chip[i10] = all_cluster_info.cluster_hit_pos_vec;
					data_all_MC_EW_chip_adcinfo = all_cluster_info.cluster_adc_vec;

					for (int i11 = 0; i11 < 4; i11++) // note : layer
					{
						for (int i12 = 0; i12 < data_all_MC_EW_chip[i10][i11].size(); i12 ++) // note : also check the # of hit
						{
							cluster_info_layer.push_back( i11 );
							cluster_info_chip.push_back(i10+1);
							cluster_info_hit.push_back( abs(int(data_all_MC_EW_chip[i10][i11][i12]))/1000 );
							cluster_info_position.push_back(  data_all_MC_EW_chip[i10][i11][i12] - (int(data_all_MC_EW_chip[i10][i11][i12])/1000)*1000 );
							cluster_info_raw.push_back( data_all_MC_EW_chip[i10][i11][i12] );		
							cluster_info_adc.push_back( data_all_MC_EW_chip_adcinfo[i11][i12] );						
						} // note : end of the check hit of the given chip and layer.
						
						
					} // note : end of the layer loop
					
				
				} // note : end of check whether this chip has hits or not. (bool)
			} // note : end of the chip loop

			if (cluster_info_layer.size() > 0)
			{
				cluster_info_output_tree -> Fill();
			}

			// note : ============== ============== clean ============== ============== 
			
			for (int i10 = 0; i10 < 13; i10++)
			{
				data_all_MC_EW_chip[i10].clear();
			}

			most_hit_info.clear();
			for (int i10=0; i10<4; i10++)
			{
				for (int i11=0; i11<26; i11++)
				{
					data_all[i10][i11].clear();
					data_all_LoE[0][i10][i11].clear();
					data_all_LoE[1][i10][i11].clear();
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


			cluster_info_layer.clear();
			cluster_info_chip.clear();
			cluster_info_hit.clear();
			cluster_info_position.clear();
			cluster_info_raw.clear();
			cluster_info_adc.clear();

        } // note : end of the event loop

		cluster_info_output_file -> cd();
		cluster_info_output_tree -> Write("", TObject::kOverwrite);
		cluster_info_output_file -> Close();

    } // note : end of the full analysis bool

} // note : end of the world
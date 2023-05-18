struct hit_info {
    int chip_id;
    int chan_id;
    int adc;
};

struct clu_info {
    int column;
    int chip_id;
    double avg_chan;
    int sum_adc;
    int size;
};

struct ladder_info{
    int FC;
    TString port;
    int ROC;
    int direction; // note : 0 : south, 1 : north 
};

// note : assumption, in one event, single channel can only be fired once. 
vector<clu_info> clustering(vector<hit_info> single_event)
{
    int hit_seat[13][256]; memset(hit_seat, -1, sizeof(hit_seat)); // note : one event, one half-ladder
    vector<hit_info>hit_vec; hit_vec.clear(); // note : chip_id is not used

    clu_info clu_unit;
    vector<clu_info> output_vec; output_vec.clear();

    double num_hit = 0;
    double chan_truck = 0;
	double chan_truck_denominator = 0;
	int standard_channel;
	int distance_meter = 0;
    int sum_adc = 0;
    
    for (int i = 0; i < single_event.size(); i++) // note : number of hits in this event, for this half-ladder
    {
        // note : U1 ~ U13, 0 ~ 127. // U14 ~ U26, -> 128 ~ 255
        int chip_conv = ( single_event[i].chip_id > 13 ) ? single_event[i].chip_id - 13  : single_event[i].chip_id;
        int chan_conv = ( single_event[i].chip_id > 13 ) ? 255 - single_event[i].chan_id : single_event[i].chan_id;

        if (hit_seat[chip_conv - 1][chan_conv]  == -1)
        {
            hit_seat[chip_conv - 1][chan_conv] = single_event[i].adc;
        }
        else 
        {
            cout<<"chip_id : "<<chip_conv<<" chan_id : "<<chan_conv<<"fired more than once !"<<endl;
            hit_seat[chip_conv - 1][chan_conv] = single_event[i].adc; // note : take the latest one.   
        }
    }

    for (int col = 0; col < 13; col++) // note : column
    {
        for (int ch = 0; ch < 256; ch++) // note : channel 
        {
            if (hit_seat[col][ch] != -1)
            {
                hit_info hit_unit;
                hit_unit.chan_id = ch;
                hit_unit.adc = hit_seat[col][ch];
                
                hit_vec.push_back(hit_unit);
            }

        }

        // note : single column
        for (int i = 0; i < hit_vec.size(); i ++)
        {
            standard_channel        = hit_vec[i].chan_id;
            chan_truck             += hit_vec[i].chan_id;
            chan_truck_denominator += 1;
            num_hit                += 1; 
            sum_adc                += hit_vec[i].adc;

            if (hit_vec.size() - i == 1) // note : for the single hit
            {
                clu_unit.column   = col + 1;
                clu_unit.avg_chan = chan_truck / chan_truck_denominator;
                clu_unit.sum_adc  = sum_adc;
                clu_unit.size     = num_hit;

                output_vec.push_back(clu_unit);
            }

            for (int i1 = 0; i1 < hit_vec.size() - (i + 1); i1++) // note : this for loop will be skipped if it's single hit
            {
                if ( fabs(standard_channel - hit_vec[i + i1 + 1].chan_id) == i1 + 1 )
                {
                    chan_truck             += hit_vec[i + i1 + 1].chan_id;
                    chan_truck_denominator += 1;
                    num_hit                += 1; 
                    sum_adc                += hit_vec[i + i1 + 1].adc;

                    distance_meter += 1;

                    if ((hit_vec.size() - (i + 1) - i1) == 1) // note : for the last hit
					{
						clu_unit.column   = col + 1;
                        clu_unit.avg_chan = chan_truck / chan_truck_denominator;
                        clu_unit.sum_adc  = sum_adc;
                        clu_unit.size     = num_hit;

                        output_vec.push_back(clu_unit);
					}
                }
                else 
                {
                    clu_unit.column   = col + 1;
                    clu_unit.avg_chan = chan_truck / chan_truck_denominator;
                    clu_unit.sum_adc  = sum_adc;
                    clu_unit.size     = num_hit;

                    output_vec.push_back(clu_unit);

                    break;
                }
            }

            i += distance_meter;
			distance_meter = 0;
			chan_truck = 0;
			chan_truck_denominator = 0;
            num_hit = 0;
            sum_adc = 0;
        }

        // note : start of cleaning 
        hit_vec.clear();

    } // note : chip loop

    return output_vec;

}

map<TString,ladder_info> intt0_map = {
    {"B1L101S", {0, "D2", 0, 0}},
    {"B0L101S", {1, "C1", 0, 0}},
    {"B1L001S", {2, "C2", 0, 0}},
    {"B1L000S", {3, "B3", 0, 0}},
    {"B1L100S", {4, "A2", 0, 0}},
    {"B0L000S", {5, "B1", 0, 0}},
    {"B0L100S", {6, "A1", 0, 0}},

    {"B1L103S", {7, "C2", 1, 0}},
    {"B0L002S", {8, "C1", 1, 0}},
    {"B0L001S", {9, "A1", 1, 0}},
    {"B1L002S", {10, "B3", 1, 0}},
    {"B1L102S", {11, "A2", 1, 0}},
    {"B0L102S", {12, "B1", 1, 0}},
    {"B1L003S", {13, "D2", 1, 0}}
};

map<TString,ladder_info> intt1_map = {
    {"B1L105S", {0, "C2", 2, 0}},  
    {"B0L104S", {1, "C1", 2, 0}},  
    {"B0L103S", {2, "A2", 2, 0}},  
    {"B1L004S", {3, "B3", 2, 0}},  
    {"B1L104S", {4, "A1", 2, 0}},  
    {"B0L003S", {5, "B1", 2, 0}},  
    {"B1L005S", {6, "D2", 2, 0}},  

    {"B1L107S", {7, "C2", 3, 0}},  
    {"B0L005S", {8, "C1", 3, 0}},  
    {"B0L004S", {9, "A1", 3, 0}},  
    {"B1L006S", {10, "B2", 3, 0}},  
    {"B1L106S", {11, "A2", 3, 0}},  
    {"B0L105S", {12, "B1", 3, 0}},  
    {"B1L007S", {13, "D1", 3, 0}}  
};

map<TString,ladder_info> intt2_map = {
    {"B0L106S", {0, "A1", 4, 0}},  
    {"B0L006S", {1, "B1", 4, 0}},  
    {"B0L107S", {2, "C1", 4, 0}},  
    {"B1L108S", {3, "A2", 4, 0}},  
    {"B1L008S", {4, "B2", 4, 0}},  
    {"B1L109S", {5, "C2", 4, 0}},  
    {"B1L009S", {6, "D1", 4, 0}},  

    {"B0L007S", {7, "A1", 5, 0}},  
    {"B0L108S", {8, "B3", 5, 0}},  
    {"B0L008S", {9, "C1", 5, 0}},  
    {"B1L110S", {10, "A2", 5, 0}},  
    {"B1L010S", {11, "B2", 5, 0}},  
    {"B1L111S", {12, "C2", 5, 0}},  
    {"B1L011S", {13, "C3", 5, 0}} 
};

map<TString,ladder_info> intt3_map = {
    {"B0L109S", {0, "A1", 6, 0}},  
    {"B0L009S", {1, "B1", 6, 0}},  
    {"B0L110S", {2, "C1", 6, 0}},  
    {"B1L112S", {3, "A2", 6, 0}},  
    {"B1L012S", {4, "B3", 6, 0}},  
    {"B1L113S", {5, "C2", 6, 0}},  
    {"B1L013S", {6, "D1", 6, 0}},  

    {"B0L010S", {7, "A1", 7, 0}},  
    {"B0L111S", {8, "B1", 7, 0}},  
    {"B0L011S", {9, "C1", 7, 0}},  
    {"B1L114S", {10, "A2", 7, 0}},  
    {"B1L014S", {11, "B3", 7, 0}},  
    {"B1L115S", {12, "C3", 7, 0}},  
    {"B1L015S", {13, "D2", 7, 0}} 
};

map<TString,ladder_info> intt4_map = {
    {"B1L101N", {0,	"B1", 0, 1}},	
    {"B0L000N", {1,	"C2", 0, 1}},	
    {"B0L100N", {2,	"D1", 0, 1}},	
    {"B1L001N", {3,	"A2", 0, 1}},	
    {"B0L101N", {4,	"B2", 0, 1}},	
    {"B1L000N", {5,	"C3", 0, 1}},	
    {"B1L100N", {6,	"D2", 0, 1}},	 

    {"B0L002N", {7, "B1", 1, 1}},	 
    {"B0L102N", {8, "C2", 1, 1}},	
    {"B0L001N", {9, "D1", 1, 1}},	
    {"B1L003N", {10, "A2", 1, 1}},	 
    {"B1L103N", {11, "B2", 1, 1}},	
    {"B1L002N", {12, "C3", 1, 1}},	
    {"B1L102N", {13, "B3", 1, 1}}
};

map<TString,ladder_info> intt5_map = {
    {"B0L003N", {0, "C1", 2, 1}},	
    {"B0L104N", {1, "B1", 2, 1}},	
    {"B0L103N", {2, "D2", 2, 1}},	 
    {"B1L004N", {3, "D1", 2, 1}},	
    {"B1L005N", {4, "A2", 2, 1}},	 
    {"B1L104N", {5, "C2", 2, 1}},	
    {"B1L105N", {6, "A1", 2, 1}},	

    {"B1L107N", {7, "C3", 3, 1}},	
    {"B1L007N", {8, "B1", 3, 1}},	
    {"B1L006N", {9, "C1", 3, 1}},	
    {"B1L106N", {10, "D2", 3, 1}},	
    {"B0L005N", {11, "A1", 3, 1}},	
    {"B0L105N", {12, "C2", 3, 1}},	
    {"B0L004N", {13, "D1", 3, 1}}
};

map<TString,ladder_info> intt6_map = {
    {"B0L106N", {0, "A1", 4, 1}},	
    {"B0L006N", {1, "C3", 4, 1}},	
    {"B0L107N", {2, "B2", 4, 1}},	
    {"B1L108N", {3, "D1", 4, 1}},	
    {"B1L008N", {4, "C2", 4, 1}},	
    {"B1L109N", {5, "B3", 4, 1}},	
    {"B1L009N", {6, "A2", 4, 1}},	

    {"B0L007N", {7, "D1", 5, 1}},	
    {"B0L108N", {8, "C3", 5, 1}},	
    {"B0L008N", {9, "A2", 5, 1}},	
    {"B1L110N", {10, "D2", 5, 1}},	  
    {"B1L010N", {11, "C2", 5, 1}},	
    {"B1L111N", {12, "B3", 5, 1}},	
    {"B1L011N", {13, "A1", 5, 1}}  
};

map<TString,ladder_info> intt7_map = {
    {"B0L109N", {0, "B3", 6, 1}},	
    {"B0L009N", {1, "C1", 6, 1}},	
    {"B0L110N", {2, "B1", 6, 1}},	
    {"B1L112N", {3, "D1", 6, 1}},	
    {"B1L012N", {4, "C2", 6, 1}},	
    {"B1L113N", {5, "B2", 6, 1}},	
    {"B1L013N", {6, "A1", 6, 1}},	

    {"B0L010N", {7, "B2", 7, 1}},	
    {"B0L111N", {8, "C2", 7, 1}},	
    {"B0L011N", {9, "B1", 7, 1}},	  
    {"B1L114N", {10, "D2", 7, 1}},	  
    {"B1L014N", {11, "C3", 7, 1}},	
    {"B1L115N", {12, "B3", 7, 1}},	
    {"B1L015N", {13, "A1", 7, 1}}
};

void misc ()
{
    vector<hit_info> single_event; single_event.clear();
    hit_info hit_unit;

    hit_unit.chip_id = 1;  hit_unit.chan_id = 127; hit_unit.adc = 3; single_event.push_back(hit_unit);
    hit_unit.chip_id = 14; hit_unit.chan_id = 127; hit_unit.adc = 4; single_event.push_back(hit_unit);
    
    hit_unit.chip_id = 16;  hit_unit.chan_id = 0; hit_unit.adc = 3; single_event.push_back(hit_unit);

    hit_unit.chip_id = 13;  hit_unit.chan_id = 14; hit_unit.adc = 7; single_event.push_back(hit_unit);
    hit_unit.chip_id = 13;  hit_unit.chan_id = 15; hit_unit.adc = 7; single_event.push_back(hit_unit);
    hit_unit.chip_id = 13;  hit_unit.chan_id = 16; hit_unit.adc = 7; single_event.push_back(hit_unit);

    hit_unit.chip_id = 13;  hit_unit.chan_id = 127; hit_unit.adc = 1; single_event.push_back(hit_unit);
    hit_unit.chip_id = 26;  hit_unit.chan_id = 127; hit_unit.adc = 1; single_event.push_back(hit_unit);

    hit_unit.chip_id = 26;  hit_unit.chan_id = 5; hit_unit.adc = 4; single_event.push_back(hit_unit);
    
    for (int i = 0; i < single_event.size(); i++)
    {
        cout<<i<<" "<<single_event[i].chip_id<<" "<<single_event[i].chan_id<<" "<<single_event[i].adc<<endl;
    }

    vector<clu_info> clu_vec = clustering(single_event);

    cout<<"--------  --------  --------  --------"<<endl;

    for (int i = 0; i < clu_vec.size(); i++)
    {
        cout<<i<<" ->  column "<<clu_vec[i].column<<" chan "<<clu_vec[i].avg_chan<<" adc "<<clu_vec[i].sum_adc<<" size "<<clu_vec[i].size<<endl;
    }


}

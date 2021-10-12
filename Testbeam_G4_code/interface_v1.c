void interface_v1 (TString file_name)
{
	TString folder_name = "/data4/chengwei/Geant4/INTT_simulation/G4/for_CW/session7_solution_build";
	
	int adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event;

	TFile *file_output = new TFile(Form("%s/%s_convert.root", folder_name.Data(), file_name.Data()), "RECREATE");
	TTree *tree_output1 = new TTree("tree", "tree");
	
	tree_output1->Branch("adc", &adc);
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
	double ev_DAC_convert;

	tree_output1->Branch("ampl", &ampl); 
	//0
	
	tree_output1->Branch("chip_id", &chip_id);
	
	tree_output1->Branch("fpga_id", &fpga_id); 
	//0

	tree_output1->Branch("module", &module);
	//assume we use 
	//C-1 for layer-0 (6)
	//C-2 for layer-1 (5)
	//C-3 for layer-2 (8)
	//A-1 for layer-3 (2)

	tree_output1->Branch("chan_id", &chan_id);
	
	tree_output1->Branch("fem_id", &fem_id); 
	//0

	tree_output1->Branch("bco", &bco);
	tree_output1->Branch("bco_full", &bco_full);
	int event_buffer;
	TRandom *rand_bco = new TRandom3 ();
	TRandom *rand_bcofull = new TRandom3 ();
	int Frand_bco =  int(rand_bco->Uniform(0.,128.)); //0 ~ 127
	int Frand_bcofull = int(rand_bcofull->Uniform(0.,65536.)); //0 ~ 65535

	tree_output1->Branch("event", &event); //event_count, to avoid the case that edep<adc0
	int event_count=0;
	

	TFile *f1 = TFile::Open(Form("%s/%s.root", folder_name.Data(), file_name.Data()));
	TTree *Gettree = (TTree *)f1->Get("Chamber1");

	int Event_ID, UpandDown, Xpos, Ypos, Zpos, silicon_type;
	double Edep;

	Gettree->SetBranchAddress("Event_ID", &Event_ID);
	Gettree->SetBranchAddress("UpandDown", &UpandDown);
	Gettree->SetBranchAddress("Xpos", &Xpos);
	Gettree->SetBranchAddress("Ypos", &Ypos);
	Gettree->SetBranchAddress("Zpos", &Zpos);
	Gettree->SetBranchAddress("silicon_type", &silicon_type);
	Gettree->SetBranchAddress("Edep", &Edep);

	int total_size = Gettree->GetEntriesFast();
	cout<<"total entry : "<<total_size<<endl;
	
	Gettree->GetEntry(0);
	event_buffer=Event_ID;
	cout<<"First G4 EventID : "<<event_buffer<<endl;

	for (int i=0; i<total_size; i++)
	{
		Gettree->GetEntry(i);

		ev_DAC_convert = (Edep*100*1.6*Gain/3.6+offset-210.)/4.;
		// cout<<i<<" "<<ev_DAC_convert<<endl;

		if (ev_DAC_convert<=set_DAC0)
		{
			continue;
		}
		else 
		{
			fpga_id=0;
			fem_id=0;
			ampl=0;
			event=event_count;
			
			if (UpandDown==1) // upper
			{
				chan_id = 127-Ypos;
				chip_id = 13-Xpos;
			}
			else //down
			{
				chan_id = Ypos;
				chip_id = 26-Xpos;	
			}

			if (Zpos==0){module=6;}
			if (Zpos==1){module=5;}
			if (Zpos==2){module=8;}
			if (Zpos==3){module=2;}
			
			if (ev_DAC_convert<=set_DAC0) {continue;}
			else if (ev_DAC_convert>set_DAC0 && ev_DAC_convert<=set_DAC1){adc=0;}
			else if (ev_DAC_convert>set_DAC1 && ev_DAC_convert<=set_DAC2){adc=1;}
			else if (ev_DAC_convert>set_DAC2 && ev_DAC_convert<=set_DAC3){adc=2;}
			else if (ev_DAC_convert>set_DAC3 && ev_DAC_convert<=set_DAC4){adc=3;}
			else if (ev_DAC_convert>set_DAC4 && ev_DAC_convert<=set_DAC5){adc=4;}
			else if (ev_DAC_convert>set_DAC5 && ev_DAC_convert<=set_DAC6){adc=5;}
			else if (ev_DAC_convert>set_DAC6 && ev_DAC_convert<=set_DAC7){adc=6;}
			else{adc=7;}

			if (Event_ID == event_buffer)
			{
				bco=Frand_bco;
				bco_full=Frand_bcofull;
			}
			else
			{
				Frand_bco =  int(rand_bco->Uniform(0.,128.)); //0 ~ 127
				Frand_bcofull = int(rand_bcofull->Uniform(0.,65536.)); //0 ~ 65535
				bco=Frand_bco;
				bco_full=Frand_bcofull;
				event_buffer=Event_ID;

			}			

			tree_output1->Fill();
			event_count+=1;	
		}

		
	}

	file_output->cd();
	//cout<<"testtest"<<endl;
	tree_output1->Write("", TObject::kOverwrite);
	cout<<"conversion done "<<endl;
	file_output->Close();
}



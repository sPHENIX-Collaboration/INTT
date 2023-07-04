std::string file_name = "/sphenix/u/jbertaux/Data/Repositories/INTT_Dealignment/dat/sensor_survey.root";
std::string tree_name = "mark_tree";

TTree* mark_tree = nullptr;
Long64_t n = 0;

double scale = 100.0;
double arrow_size = 0.05;

int layer = 0;
int ladder = 0;
int sensor = 0;

std::array<TVector3, 4> cross_pos;
std::array<TVector3, 4> cross_dis;
std::array<TVector3*, 4> cross_pos_ptr;
std::array<TVector3*, 4> cross_dis_ptr;

void drawing_macro()
{
	TFile* file = TFile::Open(file_name.c_str(), "READ");
	if(!file)
	{
		std::cout << "Error opening file:" << std::endl;
		std::cout << "\t" << file_name << std::endl;
		return;
	}
	mark_tree = (TTree*)file->Get(tree_name.c_str());
	if(!mark_tree)
	{
		std::cout << "Could not get tree \"" << tree_name << "\" in file:" << std::endl;
		std::cout << "\t" << file_name << std::endl;
		return;
	}

	mark_tree->SetBranchAddress("layer", &layer);
	mark_tree->SetBranchAddress("ladder", &ladder);
	mark_tree->SetBranchAddress("sensor", &sensor);

	for(n = 0; n < 4; ++n)
	{
		cross_pos_ptr[n] = &(cross_pos[n]);
		cross_dis_ptr[n] = &(cross_dis[n]);
	}

	mark_tree->SetBranchAddress("cross_pos0", &(cross_pos_ptr[0]));
	mark_tree->SetBranchAddress("cross_dis0", &(cross_dis_ptr[0]));

	mark_tree->SetBranchAddress("cross_pos1", &(cross_pos_ptr[1]));
	mark_tree->SetBranchAddress("cross_dis1", &(cross_dis_ptr[1]));

	mark_tree->SetBranchAddress("cross_pos2", &(cross_pos_ptr[2]));
	mark_tree->SetBranchAddress("cross_dis2", &(cross_dis_ptr[2]));

	mark_tree->SetBranchAddress("cross_pos3", &(cross_pos_ptr[3]));
	mark_tree->SetBranchAddress("cross_dis3", &(cross_dis_ptr[3]));
}

void Draw(int const& _layer = -1, int const& _ladder = -1, int const& _sensor = -2) 
{
	int i;

	TCanvas* cnvs = new TCanvas("cnvs", "cnvs", 0, 0, 1280, 720);
	cnvs->cd();

	TPad* graph_pad = new TPad("graph_pad", "graph_pad", 0.0, 0.0, 1.0, 1.0);
	graph_pad->SetFillStyle(4000);
	graph_pad->Range(0.0, 0.0, 1.0, 1.0);
	graph_pad->Draw();

	TGraph* graph = new TGraph(); 
	graph->SetMarkerStyle(20);
	graph->SetMarkerSize(0.5);
	for(n = 0; n < mark_tree->GetEntriesFast(); ++n)
	{
		mark_tree->GetEntry(n);

		if(_sensor > -2 and _sensor != sensor)continue;
		if(_layer > -1 and _layer != layer)continue;
		if(_ladder > -1 and _ladder != ladder)continue;

		for(i = 0; i < 4; ++i)
		{
			printf("\tx_%d:\t%10.6f\ty_%d:\t%10.6f\tz_%d:\t%10.6f\n", i, cross_pos[i][0], i, cross_pos[i][1], i, cross_pos[i][2]);
			graph->AddPoint(cross_pos[i][2] + scale * cross_dis[i][2], cross_pos[i][0] + scale * cross_dis[i][0]);
		}
	}
	graph->Draw("AP");

	std::cout << "x:\t" << graph->GetXaxis()->GetXmin() << "\t"  << graph->GetXaxis()->GetXmax() << std::endl;
	std::cout << "y:\t" << graph->GetYaxis()->GetXmin() << "\t"  << graph->GetYaxis()->GetXmax() << std::endl;

	double w = 0.0;
	double c = 0.0;
	double xmin = graph->GetXaxis()->GetXmin();
	double ymin = graph->GetYaxis()->GetXmin();
	double xmax = graph->GetXaxis()->GetXmax();
	double ymax = graph->GetYaxis()->GetXmax();

	w = (xmax - xmin) / 0.8;
	c = (xmax + xmin) / 2.0;
	xmin = c - w / 2;
	xmax = c + w / 2;

	w = (ymax - ymin) / 0.8;
	c = (ymax + ymin) / 2.0;
	ymin = c - w / 2;
	ymax = c + w / 2;

	cnvs->cd();
	TPad* ref_pad = new TPad("ref_pad", "ref_pad", 0.0, 0.0, 1.0, 1.0);
	ref_pad->SetFillStyle(4000);
	ref_pad->Range(xmin, ymin, xmax, ymax);
	ref_pad->Draw();

	for(n = 0; n < mark_tree->GetEntriesFast(); ++n)
	{
		mark_tree->GetEntry(n);

		if(_sensor > -2 and _sensor != sensor)continue;
		if(_layer > -1 and _layer != layer)continue;
		if(_ladder > -1 and _ladder != ladder)continue;

		for(i = 0; i < 4; ++i)
		{
			TArrow* a = new TArrow
			(
				cross_pos[i][2],
				cross_pos[i][0],
				cross_pos[i][2] + scale * cross_dis[i][2],
				cross_pos[i][0] + scale * cross_dis[i][0],
				arrow_size / scale,
				"|>"
			);
			a->SetLineColor(kRed);
			a->Draw();
		}
	}

	cnvs->Update();
	cnvs->Show();
}

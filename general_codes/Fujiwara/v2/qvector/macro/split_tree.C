#include <TFile.h>
#include <TTree.h>
#include <TEventList.h>
#include <iostream>

using namespace std;

const Int_t nq = 20;
Double_t xq[nq];
Double_t yq[nq];
TTree* EventTree = nullptr;
TTree* EventTree1 = nullptr;
double nclus,qx,qy,psi;
double nclus_south,qx_south,qy_south,psi_south;
double nclus_north,qx_north,qy_north,psi_north;
vector<float> *mbd_phi = nullptr;
vector<float> *mbd_q = nullptr;

void setupBranches(TTree*tree);

void split_tree(string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
		//"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root"
)
{

  // 入力ファイルを開く
  TFile* inputFile = TFile::Open(fname.c_str(), "UPDATE");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Failed to open file: " << fname << std::endl;
        return;
    }

    // TTreeを取得
    EventTree = (TTree*)inputFile->Get("EventTree");
    EventTree1 = (TTree*)inputFile->Get("EventTree");
    //EventTree->SetBranchStatus("*",1);
    if (!EventTree) {
        std::cerr << "Failed to find TTree: " << std::endl;
        inputFile->Close();
        return;
    }
    
    setupBranches(EventTree);

    int nmax = EventTree->GetMaximum("Adcfiltered_NClus");
    cout << "nmax =" << nmax << endl;

    int nbinwid = 10;
    TH1F*nclush = new TH1F("nclus", "multiplicity", nmax / nbinwid, 0, nmax);

    for(int i = 0;i<EventTree->GetEntries();i++)
      {
	EventTree->GetEntry(i);
	nclush->Fill(nclus);
      }

    for (Int_t i = 0; i < nq; i++)
    {
        xq[i] = Float_t(i + 1) / nq;
        cout << xq[i] << endl;
        nclush->GetQuantiles(nq, yq, xq);
        cout << "yq" << i << "= " << yq[i] << endl;
    }

    std::vector<std::string> conditions;
    for (size_t i = 0; i < nq; ++i) {
        if (i == 0) {
            conditions.push_back(Form("Adcfiltered_NClus <= %f", yq[i]));
        } else {
            conditions.push_back(Form("%f < Adcfiltered_NClus && Adcfiltered_NClus <= %f", yq[i - 1], yq[i]));
        }
    }
    //conditions.push_back(Form("%f < SomeBranch", yq[nq])); // 最後の範囲

    EventTree->SetBranchStatus("*",1);
    TEventList* eventLists;
    
    // 条件に基づいてTEventListを作成
    for (size_t i = 0; i < conditions.size(); ++i) 
      {
        std::cout << "Processing condition: " << conditions[i] << std::endl;
	
        // TEventListの作成とイベント抽出
        //eventLists[i] = new TEventList(Form("list_%zu", i), conditions[i].c_str());
        EventTree->Draw(">>list", conditions[i].c_str());

	eventLists = (TEventList*)gROOT->FindObject("list");
	
        // 新しいTTreeを作成
        TTree* newTree = EventTree->CloneTree(0); // 空のTTreeを作成
        for (int j = 0; j < eventLists->GetN(); ++j) {
	  EventTree->GetEntry(eventLists->GetEntry(j));
	  newTree->Fill();
        }

        // 新しいTTreeを追記保存
        inputFile->cd();
        newTree->Write(Form("EventTree_%zu", i),TObject::kOverwrite);

        delete newTree;
	delete eventLists;
    }

    /*
    delete EventTree;
    EventTree = nullptr;
    EventTree = (TTree*)inputFile->Get("EventTree");
    EventTree->SetBranchStatus("*",1);
    EventTree->Write("EventTree",TObject::kOverwrite);
    */

    EventTree1->SetBranchStatus("*",1);
    EventTree1->Write("EventTree",TObject::kOverwrite);
    inputFile->Close();
}

void setupBranches(TTree*itree)
{
  itree->SetBranchStatus("*",0);
  itree->SetBranchStatus("Adcfiltered_NClus", 1);
  itree->SetBranchAddress("Adcfiltered_NClus", &nclus);
  itree->SetBranchStatus("Intt_Qx", 1);
  itree->SetBranchAddress("Intt_Qx", &qx);
  itree->SetBranchStatus("Intt_Qy", 1);
  itree->SetBranchAddress("Intt_Qy", &qy);
  itree->SetBranchStatus("Intt_Psi", 1);
  itree->SetBranchAddress("Intt_Psi", &psi);

  itree->SetBranchStatus("Adcfiltered_NClus_south", 1);
  itree->SetBranchAddress("Adcfiltered_NClus_south", &nclus_south);
  itree->SetBranchStatus("Intt_Qx_south", 1);
  itree->SetBranchAddress("Intt_Qx_south", &qx_south);
  itree->SetBranchStatus("Intt_Qy_south", 1);
  itree->SetBranchAddress("Intt_Qy_south", &qy_south);
  itree->SetBranchStatus("Intt_Psi_south", 1);
  itree->SetBranchAddress("Intt_Psi_south", &psi_south);

  itree->SetBranchStatus("Adcfiltered_NClus_north", 1);
  itree->SetBranchAddress("Adcfiltered_NClus_north", &nclus_north);
  itree->SetBranchStatus("Intt_Qx_north", 1);
  itree->SetBranchAddress("Intt_Qx_north", &qx_north);
  itree->SetBranchStatus("Intt_Qy_north", 1);
  itree->SetBranchAddress("Intt_Qy_north", &qy_north);
  itree->SetBranchStatus("Intt_Psi_north", 1);
  itree->SetBranchAddress("Intt_Psi_north", &psi_north);

  itree->SetBranchStatus("MBD_phi", 1);
  itree->SetBranchAddress("MBD_phi", &mbd_phi);
  itree->SetBranchStatus("MBD_pmt_q", 1);
  itree->SetBranchAddress("MBD_pmt_q", &mbd_q);
}

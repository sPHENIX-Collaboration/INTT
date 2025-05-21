#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
#include <iostream>
#include <string>

using namespace std;


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
//R__LOAD_LIBRARY(libinttqvector.so)
//R__LOAD_LIBRARY(libinttread.so)

// how many seperate area by multiplicity
const Int_t nq = 20;

TFile *ifile;
TTree *itree;

TFile *outputFile;
string outputFilename;

double nclus;
double qx;
double qy;
double psi;

double qx_rec;
double qy_rec;
double psi_rec;

double nclus_south;
double qx_south;
double qy_south;
double psi_south;

double qx_rec_south;
double qy_rec_south;
double psi_rec_south;

double nclus_north;
double qx_north;
double qy_north;
double psi_north;

double qx_rec_north;
double qy_rec_north;
double psi_rec_north;

int nmax;
int qxmax;
int qymax;
int psimax;

int qxmin;
int qymin;
int psimin;

int qxbin;
int qybin;
int psibin;

// width of each histogram
double nbinwid = 10;
double qxbinwid = 0.01;
double qybinwid = 0.01;
double psibinwid = 0.02;

//Double_t xq[nq];
//Double_t yq[nq];

TH1F *nclush;
TH2S *allqvec;
TH1F *allpsi;

TH1F *nclush_south;
TH2S *allqvec_south;
TH1F *allpsi_south;

TH1F *nclush_north;
TH2S *allqvec_north;
TH1F *allpsi_north;

TH2S *allqvec_rec;
TH1F *allpsi_rec;

TH2S *allqvec_rec_south;
TH1F *allpsi_rec_south;

TH2S *allqvec_rec_north;
TH1F *allpsi_rec_north;

/*
TH2S *qvector[nq];
TH1F *psidis[nq];
TH1F *nclusdis[nq];

TH2S *qvector_rec[nq];
TH1F *psidis_rec[nq];

TProfile *qxprof[2];
TProfile *qyprof[2];
TProfile *qx_recprof[4];
TProfile *qy_recprof[4];
*/

void setBranches(TTree*itree);
void overwriteBranch(TTree *tree, const char *branchName);
void setHistsBinRange(TTree*itree);
void newHists();
void ClearHists();
void newQuantaileHists();
void newProfs();
void cal_rec(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void cal_rec_south(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void cal_rec_north(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void FillQvector(TTree*itree);

void INTTQvector_recentering(  
			      //string fname ="InttQvector20869_20240126_adc_rename.root"
			      //string fname = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root"
			      //string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_00000.root"
			      string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
			      //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root"
			      //int mode = 0
)
{
  /*
    // new outputfile for histgram reacord
    outputFilename = fname;
    // erase .root
    outputFilename.erase(outputFilename.end() - 5, outputFilename.end());
    outputFilename = outputFilename + "_rec_histlist.root";

    outputFile = new TFile(outputFilename.c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie())
    {
        printf("Error: cannot open output file %s\n", outputFilename.c_str());
        return;
    }
  */

    // read ttree and
    //ifile = new TFile(fname.c_str(), "UPDATE");
    //itree = ifile->Get<TTree>("EventTree");
  TFile *ifile = TFile::Open(fname.c_str(), "UPDATE");

  itree = (TTree *)ifile->Get("EventTree");
  cout<<"EventTree Name "<<itree->GetName()<<endl;
      
  itree->SetBranchStatus("*", 0);
  
  setBranches(itree);	  
  setHistsBinRange(itree);
  FillQvector(itree);  
  
  itree->SetBranchStatus("*", 1);
  itree->Write("EventTree", TObject::kOverwrite);
  delete itree;

  for(int i=0;i<nq;i++)
    {
      itree = (TTree *)ifile->Get(Form("EventTree_%d", i));
      cout<<"EventTree Name "<<itree->GetName()<<endl;

      //itree->SetBranchStatus("*", 1);
      //setHistsBinRange();
      
      itree->SetBranchStatus("*", 0);
      
      setBranches(itree);	  
      setHistsBinRange(itree);
      FillQvector(itree);

 
      itree->SetBranchStatus("*", 1);
      itree->Write(Form("EventTree_%d", i), TObject::kOverwrite);
      delete itree;
    }

    /*
    TBranch *qxrb;
    TBranch *qyrb;
    TBranch *psirb;

    TBranch *qxrb_south;
    TBranch *qyrb_south;
    TBranch *psirb_south;

    TBranch *qxrb_north;
    TBranch *qyrb_north;
    TBranch *psirb_north;

    setBranches(0,itree);
    setBranches(1,itree);
    setBranches(2,itree);
	  
    setHistsBinRange(itree);

    overwriteBranch(itree, "Qx_rec");
    overwriteBranch(itree, "Qy_rec");
    overwriteBranch(itree, "psi_rec");
    
    qxrb = itree->Branch("Qx_rec", &qx_rec);
    qyrb = itree->Branch("Qy_rec", &qy_rec);
    psirb = itree->Branch("psi_rec", &psi_rec);
    
    overwriteBranch(itree, "Qx_rec_south");
    overwriteBranch(itree, "Qy_rec_south");
    overwriteBranch(itree, "psi_rec_south");
    
    qxrb_south = itree->Branch("Qx_rec_south", &qx_rec_south);
    qyrb_south = itree->Branch("Qy_rec_south", &qy_rec_south);
    psirb_south = itree->Branch("psi_rec_south", &psi_rec_south);

    overwriteBranch(itree, "Qx_rec_north");
    overwriteBranch(itree, "Qy_rec_north");
    overwriteBranch(itree, "psi_rec_north");
	  
    qxrb_north = itree->Branch("Qx_rec_north", &qx_rec_north);
    qyrb_north = itree->Branch("Qy_rec_north", &qy_rec_north);
    psirb_north = itree->Branch("psi_rec_north", &psi_rec_north);
    */

    /*
    switch(mode)
      {
      case 0:
	{
	  setBranches(mode);
	  
	  overwriteBranch(itree, "Qx_rec");
	  overwriteBranch(itree, "Qy_rec");
	  overwriteBranch(itree, "psi_rec");
	  
	  qxrb = itree->Branch("Qx_rec", &qx_rec);
	  qyrb = itree->Branch("Qy_rec", &qy_rec);
	  psirb = itree->Branch("psi_rec", &psi_rec);
	  break;
	}

      case 1:
	{
	  setBranches(mode);
	  
	  overwriteBranch(itree, "Qx_rec_south");
	  overwriteBranch(itree, "Qy_rec_south");
	  overwriteBranch(itree, "psi_rec_south");
	  
	  qxrb = itree->Branch("Qx_rec_south", &qx_rec);
	  qyrb = itree->Branch("Qy_rec_south", &qy_rec);
	  psirb = itree->Branch("psi_rec_south", &psi_rec);
	  break;
	}

	case 2:
	{
	  setBranches(mode);
	  
	  overwriteBranch(itree, "Qx_rec_north");
	  overwriteBranch(itree, "Qy_rec_north");
	  overwriteBranch(itree, "psi_rec_north");
	  
	  qxrb = itree->Branch("Qx_rec_north", &qx_rec);
	  qyrb = itree->Branch("Qy_rec_north", &qy_rec);
	  psirb = itree->Branch("psi_rec_north", &psi_rec);
	  break;
	}
      }
    */

    //setBranches();
    //setHistsBinRange();

    //newHists();
    //newProfs();

    /*
    // Fill all Qvector, NClus, Psi, QxQyProf
    int Entries = itree->GetEntries();

    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	if (nclus != 0){
        nclush->Fill(nclus);
        allpsi->Fill(psi);
        allqvec->Fill(qx, qy);
	}
	
	if (nclus_south != 0){
        nclush->Fill(nclus_south);
        allpsi->Fill(psi_south);
        allqvec->Fill(qx_south, qy_south);
	}
	
	if (nclus_north != 0){
        nclush->Fill(nclus_north);
        allpsi->Fill(psi_north);
        allqvec->Fill(qx_north, qy_north);
	}
	
        if (isfinite(qx) == true)
        {
            qxprof[0]->Fill(nclus / nmax * 100, qx);
            qyprof[0]->Fill(nclus / nmax * 100, qy);

            qxprof[1]->Fill(nclus, qx);
            qyprof[1]->Fill(nclus, qy);
        }
	
    }
    */
    /*
    // set_quantile(yq);
    for (Int_t i = 0; i < nq; i++)
    {
        xq[i] = Float_t(i + 1) / nq;
        cout << xq[i] << endl;
        nclush->GetQuantiles(nq, yq, xq);
        cout << "yq" << i << "= " << yq[i] << endl;
    }
    */

    /*
    // recentering qx,qy using all event
    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	 if (nclus == 0)
        {
            qx_rec = std::nan("");
            qy_rec = std::nan("");
            psi_rec = std::nan("");
	    
	    qxrb->Fill();
	    qyrb->Fill();
	    psirb->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }

	 if (nclus_south == 0)
        {
            qx_rec_south = std::nan("");
            qy_rec_south = std::nan("");
            psi_rec_south = std::nan("");
	    
	    qxrb_south->Fill();
	    qyrb_south->Fill();
	    psirb_south->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }

	 if (nclus_north == 0)
        {
            qx_rec_north = std::nan("");
            qy_rec_north = std::nan("");
            psi_rec_north = std::nan("");
	    
	    qxrb_north->Fill();
	    qyrb_north->Fill();
	    psirb_north->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }


	 if(nclus!=0)
	   {
	     cal_rec(allqvec, allqvec_rec, allpsi_rec);
	     qxrb->Fill();
	     qyrb->Fill();
	     psirb->Fill();
	   }
	 
	 if(nclus_south!=0)
	   {
	     cal_rec_south(allqvec_south, allqvec_rec_south, allpsi_rec_south);
	     qxrb_south->Fill();
	     qyrb_south->Fill();
	     psirb_south->Fill();
	   }

	 if(nclus_north!=0)
	   {
	     cal_rec_north(allqvec_north, allqvec_rec_north, allpsi_rec_north);
	     qxrb_north->Fill();
	     qyrb_north->Fill();
	     psirb_north->Fill();
	   }
    }
    */
    /*
    newQuantaileHists();

    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

        for (int j = 0; j < nq; j++)
        {
            
            if (j > 0 && yq[j - 1] < nclus && nclus <= yq[j])
            {
                cout << "range =(" << (j > 0 ? yq[j - 1] : 0) << ", " << yq[j] << ")" << endl;
                qvector[j]->Fill(qx, qy);
                psidis[j]->Fill(psi);
                nclusdis[j]->Fill(nclus);
            }
            else if (j == 0 && 0 <nclus&& nclus <= yq[j])
            {
                cout << "range =(" << (j > 0 ? yq[j - 1] : 0) << ", " << yq[j] << ")" << endl;
                qvector[j]->Fill(qx, qy);
                psidis[j]->Fill(psi);
                nclusdis[j]->Fill(nclus);
            }
            
            if ((j > 0 ? yq[j - 1] : 0) < nclus && nclus <= yq[j])
            {
                qvector[j]->Fill(qx, qy);
                psidis[j]->Fill(psi);
                nclusdis[j]->Fill(nclus);
            }
        }
    }
    */
    
    /*
    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

        // クラスタが0の場合、NaNを設定してスキップ
	
        if (nclus == 0)
              continue;

        // クラスタ範囲による処理
        for (int j = 0; j < nq; j++)
        {
            if((j > 0 ? yq[j - 1] : 0) < nclus && nclus <= yq[j])
            {
                cal_rec(qvector[j], qvector_rec[j], psidis_rec[j]);
		
                qxrb->Fill();
		qyrb->Fill();
		psirb->Fill();
		
                // プロファイルヒストグラムへの記録
                double norm_nclus = nclus / nmax * 100;
                qx_recprof[2]->Fill(norm_nclus, qx_rec);
                qy_recprof[2]->Fill(norm_nclus, qy_rec);
                qx_recprof[3]->Fill(nclus, qx_rec);
                qy_recprof[3]->Fill(nclus, qy_rec);
            }
        }
    }
    */
    
    /*
    // 現在のディレクトリのオブジェクトリストを取得
    TIter next(gDirectory->GetList());
    TObject *obj;

    // オブジェクトリストをループ
    while ((obj = (TObject *)next()))
    {
        // ヒストグラムかどうかを確認 (TH1 または TH2 を継承しているか)
        if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TH2"))
        {
            // ヒストグラムを出力ファイルに保存
            outputFile->cd();
            obj->Write();
            printf("Saved: %s\n", obj->GetName());
        }
    }

    // ファイルをクローズ
    cout<<outputFilename<<" is created."<<endl;
    outputFile->Close();
    ifile->cd();
    */

    //itree->SetBranchStatus("*", 1);
    //itree->Write("", TObject::kOverwrite);
    ifile->Close();

}

void setBranches(TTree*itree)
{
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
}

void overwriteBranch(TTree *tree, const char *branchName)
{
    TBranch *branch = tree->GetBranch(branchName);
    if (branch)
    {
        tree->GetListOfBranches()->Remove(branch);
        delete branch;
    }
}

void setHistsBinRange(TTree*itree)
{
    nmax = itree->GetMaximum("Adcfiltered_NClus");
    cout << "nmax =" << nmax << endl;

    //qxmax = ceil(itree->GetMaximum("Intt_Qx"));
    //qxmin = floor(itree->GetMinimum("Intt_Qx"));
    qxmax = 1;
    qxmin = -1;
    cout << "qx range = " << qxmax << "  " << qxmin << endl;

    //qymax = ceil(itree->GetMaximum("Intt_Qy"));
    //qymin = floor(itree->GetMinimum("Intt_Qy"));
    qymax = 1;
    qymin = -1;
    cout << "qy range = " << qymax << "  " << qymin << endl;

    //psimax = ceil(itree->GetMaximum("Intt_Psi"));
    //psimin = floor(itree->GetMinimum("Intt_Psi"));
    psimax = 2;
    psimin = -2;
    cout << "psi range = " << psimax << "  " << psimin << endl;

    qxbin = (qxmax + abs(qxmin)) / qxbinwid;
    qybin = (qymax + abs(qymin)) / qybinwid;
    psibin = (psimax + abs(psimin)) / psibinwid;
    cout << "qx qy psibin " << qxbin << "  " << qybin << "  " << psibin << endl;

    if (qxbin != qybin)
    {
        int maxbin = std::max(qxbin, qybin);
        qxmax = qymax = (qxbin > qybin) ? qxmax : qymax;
        qxmin = qymin = (qxbin > qybin) ? qxmin : qymin;
        qxbin = qybin = maxbin;
    }
}

void newHists()
{
    // new Qvector all, qvector recentering all, NClus, psi all, psi recentering all hist
    allqvec = new TH2S("allqvec", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec = new TH2S("allqvec_rec", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    //nclush = new TH1F("nclus", "multiplicity", nmax / nbinwid, 0, nmax);

    allpsi = new TH1F("allpsi", "all psi", psibin, psimin, psimax);
    allpsi_rec = (TH1F *)allpsi->Clone("allpsi_rec");


    allqvec_south = new TH2S("allqvec_south", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec_south = new TH2S("allqvec_rec_south", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    //nclush_south = new TH1F("nclus_south", "multiplicity", nmax / nbinwid, 0, nmax);

    allpsi_south = new TH1F("allpsi_south", "all psi", psibin, psimin, psimax);
    allpsi_rec_south = (TH1F *)allpsi->Clone("allpsi_rec_south");

    allqvec_north = new TH2S("allqvec_north", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec_north = new TH2S("allqvec_rec_north", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    //nclush_north = new TH1F("nclus_north", "multiplicity", nmax / nbinwid, 0, nmax);

    allpsi_north = new TH1F("allpsi_north", "all psi", psibin, psimin, psimax);
    allpsi_rec_north = (TH1F *)allpsi->Clone("allpsi_rec_north");
}
/*
void newQuantaileHists()
{
    for (int i = 0; i < nq; i++)
    {
        double rangemin = (double)i / (double)nq * 100.0;

        // qvector,qvector_rec
        qvector[i] = new TH2S(Form("qvector%d", i), Form("qvector %d-%d", 100 - (int)(xq[i] * 100), 100 - (int)rangemin), qxbin, qxmin, qxmax, qybin, qymin, qymax);
        qvector_rec[i] = new TH2S(Form("qvector_rec%d", i), "qvector recentering", qxbin, -10, 10, qybin, -10, 10);

        // nclusdis
        nclusdis[i] = new TH1F(Form("nclus%d", i), Form("multiplicity %d-%d", 100 - (int)(xq[i] * 100), 100 - (int)rangemin), nmax, 0, nmax);

        // psidis,psidis_rec
        psidis[i] = new TH1F(Form("psi%d", i), Form("psi %d-%d", 100 - (int)(xq[i] * 100), 100 - (int)rangemin), psibin, psimin, psimax);
        psidis_rec[i] = new TH1F(Form("psi_rec%d", i), "psi recentering", psibin, psimin, psimax);
    }
}
*/
/*
void newProfs()
{
    qxprof[0] = new TProfile("qxmulti", "qx vs multiplicity", 20, 0, 100);
    qyprof[0] = new TProfile("qymulti", "qy vs multiplicity", 20, 0, 100);

    qxprof[1] = new TProfile("qxmulti_nclus", "qx vs multiplicity", nmax / 100, 0, nmax);
    qyprof[1] = new TProfile("qymulti_nclus", "qy vs multiplicity", nmax / 100, 0, nmax);

    qx_recprof[0] = new TProfile("qx_recmulti", "qx recentering vs multiplicity", 20, 0, 100);
    qy_recprof[0] = new TProfile("qy_recmulti", "qy recentering vs multiplicity", 20, 0, 100);

    qx_recprof[1] = new TProfile("qx_recmulti_nclus", "qx recentering vs multiplicity", nmax / 100, 0, nmax);
    qy_recprof[1] = new TProfile("qy_recmulti_nclus", "qy recentering vs multiplicity", nmax / 100, 0, nmax);

    qx_recprof[2] = new TProfile("qx_rec_sep", "qx recentering vs multiplicity", 20, 0, 100);
    qy_recprof[2] = new TProfile("qy_rec_sep", "qy recentering vs multiplicity", 20, 0, 100);

    qx_recprof[3] = new TProfile("qx_rec_sep_nclus", "qx recentering vs multiplicity", nmax / 100, 0, nmax);
    qy_recprof[3] = new TProfile("qy_rec_sep_nclus", "qy recentering vs multiplicity", nmax / 100, 0, nmax);
}
*/
void cal_rec(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec = (qx - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec = (qy - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec, qy_rec);

    psi_rec = 0.5 * atan2(qy_rec, qx_rec);

    allpsi_rec->Fill(psi_rec);
}

void cal_rec_south(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec_south = (qx_south - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec_south = (qy_south - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec_south, qy_rec_south);

    psi_rec_south = 0.5 * atan2(qy_rec_south, qx_rec_south);

    allpsi_rec->Fill(psi_rec_south);
}

void cal_rec_north(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec_north = (qx_north - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec_north = (qy_north - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec_north, qy_rec_north);

    psi_rec_north = 0.5 * atan2(qy_rec_north, qx_rec_north);

    allpsi_rec->Fill(psi_rec_north);
}

void FillQvector(TTree*itree)
{
    TBranch *qxrb;
    TBranch *qyrb;
    TBranch *psirb;

    TBranch *qxrb_south;
    TBranch *qyrb_south;
    TBranch *psirb_south;

    TBranch *qxrb_north;
    TBranch *qyrb_north;
    TBranch *psirb_north;

    //setBranches(itree);
	  
    //setHistsBinRange(itree);

    overwriteBranch(itree, "Qx_rec");
    overwriteBranch(itree, "Qy_rec");
    overwriteBranch(itree, "psi_rec");
    
    qxrb = itree->Branch("Qx_rec", &qx_rec);
    qyrb = itree->Branch("Qy_rec", &qy_rec);
    psirb = itree->Branch("psi_rec", &psi_rec);
    
    overwriteBranch(itree, "Qx_rec_south");
    overwriteBranch(itree, "Qy_rec_south");
    overwriteBranch(itree, "psi_rec_south");
    
    qxrb_south = itree->Branch("Qx_rec_south", &qx_rec_south);
    qyrb_south = itree->Branch("Qy_rec_south", &qy_rec_south);
    psirb_south = itree->Branch("psi_rec_south", &psi_rec_south);

    overwriteBranch(itree, "Qx_rec_north");
    overwriteBranch(itree, "Qy_rec_north");
    overwriteBranch(itree, "psi_rec_north");
	  
    qxrb_north = itree->Branch("Qx_rec_north", &qx_rec_north);
    qyrb_north = itree->Branch("Qy_rec_north", &qy_rec_north);
    psirb_north = itree->Branch("psi_rec_north", &psi_rec_north);

    newHists();
    //newProfs();

    // Fill all Qvector, NClus, Psi, QxQyProf
    int Entries = itree->GetEntries();

    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	if (nclus != 0){
	  //nclush->Fill(nclus);
        allpsi->Fill(psi);
        allqvec->Fill(qx, qy);
	}
	
	if (nclus_south != 0){
	  //nclush_south->Fill(nclus_south);
        allpsi_south->Fill(psi_south);
        allqvec_south->Fill(qx_south, qy_south);
	}
	
	if (nclus_north != 0){
	  //nclush_north->Fill(nclus_north);
        allpsi_north->Fill(psi_north);
        allqvec_north->Fill(qx_north, qy_north);
	}
	/*
        if (isfinite(qx) == true)
        {
            qxprof[0]->Fill(nclus / nmax * 100, qx);
            qyprof[0]->Fill(nclus / nmax * 100, qy);

            qxprof[1]->Fill(nclus, qx);
            qyprof[1]->Fill(nclus, qy);
        }
	*/
    }

    /*
    // set_quantile(yq);
    for (Int_t i = 0; i < nq; i++)
    {
        xq[i] = Float_t(i + 1) / nq;
        cout << xq[i] << endl;
        nclush->GetQuantiles(nq, yq, xq);
        cout << "yq" << i << "= " << yq[i] << endl;
    }
    */

    // recentering qx,qy using all event
    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	 if (nclus == 0)
        {
            qx_rec = std::nan("");
            qy_rec = std::nan("");
            psi_rec = std::nan("");
	    
	    qxrb->Fill();
	    qyrb->Fill();
	    psirb->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }

	 if (nclus_south == 0)
        {
            qx_rec_south = std::nan("");
            qy_rec_south = std::nan("");
            psi_rec_south = std::nan("");
	    
	    qxrb_south->Fill();
	    qyrb_south->Fill();
	    psirb_south->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }

	 if (nclus_north == 0)
        {
            qx_rec_north = std::nan("");
            qy_rec_north = std::nan("");
            psi_rec_north = std::nan("");
	    
	    qxrb_north->Fill();
	    qyrb_north->Fill();
	    psirb_north->Fill();	    
	    
	    //if(i<100)
	    //cout<<"Event "<<i<<" is no entry"<<endl;
	    //itree->Fill();	    
	    //continue;
        }


	 if(nclus!=0)
	   {
	     cal_rec(allqvec, allqvec_rec, allpsi_rec);
	     qxrb->Fill();
	     qyrb->Fill();
	     psirb->Fill();
	   }
	 
	 if(nclus_south!=0)
	   {
	     cal_rec_south(allqvec_south, allqvec_rec_south, allpsi_rec_south);
	     qxrb_south->Fill();
	     qyrb_south->Fill();
	     psirb_south->Fill();
	   }

	 if(nclus_north!=0)
	   {
	     cal_rec_north(allqvec_north, allqvec_rec_north, allpsi_rec_north);
	     qxrb_north->Fill();
	     qyrb_north->Fill();
	     psirb_north->Fill();
	   }
    }

    ClearHists();
}

void ClearHists()
{
  delete nclush;
  delete allqvec;
  delete allpsi;

  delete nclush_south;
  delete allqvec_south;
  delete allpsi_south;

  delete nclush_north;
  delete allqvec_north;
  delete allpsi_north;

  delete allqvec_rec;
  delete allpsi_rec;
  
  delete allqvec_rec_south;
  delete allpsi_rec_south;
  
  delete allqvec_rec_north;
  delete allpsi_rec_north;
}

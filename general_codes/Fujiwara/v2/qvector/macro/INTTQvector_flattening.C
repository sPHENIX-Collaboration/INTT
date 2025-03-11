#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
#include <iostream>
#include <string>
#include <numeric>
#include <TCanvas.h>
#include <TF1.h>

using namespace std;


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
// R__LOAD_LIBRARY(libinttqvector.so)
//R__LOAD_LIBRARY(libinttread.so)

const Int_t nq = 20;
TFile *ifile;
TTree *itree;

TFile *outputFile;
string outputFilename;

double psi_rec,psi_rec_south,psi_rec_north;
double psi_flat,psi_flat_south,psi_flat_north;
//float zvtx;
/*
TH1F *allpsi_flat;
TH1F *allpsi_flat_south;
TH1F *allpsi_flat_north;
*/
//int flat_coef = 8;

TProfile *cosprof;
TProfile *sinprof;

TProfile *cosprof_south;
TProfile *sinprof_south;

TProfile *cosprof_north;
TProfile *sinprof_north;

void overwriteBranch(TTree *tree, const char *branchName);
void setBranches();
void flattening(TTree*itree, int flat_coef);

void CleanProf();

double cal_sin2k_psi(int k,double psi_rec);
double cal_cos2k_psi(int k,double psi_rec);
double cal_sin2k_psi_avg(int k,double psi_rec);
double cal_cos2k_psi_avg(int k,double psi_rec);

//mode 0 = all event, 1 = south, 2=north

void INTTQvector_flattening(//string fname = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root"
			     //string fname = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_zvtxfiltered.root"
			     //string fname ="Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Surveyzvtx_-40_-35.root"
			     //string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_recented.root"
string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
//"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root"
			     //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_100k.root"
			     , int flat_coef = 8 //int mode = 2
)
{
    // read ttree
    ifile = new TFile(fname.c_str(), "UPDATE");
    itree = ifile->Get<TTree>("EventTree");
    itree->SetBranchStatus("*", 0);

    flattening(itree,flat_coef);

    itree->SetBranchStatus("*", 1);
    itree->Write("EventTree", TObject::kOverwrite);
    delete itree;

    for(int i=0;i<nq;i++)
    {
      itree = (TTree *)ifile->Get(Form("EventTree_%d", i));
      cout<<"EventTree_"<<i<<endl;
      itree->SetBranchStatus("*", 0);

      flattening(itree,flat_coef);

      itree->SetBranchStatus("*", 1);
      itree->Write(Form("EventTree_%d", i), TObject::kOverwrite);
      delete itree;
    }
    

    /*
    // new outputfile for histgram reacord
    outputFilename = fname;
    // erase .root and set name for save hist
    outputFilename.erase(outputFilename.end() - 5, outputFilename.end());
    outputFilename = outputFilename + "_histlist.root";

    outputFile = new TFile(outputFilename.c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie())
    {
        printf("Error: cannot open output file %s\n", outputFilename.c_str());
        return;
    }
    */

    /*
    setBranches();
    TBranch *psifb;
    TBranch *psifb_south;
    TBranch *psifb_north;

    overwriteBranch(itree, "psi_flat");
    psifb = itree->Branch("psi_flat", &psi_flat);
    
    overwriteBranch(itree, "psi_flat_south");
    psifb_south = itree->Branch("psi_flat_south", &psi_flat_south);
    
    overwriteBranch(itree, "psi_flat_north");
    psifb_north = itree->Branch("psi_flat_north", &psi_flat_north);
    */
    
    /*
    switch(mode)
      {
      case 0:
	{
	  overwriteBranch(itree, "psi_flat");
	  psifb = itree->Branch("psi_flat", &psi_flat);
	  cout<<"Calculate flattening All event"<<endl;
	  break;
	}
      case 1:
	{
	  overwriteBranch(itree, "psi_flat_south");
	  psifb = itree->Branch("psi_flat_south", &psi_flat);
	  cout<<"Calculate flattening south INTT"<<endl;
	  break;
	}
      case 2: 
	{
	  overwriteBranch(itree, "psi_flat_north");
	  psifb = itree->Branch("psi_flat_north", &psi_flat);
	  cout<<"Calculate flattening north INTT"<<endl;
	  break;
	}
      }
    */
      
    // set hist for flattening psi
    /*
    double psibinwid = 0.02;
    double psimax = ceil(itree->GetMaximum("Intt_Psi"));
    double psimin = floor(itree->GetMinimum("Intt_Psi"));
    int psibin = (psimax + abs(psimin)) / psibinwid;
    allpsi_flat = new TH1F("allpsi_flat", "all psi", 400, -2, 2);
    allpsi_flat_south = new TH1F("allpsi_flat_south", "all psi", 400, -2, 2);
    allpsi_flat_north = new TH1F("allpsi_flat_north", "all psi", 400, -2, 2);
    TH1F *allpsi_rec = (TH1F *)allpsi_flat->Clone("allpsi_rec");
    TH1F *allpsi_rec_south = (TH1F *)allpsi_flat->Clone("allpsi_rec_south");
    TH1F *allpsi_rec_north = (TH1F *)allpsi_flat->Clone("allpsi_rec_north");
    */

    /*
    TH1F *diff_psi = new TH1F("delta_psi", "delta psi", 40, -0.2, 0.2);
    TH2S *psi_rec_delta_psi = new TH2S("psi_recvsdeltapsi","psi_rec vs delta psi",400,-2,2,40,-0.2,0.2);
    TH2S *psi_rec_psi_flat = new TH2S("psi_recvspsi_flat","psi_rec vs psi_flat",400,-2,2,400,-2,2);
    TH2S *psi_flat_delta_psi = new TH2S("psi_flatvsdeltapsi","psi_flat vs delta psi",400,-2,2,40,-0.2,0.2);
    */

    //TH1F *zvtxdis= new TH1F("zvtxdis","zvtxdis",200,-50,50);

    /*
    double delta_psi;
    vector<double> avg_cos; //<cos(2*k*psi_rec)>
    vector<double> avg_sin; //<sin(2*k*psi_rec)>

    double delta_psi_south;
    vector<double> avg_cos_south; //<cos(2*k*psi_rec)>
    vector<double> avg_sin_south; //<sin(2*k*psi_rec)>
    
    double delta_psi_north;
    vector<double> avg_cos_north; //<cos(2*k*psi_rec)>
    vector<double> avg_sin_north; //<sin(2*k*psi_rec)>

    Long64_t nEvt = itree->GetEntries();
    // cout << "nEvt = " << nEvt << endl;
    cosprof = new TProfile("cosavg", "", flat_coef, 0, flat_coef);
    sinprof = new TProfile("sinavg", "", flat_coef, 0, flat_coef);
    
    cosprof_south = new TProfile("cosavg_south", "", flat_coef, 0, flat_coef);
    sinprof_south = new TProfile("sinavg_south", "", flat_coef, 0, flat_coef);

    cosprof_north = new TProfile("cosavg_north", "", flat_coef, 0, flat_coef);
    sinprof_north = new TProfile("sinavg_north", "", flat_coef, 0, flat_coef);
    */
    
    // calculate <cos(2*k*psi_rec)> and <sin(2*k*psi_rec)>
    /*
    for (int k = 1; k < flat_coef; k++)
    {
      avg_cos.push_back(cal_cos2k_psi_avg(k,psi_rec));
      avg_cos_south.push_back(cal_cos2k_psi_avg(k,psi_rec_south));
      avg_cos_north.push_back(cal_cos2k_psi_avg(k,psi_rec_north));
      cout << "<cos(2" << k << "psi)> = " << cal_cos2k_psi_avg(k,psi_rec) << endl;
      avg_sin.push_back(cal_sin2k_psi_avg(k,psi_rec));
      avg_sin_south.push_back(cal_sin2k_psi_avg(k,psi_rec_south));
      avg_sin_north.push_back(cal_sin2k_psi_avg(k,psi_rec_north));
      //cout << "<sin(2" << k << "psi)> = " << cal_sin2k_psi_avg(k) << endl;
    }
    */
    
    /*
    for (int k = 1; k < flat_coef; k++)
    {
        for (int i = 0; i < nEvt; i++)
        {
            itree->GetEntry(i);
            if (isnan(psi_rec))
                continue;
            cosprof->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec));
	    sinprof->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec));
	    
	    if(!isnan(psi_rec_south))
	      {
		cosprof_south->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_south));
		sinprof_south->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_south));
	      }
	    
	    if(!isnan(psi_rec_north))
	      {
		cosprof_north->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_north));
		sinprof_north->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_north));
	      }
        }
    }
    */

    /*
    for (int bin = 1; bin < flat_coef; ++bin)
    {
        double mean = cosprof_south->GetBinContent(bin);   // 各ビンの平均値を取得
        double xCenter = cosprof_south->GetBinCenter(bin); // ビンの中心のX値を取得
	std::cout << "South Bin " << bin << " (X = " << xCenter << "): Mean = " << mean << std::endl;
    }
    
    for (int bin = 1; bin < flat_coef; ++bin)
    {
        double mean = cosprof_north->GetBinContent(bin);   // 各ビンの平均値を取得
        double xCenter = cosprof_north->GetBinCenter(bin); // ビンの中心のX値を取得
	std::cout << "North Bin " << bin << " (X = " << xCenter << "): Mean = " << mean << std::endl;
    }
    */
    /*
    // calculate delta_psi and psi_flat
    for (int i = 0; i < nEvt; i++)
    {
        itree->GetEntry(i);

        delta_psi = 0;
        psi_flat = 0;

	delta_psi_south = 0;
        psi_flat_south = 0;

	delta_psi_north = 0;
        psi_flat_north = 0;

        if (isnan(psi_rec)){
	  psi_flat=std::nan("");
	  psi_flat_south=std::nan("");
	  psi_flat_north=std::nan("");
	  //cout<<"Event "<<i<<" ,psi_rec is nan"<<endl;
	  psifb->Fill();
	  psifb_south->Fill();
	  psifb_north->Fill();
	  continue;
	}

        for (int k = 1; k < flat_coef; k++)
        {
            // delta_psi = delta_psi - (2 / k * avg_sin[k - 1] * cal_cos2k_psi(k)) + (2 / k * avg_cos[k - 1] * cal_sin2k_psi(k));

            // delta_psi -= 2 / k * (avg_cos[k - 1] * cal_sin2k_psi(k) - avg_sin[k - 1] * cal_cos2k_psi(k));
	  //double cossin = avg_cos[k - 1] * cal_sin2k_psi(k,psi_rec);
	  //double sincos = avg_sin[k - 1] * cal_cos2k_psi(k,psi_rec); 
	  double cossin = cosprof->GetBinContent(k) * cal_sin2k_psi(k,psi_rec);
	  double sincos = sinprof->GetBinContent(k) * cal_cos2k_psi(k,psi_rec); 
	  delta_psi += 2 * (cossin - sincos) / k;

	  if(isnan(psi_rec_south))
	    {
	      psi_flat_south=std::nan("");
	      psifb_south->Fill();
	    }
	  else
	    {
	      double cossin_south = cosprof_south->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_south);
	      double sincos_south = sinprof_south->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_south);
	      delta_psi_south += 2 * (cossin_south - sincos_south) / k;
	    }

	  if(isnan(psi_rec_north))
	    {
	      psi_flat_north=std::nan("");
	      psifb_north->Fill();
	    }
	  else
	    {
	      double cossin_north = cosprof_north->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_north);
	      double sincos_north = sinprof_north->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_north);
	      delta_psi_north += 2 * (cossin_north - sincos_north) / k;
	    }

	  //delta_psi += 2 * (cossin - sincos) / k;
	  //delta_psi_south += 2 * (cossin_south - sincos_south) / k;
	  //delta_psi_north += 2 * (cossin_north - sincos_north) / k;

        }

        psi_flat = psi_rec + delta_psi/2;
	if(!isnan(psi_rec_south))
	  {
	    psi_flat_south = psi_rec_south + delta_psi_south/2;
	    psifb_south->Fill();
	  }
	if(!isnan(psi_rec_north))
	  {
	    psi_flat_north = psi_rec_north + delta_psi_north/2;
	    psifb_north->Fill();
	  }
    */
	/*
	if(i<5)
	  {
	    cout<<"psi rec ="<<psi_rec<<endl;
	    cout<<"psi flat = "<<psi_flat<<endl;
	    cout<<"psi rec south="<<psi_rec_south<<endl;
	    cout<<"psi flat south = "<<psi_flat_south<<endl;
	    cout<<"psi rec north="<<psi_rec_north<<endl;
	    cout<<"psi flat north = "<<psi_flat_north<<endl;
	  }
	*/

	/*
        allpsi_flat->Fill(psi_flat);
	allpsi_flat_south->Fill(psi_flat_south);
	allpsi_flat_north->Fill(psi_flat_north);
	*/
	/*
        allpsi_rec->Fill(psi_rec);
	diff_psi->Fill(delta_psi/2);
	psi_rec_delta_psi->Fill(psi_rec,delta_psi/2);
	psi_flat_delta_psi->Fill(psi_flat,delta_psi/2);
	psi_rec_psi_flat->Fill(psi_rec,psi_flat);
	*/
	//zvtxdis->Fill(zvtx);
	
	/*
        if (i < 10)
            cout << "psi_flat =" << psi_flat << endl;
        if (i < 10)
            cout << "psi_rec =" << psi_rec << endl<< endl;
	*/
        //psifb->Fill();
	//psifb_south->Fill();
	//psifb_north->Fill();
    //}
    
    /*
    allpsi_flat->Draw();
    // 直線フィット (pol1)
    TF1 *fitFunc = new TF1("fitFunc", "pol0", -2, 2); // pol1 は直線

    // フィット実行
    allpsi_flat->Fit(fitFunc, "QN"); // Qオプションはフィットのメッセージを抑制
    double ndf = fitFunc->GetNDF();

    // chi-squared を取得
    double chi2 = fitFunc->GetChisquare();
    cout << "k = " << flat_coef << endl;
    cout << "chi2 flat = " << chi2 << endl;
    std::cout << "Chi-squared/NDF: " << chi2/ndf << std::endl;
    double param0 = fitFunc->GetParameter(0); // 定数パラメータ
    double param0_error = fitFunc->GetParError(0); // 定数パラメータのエラー
    std::cout << "Parameter 0 (constant): " << param0 << " ± " << param0_error << std::endl;
    fitFunc->SetLineColor(kBlue);
    fitFunc->SetLineStyle(2);
    
    // 直線フィット (pol1)
    TF1 *fitFunc1 = new TF1("fitFunc1", "pol0", -2, 2); // pol1 は直線

    // フィット実行
    allpsi_rec->Fit(fitFunc1, "QN"); // Qオプションはフィットのメッセージを抑制

    // chi-squared を取得
    double chi2_rec = fitFunc1->GetChisquare();
    cout << "chi2 rec = " << chi2_rec << endl;
    fitFunc1->SetLineColor(kRed);
    fitFunc1->SetLineStyle(2);
    double ndf_rec = fitFunc->GetNDF();
    std::cout << "Chi-squared/NDF_rec: " << chi2_rec/ndf_rec << std::endl;    
    */
    /*
    // save histogram to root file
    TCanvas *c1 = new TCanvas("", "");
    
    allpsi_flat->SetLineColor(kBlue);
    allpsi_rec->SetLineColor(kRed);
    allpsi_flat->SetLineWidth(2);
    allpsi_rec->SetLineWidth(2);
    allpsi_flat->Draw();
    allpsi_rec->Draw("same");
    fitFunc->Draw("same");
    fitFunc1->Draw("same");
    
    //diff_psi->Draw();
    //psi_rec_delta_psi->Draw("COLZ");
    //psi_rec_psi_flat->Draw("COLZ");
    TCanvas *c2 = new TCanvas("", "");
    //psi_rec_psi_flat->Draw("COLZ");
    //diff_psi->Draw();
    //psi_rec_delta_psi->Draw("COLZ");
    //c1->SaveAs("psi_rec_flat.pdf");
    zvtxdis->Draw();
    */
    // 現在のディレクトリのオブジェクトリストを取得
    /*
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
            //obj->Write();
            //printf("Saved: %s\n", obj->GetName());
        }
    }
    */
    
    //ifile->cd();
        
    //itree->SetBranchStatus("*", 1);
    //itree->Write("", TObject::kOverwrite);    
    ifile->Close();
    
}

void setBranches()
{
    itree->SetBranchStatus("*", 0);
    //itree->SetBranchStatus("MBD_z_vtx", 1);
    //itree->SetBranchAddress("MBD_z_vtx", &zvtx);
    
    /*
    overwriteBranch(itree, "psi_flat");
    psifb = itree->Branch("psi_flat", &psi_flat);
    
    overwriteBranch(itree, "psi_flat_south");
    psifb_south = itree->Branch("psi_flat_south", &psi_flat_south);
    
    overwriteBranch(itree, "psi_flat_north");
    psifb_north = itree->Branch("psi_flat_north", &psi_flat_north);
    */
    
    itree->SetBranchStatus("psi_rec", 1);
    itree->SetBranchAddress("psi_rec", &psi_rec);
    
    itree->SetBranchStatus("psi_rec_south", 1);
    itree->SetBranchAddress("psi_rec_south", &psi_rec_south);
    
    itree->SetBranchStatus("psi_rec_north", 1);
    itree->SetBranchAddress("psi_rec_north", &psi_rec_north);
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

double cal_sin2k_psi(int k,double psi_rec)
{
    // if(isnan(psi_rec)) return;
    // double sin2k_psi = sin(2 * k * psi_rec);
    return sin(2 * k * psi_rec);
}

double cal_cos2k_psi(int k,double psi_rec)
{
    // if(isnan(psi_rec)) return;
    // double cos2k_psi = cos(2 * k * psi_rec);
    return cos(2 * k * psi_rec);
}

double cal_sin2k_psi_avg(int k,double psi_rec)
{
    vector<double> v;
    Long64_t nEvt = itree->GetEntries();

    for (int i; i < nEvt; i++)
    {
        itree->GetEntry(i);
        if (isnan(psi_rec))
            continue;
        // double sin2k_psi = cal_sin2k_psi(k);
        v.push_back(cal_sin2k_psi(k,psi_rec));
    }
    // cout<<"v size "<<v.size()<<endl;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double cal_cos2k_psi_avg(int k,double psi_rec)
{
    vector<double> v;
    Long64_t nEvt = itree->GetEntries();

    for (int i; i < nEvt; i++)
    {
        itree->GetEntry(i);
        if (isnan(psi_rec)){
	  //cout<<"Event "<<i<<" ,psi_rec is nan"<<endl;
            continue;
	}
        // double cos2k_psi = cal_cos2k_psi(k);
        v.push_back(cal_cos2k_psi(k,psi_rec));
    }

    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

void flattening(TTree*itree, int flat_coef)
{
  setBranches();
  TBranch *psifb;
  TBranch *psifb_south;
  TBranch *psifb_north;
  
  overwriteBranch(itree, "psi_flat");
  psifb = itree->Branch("psi_flat", &psi_flat);
  
  overwriteBranch(itree, "psi_flat_south");
  psifb_south = itree->Branch("psi_flat_south", &psi_flat_south);
  
  overwriteBranch(itree, "psi_flat_north");
  psifb_north = itree->Branch("psi_flat_north", &psi_flat_north);

  double delta_psi;
  double delta_psi_south;
  double delta_psi_north;
  
  Long64_t nEvt = itree->GetEntries();
  // cout << "nEvt = " << nEvt << endl;
  cosprof = new TProfile("cosavg", "", flat_coef, 0, flat_coef);
  sinprof = new TProfile("sinavg", "", flat_coef, 0, flat_coef);
  
  cosprof_south = new TProfile("cosavg_south", "", flat_coef, 0, flat_coef);
  sinprof_south = new TProfile("sinavg_south", "", flat_coef, 0, flat_coef);
  
  cosprof_north = new TProfile("cosavg_north", "", flat_coef, 0, flat_coef);
  sinprof_north = new TProfile("sinavg_north", "", flat_coef, 0, flat_coef);

  
  for (int k = 1; k < flat_coef; k++)
    {
      for (int i = 0; i < nEvt; i++)
        {
	  itree->GetEntry(i);
	  if (isnan(psi_rec))
	    continue;
	  cosprof->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec));
	  sinprof->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec));
	  
	  if(!isnan(psi_rec_south))
	    {
	      cosprof_south->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_south));
	      sinprof_south->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_south));
	    }
	  
	  if(!isnan(psi_rec_north))
	    {
		cosprof_north->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_north));
		sinprof_north->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_north));
	    }
        }
    }
  
     // calculate delta_psi and psi_flat
    for (int i = 0; i < nEvt; i++)
    {
        itree->GetEntry(i);

        delta_psi = 0;
        psi_flat = 0;

	delta_psi_south = 0;
        psi_flat_south = 0;

	delta_psi_north = 0;
        psi_flat_north = 0;

        if (isnan(psi_rec)){
	  psi_flat=std::nan("");
	  psi_flat_south=std::nan("");
	  psi_flat_north=std::nan("");
	  //cout<<"Event "<<i<<" ,psi_rec is nan"<<endl;
	  psifb->Fill();
	  psifb_south->Fill();
	  psifb_north->Fill();
	  continue;
	}

        for (int k = 1; k < flat_coef; k++)
        {
  	  double cossin = cosprof->GetBinContent(k) * cal_sin2k_psi(k,psi_rec);
	  double sincos = sinprof->GetBinContent(k) * cal_cos2k_psi(k,psi_rec); 
	  delta_psi += 2 * (cossin - sincos) / k;

	  double cossin_south = cosprof_south->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_south);
	  double sincos_south = sinprof_south->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_south);
	  delta_psi_south += 2 * (cossin_south - sincos_south) / k;
		 
	  double cossin_north = cosprof_north->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_north);
	  double sincos_north = sinprof_north->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_north);
	  delta_psi_north += 2 * (cossin_north - sincos_north) / k;
	  
	}

	psi_flat = psi_rec + delta_psi/2;

	if(isnan(psi_rec_south))
	  {
	    psi_flat_south=std::nan("");
	    psifb_south->Fill();
	  }
	else	  
	  {
	    psi_flat_south = psi_rec_south + delta_psi_south/2;
	    psifb_south->Fill();
	  }
	
	if(isnan(psi_rec_north))
	  {
	    psi_flat_north=std::nan("");
	    psifb_north->Fill();
	  }
	else
	  {
	    psi_flat_north = psi_rec_north + delta_psi_north/2;
	    psifb_north->Fill();
	  }
	
        psifb->Fill();

	if(i<5)
	  {
	    cout<<"psi flat = "<<psi_flat<<endl;
	    cout<<"psi flat south = "<<psi_flat_south<<endl;
	    cout<<"psi flat north = "<<psi_flat_north<<endl;
	  }
    }
    CleanProf();
}

void CleanProf()
{
  delete cosprof;
  delete sinprof;

  delete cosprof_south;
  delete sinprof_south;

  delete cosprof_north;
  delete sinprof_north;
}

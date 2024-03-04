#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>

void setBranches();
// input th2s, output th2s, th1f
void cal_rec(TH2S*allqvec,TH2S*allqvec_rec,TH1F*allpsi_rec);
void set_quantile(double yq[]);
//void make_hist_quantiled();

TFile *ifile;
TFile *ofile;

double nclus;
double qx;
double qy;
double psi;

double qx_rec;
double qy_rec;
double psi_rec;
int class_multi;

TTree *itree;
TTree *otree;

TH1F *nclush;
TH2S *allqvec;
TH1F* allpsi;

void INTTQvector_recentering
(string fname ="rootfile/InttQvector20869_20240126_adc.root")
{
  //string fname ="rootfile/InttQvector20869_20240115_adc.root";
  const Int_t nq = 20;
  bool pdfout = true;

  string pdfname = fname;
  //erase .root
  pdfname.erase(pdfname.end() - 5, pdfname.end());

  //remove rootfile/
  pdfname.erase(pdfname.begin(), pdfname.begin() + 9);
  
  pdfname = "pdffile/" + pdfname + "_rec.pdf";
  //string pdfname = "pdffile/"+fname.substr(9,18)+to_string(nq)+"hotdead_adccut_recenter.pdf";

  string ofname = fname;
  ofname.erase(ofname.end() -5, ofname.end());
  ofname = ofname + "_rec.root";

  ifile = new TFile(fname.c_str(),"READ");
  ofile = new TFile(ofname.c_str(),"RECREATE");
    
  itree = ifile->Get<TTree>("Qvectortree");

  otree = itree->CloneTree(0);

  TBranch* qxrb = otree->Branch("Qx_rec",&qx_rec);
  TBranch* qyrb = otree->Branch("Qy_rec",&qy_rec);
  TBranch* psirb = otree->Branch("psi_rec",&psi_rec);
  TBranch* cmb = otree->Branch("multiplicity_class",&class_multi);

  setBranches();
 
  double nbinwid = 10;
  double qxbinwid = 0.01;
  double qybinwid = 0.01;
  double psibinwid = 0.02;

  int nmax = itree->GetMaximum("nclus");
  cout<<"nmax ="<<nmax<<endl;

  int qxmax = ceil(itree->GetMaximum("Qx"));
  int qxmin = floor(itree->GetMinimum("Qx"));
  cout<<"qx range = "<<qxmax<<"  "<<qxmin<<endl;

  int qymax = ceil(itree->GetMaximum("Qy"));
  int qymin = floor(itree->GetMinimum("Qy"));
  cout<<"qy range = "<<qymax<<"  "<<qymin<<endl;

  int psimax = ceil(itree->GetMaximum("psi"));
  int psimin = floor(itree->GetMinimum("psi"));
  cout<<"psi range = "<<psimax<<"  "<<psimin<<endl;

  int qxbin = (qxmax + abs(qxmin))/qxbinwid;
  int qybin = (qymax + abs(qymin))/qybinwid;
  int psibin = (psimax + abs(psimin))/psibinwid;
  cout<<"qx qy psibin "<<qxbin<<"  "<<qybin<<"  "<<psibin<<endl;
  

  if(qxbin != qybin)
    {
      if(qxbin > qybin)
	{
	  qymax = qxmax;
	  qymin = qxmin;
	  qybin = qxbin;
	}
      else 
	{
	  qxmax = qymax;
	  qxmin = qymin;
	  qxbin = qybin;
	}
    }

  nclush = new TH1F("nclus","multiplicity",nmax/nbinwid,0,nmax);
  allqvec = new TH2S("allqvec","all qvector",qxbin,qxmin,qxmax,qybin,qymin,qymax);
  allpsi = new TH1F("allpsi","all psi",psibin,psimin,psimax);

  TH2S *allqvec_rec = new TH2S("allqvec_rec","all qvector recentering",qxbin,-10,10,qybin,-10,10);
  TH1F* allpsi_rec = (TH1F *)allpsi->Clone("allpsi_rec");
 
   for(int i =0;i<itree->GetEntries();i++)
    {
      itree->GetEntry(i);

      //otree->Fill();

      nclush->Fill(nclus);
      allpsi->Fill(psi);
      allqvec->Fill(qx,qy);
    }
   
   for(int i =0;i<itree->GetEntries();i++)
     {
       itree->GetEntry(i);
       
       cal_rec(allqvec,allqvec_rec,allpsi_rec);

     }

   Double_t xq[nq]; 
   Double_t yq[nq];

   //set_quantile(yq);

   for (Int_t i=0;i<nq;i++) 
    {
      xq[i] = Float_t(i+1)/nq;
      cout<<xq[i]<<endl;
      nclush->GetQuantiles(nq,yq,xq);
      cout<<"yq "<<yq[i]<<endl;
    }

   TH2S * qvector[nq];
   TH1F * psidis[nq];
   TH1F * nclusdis[nq];

   //set hists names   
   for(int i=0;i<nq;i++)
     {
       double rangemin = (double)i/(double)nq*100.0;
       qvector[i] = new TH2S(Form("qvector%d",i),Form("qvector %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),qxbin,qxmin,qxmax,qybin,qymin,qymax);
       psidis[i] = new TH1F(Form("psi%d",i),Form("psi %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),psibin,psimin,psimax);
       nclusdis[i] = new TH1F(Form("nclus%d",i),Form("multiplicity %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),nmax/10,0,nmax);
     }
     

   //fill quantiled hists
   int minrange =0;
   for(int i =0;i<itree->GetEntries();i++)
     {
       itree->GetEntry(i);
       
       for(int j =0;j<nq;j++)
	 {
	   if(j==0)
	     { 
	       minrange =0;
	     }else
	     {
	       minrange = yq[j-1];
	     }
	   
	   if(minrange<nclus && nclus<=yq[j])
	    {
	      qvector[j]->Fill(qx,qy);
	      //cout<<"qx qy "<<qx<<qy<<endl;
	      psidis[j]->Fill(psi);
	      nclusdis[j]->Fill(nclus);
	    }
	}
     }
   
   TH2S *qvector_rec[nq];
   TH1F * psidis_rec[nq];

   //set hists names recentering
   for(int i=0;i<nq;i++)
     {
       double rangemin = (double)i/(double)nq*100.0;
       qvector_rec[i] = new TH2S(Form("qvector_rec%d",i),"qvector recentering",qxbin,-10,10,qybin,-10,10);
       psidis_rec[i] = new TH1F(Form("psi_rec%d",i),"psi recentering",psibin,psimin,psimax);
     }
    
   minrange =0;
   int count=0;
   for(int i =0;i<itree->GetEntries();i++)
     {
       itree->GetEntry(i);
       //otree->Fill();
       if(nclus == 0)
	 {
	   qx_rec = std::nan("");
	   qy_rec = std::nan("");
	   psi_rec = std::nan("");
	   class_multi = std::nan("");

	   /*
	   qxrb->Fill();
	   qyrb->Fill();
	   psirb->Fill();
	   cmb->Fill();
	   */
	   otree->Fill();
	   continue;
	 }
       
       for(int j =0;j<nq;j++)
	 {
	   if(j==0) minrange =0;
	   else minrange = yq[j-1];
	   
	   if(minrange<nclus && nclus<=yq[j])
	    {
	      cal_rec(qvector[j],qvector_rec[j],psidis_rec[j]);
	      class_multi = j;
	      //addtree->Fill();
	      /*
	      qxrb->Fill();
	      qyrb->Fill();
	      psirb->Fill();
	      cmb->Fill();
	      */
	      otree->Fill();
	      //if(i==1)cout<<"qxrec "<<qx_rec<<" qyrec "<<qy_rec<<" psirec "<<psi_rec<<endl;
	    }
	}
     }

   
   
   TCanvas *c1[nq+1];

   for(int i=0;i<nq;i++)
    {
      c1[i]=new TCanvas(Form("canvas%d",i),Form("canvas%d",i),1800,600);
      c1[i]->Divide(3,1);
      c1[i]->cd(1);
      qvector_rec[i]->Draw("COLZ");
      c1[i]->cd(2);
      psidis_rec[i]->SetLineColor(kRed);
      psidis[i]->Rebin(4)->Draw();
      psidis_rec[i]->Rebin(4)->Draw("same");
      c1[i]->cd(3);
      gPad->SetLogy(1);
      nclusdis[i]->Draw();
      if(pdfout == true){
	if(i==0) c1[i]->Print((pdfname+"(").c_str());
	else c1[i]->Print(pdfname.c_str());
      }
    }

   c1[nq]=new TCanvas(Form("canvas%d",nq),"canvas all",1800,600);
   c1[nq]->Divide(3,1);
   c1[nq]->cd(1);
   allqvec_rec->Draw("COLZ");
   c1[nq]->cd(2);
   allpsi_rec->SetLineColor(kRed);
   allpsi->Rebin(4)->Draw();
   allpsi_rec->Rebin(4)->Draw("same");
   c1[nq]->cd(3);
   gPad->SetLogy(1);
   nclush->Draw();
   if(pdfout==true) c1[nq]->Print((pdfname+")").c_str());

   //itree->Write("tree",TObject::kOverwrite);
   otree->Print();
   otree->Write("Qvectortree",TObject::kOverwrite);
   //ofile->Write();
   //file->Close();

   //addtree->GetEntries();
   //addtree->Print();

   //addtree->AddFriend("tree",fname.c_str());
   //addtree->Print();
   //itree->AddFriend("RecQvectortree");
   //itree->Print();
   //itree->Write();
   //addtree->Write();
   //fout->Close();
}

void setBranches()
{
  itree->SetBranchAddress("nclus",&nclus);
  itree->SetBranchAddress("Qx",&qx);
  itree->SetBranchAddress("Qy",&qy);
  itree->SetBranchAddress("psi",&psi);
}

void cal_rec(TH2S*allqvec,TH2S*allqvec_rec,TH1F*allpsi_rec)
{
  qx_rec = (qx - allqvec->GetMean(1))/allqvec->GetStdDev(1);
  qy_rec = (qy - allqvec->GetMean(2))/allqvec->GetStdDev(2);

  allqvec_rec->Fill(qx_rec,qy_rec);

  psi_rec = 0.5*atan2(qx_rec,qy_rec);

  allpsi_rec->Fill(psi_rec);
  //cout<<"qxrec "<<qx_rec<<" qyrec "<<qy_rec<<" psirec "<<psi_rec<<endl;
}

/*
void set_quantile(double yq[])
{
  double xq[nq];

  for (Int_t i=0;i<nq;i++) 
    {
      xq[i] = Float_t(i+1)/nq;
      cout<<xq[i]<<endl;
      nclush->GetQuantiles(nq,yq,xq);
      cout<<"yq "<<yq[i]<<endl;
    }
}
*/

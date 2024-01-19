#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>

void read_tree(string fname);
// input th2s, output th2s, th1f
void cal_rec(TH2S*allqvec,TH2S*allqvec_rec,TH1F*allpsi_rec);
void set_quantile(double yq[]);
//void make_hist_quantiled();

double nclus;
double qx;
double qy;
double psi;

TTree *tree_;

TH1F *nclush;
TH2S *allqvec;
TH1F* allpsi;

void INTTQvector_recentering()
{
  string fname ="rootfile/InttQvector_adccut.root";
  const Int_t nq = 20;

  string pdfname = "pdffile/"+fname.substr(9,18)+to_string(nq)+"quantile_recenter.pdf";

  read_tree(fname);

  double nbinwid = 10;
  double qxbinwid = 0.01;
  double qybinwid = 0.01;
  double psibinwid = 0.02;

  int nmax = tree_->GetMaximum("nclus");
  cout<<"nmax ="<<nmax<<endl;

  int qxmax = ceil(tree_->GetMaximum("Qx"));
  int qxmin = floor(tree_->GetMinimum("Qx"));
  cout<<"qx range = "<<qxmax<<"  "<<qxmin<<endl;

  int qymax = ceil(tree_->GetMaximum("Qy"));
  int qymin = floor(tree_->GetMinimum("Qy"));
  cout<<"qy range = "<<qymax<<"  "<<qymin<<endl;

  int psimax = ceil(tree_->GetMaximum("psi"));
  int psimin = floor(tree_->GetMinimum("psi"));
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
 
   for(int i =0;i<tree_->GetEntries();i++)
    {
      tree_->GetEntry(i);

      nclush->Fill(nclus);
      allpsi->Fill(psi);
      allqvec->Fill(qx,qy);
    }
   
   for(int i =0;i<tree_->GetEntries();i++)
     {
       tree_->GetEntry(i);
       
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

   for(int i=0;i<nq;i++)
     {
       double rangemin = (double)i/(double)nq*100.0;
       qvector[i] = new TH2S(Form("qvector%d",i),Form("qvector %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),qxbin,qxmin,qxmax,qybin,qymin,qymax);
       psidis[i] = new TH1F(Form("psi%d",i),Form("psi %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),psibin,psimin,psimax);
       nclusdis[i] = new TH1F(Form("nclus%d",i),Form("multiplicity %d-%d",100-(int)(xq[i]*100),100-(int)rangemin),nmax/10,0,nmax);
     }
     
   int minrange =0;
   for(int i =0;i<tree_->GetEntries();i++)
     {
       tree_->GetEntry(i);
       
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

   for(int i=0;i<nq;i++)
     {
       double rangemin = (double)i/(double)nq*100.0;
       qvector_rec[i] = new TH2S(Form("qvector_rec%d",i),"qvector recentering",qxbin,-10,10,qybin,-10,10);
       psidis_rec[i] = new TH1F(Form("psi_rec%d",i),"psi recentering",psibin,psimin,psimax);
     }
    
   minrange =0;
   for(int i =0;i<tree_->GetEntries();i++)
     {
       tree_->GetEntry(i);
       
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
	      cal_rec(qvector[j],qvector_rec[j],psidis_rec[j]);
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
      psidis_rec[i]->Draw();
      c1[i]->cd(3);
      gPad->SetLogy(1);
      nclusdis[i]->Draw();
      if(i==0) c1[i]->Print((pdfname+"(").c_str());
      else c1[i]->Print(pdfname.c_str());
    }

   c1[nq]=new TCanvas(Form("canvas%d",nq),"canvas all",1800,600);
   c1[nq]->Divide(3,1);
   c1[nq]->cd(1);
   allqvec_rec->Draw("COLZ");
   c1[nq]->cd(2);
   allpsi_rec->Draw();
   c1[nq]->cd(3);
   gPad->SetLogy(1);
   nclush->Draw();
   c1[nq]->Print((pdfname+")").c_str());

   /*
   TCanvas *c1 = new TCanvas("","");
   c1->Divide(2,1);
   c1->cd(1);
   allqvec_rec->Draw("COLZ");
   c1->cd(2);
   allpsi_rec->Draw();
   */
   
}

void read_tree(string fname)
{
  TFile *file = TFile::Open(fname.c_str());
    
  tree_ = (TTree *)file->Get("Qvectortree");

  tree_->SetBranchAddress("nclus",&nclus);
  tree_->SetBranchAddress("Qx",&qx);
  tree_->SetBranchAddress("Qy",&qy);
  tree_->SetBranchAddress("psi",&psi);
}

void cal_rec(TH2S*allqvec,TH2S*allqvec_rec,TH1F*allpsi_rec)
{
  double qx_rec = (qx - allqvec->GetMean(1))/allqvec->GetStdDev(1);
  double qy_rec = (qy - allqvec->GetMean(2))/allqvec->GetStdDev(2);

  allqvec_rec->Fill(qx_rec,qy_rec);

  double psi_rec = 0.5*atan2(qx_rec,qy_rec);

  allpsi_rec->Fill(psi_rec);
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

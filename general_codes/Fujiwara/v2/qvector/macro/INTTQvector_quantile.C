#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
//#include <../src/InttOfflineCluster.h>

void INTTQvector_quantile()
{
  string fname ="rootfile/InttQvector_adccut.root";
  const Int_t nq = 20;

  TFile *file = TFile::Open(fname.c_str());
    
  TTree *tree_ = (TTree *)file->Get("Qvectortree");

  string pdfname = "pdffile/"+fname.substr(9,18)+to_string(nq)+"quantile.pdf";
 
  double nclus;
  double qx;
  double qy;
  double psi;

  tree_->SetBranchAddress("nclus",&nclus);
  tree_->SetBranchAddress("Qx",&qx);
  tree_->SetBranchAddress("Qy",&qy);
  tree_->SetBranchAddress("psi",&psi);

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

  int qxbin = (qxmax + abs(qxmin))/0.01;
  int qybin = (qymax + abs(qymin))/0.01;
  int psibin = (psimax + abs(psimin))/0.02;
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

  TH1F *nclush = new TH1F("nclus","multiplicity",nmax/10,0,nmax);
  TH2S *allqvec = new TH2S("allqvec","all qvector",qxbin,qxmin,qxmax,qybin,qymin,qymax);
  TH1F* allpsi = new TH1F("allpsi","all psi",psibin,psimin,psimax);
  //tree_->Draw("nclus");

  TProfile *qxprof[2];
  TProfile *qyprof[2];
 
  qxprof[0] = new TProfile("qxmulti","qx vs multiplicity",20,0,100);
  qyprof[0] = new TProfile("qymulti","qy vs multiplicity",20,0,100);

  qxprof[1] = new TProfile("qxmulti","qx vs multiplicity",nmax/100,0,nmax);
  qyprof[1] = new TProfile("qymulti","qy vs multiplicity",nmax/100,0,nmax);

  for(int i =0;i<tree_->GetEntries();i++)
    {
      tree_->GetEntry(i);

      	nclush->Fill(nclus);
	allpsi->Fill(psi);
	allqvec->Fill(qx,qy);
	
	//cout<<"nclus/nmax*100 ="<<nclus/nmax*100<<endl;
	//cout<<"qx, qy ="<<qx<<"  "<<qy<<endl;
	 if(isfinite(qx)==true)
	   {
	     qxprof[0]->Fill(nclus/nmax*100,qx);
	     qyprof[0]->Fill(nclus/nmax*100,qy);

	     qxprof[1]->Fill(nclus,qx);
	     qyprof[1]->Fill(nclus,qy);
	   }
    }  
  
  Double_t xq[nq]; 
  Double_t yq[nq];

  TH2S * qvector[nq];
  TH1F * psidis[nq];
  TH1F * nclusdis[nq];

  for (Int_t i=0;i<nq;i++) 
    {
      xq[i] = Float_t(i+1)/nq;
      cout<<xq[i]<<endl;
      nclush->GetQuantiles(nq,yq,xq);
      cout<<"yq "<<yq[i]<<endl;

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
  
  
  TCanvas *c1[nq+3];
  int entries = 0;

  for(int i=0;i<nq;i++)
    {
      c1[i]=new TCanvas(Form("canvas%d",i),Form("canvas%d",i),1800,600);
      c1[i]->Divide(3,1);
      c1[i]->cd(1);
      qvector[i]->Draw("COLZ");
      c1[i]->cd(2);
      psidis[i]->Draw();
      c1[i]->cd(3);
      gPad->SetLogy(1);
      nclusdis[i]->Draw();

      cout<<"nclus max  "<<nclusdis[i]->GetMaximum()<<endl;

      entries = entries + nclusdis[i]->GetEntries();

      if(i==0) c1[i]->Print((pdfname+"(").c_str());
      //else if(i==nq-1) c1[i]->Print((pdfname+")").c_str());
      else c1[i]->Print(pdfname.c_str());      
    }
  cout <<"entries "<<entries<<endl;
  cout <<"tree entries "<<tree_->GetEntries()<<endl;

  c1[nq]=new TCanvas(Form("canvas%d",nq),"canvas all",1800,600);
  c1[nq]->Divide(3,1);
  c1[nq]->cd(1);
  allqvec->Draw("COLZ");
  c1[nq]->cd(2);
  allpsi->Draw();
  c1[nq]->cd(3);
  gPad->SetLogy(1);
  nclush->Draw();
  c1[nq]->Print(pdfname.c_str());

  
  c1[nq+1]=new TCanvas(Form("canvas%d",nq+1),"canvas qvec",1200,600);
  c1[nq+1]->Divide(2,1);
  c1[nq+1]->cd(1);
  qxprof[0]->Draw();
  c1[nq+1]->cd(2);
  qyprof[0]->Draw();
  c1[nq+1]->Print(pdfname.c_str());

  c1[nq+2]=new TCanvas(Form("canvas%d",nq+2),"canvas qvec",1200,600);
  c1[nq+2]->Divide(2,1);
  c1[nq+2]->cd(1);
  qxprof[1]->Draw();
  c1[nq+2]->cd(2);
  qyprof[1]->Draw();
  c1[nq+2]->Print((pdfname+")").c_str());
  
}

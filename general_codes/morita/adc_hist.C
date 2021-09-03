//MiuMorita
//20200706
//20200707
//20200713
//20200715
//20200804
//
//amplitude fitting gaussian -> slope vs chip

#include<string>

 using namespace std;

 void adc_hist(const char *fname = "fphx.root"){

	TFile *file = TFile::Open(fname);

	const Int_t a = 7;//adc number
	const Int_t aa = 2;//adc start
	const Int_t c = 128;//channel
	const Int_t cc = 26;//chip

	const Int_t b = 12;//bco_full%16==
	const Int_t f = 4;//fem_id==
	const Int_t m = 8;//module==
	
	TH1F *h1[cc][c][a];
	
	TCanvas *c1 = new TCanvas("c1", "1-histogram", 600, 450);
	
	for(int k = 0; k < cc; k++){
		for(int j = 0; j < c; j++){
			for(int i = aa; i < a; i++){
				if(j==0&&i==0)cout<<k+1<<endl;
				h1[k][j][i] = new TH1F(Form("h1[%d][%d][%d]", k, j, i),"counts vs amplitude", 70, 0, 70);
				//tree->Draw(Form("ampl>>h1[%d][%d][%d]", k, j, i), Form("chip_id == %d && chan_id == %d && adc == %d && fem_id == 4 && module == 8", k+1, j, i));
				tree->Draw(Form("ampl>>h1[%d][%d][%d]", k, j, i), Form("chip_id == %d && chan_id == %d && adc == %d && fem_id == %d && module == %d && bco_full%%16 == %d", k+1, j, i, f, m, b),"goff");
				//tree->Draw(Form("ampl>>h1[%d][%d][%d]", k, j, i), Form("chip_id == %d && chan_id == %d && adc == %d && fem_id == 8 && module == 5", k+1, j, i),"goff");
				if(i==0&&(j==0||j==20||j==40||j==60||j==80||j==100||j==120))cout<<k+1<<"-"<<j<<endl;
			}
		}
	}

	TString sname = gSystem->BaseName(fname);
	sname.Prepend("adc_hist_");
	cout<<sname<<endl;

	TFile *frootout = TFile::Open(sname.Data(), "recreate");
	for(int k = 0; k < cc; k++){
		for(int j = 0; j < c; j++){
			for(int i = 0; i < a; i++){
				h1[k][j][i]->Write();
			}
		}
	}
	frootout->Close();

 }

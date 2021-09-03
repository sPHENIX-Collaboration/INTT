void run_readtree(const char *infile= "fphx_raw_20200727-1917_0.root")
{
//  gROOT->ProcessLine(".L readtree.c");
//  const char *outfile="histo.root";

//	string input = infile;
//	string output = "C:/root_v5.34.36/macros/cosmic_macro_20200728_shibata/" 
//		+ "histo" + input.substr(input.find_last_of("/")+1, input.size() - input.find_last_of("/") - 1);
  TString s_outfile(infile);
	//TString s_outfile( output.c_str() );
	s_outfile.Prepend("histo");
  TFile *f = TFile::Open(infile); 
  TTree *tree = (TTree*)f->Get("tree");
  readtree t(tree, s_outfile.Data());
  t.Loop();
  t.end();
}

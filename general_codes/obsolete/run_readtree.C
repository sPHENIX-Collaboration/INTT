void run_readtree(const char *infile= "fphx_raw_20200728-1905_0.root")
{
//  gROOT->ProcessLine(".L readtree.c");
//  const char *outfile="histo.root";
  TString s_outfile(infile);
  s_outfile.Prepend("histo");
  TFile *f = TFile::Open(infile); 
  TTree *tree = (TTree*)f->Get("tree");
  readtree t(tree, s_outfile.Data());
  t.Loop();
  t.end();
}

#include "InttMixupQA.h"

using namespace std;

InttMixupQA::InttMixupQA(const string &name, const int run_num)
  : SubsysReco(name)
{
  run_num_ = run_num;
}

InttMixupQA::~InttMixupQA() {}

int InttMixupQA::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5) {
    std::cout << "Beginning Init in InttMixupQA" << std::endl;
  }
    
  return 0;
}

int InttMixupQA::InitRun(PHCompositeNode *topNode)
{
  
  if (!topNode)
  {
    std::cout << "InttMixupQA::InitRun(PHCompositeNode* topNode)" << std::endl;
    std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

    return 1;
  }

  // Initialize histograms
  for( int felix=0; felix<kFelix_num_;felix++ )  {
    string name = "allmulti_intt" + to_string( felix );
    string title = name + ": with clone cut";
    h_allmulti_[felix] = new TH1F( name.c_str(), title.c_str(), 200, 0, 200 );
    h_allmulti_[felix]->SetXTitle("Multiplicity");
    h_allmulti_[felix]->SetLineColor(felix+1);

    name = "allclone_intt" + to_string( felix );
    title = name;
    h_allclone_[felix] = new TH1F( name.c_str(), title.c_str(), 200, 0, 200 );
    h_allclone_[felix]->SetXTitle("Clone multiplicity");
    h_allclone_[felix]->SetLineColor(felix+1);
  }
  
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttMixupQA::process_event(PHCompositeNode *topNode)
{

  string m_InttRawNodeName = "INTTRAWHIT";
  InttRawHitContainer *inttcont = findNode::getClass<InttRawHitContainer>(topNode, m_InttRawNodeName);
  if (!inttcont) {
    cout << PHWHERE << endl;
    cout << "InttMixupQA::process_event(PHCompositeNode* topNode)" << endl;
    cout << "Could not get \"" << m_InttRawNodeName << "\" from Node Tree" << endl;
    cout << "Exiting" << endl;
    gSystem->Exit(1);
    exit(1);
  }

  ievent_++;

  if ( (ievent_%100==0 && ievent_<1000) || ievent_%1000==0 ) {
    cout << "Process event #" << ievent_ << endl;
  }
  
  int nhits = inttcont->get_nhits();
  if (Verbosity() > 5) {
    cout<<"Nhits = "<<nhits<<endl;
  }
  map<int,int> map_hit;
  int nhit_fx[8] = {0,0,0,0,0,0,0,0};
  int ncln_fx[8] = {0,0,0,0,0,0,0,0};
  
  // Loop over all INTTRAWHIT
  for (int i = 0; i < nhits; i++) {
    
    InttRawHit *intthit = inttcont->get_hit(i);
    
    //    uint64_t bco_full = intthit->get_bco();
    
    int fnum	= intthit->get_packetid() - 3001;
    int fchn	= intthit->get_fee();
    int adc     = intthit->get_adc();
    int chip    = intthit->get_chip_id();
    int chan	= intthit->get_channel_id();
    int bco	= intthit->get_FPHX_BCO();
    int hitID   = 100000000 * (fnum+1) + 1000000 * fchn + 100009 * chip + chan * 10 + adc;

    auto itrHit = map_hit.find(hitID);
    if (itrHit == map_hit.end()) {
      map_hit.insert(make_pair(hitID, 0));
      if (Verbosity() > 5) {
	cout << hitID << " " << chip << " " << adc << " "<< bco << endl;
      }
      nhit_fx[fnum]++;
    } else {
      ncln_fx[fnum]++;
    }
      

    //    int bco_diff = (bco_full & 0x7FU) - bco;

  }

  for( int felix=0; felix<kFelix_num_;felix++ )  {
    h_allmulti_[felix]->Fill(nhit_fx[felix]);
    h_allclone_[felix]->Fill(ncln_fx[felix]);
  }
  
  
  //  h_allmulti_[fnum]->Fill(nhits);  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttMixupQA::End(PHCompositeNode * /*topNode*/)
{
  
  FindMixupEvent();

  if (Verbosity() > 1) {
    std::cout << "Processing InttMixupQA done" << std::endl;
  }

  this->DrawHists();

  for( auto& hist : h_allmulti_ ) {
    tf_output_->WriteTObject( hist, hist->GetName() );
  }

  tf_output_->Close();

  return Fun4AllReturnCodes::EVENT_OK;
 
}

int InttMixupQA::EndRun(PHCompositeNode * /*topNode*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttMixupQA::SetOutputDir( string dir )
{
  output_dir_ = dir;

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  output_root_ = output_dir_ + "root/" + output_basename_ + run_num_str + ".root";
  output_pdf_  = output_dir_ + "plots/" + output_basename_ + run_num_str + ".pdf";


  return Fun4AllReturnCodes::EVENT_OK;                         
}


// Private functions
void InttMixupQA::FindMixupEvent()
{
  if (Verbosity() > 1) {
    std::cout << "Finding Mixup Event..." << std::endl;
  }
}

void InttMixupQA::DrawHists()
{

  std::cout << "output pdf: " << output_pdf_ << std::endl;

  TCanvas* c = new TCanvas( "canvas", "title", 1600, 1200 );
  c->Print( (output_pdf_+"[").c_str() );
  gPad->SetGrid( true, true );
  //  gStyle->SetOptStat( 111111 );
  gStyle->SetOptStat( 0 );


  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  // DrawStats( hist_nhit_, 0.9, 0.65, 0.99, 0.9 );
  for( int felix=0; felix<kFelix_num_;felix++ )  {
    if(felix==0) h_allmulti_[felix]->Draw();
    else h_allmulti_[felix]->Draw("same");
  }

  TLegend *leg = new TLegend(0.75, 0.5, 0.9, 0.9);
  for (int felix=0; felix<kFelix_num_; felix++) {
    leg->AddEntry(h_allmulti_[felix], Form("intt-%d",felix), "l");
  }
  leg->Draw("same");
  c->Print( output_pdf_.c_str() );  
  c->Clear();

  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  // DrawStats( hist_nhit_, 0.9, 0.65, 0.99, 0.9 );
  for( int felix=0; felix<kFelix_num_;felix++ )  {
    if(felix==0) h_allclone_[felix]->Draw();
    else h_allclone_[felix]->Draw("same");
  }
  leg->Draw("same");
  c->Print( output_pdf_.c_str() );  
  c->Clear();

  c->SetCanvasSize( 800, 800 );
  c->Print( (output_pdf_+"]").c_str() );
  
}



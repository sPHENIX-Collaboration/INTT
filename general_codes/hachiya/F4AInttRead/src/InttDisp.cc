#include "InttDisp.h"
#include "InttAna.h"

/// Cluster/Calorimeter includes
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Shower.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>


/// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop


#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/InttDefs.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>


#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <g4eval/CaloEvalStack.h>


#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLine.h>
#include <TFile.h>
#include <TGraph.h>
#include <TEllipse.h>
#include <TMath.h>

#include <filesystem>

using namespace std;


/**
 * Constructor of module
 */
InttDisp::InttDisp(const std::string &name)
  : SubsysReco(name)
{

  m_gxy = new TGraph();
  m_grz = new TGraph();
  m_gxy->SetMarkerStyle(20);
  m_gxy->SetMarkerSize(1);
  m_grz->SetMarkerStyle(20);
  m_grz->SetMarkerSize(1);

  m_gvtx_xy = new TGraph();
  m_gvtx_rz = new TGraph();
  m_gvtx_xy->SetMarkerStyle(20);
  m_gvtx_xy->SetMarkerSize(1);
  m_gvtx_xy->SetMarkerColor(2);
  m_gvtx_rz->SetMarkerStyle(20);
  m_gvtx_rz->SetMarkerSize(1);
  m_gvtx_rz->SetMarkerColor(2);

  m_gxy_emc = new TGraph();
  m_grz_emc = new TGraph();
  m_gphiz_emc = new TGraph();
  m_gphiz_emcprj = new TGraph();
  m_gxy_emctru = new TGraph();
  m_grz_emctru = new TGraph();

  m_gxy_emc->SetMarkerStyle(20);
  m_gxy_emc->SetMarkerSize(1);
  m_grz_emc->SetMarkerStyle(20);
  m_grz_emc->SetMarkerSize(1);
  m_gphiz_emc->SetMarkerStyle(20);
  m_gphiz_emc->SetMarkerSize(1);
  m_gphiz_emcprj->SetMarkerStyle(24);
  m_gphiz_emcprj->SetMarkerSize(2);
  m_gphiz_emcprj->SetMarkerColor(2);
  m_gxy_emctru->SetMarkerStyle(20);
  m_gxy_emctru->SetMarkerSize(1);
  m_gxy_emctru->SetMarkerColor(3);
  m_grz_emctru->SetMarkerStyle(20);
  m_grz_emctru->SetMarkerSize(1);
  m_grz_emctru->SetMarkerColor(3);
}

/**
 * Destructor of module
 */
InttDisp::~InttDisp()
{
//  delete m_badmap;
//  delete m_bcomap;
}

/**
 * Initialize the module and prepare looping over events
 */
int InttDisp::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttDisp" << std::endl;
  }

  return 0;
}

int InttDisp::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttDisp" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttDisp::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

  // init calib
  //m_feemap->LoadFromCDB();

//  TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");



  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttDisp::process_event(PHCompositeNode* topNode)
{
  static int event=0;
  event++;

  ActsGeometry *m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
  {
    std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  TrkrClusterContainer *m_clusterMap = 
	  findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!m_clusterMap)
  {
      cout << PHWHERE << "TrkrClusterContainer node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  //////////////////////////
  // vertex
  m_gvtx_xy->Set(0);
  m_gvtx_rz->Set(0);

  double vr=0;
  if(m_inttana!=nullptr){
    m_gvtx_xy->SetPoint(0, m_inttana->m_xvertex, m_inttana->m_yvertex);
    vr = sqrt(m_inttana->m_xvertex* m_inttana->m_xvertex + m_inttana->m_yvertex*m_inttana->m_yvertex);
    m_gvtx_rz->SetPoint(0, vr, m_inttana->m_zvertex);
   
  }

  m_gxy->Set(0);
  m_grz->Set(0);

  //////////////////////////
  // cluster
  int nCluster=0;
  bool exceedNwrite=false;
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        const auto cluskey = clusIter->first;
        const auto cluster = clusIter->second;

        const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);
        double r = sqrt(globalPos.x()*globalPos.x() + globalPos.y()*globalPos.y());

        if(nCluster<5) {
            cout<<"xyz : "<<globalPos.x()<<" "<< globalPos.y()<<" "<< globalPos.z()<<" :  "
                <<cluster->getPosition(0)<<" "
                <<cluster->getPosition(1)<<" : "
                <<cluster->getAdc()<<endl;
        }
        else {
          if(!exceedNwrite) {
            cout<<" exceed : ncluster limit.  no more cluster xyz printed"<<endl;
            exceedNwrite=true;
          }
        }

        // = new TNtuple("ntp_clus", "Cluster Ntuple", "bco_full:evt:size:adc:x:y:z:lay:lad:sen");
        m_gxy->SetPoint(nCluster, globalPos.x(), globalPos.y());
        m_grz->SetPoint(nCluster, r, globalPos.z());

        nCluster++;
      }
    }
  }
  cout<<"nCluster : "<<nCluster<<endl;

  analyzeEMC(topNode);

  //
  //////////////////////////
  // track
  for(uint i=0; i<m_vtrk_xy.size(); i++){
    delete m_vtrk_xy[i];
  }
  m_vtrk_xy.clear();

  for(uint i=0; i<m_vtrk_rz.size(); i++){
    delete m_vtrk_rz[i];
  }
  m_vtrk_rz.clear();

  m_gphiz_emcprj->Set(0);
  if(m_inttana!=nullptr){
    vector<InttAna::cluspair>& vcluspair = m_inttana->vcluspair;

    TF1* f1 = new TF1("f1", "pol1", -4, 20);
    cout<<"ntrk : "<<vcluspair.size()<<endl;
    for(uint itrk=0; itrk<vcluspair.size(); itrk++){
      InttAna::cluspair& pair = vcluspair[itrk];

      double proj[2], ang0, ang1;
      double proj_emc[2];
      //calcProjAndAngle(pair.cx, pair.cy, pair.radius, pair.px, pair.py, 13, proj[0], proj[1], ang0, ang1);
      calcProjAndAngle(pair.cx, pair.cy, pair.radius, pair.px, pair.py, 93.5, proj_emc[0], proj_emc[1], ang0, ang1);

      calcProjAndAngle(pair.cx, pair.cy, pair.radius, pair.px, pair.py, 100., proj[0], proj[1], ang0, ang1);

      

      if(pair.chi2_rz<2){ // temporary, should be changed

        TEllipse *trk_xy = new TEllipse(pair.cx, pair.cy, pair.radius, 0, ang0*180/TMath::Pi(), ang1*180/TMath::Pi());
        trk_xy->SetFillStyle(20);
        trk_xy->SetLineWidth(1.5);
        trk_xy->SetLineColor(1);
        m_vtrk_xy.push_back(trk_xy);

        f1->SetParameters(pair.offset_rz, pair.slope_rz);
        //TLine *trk_rz = new TLine(vr, f1->Eval(vr), 15, f1->Eval(15));
        TLine *trk_rz = new TLine(vr, f1->Eval(vr), 95, f1->Eval(95));
        trk_rz->SetLineWidth(1.5);
        trk_rz->SetLineColor(1);
        m_vtrk_rz.push_back(trk_rz);

        double prj_emc_phi = atan2(proj_emc[1], proj_emc[0]);
        int nprj = m_gphiz_emcprj->GetN();
        m_gphiz_emcprj->SetPoint(nprj, f1->Eval(93.5), prj_emc_phi);

        cout<<"  "<<itrk<<" "<<pair.cx<<" "<<pair.cy<<" "<<pair.radius
            <<" : rz "<<pair.offset_rz<<" "<<pair.slope_rz<<" : "<<pair.chi2_rz<<endl;
      }
    }
    delete f1;
    
  }

  cout<<"HepMC"<<endl;
  analyzeHepMC(topNode);
  cout<<"PHG4"<<endl;
  analyzePHG4(topNode);

  //////////////////////////
  // draw
  static TEllipse *intt_cage[4];
  static TLine *intt_cage_rz[4];
  static TCanvas *c1 = nullptr;
  if(c1==nullptr) {
    c1 = new TCanvas("c1", "c1", 1200, 600);
    c1->Divide(2,1);

    intt_cage[0] = new TEllipse(0, 0,  7.2);
    intt_cage[1] = new TEllipse(0, 0,  7.8);
    intt_cage[2] = new TEllipse(0, 0,  9.7);
    intt_cage[3] = new TEllipse(0, 0, 10.3);
    intt_cage[0]->SetFillStyle(0);
    intt_cage[1]->SetFillStyle(0);
    intt_cage[2]->SetFillStyle(0);
    intt_cage[3]->SetFillStyle(0);

    intt_cage_rz[0] = new TLine(7.2, -23, 7.2,  23);
    intt_cage_rz[1] = new TLine(7.8, -23, 7.8,  23);
    intt_cage_rz[2] = new TLine(9.7, -23, 9.7,  23);
    intt_cage_rz[3] = new TLine(10.3,-23, 10.3, 23);
  }

  c1->cd(1);
  TH1* frm=gPad->DrawFrame(-13, -13, 13, 13);
  intt_cage[0]->Draw("same");
  intt_cage[1]->Draw("same");
  intt_cage[2]->Draw("same");
  intt_cage[3]->Draw("same");
  for(uint i=0; i<m_vtrutrk_xy.size(); i++){
    m_vtrutrk_xy[i]->Draw("only");
  }
  for(uint i=0; i<m_vtrk_xy.size(); i++){
    m_vtrk_xy[i]->Draw("only");
  }
  m_gxy->Draw("P");
  m_gvtx_xy->Draw("P");
  frm->Draw("same");

  c1->cd(2);
  gPad->DrawFrame(-1, -25, 13, 25);
  m_grz->Draw("P");
  m_gvtx_rz->Draw("P");
  intt_cage_rz[0]->Draw("same");
  intt_cage_rz[1]->Draw("same");
  intt_cage_rz[2]->Draw("same");
  intt_cage_rz[3]->Draw("same");
  for(uint i=0; i<m_vtrutrk_rz.size(); i++){
    m_vtrutrk_rz[i]->Draw();
  }
  for(uint i=0; i<m_vtrk_rz.size(); i++){
    m_vtrk_rz[i]->Draw();
  }

  //////////////////////////

  static TEllipse* emc_cage;
  static TLine *emc_cage_rz;
  static TCanvas *c2 = nullptr;
  if(c2==nullptr) {
    c2 = new TCanvas("c2", "c2", 1200, 600);
    c2->Divide(2,1);

    emc_cage = new TEllipse(0, 0,  93.5);
    emc_cage->SetFillStyle(0);

    emc_cage_rz = new TLine(93.5, -135, 93.5, 135);
  }
  c2->cd(1);
  //TH1* frm2= gPad->DrawFrame(-105, -105, 105, 105);
  TH1* frm2= gPad->DrawFrame(-125, -125, 125, 125);
  //for(uint i=0; i<m_vtrutrk_xy.size(); i++){
  //  m_vtrutrk_xy[i]->Draw("only");
  //}
  for(uint i=0; i<m_vtrutrk_xy.size(); i++){
    m_vtrutrk_xy_emc[i]->Draw("only");
  }
  for(uint i=0; i<m_vtrk_xy.size(); i++){
    m_vtrk_xy[i]->Draw("only");
  }
  frm2->Draw("same");
  emc_cage->Draw("same");
  m_gxy->Draw("P");
  m_gvtx_xy->Draw("P");
  m_gxy_emc->Draw("P");
  m_gxy_emctru->Draw("P");

  c2->cd(2);
  //TH1* rzfrm2= gPad->DrawFrame(-5, -135, 110, 135);
  TH1* rzfrm2= gPad->DrawFrame(-5, -135, 130, 135);
  for(uint i=0; i<m_vtrk_xy.size(); i++){
    m_vtrk_rz[i]->Draw("only");
  }
  rzfrm2->Draw("same");
  m_grz->Draw("P");
  m_gvtx_rz->Draw("P");
  m_grz_emc->Draw("P");
  intt_cage_rz[0]->Draw("same");
  intt_cage_rz[1]->Draw("same");
  intt_cage_rz[2]->Draw("same");
  intt_cage_rz[3]->Draw("same");
  emc_cage_rz->Draw("same");
  m_grz_emctru->Draw("P");

  //////////////////////////

  static TCanvas *c3 = nullptr;
  if(c3==nullptr) {
    c3 = new TCanvas("c3", "c3", 800, 800);
  }
  c3->cd();
  gPad->DrawFrame(-140, -3.2, 140, 3.2);
  m_gphiz_emcprj->Draw("P");
  m_gphiz_emc->Draw("P");

  //for(uint i=0; i<m_vtrutrk_xy.size(); i++){
  //  m_vtrutrk_xy_emc[i]->Draw("only");
  //}
  //for(uint i=0; i<m_vtrk_xy.size(); i++){
  //  m_vtrk_xy[i]->Draw("only");
  //}
  //
  c1->Modified();
  c1->Update();
  c2->Modified();
  c2->Update();
  c3->Modified();
  c3->Update();

  char a[5];
  cin>>a;
  if(a[0]=='q'){
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttDisp::End(PHCompositeNode * /*topNode*/)
{

  return 0;
}


bool InttDisp::calcProjAndAngle(
                  double cx, double cy, double radius, double px, double py, 
                  double projRadius,
                  double &prjx, double& prjy, double& ang0, double& ang1)
{
  double track_radius = radius;
  double track_c[2] = {cx,cy};

 double L = sqrt(track_c[0]*track_c[0] + track_c[1]*track_c[1]);
  if ((L+track_radius)< projRadius){
    //cout<<" no overlap"<<endl;
    prjx=0;
    prjy=0;
    ang0=0;
    ang1=0;
    return false;
  }
  else {
    ///////////////////////
    double a = track_c[0], b=track_c[1];
    double r = projRadius, R=track_radius;
    double A = R*R - r*r - a*a - b*b;
    double B = (fabs(b)<0.0000001) ? 0 : -a/b;
    double C = (fabs(b)<0.0000001) ? 0 : -1./(2*b);
    double CA = C*A;
    
    double dir = 1;
    
    if(fabs(b)<0.0000001){
            prjx = -A/(2*a);
            prjy = sqrt(r*r - prjx*prjx);
    }
    else {
            double par = sqrt((B*B*CA*CA) - (1.+B*B)*(CA*CA - r*r));
            prjx = (-B*CA + par)/(1.+B*B);
            prjy = B*prjx + CA;
    
            dir = px*prjx+py*prjy;
            if(dir<0){
          	  prjx = (-B*CA - par)/(1.+B*B);
          	  prjy = B*prjx + CA;
            }
    }
    
    //cout<<"x : y = "<<prjx<<" "<<prjy<<endl;
    
    ang0 = atan2(0.  -track_c[1],   0.-track_c[0]);
    ang1 = atan2(prjy-track_c[1], prjx-track_c[0]);
    
    double dphi = ang1 - ang0;
    if(     dphi> TMath::Pi())  dphi-= 2*TMath::Pi();
    else if(dphi<-TMath::Pi()) dphi+= 2*TMath::Pi();

    //cout<<ang0<<" "<<ang1<<" "<<dir<<" "<<dphi<<endl;

    if(dphi<0) {
      double tmp_ang = ang0;
      ang0 = ang1;
      ang1 = tmp_ang;

    }
    if(ang0>ang1) ang1 += 2*TMath::Pi();
  }

  return true;
}

void InttDisp::analyzeHepMC(PHCompositeNode *topNode)
{
  /// Get the node from the node tree
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  
  /// If the node was not properly put on the tree, return
  if (!hepmceventmap)
  {
    std::cout << PHWHERE
    	<< "HEPMC event map node is missing, can't collected HEPMC truth particles"
    	<< std::endl;
    return;
  }
  
  /// Could have some print statements for debugging with verbosity
  if (Verbosity() > 1)
  {
    std::cout << "Getting HEPMC truth particles " << std::endl;
  }
  
  /// You can iterate over the number of events in a hepmc event
  /// for pile up events where you have multiple hard scatterings per bunch crossing
  for (PHHepMCGenEventMap::ConstIter eventIter = hepmceventmap->begin();
  		eventIter != hepmceventmap->end();
  		++eventIter)
  {
    /// Get the event
    PHHepMCGenEvent *hepmcevent = eventIter->second;
  
    if (hepmcevent)
    {
      /// Get the event characteristics, inherited from HepMC classes
      HepMC::GenEvent *truthevent = hepmcevent->getEvent();
      if (!truthevent)
      {
        std::cout << PHWHERE
          << "no evt pointer under phhepmvgeneventmap found "
          << std::endl;
        return;
      }
  
      //--/// Get the parton info
      //--HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();
  
      //--/// Get the parton info as determined from HEPMC
      //--m_partid1 = pdfinfo->id1();
      //--m_partid2 = pdfinfo->id2();
      //--m_x1 = pdfinfo->x1();
      //--m_x2 = pdfinfo->x2();
  
      //--/// Are there multiple partonic intercations in a p+p event
      //--m_mpi = truthevent->mpi();
  
      //--/// Get the PYTHIA signal process id identifying the 2-to-2 hard process
      //--m_process_id = truthevent->signal_process_id();
  
      //--if (Verbosity() > 2)
      //--{
      //--  std::cout << " Iterating over an event" << std::endl;
      //--}
      /// Loop over all the truth particles and get their information
      for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin();
          iter != truthevent->particles_end();
          ++iter)
      {
        //Get each pythia particle characteristics
        //--double truthenergy = (*iter)->momentum().e();
        int    truthpid    = (*iter)->pdg_id();
  
        //double trutheta = (*iter)->momentum().pseudoRapidity();
        //double truthphi = (*iter)->momentum().phi();
        double truthpx  = (*iter)->momentum().px();
        double truthpy  = (*iter)->momentum().py();
        double truthpz  = (*iter)->momentum().pz();
        //double truthpt  = sqrt(truthpx * truthpx + truthpy * truthpy);

        HepMC::GenVertex* vtx = (*iter)->production_vertex();
        double truthx  = (vtx!=nullptr) ? vtx->position().x() : -9999.;
        double truthy  = (vtx!=nullptr) ? vtx->position().y() : -9999.;
        double truthz  = (vtx!=nullptr) ? vtx->position().z() : -9999.;
        int    truthsts= (*iter)->status();

        static bool printOK=false;
        if(printOK)
          cout<<"tru : "<<truthpid<<" "<<truthsts<<" : "<<truthpx<<" "<<truthpy<<" "<<truthpz
               <<" : "  <<truthx<<" "<<truthy<<" "<<truthz<<endl;
  
        /// Fill the truth tree
        //m_numparticlesinevent++;
      }
    }
  }
  
}

void InttDisp::analyzePHG4(PHCompositeNode *topNode)
{
  /// G4 truth particle node
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  
  if (!truthinfo)
  {
    std::cout << PHWHERE
      << "PHG4TruthInfoContainer node is missing, can't collect G4 truth particles"
      << std::endl;
    return;
  }

  ///////////////////
  //  adding EMCEvalStack
  CaloEvalStack caloeval(topNode, "CEMC");
  CaloTruthEval *calotruth = caloeval.get_truth_eval();
          
  //////////////////////////
  // track
  for(uint i=0; i<m_vtrutrk_xy.size(); i++){
    delete m_vtrutrk_xy[i];
  }
  m_vtrutrk_xy.clear();
  for(uint i=0; i<m_vtrutrk_rz.size(); i++){
    delete m_vtrutrk_rz[i];
  }
  m_vtrutrk_rz.clear();
  for(uint i=0; i<m_vtrutrk_xy_emc.size(); i++){
    delete m_vtrutrk_xy_emc[i];
  }
  m_vtrutrk_xy_emc.clear();
  m_gxy_emctru->Set(0);
  m_grz_emctru->Set(0);

  TF1* f1tru = new TF1("f1tru","pol1");
  
  /// Get the primary particle range
  PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();
  
  /// Loop over the G4 truth (stable) particles
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
      iter != range.second;
      ++iter)
  {
    /// Get this truth particle
    PHG4Particle *truth = iter->second;
  
    /// Get this particles momentum, etc.
    double truthpx = truth->get_px();
    double truthpy = truth->get_py();
    double truthpz = truth->get_pz();
    //double truthp = sqrt(truthpx * truthpx + truthpy * truthpy + truthpz * truthpz);
    double truthenergy = truth->get_e();
  
    double truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);
  
    //double truthphi = atan(truthpy / truthpx);
  
    double trutheta = atanh(truthpz / truthenergy);
    bool   isprimary = truthinfo->is_primary(truth);
    /// Check for nans
    if (!std::isfinite(trutheta))
    {
      trutheta = -99;
    }
    double truthpid = truth->get_pid();
    int    truthid = truth->get_track_id();
    int    vertexid = truth->get_vtx_id();

    PHG4VtxPoint* vtx = truthinfo->GetVtx(vertexid);
    double x = vtx->get_x();
    double y = vtx->get_y();
    double z = vtx->get_z();
   
    double r = sqrt(x*x + y*y);

    

    if(isprimary)
    {
      //if(r<2&&abs(z)<20)
      if(r<12&&fabs(z)<20)
      {
        // project INTT layer
        double slope  = truthpz/truthpt;
        double offset = z;

        double zpos = slope*(10.33 - r)+offset;

        static const bool printOK=false;
        if(printOK)
          cout<<"g4 : "<<truthid<<" "<<vertexid<<" "<<truthpid<<" : "<<truthpx<<" "<<truthpy<<" "<<truthpz
              <<"  "<<(isprimary?"Primary":"Secondary")
              <<" : "<<x<<" "<<y<<" "<<z
              <<" : "<<zpos
              <<endl;

        bool pidOK = ( 
                     abs(truthpid)==211 
                   ||abs(truthpid)==321
                   ||abs(truthpid)==2212
                   ||abs(truthpid)==11
                   ||abs(truthpid)==13
                     );

        if(fabs(zpos)<22.8 && pidOK){
          static const double Bf = 1.4;
          double sign   = (truthpid>0) ? 1. : -1.;
          if(abs(truthpid)<14) sign*=-1.; // for lepton, e+ is anti-particle
          double radius = truthpt/(0.3*Bf)*100.; // m -> cm
          double cx     = radius * ( truthpy/truthpt)*sign;
          double cy     = radius * (-truthpx/truthpt)*sign;

          double proj[2], ang0, ang1;
          calcProjAndAngle(cx, cy, radius, truthpx, truthpy, 13, proj[0], proj[1], ang0, ang1);

          TEllipse *trk_xy = new TEllipse(cx, cy, radius, 0, ang0*180/TMath::Pi(), ang1*180/TMath::Pi());
          trk_xy->SetFillStyle(20);
          trk_xy->SetLineWidth(3);
          trk_xy->SetLineColor(2);
          m_vtrutrk_xy.push_back(trk_xy);

          calcProjAndAngle(cx, cy, radius, truthpx, truthpy, 93, proj[0], proj[1], ang0, ang1);

          TEllipse *trk_xy_emc = new TEllipse(cx, cy, radius, 0, ang0*180/TMath::Pi(), ang1*180/TMath::Pi());
          trk_xy_emc->SetFillStyle(20);
          trk_xy_emc->SetLineWidth(3);
          trk_xy_emc->SetLineColor(2);
          m_vtrutrk_xy_emc.push_back(trk_xy_emc);

          f1tru->SetParameters(offset, slope);
          TLine *trk_rz = new TLine(r, f1tru->Eval(r), 15, f1tru->Eval(15));
          trk_rz->SetLineWidth(3);
          trk_rz->SetLineColor(2);
          m_vtrutrk_rz.push_back(trk_rz);

          //cout<<"  "<<cx<<" "<<cy<<" "<<radius<<endl;
          //if(printOK)
          //  cout<<"g4 : "<<truthid<<" "<<vertexid<<" "<<truthpid<<" : "<<truthpx<<" "<<truthpy<<" "<<truthpz
          //      <<"  "<<(isprimary?"Primary":"Secondary")
          //      <<" : "<<x<<" "<<y<<" "<<z
          //      <<" : "<<zpos
          //      <<endl;

          // calo truth info;
          PHG4Shower* g4shower = calotruth->get_primary_shower(truth);
          double emcr_tru = -9999;
          if(g4shower!=nullptr) {
            emcr_tru = sqrt(g4shower->get_x()*g4shower->get_x() + g4shower->get_y()*g4shower->get_y());

            int ntru = m_gxy_emctru->GetN();
            m_gxy_emctru->SetPoint(ntru, g4shower->get_x(), g4shower->get_y());
            m_grz_emctru->SetPoint(ntru, emcr_tru,          g4shower->get_z());


            cout<<"  shower : "<<g4shower->get_x()<<" "<<g4shower->get_y()<<" "<<g4shower->get_z()<<endl;
          }
        }
      }
      //else {
      //  cout<<"g4 : "<<truthid<<" "<<vertexid<<" "<<truthpid<<" : "<<truthpx<<" "<<truthpy<<" "<<truthpz
      //      <<"  "<<"Not Primary"
      //      <<" : "<<x<<" "<<y<<" "<<z
      //      <<endl;
      //}
    }
    //--else { // secondary
    //--  if(r<12&&abs(z)<20) {
    //--  cout<<"g4 : "<<truthid<<" "<<vertexid<<" "<<truthpid
    //--      <<" : "<<truthpx<<" "<<truthpy<<" "<<truthpz
    //--      <<"  "<<"Secondary INTT "
    //--      <<" : "<<x<<" "<<y<<" "<<z
    //--      <<endl;
    //--  }
    //--}
  }
  delete f1tru;
}

void InttDisp::analyzeEMC(PHCompositeNode *topNode)
{
/////////////
// EMC Cluster
  RawClusterContainer *clusterContainer 
                     = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  if(!clusterContainer)
  {
    std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTERINFO_POS_COR_CEMC node is missing. Try next" << std::endl;
    clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");

    if(!clusterContainer)
    {
      std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
    }
  }
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  //
  

  m_gxy_emc->Set(0);
  m_grz_emc->Set(0);
  m_gphiz_emc->Set(0);

  if(!clusterContainer)
  {
    std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
  }
  else {
    //This is how we iterate over items in the container.
    RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
    RawClusterContainer::ConstIterator clusterIter;

    for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;

      double x = recoCluster->get_x();
      double y = recoCluster->get_y();
      double z = recoCluster->get_z();
      double r = sqrt(x*x + y*y);
      double phi = atan2(y, x);
      if(recoCluster->get_ecore()>0.10){
        int n = m_gxy_emc->GetN();
        m_gxy_emc->SetPoint(n, x, y);
        m_grz_emc->SetPoint(n, r, z);
        m_gphiz_emc->SetPoint(n, z, phi);
      }
    }
  }


}

//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in tutorial.h.
//
// tutorial(const std::string &name = "tutorial")
// everything is keyed to tutorial, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// tutorial::~tutorial()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int tutorial::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int tutorial::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int tutorial::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int tutorial::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int tutorial::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int tutorial::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int tutorial::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void tutorial::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "tutorial.h"

//____________________________________________________________________________..
tutorial::tutorial(
  const std::string & name_in,
    std::string output_path_in,
    std::string output_rootfile_name_in):
  SubsysReco(name_in),
  output_path(output_path_in),
  output_rootfile_name(output_rootfile_name_in),
  file_out(nullptr),
  output( nullptr )
{
  std::cout << "tutorial::tutorial(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
tutorial::~tutorial()
{
  std::cout << "tutorial::~tutorial() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int tutorial::Init(PHCompositeNode *topNode)
{
  std::cout << "tutorial::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  ////////////////////////////////////////////////////////
  // Initialization of the member                       //
  ////////////////////////////////////////////////////////
  output = new TFile( output_path.c_str(), "RECREATE" );
  
    std::cout << "tutorial::Init(PHCompositeNode *topNode) Initializing" << std::endl;

    eventID = 0;
    NClus = 0;
    clus_system.clear();
    clus_layer.clear();
    clus_adc.clear();
    clus_X.clear();
    clus_Y.clear();
    clus_Z.clear();
    clus_size.clear();
    clus_phi_size.clear();
    clus_z_size.clear();

    nTowers = 0;
    tower_X.clear();
    tower_Y.clear();
    tower_Z.clear();
    tower_Eta.clear();
    tower_Phi.clear();
    tower_Eta_test.clear();
    tower_Phi_test.clear();
    tower_Eta_bin.clear();
    tower_Phi_bin.clear();
    tower_edep.clear();
    tower_system.clear();

    nCaloClus = 0;
    caloClus_X.clear();
    caloClus_Y.clear();
    caloClus_Z.clear();
    caloClus_R.clear();
    caloClus_Phi.clear();
    caloClus_edep.clear();
    caloClus_system.clear();

    // note : Truth primary vertex information
    TruthPV_trig_x_ = -999;
    TruthPV_trig_y_ = -999;
    TruthPV_trig_z_ = -999;
    NTruthVtx_ = 0;

    // note : PHG4 information (from all PHG4Particles)
    NPrimaryG4P_ = 0;
    NPrimaryG4P_promptChargeHadron_ = 0;
    PrimaryG4P_Pt_.clear();
    PrimaryG4P_Eta_.clear();
    PrimaryG4P_Phi_.clear();
    PrimaryG4P_E_.clear();
    PrimaryG4P_PID_.clear();
    PrimaryG4P_ParticleClass_.clear();
    PrimaryG4P_isStable_.clear();
    PrimaryG4P_Charge_.clear();
    PrimaryG4P_isChargeHadron_.clear();

    ////////////////////////////////////////////////////////
    // Initialization of the member                       //
    ////////////////////////////////////////////////////////
    file_out = new TFile((output_path + "/" + output_rootfile_name).c_str(), "RECREATE");
    tree_out = new TTree("tree", "sPHENIX info.");
    
    // note : the tracker cluster
    tree_out -> Branch("trk_NClus", & NClus);
    tree_out -> Branch("trk_system", & clus_system);
    tree_out -> Branch("trk_layer", & clus_layer);
    tree_out -> Branch("trk_adc", & clus_adc);
    tree_out -> Branch("trk_X", & clus_X);
    tree_out -> Branch("trk_Y", & clus_Y);
    tree_out -> Branch("trk_Z", & clus_Z);
    tree_out -> Branch("trk_size", & clus_size);
    tree_out -> Branch("trk_phi_size", & clus_phi_size);
    tree_out -> Branch("trk_Z_size", & clus_z_size);


    // note : the calorimeter raw tower information with the calibration, and introduction of noise hits
    tree_out -> Branch("nTowers", & nTowers);
    tree_out -> Branch("tower_system", & tower_system);
    tree_out -> Branch("tower_X", & tower_X);
    tree_out -> Branch("tower_Y", & tower_Y);
    tree_out -> Branch("tower_Z", & tower_Z);
    tree_out -> Branch("tower_Eta", & tower_Eta);
    tree_out -> Branch("tower_Phi", & tower_Phi);
    tree_out -> Branch("tower_Eta_test", & tower_Eta_test);
    tree_out -> Branch("tower_Phi_test", & tower_Phi_test);
    tree_out -> Branch("tower_Eta_bin", & tower_Eta_bin);
    tree_out -> Branch("tower_Phi_bin", & tower_Phi_bin);
    tree_out -> Branch("tower_edep", & tower_edep);


    // note : the calorimeter raw tower information with the calibration, and introduction of noise hits
    tree_out -> Branch("nCaloClus", & nCaloClus);
    tree_out -> Branch("caloClus_system", & caloClus_system);
    tree_out -> Branch("caloClus_X", & caloClus_X);
    tree_out -> Branch("caloClus_Y", & caloClus_Y);
    tree_out -> Branch("caloClus_Z", & caloClus_Z);
    tree_out -> Branch("caloClus_R", & caloClus_R);
    tree_out -> Branch("caloClus_Phi", & caloClus_Phi);
    tree_out -> Branch("caloClus_edep", & caloClus_edep);



    // note : true vertex information from G4Particle 
    tree_out->Branch("NTruthVtx", &NTruthVtx_);
    tree_out->Branch("TruthPV_trig_x", &TruthPV_trig_x_);
    tree_out->Branch("TruthPV_trig_y", &TruthPV_trig_y_);
    tree_out->Branch("TruthPV_trig_z", &TruthPV_trig_z_);
    // note : Primary PHG4Particle information
    tree_out->Branch("NPrimaryG4P", &NPrimaryG4P_);
    tree_out->Branch("PrimaryG4P_Pt", &PrimaryG4P_Pt_);
    tree_out->Branch("PrimaryG4P_Eta", &PrimaryG4P_Eta_);
    tree_out->Branch("PrimaryG4P_Phi", &PrimaryG4P_Phi_);
    tree_out->Branch("PrimaryG4P_E", &PrimaryG4P_E_);
    tree_out->Branch("PrimaryG4P_PID", &PrimaryG4P_PID_);
    tree_out->Branch("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron_);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::InitRun(PHCompositeNode *topNode)
{
  std::cout << "tutorial::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}



//____________________________________________________________________________..
int tutorial::prepareTracker(PHCompositeNode * topNode) 
{
  std::cout << "tutorial::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  // Getting Nodes                                                            //
  /////////////////////////////////////////////////////////////////////////////  
  // TRKR_CLUSTER node: Information of TrkrCluster
  auto *node_cluster_map = 
    findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");

  if (!node_cluster_map)
    {
      std::cerr << PHWHERE << "TrkrClusterContainer node is missing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  // ActsGeometry node: for the global coordinate
  ActsGeometry *node_acts = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if ( !node_acts )
    {
      std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  
    std::vector <TrkrCluster *> mvtx_clusters; mvtx_clusters.clear();
    std::vector <TrkrCluster *> intt_clusters; intt_clusters.clear();
    std::vector <TrkrCluster *> tpc_clusters; tpc_clusters.clear();

    // note : mvtx
    for (unsigned int mvtxlayer = 0; mvtxlayer < 3; mvtxlayer++) {
        //      cout << " INTT layer " << mvtxlayer << endl;
        //      int layer= ( mvtxlayer < 2 ? 0 : 1 );

        // loop over all hits
        for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::mvtxId, mvtxlayer)) {

            // type: std::pair<ConstIterator, ConstIterator> ConstRange
            // here, MMap::const_iterator ConstIterator;
            auto range = node_cluster_map -> getClusters(hitsetkey);

            // loop over iterators of this cluster
            for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
                const auto cluskey = clusIter -> first;
                const auto cluster = clusIter -> second;
                mvtx_clusters.push_back(cluster);

                const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

                // int ladder_z   = InttDefs::getLadderZId(cluskey);
                // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
                int size = cluster -> getSize();

                cluster -> setPosition(0, globalPos.x());
                cluster -> setPosition(1, globalPos.y());
                cluster -> setPosition(2, globalPos.z());

                clus_system.push_back(0); // note : the cluster system is 0 for MVTX
                clus_layer.push_back(mvtxlayer);
                clus_adc.push_back(cluster -> getAdc());
                clus_X.push_back(globalPos.x());
                clus_Y.push_back(globalPos.y());
                clus_Z.push_back(globalPos.z());
                clus_size.push_back(size);
                int phisize = cluster -> getPhiSize();
                // if (phisize <= 0) {phisize += 256;}
                clus_phi_size.push_back(phisize);
                clus_z_size.push_back(cluster -> getZSize());

                // cluster->setPosition(0,  globalPos.x() );
                // cluster->setPosition(1,  globalPos.y() );
                // cluster->setPosition(2,  globalPos.z() );
            }
        }
    }

    // note : intt
    for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++) {
        //      cout << " INTT layer " << inttlayer << endl;
        //      int layer= ( inttlayer < 2 ? 0 : 1 );

        // loop over all hits
        for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3)) {

            // type: std::pair<ConstIterator, ConstIterator> ConstRange
            // here, MMap::const_iterator ConstIterator;
            auto range = node_cluster_map -> getClusters(hitsetkey);

            // loop over iterators of this cluster
            for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
                const auto cluskey = clusIter -> first;
                const auto cluster = clusIter -> second;
                intt_clusters.push_back(cluster);

                const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

                // int ladder_z   = InttDefs::getLadderZId(cluskey);
                // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
                int size = cluster -> getSize();

                cluster -> setPosition(0, globalPos.x());
                cluster -> setPosition(1, globalPos.y());
                cluster -> setPosition(2, globalPos.z());

                clus_system.push_back(1); // note : the cluster system is 1 for INTT
                clus_layer.push_back(inttlayer + 3);
                clus_adc.push_back(cluster -> getAdc());
                clus_X.push_back(globalPos.x());
                clus_Y.push_back(globalPos.y());
                clus_Z.push_back(globalPos.z());
                clus_size.push_back(size);
                int phisize = cluster -> getPhiSize();
                if (phisize <= 0) {
                    phisize += 256;
                }
                clus_phi_size.push_back(phisize);
                clus_z_size.push_back(cluster -> getZSize());


                // cluster->setPosition(0,  globalPos.x() );
                // cluster->setPosition(1,  globalPos.y() );
                // cluster->setPosition(2,  globalPos.z() );
            }
        }
    }

    // note : TPC
    for (const auto & hitsetkey: node_cluster_map -> getHitSetKeys(TrkrDefs::TrkrId::tpcId)) {

        // type: std::pair<ConstIterator, ConstIterator> ConstRange
        // here, MMap::const_iterator ConstIterator;
        auto range = node_cluster_map -> getClusters(hitsetkey);

        // loop over iterators of this cluster
        for (auto clusIter = range.first; clusIter != range.second; ++clusIter) {
            const auto cluskey = clusIter -> first;
            const auto cluster = clusIter -> second;
            tpc_clusters.push_back(cluster);

            const auto globalPos = node_acts -> getGlobalPosition(cluskey, cluster);

            // int ladder_z   = InttDefs::getLadderZId(cluskey);
            // int ladder_phi = InttDefs::getLadderPhiId(cluskey);
            int size = cluster -> getSize();

            cluster -> setPosition(0, globalPos.x());
            cluster -> setPosition(1, globalPos.y());
            cluster -> setPosition(2, globalPos.z());

            clus_system.push_back(2); // note : the cluster system is 2 for TPC
            clus_layer.push_back(-999);
            clus_adc.push_back(cluster -> getAdc());
            clus_X.push_back(globalPos.x());
            clus_Y.push_back(globalPos.y());
            clus_Z.push_back(globalPos.z());
            clus_size.push_back(size);
            int phisize = cluster -> getPhiSize();
            // if (phisize <= 0) {phisize += 256;}
            clus_phi_size.push_back(phisize);
            clus_z_size.push_back(cluster -> getZSize());

            // cluster->setPosition(0,  globalPos.x() );
            // cluster->setPosition(1,  globalPos.y() );
            // cluster->setPosition(2,  globalPos.z() );
        }
    }

    NClus = mvtx_clusters.size() + intt_clusters.size() + tpc_clusters.size();

    std::cout << "N mvtx cluster : " << mvtx_clusters.size() << std::endl;
    std::cout << "N intt cluster : " << intt_clusters.size() << std::endl;
    std::cout << "N tpc cluster : " << tpc_clusters.size() << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;

}


// CEMC (PHCompositeNode)/
//        G4HIT_ABSORBER_CEMC (IO,PHG4HitContainer)
//        G4HIT_CEMC (IO,PHG4HitContainer)
//        G4CELL_CEMC (IO,PHG4CellContainer)
//        TOWER_SIM_CEMC (IO,RawTowerContainer)
//        TOWERINFO_SIM_CEMC (IO,TowerInfoContainerv1)
//        TOWER_RAW_CEMC (IO,RawTowerContainer)
//        TOWERINFO_RAW_CEMC (IO,TowerInfoContainerv1)
//        TOWER_CALIB_CEMC (IO,RawTowerContainer)
//        TOWERINFO_CALIB_CEMC (IO,TowerInfoContainerv1)
//        CLUSTER_CEMC (IO,RawClusterContainer)
//        CLUSTER_POS_COR_CEMC (IO,RawClusterContainer)

int tutorial::prepareEMCal(PHCompositeNode * topNode) {
    geomEM = findNode::getClass <RawTowerGeomContainer> (topNode, "TOWERGEOM_CEMC");
    EMCal_tower_sim_info = findNode::getClass <TowerInfoContainerv1> (topNode, "TOWERINFO_SIM_CEMC");
    EMCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_CEMC");
    EMCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, emcal_node_name);

    if (!EMCal_tower_calib) {
        std::cout << "tutorial::process_event Could not find node " << emcal_node_name << std::endl;
        exit(1);
    }

    // note : EMCal
    for (int i = 0; i < EMCal_tower_calib -> size(); ++i) //loop over channels 
    {
        TowerInfo * tower = EMCal_tower_calib -> get_tower_at_channel(i); //get EMCal tower
        int key = EMCal_tower_calib -> encode_key(i);
        int etabin = EMCal_tower_calib -> getTowerEtaBin(key);
        int phibin = EMCal_tower_calib -> getTowerPhiBin(key);
        // float time = EMCal_tower_calib->get_tower_at_channel(i)->get_time_float(); //get time

        if (tower -> get_energy() <= 0) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, etabin, phibin);
        RawTowerGeom * tower_geom = geomEM -> get_tower_geometry(geomkey); //encode tower geometry
        double EMCal_pos_x   = tower_geom -> get_center_x();
        double EMCal_pos_y   = tower_geom -> get_center_y();
        double EMCal_pos_z   = tower_geom -> get_center_z();
        double EMCal_pos_eta = tower_geom -> get_eta();
        double EMCal_pos_phi = tower_geom -> get_phi();
        double EMCal_energy  = tower -> get_energy();

        tower_system.push_back(0);
        tower_X.push_back(EMCal_pos_x);
        tower_Y.push_back(EMCal_pos_y);
        tower_Z.push_back(EMCal_pos_z);
        tower_Eta.push_back(EMCal_pos_eta);
        tower_Phi.push_back(EMCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(EMCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}

int tutorial::prepareiHCal(PHCompositeNode * topNode) {
    // HCALIN (PHCompositeNode)/
    //   G4HIT_ABSORBER_HCALIN (IO,PHG4HitContainer)
    //   G4HIT_HCALIN (IO,PHG4HitContainer)
    //   G4CELL_HCALIN (IO,PHG4CellContainer)
    //   TOWER_SIM_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_SIM_HCALIN (IO,TowerInfoContainerv1)
    //   TOWER_RAW_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_RAW_HCALIN (IO,TowerInfoContainerv1)
    //   TOWER_CALIB_HCALIN (IO,RawTowerContainer)
    //   TOWERINFO_CALIB_HCALIN (IO,TowerInfoContainerv1)
    //   CLUSTER_HCALIN (IO,RawClusterContainer)

    geomIH = findNode::getClass <RawTowerGeomContainer> (topNode, "TOWERGEOM_HCALIN");
    iHCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_HCALIN");
    iHCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, ihcal_node_name);

    if (!iHCal_tower_calib) {
        std::cout << "tutorial::process_event Could not find node " << ihcal_node_name << std::endl;
        exit(1);
    }

    // note : iHCal
    for (int i = 0; i < iHCal_tower_calib -> size(); ++i) //loop over channels 
    {
        TowerInfo * tower = iHCal_tower_calib -> get_tower_at_channel(i); //get iHCal tower
        int key = iHCal_tower_calib -> encode_key(i);
        int etabin = iHCal_tower_calib -> getTowerEtaBin(key);
        int phibin = iHCal_tower_calib -> getTowerPhiBin(key);
        float time = iHCal_tower_calib->get_tower_at_channel(i)->get_time_float(); //get time
        if (tower -> get_energy() <= 0) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, etabin, phibin);
        RawTowerGeom * tower_geom = geomIH -> get_tower_geometry(geomkey); //encode tower geometry
        double iHCal_pos_x   = tower_geom -> get_center_x();
        double iHCal_pos_y   = tower_geom -> get_center_y();
        double iHCal_pos_z   = tower_geom -> get_center_z();
        double iHCal_pos_eta = tower_geom -> get_eta();
        double iHCal_pos_phi = tower_geom -> get_phi();
        double iHCal_energy  = tower -> get_energy();

        tower_system.push_back(1);
        tower_X.push_back(iHCal_pos_x);
        tower_Y.push_back(iHCal_pos_y);
        tower_Z.push_back(iHCal_pos_z);
        tower_Eta.push_back(iHCal_pos_eta);
        tower_Phi.push_back(iHCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(iHCal_energy);
        // std::cout<<"test iHCal 15"<<std::endl;
    }

    return Fun4AllReturnCodes::EVENT_OK;

}
int tutorial::prepareoHCal(PHCompositeNode * topNode) {
    //     HCALOUT (PHCompositeNode)/
    //        G4HIT_ABSORBER_HCALOUT (IO,PHG4HitContainer)
    //        G4HIT_HCALOUT (IO,PHG4HitContainer)
    //        G4CELL_HCALOUT (IO,PHG4CellContainer)
    //        TOWER_SIM_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_SIM_HCALOUT (IO,TowerInfoContainerv1)
    //        TOWER_RAW_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_RAW_HCALOUT (IO,TowerInfoContainerv1)
    //        TOWER_CALIB_HCALOUT (IO,RawTowerContainer)
    //        TOWERINFO_CALIB_HCALOUT (IO,TowerInfoContainerv1)
    //        CLUSTER_HCALOUT (IO,RawClusterContainer)

    geomOH = findNode::getClass <RawTowerGeomContainer> (topNode, "TOWERGEOM_HCALOUT");
    oHCal_tower_sim = findNode::getClass <RawTowerContainer> (topNode, "TOWER_SIM_HCALOUT");
    oHCal_tower_calib = findNode::getClass <TowerInfoContainerv2> (topNode, ohcal_node_name);

    if (!oHCal_tower_calib) {
        std::cout << "tutorial::process_event Could not find node " << ohcal_node_name << std::endl;
        exit(1);
    }

    // note : oHCal
    for (int i = 0; i < oHCal_tower_calib -> size(); ++i) //loop over channels 
    {
        // std::cout<<"test oHCal 1"<<std::endl;
        TowerInfo * tower = oHCal_tower_calib -> get_tower_at_channel(i); //get oHCal tower
        // std::cout<<"test oHCal 2"<<std::endl;
        int key = oHCal_tower_calib -> encode_key(i);
        int etabin = oHCal_tower_calib -> getTowerEtaBin(key);
        int phibin = oHCal_tower_calib -> getTowerPhiBin(key);

        if (tower -> get_energy() <= 0) {continue;}

        const RawTowerDefs::keytype geomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, etabin, phibin);
        RawTowerGeom * tower_geom = geomOH -> get_tower_geometry(geomkey); //encode tower geometry
        double oHCal_pos_x   = tower_geom -> get_center_x();
        double oHCal_pos_y   = tower_geom -> get_center_y();
        double oHCal_pos_z   = tower_geom -> get_center_z();
        double oHCal_pos_eta = tower_geom -> get_eta();
        double oHCal_pos_phi = tower_geom -> get_phi();
        double oHCal_energy  = tower -> get_energy();

        tower_system.push_back(2);
        tower_X.push_back(oHCal_pos_x);
        tower_Y.push_back(oHCal_pos_y);
        tower_Z.push_back(oHCal_pos_z);
        tower_Eta.push_back(oHCal_pos_eta);
        tower_Phi.push_back(oHCal_pos_phi);
        tower_Eta_bin.push_back(etabin);
        tower_Phi_bin.push_back(phibin);
        tower_edep.push_back(oHCal_energy);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

// === s === ChcKuma add Cal Clus ===============================================
int tutorial::prepareEMCalClus(PHCompositeNode * topNode) {
    EMCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_CEMC");

    if (!EMCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << emcalClus_node_name << std::endl;
        exit(1);
    }

    // note : EMCal
    for (int i = 0; i < EMCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = EMCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0) {continue;}

        double EMCal_pos_x   = cluster -> get_x();
        double EMCal_pos_y   = cluster -> get_y();
        double EMCal_pos_z   = cluster -> get_z();
        double EMCal_pos_r   = cluster -> get_r();
        double EMCal_pos_phi = cluster -> get_phi();
        double EMCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(0);
        caloClus_X.push_back(EMCal_pos_x);
        caloClus_Y.push_back(EMCal_pos_y);
        caloClus_Z.push_back(EMCal_pos_z);
        caloClus_R.push_back(EMCal_pos_r);
        caloClus_Phi.push_back(EMCal_pos_phi);
        caloClus_edep.push_back(EMCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}


int tutorial::prepareiHCalClus(PHCompositeNode * topNode) {
    iHCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_HCALIN");

    if (!iHCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << ihcalClus_node_name << std::endl;
        exit(1);
    }

    // note : iHCal
    for (int i = 0; i < iHCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = iHCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0) {continue;}

        double iHCal_pos_x   = cluster -> get_x();
        double iHCal_pos_y   = cluster -> get_y();
        double iHCal_pos_z   = cluster -> get_z();
        double iHCal_pos_r   = cluster -> get_r();
        double iHCal_pos_phi = cluster -> get_phi();
        double iHCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(1);
        caloClus_X.push_back(iHCal_pos_x);
        caloClus_Y.push_back(iHCal_pos_y);
        caloClus_Z.push_back(iHCal_pos_z);
        caloClus_R.push_back(iHCal_pos_r);
        caloClus_Phi.push_back(iHCal_pos_phi);
        caloClus_edep.push_back(iHCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}


int tutorial::prepareoHCalClus(PHCompositeNode * topNode) {
    oHCal_cluster_cont = findNode::getClass <RawClusterContainer> (topNode, "CLUSTER_HCALIN");

    if (!oHCal_cluster_cont) {
        std::cout << "tutorial::process_event Could not find node " << ohcalClus_node_name << std::endl;
        exit(1);
    }

    // note : oHCal
    for (int i = 0; i < oHCal_cluster_cont -> size(); ++i) //loop over channels 
    {
        RawCluster * cluster = oHCal_cluster_cont  -> getCluster(i); //get EMCal tower
        if (cluster -> get_energy() <= 0) {continue;}

        double oHCal_pos_x   = cluster -> get_x();
        double oHCal_pos_y   = cluster -> get_y();
        double oHCal_pos_z   = cluster -> get_z();
        double oHCal_pos_r   = cluster -> get_r();
        double oHCal_pos_phi = cluster -> get_phi();
        double oHCal_energy  = cluster -> get_energy();

        caloClus_system.push_back(2);
        caloClus_X.push_back(oHCal_pos_x);
        caloClus_Y.push_back(oHCal_pos_y);
        caloClus_Z.push_back(oHCal_pos_z);
        caloClus_R.push_back(oHCal_pos_r);
        caloClus_Phi.push_back(oHCal_pos_phi);
        caloClus_edep.push_back(oHCal_energy);
    }   
    return Fun4AllReturnCodes::EVENT_OK;
}

// === e === ChcKuma add Cal Clus ===============================================



int tutorial::prepareG4Turth(PHCompositeNode * topNode){
    std::cout << "Get PHG4 info.: truth primary vertex" << std::endl;
    m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");   

    if (!m_truth_info)
    {
        std::cout << PHWHERE << "Error, can't find G4TruthInfo" << std::endl;
        exit(1);
    }

    // note : Truth vertex
    auto vrange = m_truth_info->GetPrimaryVtxRange();
    int NTruthPV = 0, NTruthPV_Embeded0 = 0;
    for (auto iter = vrange.first; iter != vrange.second; ++iter) // process all primary vertices
    {
        const int point_id = iter->first;
        PHG4VtxPoint *point = iter->second;
        if (point)
        {
            if (m_truth_info->isEmbededVtx(point_id) == 0)
            {
                TruthPV_trig_x_ = point->get_x();
                TruthPV_trig_y_ = point->get_y();
                TruthPV_trig_z_ = point->get_z();
                
                NTruthPV_Embeded0++;
            }
            NTruthPV++;
        }
    }

    NTruthVtx_ = NTruthPV;

    // note : PHG4Particle
    std::vector<int> tmpv_chargehadron;
    tmpv_chargehadron.clear();
    std::cout << "Get PHG4 info.: truth primary G4Particle" << std::endl;
    const auto prange = m_truth_info->GetPrimaryParticleRange();
    // const auto prange = m_truth_info->GetParticleRange();
    for (auto iter = prange.first; iter != prange.second; ++iter)
    {
        PHG4Particle *ptcl = iter->second;
        // particle->identify();
        if (ptcl)
        {
            PrimaryG4P_PID_.push_back(ptcl->get_pid());
            TLorentzVector p;
            p.SetPxPyPzE(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());
            PrimaryG4P_E_.push_back(ptcl->get_e());
            PrimaryG4P_Pt_.push_back(p.Pt());
            PrimaryG4P_Eta_.push_back(p.Eta());
            PrimaryG4P_Phi_.push_back(p.Phi());

            TString particleclass = TString(TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->ParticleClass());
            bool isStable = (TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Stable() == 1) ? true : false;
            double charge = TDatabasePDG::Instance()->GetParticle(ptcl->get_pid())->Charge();
            bool isHadron = (particleclass.Contains("Baryon") || particleclass.Contains("Meson"));
            bool isChargeHadron = (isStable && (charge != 0) && isHadron);
            if (isChargeHadron)
                tmpv_chargehadron.push_back(ptcl->get_pid());

            PrimaryG4P_ParticleClass_.push_back(particleclass);
            PrimaryG4P_isStable_.push_back(isStable);
            PrimaryG4P_Charge_.push_back(charge);
            PrimaryG4P_isChargeHadron_.push_back(isChargeHadron);

        }
    }
    NPrimaryG4P_ = PrimaryG4P_PID_.size();
    NPrimaryG4P_promptChargeHadron_ = tmpv_chargehadron.size();

    return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
int tutorial::process_event(PHCompositeNode * topNode) {

    prepareTracker(topNode);
    prepareEMCal(topNode);
    prepareiHCal(topNode);
    prepareoHCal(topNode);
    prepareEMCalClus(topNode);
    prepareiHCalClus(topNode);
    prepareoHCalClus(topNode);
    
    prepareG4Turth(topNode);


    nTowers = tower_system.size();
    
    
    tree_out -> Fill();

    return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int tutorial::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "tutorial::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

    NClus = 0;
    clus_system.clear();
    clus_layer.clear();
    clus_adc.clear();
    clus_X.clear();
    clus_Y.clear();
    clus_Z.clear();
    clus_size.clear();
    clus_phi_size.clear();
    clus_z_size.clear();

    nTowers = 0;
    tower_system.clear();
    tower_X.clear();
    tower_Y.clear();
    tower_Z.clear();
    tower_Eta.clear();
    tower_Phi.clear();
    tower_Eta_test.clear();
    tower_Phi_test.clear();
    tower_Eta_bin.clear();
    tower_Phi_bin.clear();
    tower_edep.clear();


    nCaloClus = 0;
    caloClus_system.clear();
    caloClus_X.clear();
    caloClus_Y.clear();
    caloClus_Z.clear();
    caloClus_R.clear();
    caloClus_Phi.clear();
    caloClus_edep.clear();

    // note : Truth primary vertex information
    TruthPV_trig_x_ = -999;
    TruthPV_trig_y_ = -999;
    TruthPV_trig_z_ = -999;
    NTruthVtx_ = 0;
    // note : PHG4 information (from all PHG4Particles)
    NPrimaryG4P_ = 0;
    NPrimaryG4P_promptChargeHadron_ = 0;
    PrimaryG4P_Pt_.clear();
    PrimaryG4P_Eta_.clear();
    PrimaryG4P_Phi_.clear();
    PrimaryG4P_E_.clear();
    PrimaryG4P_PID_.clear();
    PrimaryG4P_ParticleClass_.clear();
    PrimaryG4P_isStable_.clear();
    PrimaryG4P_Charge_.clear();
    PrimaryG4P_isChargeHadron_.clear();


    eventID += 1;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::EndRun(const int runnumber)
{
    std::cout << "tutorial::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::End(PHCompositeNode *topNode)
{
  std::cout << "tutorial::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  ////////////////////////////////////////////////////////
  // Writing objects to the output file                 //
  ////////////////////////////////////////////////////////
  file_out -> cd();
  tree_out -> Write();
  file_out -> Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int tutorial::Reset(PHCompositeNode *topNode)
{
 std::cout << "tutorial::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void tutorial::Print(const std::string &what) const
{
  std::cout << "tutorial::Print(const std::string &what) const Printing info for " << what << std::endl;
}

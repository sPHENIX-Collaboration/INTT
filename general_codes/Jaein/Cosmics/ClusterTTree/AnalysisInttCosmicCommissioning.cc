#include "AnalysisInttCosmicCommissioning.h"

using namespace std;

AnalysisInttCosmicCommissioning::AnalysisInttCosmicCommissioning(const std::string &name,const std::string &output_name):
 SubsysReco(name)
{
  output_name_= output_name;
  c_ = new TCanvas( "name", "title", 800, 800 );
  c_->Print( "xy_plane.pdf[" );
  c_->Print( "yz_plane.pdf[" );
}


AnalysisInttCosmicCommissioning::~AnalysisInttCosmicCommissioning()
{

}

//////////////////////////////////////////////////////////////////////////////
// private ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int AnalysisInttCosmicCommissioning::GetNodes(PHCompositeNode *topNode)
{
  // sample DST //////////////////////////////////////////////////////////
  // TOP (PHCompositeNode)/
  //  DST (PHCompositeNode)/
  //     INTT (PHCompositeNode)/
  //        INTTRAWHIT (IO,InttRawHitContainerv1)
  //        INTTEVENTHEADER (IO,InttEventInfov1)
  //     TRKR (PHCompositeNode)/
  //        TRKR_HITSET (IO,TrkrHitSetContainerv1)
  //        TRKR_CLUSTER (IO,TrkrClusterContainerv4)
  //        TRKR_CLUSTERHITASSOC (IO,TrkrClusterHitAssocv3)
  //        TRKR_CLUSTERCROSSINGASSOC (IO,TrkrClusterCrossingAssocv1)
  //  RUN (PHCompositeNode)/
  //     Flags (IO,FlagSavev1)
  //  PAR (PHCompositeNode)/
  /////////////////////////////////////////////////////////////////////////

  //  List of Nodes in Fun4AllServer:
  //Node Tree under TopNode TOP
  //TOP (PHCompositeNode)/
  //   DST (PHCompositeNode)/
  //      PHHepMCGenEventMap (IO,PHHepMCGenEventMap)
  //      PHG4INEVENT (PHDataNode)
  //      PIPE (PHCompositeNode)/
  //         G4HIT_PIPE (IO,PHG4HitContainer)
  //      MVTX (PHCompositeNode)/
  //         G4HIT_MVTX (IO,PHG4HitContainer)
  //      INTT (PHCompositeNode)/
  //         G4HIT_INTT (IO,PHG4HitContainer)
  //      TPC (PHCompositeNode)/
  //         G4HIT_ABSORBER_TPC (IO,PHG4HitContainer)
  //         G4HIT_TPC (IO,PHG4HitContainer)
  //      TPC_ENDCAP (PHCompositeNode)/
  //         G4HIT_TPC_ENDCAP (IO,PHG4HitContainer)
  //      MICROMEGAS (PHCompositeNode)/
  //         G4HIT_MICROMEGAS (IO,PHG4HitContainer)
  //      MAGNET (PHCompositeNode)/
  //         G4HIT_MAGNET (IO,PHG4HitContainer)
  //      G4HIT_BH_1 (IO,PHG4HitContainer)
  //      BH_FORWARD_PLUS (PHCompositeNode)/
  //         G4HIT_BH_FORWARD_PLUS (IO,PHG4HitContainer)
  //      BH_FORWARD_NEG (PHCompositeNode)/
  //         G4HIT_BH_FORWARD_NEG (IO,PHG4HitContainer)
  //      G4TruthInfo (IO,PHG4TruthInfoContainer)
  //      TRKR (PHCompositeNode)/
  //         TRKR_HITSET (IO,TrkrHitSetContainerv1)
  //         TRKR_HITTRUTHASSOC (IO,TrkrHitTruthAssocv1)
  //         TRKR_CLUSTER (IO,TrkrClusterContainerv4)
  //         TRKR_CLUSTERHITASSOC (IO,TrkrClusterHitAssocv3)
  //         TRKR_CLUSTERCROSSINGASSOC (IO,TrkrClusterCrossingAssocv1)
  //         TRAINING_HITSET (IO,TrainingHitsContainer)
  //      TRKR_TRUTHTRACKCONTAINER (IO,TrkrTruthTrackContainerv1)
  //      TRKR_TRUTHCLUSTERCONTAINER (IO,TrkrClusterContainerv4)
  //      alignmentTransformationContainer (PHDataNode)
  //      alignmentTransformationContainerTransient (PHDataNode)
  //      SVTX (PHCompositeNode)/
  //         SiliconTrackSeedContainer (IO,TrackSeedContainer_v1)
  //         TpcTrackSeedContainer (IO,TrackSeedContainer_v1)
  //         SvtxTrackSeedContainer (IO,TrackSeedContainer_v1)
  //         ActsTrajectories (PHDataNode)
  //         SvtxTrackMap (IO,SvtxTrackMap_v2)
  //         SvtxAlignmentStateMap (PHDataNode)
  //         SvtxVertexMap (IO,SvtxVertexMap_v1)
  //   RUN (PHCompositeNode)/
  //      PHGenIntegral (IO,PHGenIntegralv1)
  //      Flags (IO,FlagSavev1)
  //      FIELD_CONFIG (IO,PHFieldConfigv1)
  //      PIPE (PHCompositeNode)/
  //         G4GEOPARAM_PIPE (IO,PdbParameterMapContainer)
  //         CYLINDERGEOM_PIPE (IO,PHG4CylinderGeomContainer)
  //      MVTX (PHCompositeNode)/
  //         G4GEOPARAM_MVTX (IO,PdbParameterMapContainer)
  //         G4CELLPARAM_MVTX (IO,PdbParameterMap)
  //      INTT (PHCompositeNode)/
  //         G4GEOPARAM_INTT (IO,PdbParameterMapContainer)
  //         DEADMAP_INTT (IO,InttDeadMapv1)
  //         G4CELLPARAM_INTT (IO,PdbParameterMap)
  //      TPC (PHCompositeNode)/
  //         G4GEOPARAM_TPC (IO,PdbParameterMapContainer)
  //         G4CELLPARAM_TPC (IO,PdbParameterMap)
  //         G4TPCPADPLANE (IO,PdbParameterMap)
  //      TPC_ENDCAP (PHCompositeNode)/
  //         G4GEOPARAM_TPC_ENDCAP (IO,PdbParameterMapContainer)
  //      MICROMEGAS (PHCompositeNode)/
  //         G4GEOPARAM_MICROMEGAS (IO,PdbParameterMapContainer)
  //      MAGNET (PHCompositeNode)/
  //         G4GEOPARAM_MAGNET (IO,PdbParameterMapContainer)
  //         CYLINDERGEOM_MAGNET (IO,PHG4CylinderGeomContainer)
  //      G4GEOPARAM_BH_1 (IO,PdbParameterMapContainer)
  //      CYLINDERGEOM_BH_1 (IO,PHG4CylinderGeomContainer)
  //      BH_FORWARD_PLUS (PHCompositeNode)/
  //         G4GEOPARAM_BH_FORWARD_PLUS (IO,PdbParameterMapContainer)
  //         CYLINDERGEOM_BH_FORWARD_PLUS (IO,PHG4CylinderGeomContainer)
  //      BH_FORWARD_NEG (PHCompositeNode)/
  //         G4GEOPARAM_BH_FORWARD_NEG (IO,PdbParameterMapContainer)
  //         CYLINDERGEOM_BH_FORWARD_NEG (IO,PHG4CylinderGeomContainer)
  //      CYLINDERGEOM_MVTX (IO,PHG4CylinderGeomContainer)
  //      CYLINDERGEOM_INTT (IO,PHG4CylinderGeomContainer)
  //      CYLINDERCELLGEOM_SVTX (IO,PHG4TpcCylinderGeomContainer)
  //      CYLINDERGEOM_MICROMEGAS_FULL (IO,PHG4CylinderGeomContainer)
  //      GEOMETRY_IO (IO,PHGeomIOTGeo)
  //   PAR (PHCompositeNode)/
  //      FIELD_MAP (PHDataNode)
  //      PIPE (PHCompositeNode)/
  //         G4GEO_PIPE (PHDataNode)
  //      MVTX (PHCompositeNode)/
  //         G4GEO_MVTX (PHDataNode)
  //      INTT (PHCompositeNode)/
  //         G4GEO_INTT (PHDataNode)
  //         CYLINDERGEOM_INTT (PHDataNode)
  //         G4CELLGEO_INTT (PHDataNode)
  //      TPC (PHCompositeNode)/
  //         G4GEO_TPC (PHDataNode)
  //         G4CELLPAR_TPC (PHDataNode)
  //         G4TPCPADPLANEPAR (PHDataNode)
  //      TPC_ENDCAP (PHCompositeNode)/
  //         G4GEO_TPC_ENDCAP (PHDataNode)
  //      MICROMEGAS (PHCompositeNode)/
  //         G4GEO_MICROMEGAS (PHDataNode)
  //      MAGNET (PHCompositeNode)/
  //         G4GEO_MAGNET (PHDataNode)
  //      G4GEO_BH_1 (PHDataNode)
  //      BH_FORWARD_PLUS (PHCompositeNode)/
  //         G4GEO_BH_FORWARD_PLUS (PHDataNode)
  //      BH_FORWARD_NEG (PHCompositeNode)/
  //         G4GEO_BH_FORWARD_NEG (PHDataNode)
  //      G4GDML_CONFIG (PHDataNode)
  //      GEOMETRY (PHDataNode)
  //      SVTX (PHCompositeNode)/
  //         ActsGeometry (PHDataNode)
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // INTT event header
  string node_name_intteventheader = "INTTEVENTHEADER";
  node_intteventheader_map_ =
    findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader );

  if(!node_intteventheader_map_)
    {
      cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /////////////////////////////////////////////////////////////////////////
  // INTT raw hit
  string node_name_inttrawhit = "INTTRAWHIT";
  node_inttrawhit_map_ =
    findNode::getClass<InttRawHitContainer>(topNode, node_name_inttrawhit );

  if(!node_inttrawhit_map_)
    {
      cerr << PHWHERE << node_name_inttrawhit << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }


  /////////////////////////////////////////////////////////////////////////
  // TrkrCluster
  // TRKR_CLUSTER node: Information of TrkrCluster
  string node_name_trkr_cluster = "TRKR_CLUSTER";
  node_cluster_map_ =
    findNode::getClass<TrkrClusterContainerv4>(topNode, node_name_trkr_cluster);

  if(!node_cluster_map_ )
    {
      std::cerr << PHWHERE << node_name_trkr_cluster << " node is missing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  // ActsGeometry node: for the global coordinate
  string node_name_acts = "ActsGeometry";
  node_acts_ = findNode::getClass<ActsGeometry>(topNode, node_name_acts);
  if( !node_acts_ )
    {
      std::cout << PHWHERE << node_name_acts << " node is missging." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }


  return 0;
}

vector < pair < TrkrCluster*, const Acts::Vector3 > >
AnalysisInttCosmicCommissioning::GetClusters()
{
  vector < pair < TrkrCluster*, const Acts::Vector3 > > values;

  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
    {

      // loop over all hits
      for (const auto &hitsetkey : node_cluster_map_->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3) )
	{

	  // type: std::pair<ConstIterator, ConstIterator> ConstRange
	  // here, MMap_::const_iterator ConstIterator;
	  auto range = node_cluster_map_->getClusters(hitsetkey);

	  // loop over iterators of this cluster
	  for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	    {
	      const auto cluskey = clusIter->first;
	      const auto cluster = clusIter->second;
	      const auto globalPos = node_acts_->getGlobalPosition(cluskey, cluster);

	      cluster->setPosition(0,  globalPos.x() );
	      cluster->setPosition(1,  globalPos.y() );
	      cluster->setPosition(2,  globalPos.z() );
	      
	      pair < TrkrCluster*, const Acts::Vector3 > val( cluster, globalPos );
	      values.push_back( val );
	      
	    }
	}
    }
  
  return values;
}

//////////////////////////////////////////////////////////////////////////////
// public ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


int AnalysisInttCosmicCommissioning::Init(PHCompositeNode *topNode)
{
  std::string outFilename = output_path_+output_name_;

  outFile_ = new TFile(outFilename.c_str(),"RECREATE");
  n_cluster_ =0; 
  slope_xy_ =0; 
  constant_xy_ =0;
  slope_yz_ =0; 
  constant_yz_ =0;
  cluster_size_.clear(); 
  posX_.clear();
  posY_.clear();
  posZ_.clear();
  posZ_.clear();
  outTree_ = new TTree("cluster_tree","cluster_tree");
  outTree_ -> Branch("n_cluster",&n_cluster_);
  outTree_ -> Branch("event",&misc_counter_); // misc_counter_ = event number
  outTree_ -> Branch("bco",&bco_); // misc_counter_ = event number
  outTree_ -> Branch("posX",&posX_);
  outTree_ -> Branch("posY",&posY_);
  outTree_ -> Branch("posZ",&posZ_);
  outTree_ -> Branch("radius",&radius_);
  outTree_ -> Branch("adc",&adc_);
  outTree_ -> Branch("cluster_size",&cluster_size_);
  outTree_ -> Branch("slope_xy",&slope_xy_);
  outTree_ -> Branch("constant_xy",&constant_xy_);
  outTree_ -> Branch("chi2_xy",&chi2_xy_);
  outTree_ -> Branch("ndf_xy",&ndf_xy_);
  outTree_ -> Branch("chi2ndf_xy",&chi2ndf_xy_);
  outTree_ -> Branch("average_dist_xy",&average_dist_xy_);
  outTree_ -> Branch("average_dist_yz",&average_dist_yz_);
  outTree_ -> Branch("slope_yz",&slope_yz_);
  outTree_ -> Branch("constant_yz",&constant_yz_);
  outTree_ -> Branch("chi2_yz",&chi2_yz_);
  outTree_ -> Branch("ndf_yz",&ndf_yz_);
  outTree_ -> Branch("chi2ndf_yz",&chi2ndf_yz_);
  return Fun4AllReturnCodes::EVENT_OK;
}


int AnalysisInttCosmicCommissioning::InitRun(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::process_event(PHCompositeNode *topNode)
{

  auto status = this->GetNodes(topNode);
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  cout << "Event: " << misc_counter_ << "\t"
       << setprecision(15) << setw(17) << node_intteventheader_map_->get_bco_full()
       << endl;
  misc_counter_++;

  if (false)
  {
    cout << string(50, '=') << "\n"
         << " Event information: "
         << "bco full: " << setw(16) << node_intteventheader_map_->get_bco_full()
         << endl;
  }

  auto raw_hit_num = node_inttrawhit_map_->get_nhits();
  cout << "#INTT RAW HIT:" << raw_hit_num << endl;
  for (unsigned int i = 0; i < raw_hit_num; i++)
  {
    auto hit = node_inttrawhit_map_->get_hit(i);
    if (i == 0)
      bco_ = hit->get_bco();
    if (false)
    {
      cout << setw(5) << i << " "
           << setw(4) << hit->get_packetid() << " "   // Packet ID
           << setw(2) << hit->get_fee() << " "        // FELIX CH (module)
           << setw(2) << hit->get_chip_id() << " "    // chip_id
           << setw(3) << hit->get_channel_id() << " " // chan_id
           << setw(1) << hit->get_adc() << " "        // adc
           << setw(3) << hit->get_FPHX_BCO() << " "   // bco
           << setw(12) << hit->get_bco() << " "       // bco_full
           << setw(3) << (hit->get_bco() & 0x7F) - hit->get_FPHX_BCO() << " "
           << endl;
    }
  }

  // cout << "#" << node_name_inttrawhit << " " << node_inttrawhit_map_->get_nhits() << "\t"
  //      << "#" << node_name_trkr_cluster << " " << node_cluster_map_->size()
  //      << endl;

//  return Fun4AllReturnCodes::EVENT_OK;
  auto cluster_pos_pair = this->GetClusters(); // vector < pair < TrkrCluster*, const Acts::vector3 > >
  if (cluster_pos_pair.size() != 0)
  {

    cout << setw(5) << node_inttrawhit_map_->get_nhits() << " "
         << setw(5) << cluster_pos_pair.size()
         << endl;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // event selection /////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

//  if (raw_hit_num > 40)
//    return Fun4AllReturnCodes::EVENT_OK;

//  if (cluster_pos_pair.size() > 10 || cluster_pos_pair.size() < 3)
//    return Fun4AllReturnCodes::EVENT_OK;

  vector<double> distances;
  for (int i = 0; i < int(cluster_pos_pair.size()); i++)
  {
    auto pos_cluster1 = cluster_pos_pair[i].second;
    for (int j = i + 1; j < int(cluster_pos_pair.size()); j++)
    {
      auto pos_cluster2 = cluster_pos_pair[j].second;

      // auto pos_diff = pos_cluster1 - pos_cluster2;

      distances.push_back(GetDistanceXY(pos_cluster1, pos_cluster2));
    }
  }

  //if (*max_element(distances.begin(), distances.end()) < 8)
  //  return Fun4AllReturnCodes::EVENT_OK;

  TGraph *g = new TGraph(); //For xy plane
  g->SetMarkerStyle(20);
  g->SetMarkerColor(kBlue);
  TGraph *g_2 = new TGraph(); // for rz plane
  g_2->SetMarkerStyle(20);
  g_2->SetMarkerColor(kBlue);
  std::cout << "test" << std::endl;
  int i=0;
  for (auto &val : cluster_pos_pair)
  {
    auto cluster = val.first;
    double clus_x = cluster->getPosition(0);
    double clus_y = cluster->getPosition(1);
    double clus_z = cluster->getPosition(2);
    double rad = std::sqrt((clus_x) * (clus_x) + (clus_y) * (clus_y) + (clus_z) * (clus_z));
    //  if (false)
    {
      if( clus_y < 0)
      {
        rad = -rad;
      }
      std::cout
          // << "xyz("
          //    << std::setprecision(4) << std::setw(8) << globalPos.x() << ", "
          //    << std::setprecision(4) << std::setw(8) << globalPos.y() << ", "
          //    << std::setprecision(4) << std::setw(8) << globalPos.z()
          //    << ") \t"
          << "xyz("
          << std::setprecision(4) << std::setw(8) << cluster->getPosition(0) << ", "
          << std::setprecision(4) << std::setw(8) << cluster->getPosition(1) << ", "
          << std::setprecision(4) << std::setw(8) << cluster->getPosition(2) << ") "
          << "local xy("
          << std::setprecision(4) << std::setw(8) << cluster->getLocalX() << ", "
          << std::setprecision(4) << std::setw(8) << cluster->getLocalY() << ")\t "

          << cluster->getAdc() << " "
          << cluster->getSize() << " "


          //	<< inttlayer << " "
          // << ladder_z << " "
          // << ladder_phi
          << std::endl;

          posX_.push_back(cluster->getPosition(0));
          posY_.push_back(cluster->getPosition(1));
          posZ_.push_back(cluster->getPosition(2));
          radius_.push_back(rad);
          adc_.push_back(cluster->getAdc());
          cluster_size_.push_back(cluster->getSize());
          n_cluster_++;
    }

    g->AddPoint(cluster->getPosition(0), cluster->getPosition(1)); // X-axis(Tgraph) = X direction(Real); Y-axis(Tgraph) = Y direction(Real)
    g_2->AddPoint(cluster->getPosition(1), cluster->getPosition(2)); // X-axis(TGraph) = Z direction(Real); Y-axis(TGraph) = radius(Real)
    i++; 
  }
  if(g->GetN()>=4 && g->GetN()<=8) //Condition to draw pdf and outTree_->Fill()
  {
    /////////////////////////////// XY plane ////////////////////////////
    g->SetTitle(Form("Event : %d", misc_counter_));
    c_->DrawFrame(-11, -11, 11, 11);
    c_->SetTitle(Form("Event : %d", misc_counter_));
    g->Draw("P");
    TF1 *fitFunc = new TF1("fitFunc", "[0] + [1]*x", -11, 11); // y=ax+b
    g->Fit(fitFunc,"RQ");
    c_->Update();
    c_->Print("xy_plane.pdf");
    slope_xy_ = fitFunc->GetParameter(1);
    constant_xy_ = fitFunc->GetParameter(0);
    chi2_xy_ = fitFunc->GetChisquare(); 
    ndf_xy_ = fitFunc->GetNDF(); 
    chi2ndf_xy_ = chi2_xy_/ndf_xy_; 
    ///////////////////// Calculate the average distance ///////////////////
    double sumDistance = 0.0;
    for(int i=0;i<g->GetN();++i)
    {
      double x,y;
      g->GetPoint(i,x,y);
      double distance = TMath::Abs((slope_xy_*x-y+constant_xy_)) / TMath::Sqrt(1+slope_xy_*slope_xy_);
      sumDistance += distance;
    }
    average_dist_xy_ = sumDistance / g->GetN();
  ////////////////////////////////// YZ plane ////////////////////////////// 
    c_->DrawFrame(-11, -25, 11, 25);
    g_2->Draw("P");
    TF1 *fitFunc_2 = new TF1("fitFunc_2", "[0] + [1]*x", -11, 11); 
    g_2->Fit(fitFunc_2,"RQ");
    c_->Update(); 
    c_->Print("yz_plane.pdf");
    slope_yz_ = fitFunc_2->GetParameter(1);
    constant_yz_ = fitFunc_2->GetParameter(0);
    chi2_yz_ = fitFunc_2->GetChisquare(); 
    ndf_yz_ = fitFunc_2->GetNDF();
    sumDistance = 0.0;
    for (int i = 0; i < g->GetN(); ++i)
    {
      double x, y;
      g->GetPoint(i, x, y);
      double distance = TMath::Abs((slope_yz_ * x - y + constant_yz_)) / TMath::Sqrt(1 + slope_yz_ * slope_yz_);
      sumDistance += distance;
    }
    average_dist_yz_ = sumDistance / g->GetN();

    chi2ndf_yz_ = chi2_yz_/ndf_yz_; 
    outTree_->Fill();


    delete fitFunc;
    delete fitFunc_2;
  }
  // initialize branches
  posX_.clear();
  posY_.clear();
  posZ_.clear();
  adc_.clear();
  cluster_size_.clear();
  n_cluster_ = 0;
  slope_xy_ = -99999;
  constant_xy_ = -99999;
  average_dist_xy_ = -99999;
  average_dist_yz_ = -99999;
  slope_yz_ = -99999;
  constant_yz_ = -99999;
  std::cout << "HELLO" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}


int AnalysisInttCosmicCommissioning::EndRun(const int runnumber)
{
  cout << "int AnalysisInttCosmicCommissioning::EndRun(const int runnumber)" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


int AnalysisInttCosmicCommissioning::End(PHCompositeNode *topNode)
{
  c_->DrawFrame(-11,-11,11,11);
  c_->Update();
  c_->Print( "xy_plane.pdf]" );
  c_->Update();
  c_->Print( "yz_plane.pdf]" );
  outFile_->cd();
  outTree_->Write();
  outFile_->Close();
  delete outFile_;
  delete outTree_;
  
  return Fun4AllReturnCodes::EVENT_OK;
}


int AnalysisInttCosmicCommissioning::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}


void AnalysisInttCosmicCommissioning::Print(const std::string &what) const
{

}

double AnalysisInttCosmicCommissioning::GetDistance( const Acts::Vector3 a, const Acts::Vector3 b, bool use_x, bool use_y, bool use_z )
{
  double num = 0.0;
  if( use_x )
    num += a.x() * b.x();
      
  if( use_y )
    num += a.y() * b.y();
      
  if( use_z )
    num += a.z() * b.z();
      
  return sqrt( num );
}

void AnalysisInttCosmicCommissioning::SetData(const std::string path )
{
  // File name assumption: test_DST_cosmics_intt_00026975.root
  // -> [additional_tag]_DST_[run type]_intt_[run number].root
  
  data_ = path; // the path to the DST data

  ////////////////////////////////////////////////////////////////////////////////
  // information /////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  string run_num_str = data_.substr( data_.find_last_of( "_" ), data_.size() );
  run_num_str = run_num_str.substr( 1, run_num_str.size() - string( ".root" ).size() );
  cout << run_num_str << endl;
  run_num_ = stoi( run_num_str );

  ////////////////////////////////////////////////////////////////////////////////
  // for output //////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  auto slash_position = data_.find_last_of( "/" );
  if( slash_position == string::npos )
    slash_position = 0;

  string data_basename = data_.substr( slash_position, data_.size() ); // including the prefix (.root)
  data_basename = data_basename.substr( 0, data_basename.size() - 5 ); // the prefix (.root) is removed.

  cout << "data basename: " << data_basename << endl;
  output_pdf_ = output_path_ ;
  if( output_path_[ output_path_.size() -1 ] != '/' )
    {
      output_pdf_ += "/";
    }

  output_pdf_ += data_basename + ".pdf";
  
  cout << "PDF output: " << output_pdf_ << endl;
}

//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in AnalysisInttCosmicCommissioning.h.
//
// AnalysisInttCosmicCommissioning(const std::string &name = "AnalysisInttCosmicCommissioning")
// everything is keyed to AnalysisInttCosmicCommissioning, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// AnalysisInttCosmicCommissioning::~AnalysisInttCosmicCommissioning()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int AnalysisInttCosmicCommissioning::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int AnalysisInttCosmicCommissioning::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int AnalysisInttCosmicCommissioning::process_event(PHCompositeNode *topNode)
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
// int AnalysisInttCosmicCommissioning::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int AnalysisInttCosmicCommissioning::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int AnalysisInttCosmicCommissioning::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int AnalysisInttCosmicCommissioning::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void AnalysisInttCosmicCommissioning::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//


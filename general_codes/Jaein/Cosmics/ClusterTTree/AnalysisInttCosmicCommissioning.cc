#include "AnalysisInttCosmicCommissioning.h"

using namespace std;

AnalysisInttCosmicCommissioning::AnalysisInttCosmicCommissioning(const std::string &name, const std::string &output_name) : SubsysReco(name)
{
  output_name_ = output_name;
  c_ = new TCanvas("name", "title", 800, 800);

  //
  // +-------------+
  // |  xy  |  zy  |
  // +-------------+
  // |      |  zx  |
  // +-------------+

  c_->Divide(2, 2);

  vcoordinates_.push_back(pair<int, int>(0, 1)); // x vs y
  vcoordinates_.push_back(pair<int, int>(2, 1)); // z vs y
  vcoordinates_.push_back(pair<int, int>(2, 0)); // z vs x
  vcoordinates_.push_back(pair<int, int>(2, 3)); // z vs r

  for (unsigned int i = 0; i < vcoordinates_.size(); i++)
  {
    string name = "line_" + Int2Coordinate(vcoordinates_[i].first) + Int2Coordinate(vcoordinates_[i].second);

    // Setting range of the fitting function
    // 2 means z, so 25 is enough. Others can be x or y, so 11 is enough.
    double xmax = (vcoordinates_[i].first == 2 ? 25 : 11);
    lines_[i] = new TF1(name.c_str(), "pol1", -xmax, xmax);
  }

}

AnalysisInttCosmicCommissioning::~AnalysisInttCosmicCommissioning()
{
}



//////////////////////////////////////////////////////////////////////////////
// private ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void AnalysisInttCosmicCommissioning::DrawIntt(double xmax, double ymax)
{

  if (xmax - ymax < 1e-4) // cross-section mode
  {
    for (int i = 0; i < 4; i++)
    {
      auto circle = new TEllipse(0.0, 0.0, kLayer_radii[i]);
      circle->SetLineColorAlpha(kGray, 0.5);
      circle->SetLineWidth(2);
      circle->SetFillStyle(0);
      circle->Draw("same");
    }
  }
  else // z-y or z-x
    {
      for( int i=0; i<4; i++ )
	{
	  auto box = new TBox( -20, -kLayer_radii[i], 20, kLayer_radii[i] );
	  box->SetLineColorAlpha( kGray, 0.5 );
	  box->SetLineWidth( 2 );
	  box->SetFillStyle( 0 );
	  box->Draw( "same" );
	}
    }
}

int AnalysisInttCosmicCommissioning::Fit()
{

  bool is_fitting_good = false;
  c_->Clear();
  c_->Divide(2, 2);
  
  for (unsigned int i = 0; i < vcoordinates_.size(); i++)
  {
    // if (graphs_[i]->GetN() < 4 || graphs_[i]->GetN() > 8) // Condition to draw pdf and outTree_->Fill()
    //   return 0; // Not used. Event selection should be done in process_event.

    if( IsFittable( graphs_[i] ) == true )
      {
	graphs_[i]->Fit(lines_[i], "RQN");
	
	constants_[i] = lines_[i]->GetParameter(0);
	slopes_[i] = lines_[i]->GetParameter(1);
	chi2ndfs_[i] = lines_[i]->GetChisquare() / lines_[i]->GetNDF();
	
	///////////////////// Calculate the average distance ///////////////////
	double sumDistance = 0.0;
	for (int i = 0; i < graphs_[i]->GetN(); ++i)
	  {
	    double x, y;
	    graphs_[i]->GetPoint(i, x, y);
	    double distance = TMath::Abs((slopes_[i] * x - y + constants_[i])) / TMath::Sqrt(1 + slopes_[i] * slopes_[i]);
	    sumDistance += distance;
	  }
	average_distances_[i] = sumDistance / graphs_[i]->GetN();
	
      }
    
    graphs_[i]->SetTitle(Form("Event : %d", misc_counter_));
    int x_axis_num = vcoordinates_[i].first;
    int y_axis_num = vcoordinates_[i].second;
    double xmax = x_axis_num == 2 ? 25 : 11; // 2 means z, so 25 is enough. Others can be x or y, so 11 is enough.
    double ymax = y_axis_num == 2 ? 25 : 11;

    c_->cd(i + 1);
    if (i == 2)
      c_->cd(4);
    else if (i == 3)
      c_->cd(3);

    string frame_title = Int2Coordinate(x_axis_num) + " vs " + Int2Coordinate(y_axis_num) +" Event : "+std::to_string(misc_counter_) +";" // pad title
                         + Int2Coordinate(x_axis_num) + " (cm);"                                // x-axis title
                         + Int2Coordinate(y_axis_num) + " (cm)";                                // y-axis title
    auto frame = gPad->DrawFrame(-xmax, -ymax, xmax, ymax, frame_title.c_str());
    frame->GetXaxis()->CenterTitle();
    frame->GetYaxis()->CenterTitle();
    gPad->SetGrid(true, true);
    DrawIntt(xmax, ymax);

    c_->SetTitle(Form("Event : %d", misc_counter_));
    graphs_[i]->Draw("P");
    lines_[i]->Draw( "same" );
    c_->Update();
  }

  if( IsYFired_[0] && IsYFired_[1] )
    {
      if( print_counter_ < 101 )
	{
	  c_->Print(output_pdf_file_.c_str());
	}
      print_counter_++;
    }
  
  n_cluster_ = posX_.size();

  // evaluate fitting quality
  if( is_fitting_good == true )
    {
      slope_consistency_ = (slopes_[0] - (slopes_[2] / slopes_[1]) ) / slopes_[0]; // {a - e/c} / a 
      constant_consistency_ = (constants_[0] - (constants_[2] - constants_[1]) / slopes_[1] ) / constants_[0]; // {b - (f-d)/c} / b
    }
  
  outTree_->Fill();
  std::cout << outTree_->GetEntries() << std::endl;

  return 0;
}

vector<pair<TrkrCluster *, const Acts::Vector3>>
AnalysisInttCosmicCommissioning::GetClusters()
{
  vector<pair<TrkrCluster *, const Acts::Vector3>> values;

  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {

    // loop over all hits
    for (const auto &hitsetkey : node_cluster_map_->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
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

        cluster->setPosition(0, globalPos.x());
        cluster->setPosition(1, globalPos.y());
        cluster->setPosition(2, globalPos.z());

        pair<TrkrCluster *, const Acts::Vector3> val(cluster, globalPos);
        values.push_back(val);
      }
    }
  }

  return values;
}

double AnalysisInttCosmicCommissioning::GetDistance(const Acts::Vector3 a, const Acts::Vector3 b, bool use_x, bool use_y, bool use_z)
{
  double num = 0.0;
  if (use_x)
    num += a.x() * b.x();

  if (use_y)
    num += a.y() * b.y();

  if (use_z)
    num += a.z() * b.z();

  return sqrt(num);
}

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
  // Node Tree under TopNode TOP
  // TOP (PHCompositeNode)/
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
      findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader);

  if (!node_intteventheader_map_)
  {
    cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /////////////////////////////////////////////////////////////////////////
  // INTT raw hit
  string node_name_inttrawhit = "INTTRAWHIT";
  node_inttrawhit_map_ =
      findNode::getClass<InttRawHitContainer>(topNode, node_name_inttrawhit);

  if (!node_inttrawhit_map_)
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

  if (!node_cluster_map_)
  {
    std::cerr << PHWHERE << node_name_trkr_cluster << " node is missing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // ActsGeometry node: for the global coordinate
  string node_name_acts = "ActsGeometry";
  node_acts_ = findNode::getClass<ActsGeometry>(topNode, node_name_acts);
  if (!node_acts_)
  {
    std::cout << PHWHERE << node_name_acts << " node is missging." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return 0;
}

void AnalysisInttCosmicCommissioning::InitPaths()
{
  /*
  auto slash_position = output_name_.find_last_of("/");
  if (slash_position == string::npos)
    slash_position = 0;
  else
    slash_position++;
      
  string output_basename = output_name_.substr(slash_position, output_name_.size()); // including the prefix (.root)
  output_pdf_ = output_basename.substr(0, output_basename.size() - 5) + ".pdf";
  
  / *
      string output_basename = output_name_.substr( slash_position, output_name_.size() ); // including the prefix (.root)
  output_pdf_ = output_name_.substr( 0, output_name_.find_last_of( "/" ) )  + "/../plots/"
    + output_basename.substr( 0, output_basename.size()-5 ) + ".pdf";
  
  */

  string output_root_path = output_path_ + "root/" + to_string( year_ ) + "/";
  output_root_file_ = output_root_path + output_name_;
  
  string output_pdf_path = output_path_ + "plots/" + to_string( year_ ) + "/";
  output_pdf_file_ = output_pdf_path + output_name_.substr( 0, output_name_.size() - 5 ) + ".pdf";
}

std::string AnalysisInttCosmicCommissioning::Int2Coordinate(int num)
{
  if (num == 0)
    return "x";
  else if (num == 1)
    return "y";
  else if (num == 2)
    return "z";
  else if (num == 3)
    return "r";

  return "unkown_coordinate_num" + to_string(num);
}

bool AnalysisInttCosmicCommissioning::IsFittable( TGraphErrors* g )
{
  /*!
    @brief Check whether this graph can be fitting with pol1
    @details If all clusters are on the same chip columns, they have the same z-coordinate (if ladder positions are not modified by the survery geometry). In this case, this graph cannot be fitted.
  */

  // get all x values
  vector < double > values;
  for( int i=0; i<g->GetN(); i++ )
    {
      double x, y;
      g->GetPoint( i, x, y );
      values.push_back( x );
    }

  // compare y values
  for( unsigned int i=1; i<values.size(); i++ )
    {
      // if the difference is larger than the length of a chip type-A, it's OK to be fitted.
      if( fabs(values[0] - values[i]) >= 0.16 )
	return true;
    }

  // If you come here, all comparisons showed smaller value than the length of a chip type-A. OK, don't fit.
  return false;
}

int AnalysisInttCosmicCommissioning::MakeGraphs(vector<pair<TrkrCluster *, const Acts::Vector3> > &cluster_pos_pairs)
{
  // init
  for (unsigned int i = 0; i < vcoordinates_.size(); i++)
  {
    graphs_[i] = new TGraphErrors();
    graphs_[i]->SetMarkerStyle(20);
    graphs_[i]->SetMarkerColor(kAzure + 1);
  }

  for (auto &val : cluster_pos_pairs)
  {

    auto cluster = val.first;

    posX_.push_back(cluster->getPosition(0));
    posY_.push_back(cluster->getPosition(1));
    posZ_.push_back(cluster->getPosition(2));
    
    if (cluster->getPosition(1) > 0)
      IsYFired_[0] = true; // cluster y>0
    else
      IsYFired_[1] = true; //cluster y<=0
    
    double rad = 0.0; // distance of the cluster point from the origin of the coodinate
    // loop over 3 graphs
    for (unsigned int i = 0; i < vcoordinates_.size(); i++)
    {
      if (vcoordinates_[i].first == 3 || vcoordinates_[i].second == 3) //If the coordinate is r
	{
	  continue;
	}
      else if (i < 2)
	{
	  //radius in xy plane
	  rad += cluster->getPosition(i) * cluster->getPosition(i); // add a squared distance in i-th coordinate
	}
      
      graphs_[i]->AddPoint(cluster->getPosition(vcoordinates_[i].first),   // x, z, z
                           cluster->getPosition(vcoordinates_[i].second)); // y, y, x
    }
    
    // Apply  squart. Sign depends on the sign of y coordinate.
    rad = sqrt(rad) * (posY_[posY_.size() - 1] < 0 ? -1 : 1);
    //graph for rz plane
    graphs_[3]->AddPoint(cluster->getPosition(2),rad);

    radius_.push_back(rad);
    adc_.push_back(cluster->getAdc());
    cluster_size_.push_back(cluster->getSize());

    //cout << cluster->getPosition(2) << "\t" << cluster->getPhiSize() << "\t" << cluster->getZSize() << endl;
  }

  return 0;
}

int AnalysisInttCosmicCommissioning::ProcessEventRawHit()
{
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
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// public ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int AnalysisInttCosmicCommissioning::Init(PHCompositeNode *topNode)
{
  InitPaths();
  std::string outFilename = output_root_file_; // output_name_;

  outFile_ = new TFile(outFilename.c_str(), "RECREATE");

  outTree_ = new TTree("cluster_tree", "cluster_tree");

  // event information
  outTree_->Branch("n_cluster", &n_cluster_);
  outTree_->Branch("event", &misc_counter_); // misc_counter_ = event number
  outTree_->Branch("bco", &bco_);            // misc_counter_ = event number

  // cluster information
  outTree_->Branch("posX", &posX_);
  outTree_->Branch("posY", &posY_);
  outTree_->Branch("posZ", &posZ_);

  outTree_->Branch("radius", &radius_);
  outTree_->Branch("adc", &adc_);
  outTree_->Branch("cluster_size", &cluster_size_);

  // fitted line information
  for (unsigned int i = 0; i < vcoordinates_.size(); i++)
  {
    string coordinate_name = Int2Coordinate(vcoordinates_[i].first) + Int2Coordinate(vcoordinates_[i].second);

    string line_name = "line_" + coordinate_name;
    outTree_->Branch(line_name.c_str(), &lines_[i]);

    string slope_name = "slope_" + coordinate_name;
    outTree_->Branch(slope_name.c_str(), &slopes_[i]);

    string constant_name = "constant_" + coordinate_name;
    outTree_->Branch(constant_name.c_str(), &constants_[i]);

    string chi2ndf_name = "chi2ndf_" + coordinate_name;
    outTree_->Branch(chi2ndf_name.c_str(), &chi2ndfs_[i]);

    string average_dist_name = "average_dist_" + coordinate_name;
    outTree_->Branch(average_dist_name.c_str(), &average_distances_[i]);
  }

  outTree_->Branch( "slope_consistency", &slope_consistency_ );
  outTree_->Branch( "constant_consistency", &constant_consistency_ );
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::InitRun(PHCompositeNode *topNode)
{
  
  c_->Print( (output_pdf_file_+"[").c_str() );
  this->ResetEvent( topNode );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::process_event(PHCompositeNode *topNode)
{
  auto status = this->GetNodes(topNode);
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  // cout << "Event: " << misc_counter_ << "\t"
  //      << setprecision(15) << setw(17) << node_intteventheader_map_->get_bco_full();
  
  misc_counter_++;
  bco_ = node_intteventheader_map_->get_bco_full();
  
  if (false)
  {
    cout << string(50, '=') << "\n"
         << " Event information: "
         << "bco full: " << setw(16) << node_intteventheader_map_->get_bco_full()
         << endl;
  }
  bco_ = node_intteventheader_map_->get_bco_full();
  if (false)
    this->ProcessEventRawHit();

  auto cluster_pos_pair = this->GetClusters(); // vector < pair < TrkrCluster*, const Acts::vector3 > >

  ////////////////////////////////////////////////////////////////////////////////
  // event selection /////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  //  if (raw_hit_num > 40)
  //    return Fun4AllReturnCodes::EVENT_OK;  
  //  if (cluster_pos_pair.size() > 10 || cluster_pos_pair.size() < 3)
  //    return Fun4AllReturnCodes::EVENT_OK;  
  //    return Fun4AllReturnCodes::EVENT_OK;

  // analyze events having 4 <= #cluster <= 5 only
  if (cluster_pos_pair.size() < 4 || cluster_pos_pair.size() > 5 )
    return Fun4AllReturnCodes::EVENT_OK;
  
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

  this->MakeGraphs(cluster_pos_pair);
  // if (*max_element(distances.begin(), distances.end()) < 8)
  //   return Fun4AllReturnCodes::EVENT_OK;
  
  // if (!(IsYFired_[0] && IsYFired_[1]))
  //   return Fun4AllReturnCodes::EVENT_OK;
  
  this->Fit();

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::ResetEvent(PHCompositeNode *topNode)
{

  for( unsigned int i=0; i<vcoordinates_.size(); i++ )
    {
      slopes_[i]
	= constants_[i]
	= chi2ndfs_[i]
	= average_distances_[i]
	= -9999;
    }
  
  // initialize branches
  posX_.clear();
  posY_.clear();
  posZ_.clear();

  adc_.clear();
  cluster_size_.clear();

  n_cluster_
    = slope_xy_ =  constant_xy_ =   average_dist_xy_
    = slope_yz_ =  constant_yz_ =   average_dist_yz_
    = slope_consistency_ = constant_consistency_
    = -99999;

  //initialize bool
  IsYFired_[0] = IsYFired_[1] = false;
  
  //Event counter
  misc_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::EndRun(const int runnumber)
{

  c_->Print( (output_pdf_file_+"]").c_str() );

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::End(PHCompositeNode *topNode)
{

  // c_->DrawFrame(-11,-11,11,11);
  // c_->Update();
  // c_->Print( "xy_plane.pdf]" );
  // c_->Update();
  // c_->Print( "yz_plane.pdf]" );
  
  outFile_->cd();
  outTree_->Write();
  outFile_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnalysisInttCosmicCommissioning::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void AnalysisInttCosmicCommissioning::Print(const std::string &what) const
{

}

/*
void AnalysisInttCosmicCommissioning::SetData(const std::string path )
{
  // File name assumption: test_DST_cosmics_intt_00026975.root
  // obsolete: -> [additional_tag]_DST_[run type]_intt_[run number].root
  // -> DST_[run type]_intt_[run number]_no_hot_clusterized.root
  // For example: /sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_cosmics_intt_00038984_no_hot_clusterized.root

  if( path != "" ) // If nothing is give, something in data_ is used anyway. It's useful for updating the output path.
    data_ = path; // the path to the DST data

  if( data_ == "" )
    {
      cerr << "void AnalysisInttCosmicCommissioning::SetData(const std::string path )" << endl;
      cerr << "No file is set. Something wrong." << endl;
      return;
    }

  if( path != "" ) // If nothing is give, something in data_ is used anyway. It's useful for updating the output path.
    data_ = path; // the path to the DST data

  if( data_ == "" )
    {
      cerr << "void AnalysisInttCosmicCommissioning::SetData(const std::string path )" << endl;
      cerr << "No file is set. Something wrong." << endl;
      return;
    }
  
  ////////////////////////////////////////////////////////////////////////////////
  // information /////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  // string run_num_str = data_.substr( data_.find_last_of( "_" ), data_.size() );
  // run_num_str = run_num_str.substr( run_num_str.find_last_of( "intt" ), run_num_str.size() );
  //run_num_str = run_num_str.substr( 1, run_num_str.size() - string( ".root" ).size() );

  string run_num_str = data_.substr( data_.find_last_of( "/" ), data_.size() ); // removing path

  // /DST_cosmics_intt_00038984_no_hot_clusterized.root -> cosmics_intt_00038984_no_hot_clusterized.root
  run_num_str = run_num_str.substr( run_num_str.find_first_of( "_" )+1, run_num_str.size() );

  // cosmics_intt_00038984_no_hot_clusterized.root -> intt_00038984_no_hot_clusterized.root
  run_num_str = run_num_str.substr( run_num_str.find_first_of( "_" )+1, run_num_str.size() );

  // intt_00038984_no_hot_clusterized.root -> 00038984_no_hot_clusterized.root
  run_num_str = run_num_str.substr( run_num_str.find_first_of( "_" )+1, run_num_str.size() );

  // 00038984_no_hot_clusterized.root -> 00038984
  run_num_str = run_num_str.substr( 0, run_num_str.find_first_of( "_" ) );

  run_num_ = stoi( run_num_str );

  ////////////////////////////////////////////////////////////////////////////////
  // for output //////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  auto slash_position = data_.find_last_of( "/" );
  if( slash_position == string::npos )
    slash_position = 0;

  string data_basename = data_.substr( slash_position, data_.size() ); // including the prefix (.root)
  data_basename = data_basename.substr( 0, data_basename.size() - 5 ); // the prefix (.root) is removed.

  output_pdf_ = output_path_ ;
  if( output_path_[ output_path_.size() -1 ] != '/' )
    {
      output_pdf_ += "/";
    }

  output_pdf_ += data_basename + ".pdf";

  cout << "PDF output: " << output_pdf_ << endl;
}
*/

void AnalysisInttCosmicCommissioning::SetOutputPath( string path )
{
  output_path_ = path;
  InitPaths();
  //  this->SetData( "" ); // updating the output path
};


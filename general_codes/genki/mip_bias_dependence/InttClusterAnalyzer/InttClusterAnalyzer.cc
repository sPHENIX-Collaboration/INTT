
#include "InttClusterAnalyzer.h"

using namespace std;
InttClusterAnalyzer::InttClusterAnalyzer(const std::string &name):
 SubsysReco(name)
{

}

InttClusterAnalyzer::~InttClusterAnalyzer()
{

}

int InttClusterAnalyzer::Init(PHCompositeNode *topNode)
{
  if( output_ == "" )
    output_ = output_dir_ + output_name_;
  else if( run_num_ != -1 || output_name_ == "output.root" )
    output_ = output_dir_ + "run_" + to_string( run_num_ ) + ".root";

  output_ = output_dir_ + "run_" + to_string( run_num_ ) + ".root";
  cout << "Output: " << output_ << endl;
  
  tf_ = new TFile( output_.c_str(), "RECREATE" );
  tr_ = new TTree( "tree", "TTree for INTT cluster analysis" );

  event_ = new InttEvent();
  event_->SetVerbosity( this->Verbosity() );
  tr_->Branch( "event", &event_ );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::InitRun(PHCompositeNode *topNode)
{
  event_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::GetNode( PHCompositeNode *topNode )
{

  //--------------------------------------------------------------//
  // Getting InttEventInfo                                        //
  //--------------------------------------------------------------//
  string node_name_intteventheader = "INTTEVENTHEADER";
  node_intteventheader_map_ =
    findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader);
  
  if (!node_intteventheader_map_)
    {
      cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
      //intt_header_found_ = false;
	    //return Fun4AllReturnCodes::ABORTEVENT;
    }

  //------------------------------------------------//
  // Getting TrkrClusterContainer node              //
  // TRKR_CLUSTER node: Information of TrkrCluster  //
  //------------------------------------------------//
  string node_name_trkr_cluster = "TRKR_CLUSTER";
  node_cluster_map_ =
    findNode::getClass<TrkrClusterContainerv4>(topNode, node_name_trkr_cluster);

  if(!node_cluster_map_ )
    {
      cerr << PHWHERE << node_name_trkr_cluster << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //--------------------------------------------------------------//
  // Getting Acts node to assign (x, y, z) coordinate to clusters //
  // ActsGeometry node: for the global coordinate                 //
  //--------------------------------------------------------------//
  node_acts_ = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if ( !node_acts_ )
    {
      cerr << PHWHERE << "No ActsGeometry on node tree. Bailing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //--------------------------------------------------------------//
  // Getting Intt vertex map node                                 //
  //--------------------------------------------------------------//
  node_intt_vertex_map_ = findNode::getClass< InttVertexMapv1 >( topNode, "InttVertexMap" );
  if( !node_intt_vertex_map_ )
    {
      cerr << PHWHERE << "No InttVertexMap node tree found. This event is skipped." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::process_event(PHCompositeNode *topNode)
{

  this->GetNode( topNode );
  event_->SetRunNum( run_num_ );
  event_->SetEventId( event_counter_ );

  this->VertexAnalysis( topNode );
  
  // loop over all INTT layers (0: inner of inner barrel, 1: outer of inner, 2: inner of outer, 3: outer of outer)
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
    {

      // get clusters only on the INTT layer, and loop over them
      for (const auto &hitsetkey
	     : node_cluster_map_->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3) )
	{

	  auto range = node_cluster_map_->getClusters(hitsetkey);

	  // loop over iterators of this cluster
	  for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	    {
	      
	      const auto cluskey = clusIter->first;
	      const auto cluster = clusIter->second;

	      // Get cluster position in lab-coordinate using Acts
	      const auto globalPos = node_acts_->getGlobalPosition(cluskey, cluster);

	      // Set cluster position in lab-coordinate to this cluster object
	      cluster->setPosition(0,  globalPos.x() );
	      cluster->setPosition(1,  globalPos.y() );
	      cluster->setPosition(2,  globalPos.z() );

	      InttCluster* intt_cluster = new InttCluster( cluster->getPosition( 0 ),
							   cluster->getPosition( 1 ),
							   cluster->getPosition( 2 ) );
	      intt_cluster->SetAdc( cluster->getAdc() );
	      intt_cluster->SetLayer( inttlayer);
	      intt_cluster->SetSizePhi( cluster->getPhiSize() );
	      intt_cluster->SetSizeZ( cluster->getZSize() );

	      intt_cluster->SetVertex( TVector3(0, 0, 0) );
	      event_->AddCluster( intt_cluster );

	      // If user wants to see cluster information, do it
	      if( this->Verbosity() > 1 )
		{
		  intt_cluster->Print();
		} // End of if( this->Verbosity() > 1 )
	      
	    } // End of for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	} // End of for (const auto &hitsetkey : node_cluster_map_->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3) )
    } // End of for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)

  // Don't forget to do it
  event_->Calc();

  // If users want to see
  if( this->Verbosity() > 1 )
    {
      event_->Print();
    }
  
  tr_->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::VertexAnalysis( PHCompositeNode *topNode )
{

  if( this->Verbosity() > 0 )
    {

      cout << event_counter_ << endl;
      if( this->Verbosity() > 1 )
	{
	  node_intt_vertex_map_->identify();
	}
      
      cout << "vertex map size: " << node_intt_vertex_map_->size() << endl;
    }

  if( node_intt_vertex_map_->size() != 1 )
    {
      event_->SetWhetherVertexGood( false );
      event_->SetVertex( 0, 0, 0 );
      return -1;

    }
  
  InttVertex* vertex = node_intt_vertex_map_->begin()->second;
  event_->SetWhetherVertexGood( vertex->get_good() );
  event_->SetVertexZChi2Ndf( vertex->get_chi2ndf() );
  event_->SetVertex( vertex->get_x(), vertex->get_y(), vertex->get_z() );
  
  if( this->Verbosity() == 1 )
    {
      cout << "("
	   << setw(7) << setprecision(3) << vertex->get_x()
	   << ", "
	   << setw(7) << setprecision(3) << vertex->get_y()
	   << ", "
	   << setw(7) << setprecision(3) << vertex->get_z()
	   << ")\t";
      
      cout << endl;
    }
  
  return 0;

}

int InttClusterAnalyzer::ResetEvent(PHCompositeNode *topNode)
{

  event_counter_++;
  event_->Reset();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::EndRun(const int runnumber)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::End(PHCompositeNode *topNode)
{

  tf_->WriteTObject( tr_, tr_->GetName() );
  tf_->Close();

  cout << output_ << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterAnalyzer::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttClusterAnalyzer::Print(const string &what) const
{

}

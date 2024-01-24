#include "InttAnalysisCosmic.h"

using namespace std;

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
InttAnalysisCosmic::InttAnalysisCosmic(const std::string &name, const string &filename )
  : SubsysReco(name)
    //  , _rawModule(nullptr)
  , fname_(filename)
    //  , anafile_(nullptr)
{
}

/**
 * Destructor of module
 */
InttAnalysisCosmic::~InttAnalysisCosmic()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttAnalysisCosmic::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
    {
      std::cout << "Beginning Init in InttAnalysisCosmic" << std::endl;
    }

  event_counter_ = 0;
  anafile_ = new TFile(fname_.c_str(), "recreate");
  // h_dca2d_zero  = new TH1F("h_dca2d_zero", "DCA2D to 0", 500, -10, 10);
  // h2_dca2d_zero = new TH2F("h2_dca2d_zero", "DCA2D to 0 vs nclus", 500, -10, 10, 100, 0, 10000);
  // h2_dca2d_len  = new TH2F("h2_dca2d_len", "DCA2D to 0 vs len", 500, -10, 10, 500, -10, 10);
  // h_zvtx        = new TH1F("h_zvtx", "Zvertex", 400, -40, 40);

  // h_ntp_clus = new TNtuple("ntp_clus", "Cluster Ntuple", "nclus:nclus2:bco_full:evt:size:adc:x:y:z:lay:lad:sen");
  // h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", "nclus:nclus2:bco_full:evt:ang1:ang2:z1:z2:dca2d:len:unorm:l1:l2:vx:vy:vz");

  // h_zvtxseed_ = new TH1F("h_zvtxseed", "Zvertex Seed histogram", 200, -50, 50);
  
  ev_ = new event();
  tree_ = new TTree( "tree", "a test ttree" );
  //tree_->Branch( "hit_position", &hit_positions_ );
  tree_->Branch( "event", &ev_ );
  tree_->Print();
  return 0;
}

int InttAnalysisCosmic::InitRun(PHCompositeNode * /*topNode*/)
{

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttAnalysisCosmic::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
    cout << "Event" << setw(8) << event_counter_ << endl;
  else if (Verbosity() > 1)
    cout << "\rEvent" << setw(8) << event_counter_ << endl;
  else
    if( event_counter_ % 1000 == 0 )
      cout << "\rEvent" << setw(8) << event_counter_ << endl;
      
  if (Verbosity() > 5)
    {
      std::cout << "Beginning process_event in AnaTutorial" << std::endl;
    }

  //readRawHit(topNode);

  ActsGeometry *m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
    {
      std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //  TrkrClusterContainer *m_clusterMap =
  auto *m_clusterMap = 
    findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");

  //const type_info& class_info_cluster_container = typeid( m_clusterMap );
  //cout << "TrkrClusterContainer type: " << class_info_cluster_container.name() << endl;
  
  if (!m_clusterMap)
    {
      cout << PHWHERE << "TrkrClusterContainer node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  auto *m_event_header = 
    findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  // cout << "----------- Event infomation (InttAnalysisCosmic ----------------" << endl;
  // cout << " - Run number    : " << m_event_header->get_RunNumber() << endl;
  // cout << " - Event sequence: " << m_event_header->get_EvtSequence() << endl;
  // cout << " - Bunch crossing (BCO): " << m_event_header->get_BunchCrossing() << endl;
  // cout << " - Event type    : " << m_event_header->get_EvtType() << endl;
  // cout << " - Time stamp    : " << m_event_header->get_TimeStamp() << endl;

  /////////////
  // InttEvent *inttEvt = nullptr;
  ///  uint64_t bco = 0;
  // if(inttEvt!=NULL) {
  //   bco    = inttEvt->bco;
  //   evtseq = inttEvt->evtSeq;
  // }

  /////////////
  struct ClustInfo {
    int layer;
    Acts::Vector3 pos;
  };

  //  float ntpval[20];
  int nCluster = 0;
  bool exceedNwrite = false;
  
  //std::vector<Acts::Vector3> clusters[2]; // inner=0; outer=1
  //  std::vector<ClustInfo> clusters[2]; // inner=0; outer=1
  vector < TrkrCluster* > clusters;
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
    {
      //      cout << " INTT layer " << inttlayer << endl;
      //      int layer= ( inttlayer < 2 ? 0 : 1 );

      // loop over all hits
      for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3) )
	{

	  // getHitSetKeys -> using HitSetKeyList = std::vector<TrkrDefs::hitsetkey>;
	  // TrkrDefs::hitsetkey -> typedefuint32_t	  
	    //cout << "\tHit set key: " << hitsetkey << endl;
	  
	  auto range = m_clusterMap->getClusters(hitsetkey);
	  // type: std::pair<ConstIterator, ConstIterator> ConstRange
	  // here, MMap::const_iterator ConstIterator;
      
	  // loop over iterators of this cluster
	  for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	    {
	      const auto cluskey = clusIter->first;
	      const auto cluster = clusIter->second;
	      clusters.push_back( cluster );
	      
	      //	      cout << "\t\t" << cluskey << endl;
	      
	      //const type_info& class_info = typeid( cluster );
	      //cout << "Cluster type: " << class_info.name() << endl;
	
	      // Acts::Vector3 getGlobalPosition( TrkrDefs:: cluskey key, TrkrCluster* cluster);
	      const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);
	
	      int ladder_z   = InttDefs::getLadderZId(cluskey);
	      int ladder_phi = InttDefs::getLadderPhiId(cluskey);
	      int size       = cluster->getSize();

	      //	      if( nCluster < 5 )
	      if (Verbosity() > 5)
		{
		  cout << "xyz("
		       << setprecision(4) << setw(8) << globalPos.x() << ", "
		       << setprecision(4) << setw(8) << globalPos.y() << ", "
		       << setprecision(4) << setw(8) << globalPos.z()
		       << ") \t"
		       << "xyz("
		       << setprecision(4) << setw(8) << cluster->getPosition( 0 ) << ", "
		       << setprecision(4) << setw(8) << cluster->getPosition( 1 ) << ", "
		       << setprecision(4) << setw(8) << cluster->getPosition( 2 ) << ") "
		       << "local xy("
		       << setprecision(4) << setw(8) << cluster->getLocalX() << ", "
		       << setprecision(4) << setw(8) << cluster->getLocalY() << ")\t "
		    
		       << cluster->getAdc() << " "
		       << size << " "
		       << inttlayer << " "
		       << ladder_z << " "
		       << ladder_phi
		       << endl;
		}

		cluster->setPosition(0,  globalPos.x() );
		cluster->setPosition(1,  globalPos.y() );
		cluster->setPosition(2,  globalPos.z() );
	      // ClustInfo info;
	      // info.layer = inttlayer;
	      // info.pos   = globalPos;

	      //clusters[layer].push_back(globalPos);
	      // clusters[layer].push_back(info);
	      nCluster++;

	      //ntpval[0] = nclusadd; //bco_full
	      //ntpval[1] = nclusadd2; //bco_full
	      // ntpval[2] = bco; //bco_full
	      // ntpval[3] = event_counter_; //evtCount;
	      // ntpval[4] = size; // size
	      // ntpval[5] = cluster->getAdc(); // size
	      // ntpval[6] = globalPos.x();
	      // ntpval[7] = globalPos.y();
	      // ntpval[8] = globalPos.z();
	      // ntpval[9] = inttlayer;
	      // ntpval[10] = ladder_phi;
	      // ntpval[11] = ladder_z;
	      // h_ntp_clus->Fill(ntpval);
	      // = new TNtuple("ntp_clus", "Cluster Ntuple", "bco_full:evt:size:adc:x:y:z:lay:lad:sen");
	    }
	}
    }

  if (Verbosity() > 5)
    cout << "InttAnalysisCosmic, end of event" << endl << endl;

  if( clusters.size() == 1 )
    for( auto& cluster : clusters )
      if( cluster->getPhiSize() == 3 ) 
	cout << m_event_header->get_EvtSequence()  << "\t"
	  // << inttlayer << " "
	  // << ladder_z << " "
	  // << ladder_phi << " "
	  //<< cluster->getSize() << "\t"
	     << cluster->getPhiSize() << "\t"
	  // << cluster->getZSize() << "\t"
	     << cluster->getPosition(0) << "\t"
	     << cluster->getPosition(1) << "\t"
	     << cluster->getPosition(2) << "\t"
	     << cluster->getAdc() << "\t"      
	     << endl;

  ev_->SetEventId( m_event_header->get_EvtSequence() );
  ev_->SetBcoFull( m_event_header->get_BunchCrossing() );
  // this->Analysis( clusters );
  // this->CleanVectors();
  
  event_counter_++; // for the next event
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttAnalysisCosmic::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
    {
      std::cout << "Ending InttAnalysisCosmic analysis package" << std::endl;
    }
  
  if( anafile_ != nullptr ){
    //anafile_->Write();
    tree_->Print();
    anafile_->WriteTObject( tree_ );
    anafile_->Close();
  }

  return 0;
}


void InttAnalysisCosmic::Analysis( vector < TrkrCluster* >& clusters )
{
  for( auto& cluster : clusters )
    {

      ev_->AddHit( cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2),
		   cluster->getSize(),
		   cluster->getRPhiError(),
		   cluster->getZError(),
		   cluster->getAdc()
		   );

    }

  ev_->SetEventId( event_counter_ );
  ev_->Process();

  if( clusters.size() > 1 )
    tree_->Fill();
  
  ev_->Clear();
}

void InttAnalysisCosmic::CleanVectors()
{

}

template < class T >
void InttAnalysisCosmic::CleanVector( vector < T* >& vec )
{
  vec.erase( vec.begin(), vec.end() );
}

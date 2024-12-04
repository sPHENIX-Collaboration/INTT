#include "InttVertexXY.h"

using namespace std;
InttVertexXY::InttVertexXY(const std::string &name):
 SubsysReco(name)
{

}

InttVertexXY::~InttVertexXY()
{

}

int InttVertexXY::Init(PHCompositeNode *topNode)
{

  output_ = output_dir_ + "run_" + to_string( run_num_ ) + ".root";
  output_txt_ = output_dir_ + "run_" + to_string( run_num_ ) + ".txt";
  
  tf_ = new TFile( output_.c_str(), "RECREATE" );
  tr_ = new TTree( "tree", "TTree for INTT cluster analysis" );

  //tr_->Branch( "event", &event_ );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::InitRun(PHCompositeNode *topNode)
{
  event_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::GetNode( PHCompositeNode *topNode )
{

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

int InttVertexXY::process_event(PHCompositeNode *topNode)
{

  this->GetNode( topNode );

  if( this->Verbosity() > 0 )
    {
      cout << event_counter_ << endl;
      cout << "vertex map size: " << node_intt_vertex_map_->size() << endl;

      if( this->Verbosity() > 1 )
	{
	  node_intt_vertex_map_->identify();
	}
    }

  auto iter = node_intt_vertex_map_->begin(); // [0]
  for( int i=0; iter!=node_intt_vertex_map_->end(); iter++ )
    {
      InttVertex* ver = iter->second;
      vertex_x_ = ver->get_x();
      vertex_y_ = ver->get_y();
      if( this->Verbosity() == 1 )
	cout << "("
	     << setw(7) << setprecision(3) << ver->get_x()
	     << ", "
	     << setw(7) << setprecision(3) << ver->get_y()
	     << ")\t";
    }

  if( this->Verbosity() == 1 )
    cout << endl;

  //tr_->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::VertexAnalysis( PHCompositeNode *topNode )
{

  
  return Fun4AllReturnCodes::EVENT_OK;

}

int InttVertexXY::ResetEvent(PHCompositeNode *topNode)
{

  event_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::EndRun(const int runnumber)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::End(PHCompositeNode *topNode)
{
  event_counter_--;
  
  tf_->WriteTObject( tr_, tr_->GetName() );
  tf_->Close();

  // write last vertex position into a text file
  ofstream ofs( output_txt_.c_str() );
  ofs << vertex_x_ << "\t" << vertex_y_ << endl;
  ofs.close();

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttVertexXY::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttVertexXY::Print(const string &what) const
{
  cout << "Output: " << output_ << endl;
  cout << "Output (txt): " << output_txt_ << endl;
  cout << "Event counter: " << event_counter_ << endl;
}

#include "TrackMC.hh"

ClassImp( TrackMC );

TrackMC::TrackMC( int id, string name, int q ) :
  id_( id ),
  event_id_( -1 ),
  particle_name_( name ),
  q_( q ),
  is_primary_( false ),
  total_energy_( -1 ),
  kinetic_energy_( -1 ),
  track_length_( -1 ),
  track_time_( -1 ),
  momentum_( 3 ),
  position_( 3 ),
  steps_( 0 )
{}

// void TrackMC::AddVertexMomentum( TVector3* point, TVector3* momentum )
// {
//   vertices_.push_back( point );
//   momentum_.push_back( momentum );

//   vertex_momentum_.push_back( pair<TVector3*, TVector3*>( point, momentum ) );

// }

void TrackMC::AddStepMC( StepMC* step )
{
  if( id_ == step->GetTrackID() )
    {
      //      cout << "void TrackMC::AddStepMC( StepMC* step ), energy deposit: " << step->GetEnergyDeposit() << endl;
      //step->Print( 1 );
      steps_.push_back( step );
    }

}

TGraph2D* TrackMC::GetGraph()
{
  TGraph2D* g_rtn = new TGraph2D();
  g_rtn->SetTitle( "3D graph;x (mm);y (mm);z (mm)" );

  // take the position of the pre stepping point only for the first step
  if( steps_.size() > 0 )
    {
      auto position_pre = steps_[0]->GetPositionPre();
      g_rtn->SetPoint( g_rtn->GetN(), position_pre[0], position_pre[1], position_pre[2] );
    }

  // loop over all steps to set positions of each post stepping point 
  for( auto step : steps_ )
    {
      auto position = step->GetPosition();
      g_rtn->SetPoint( g_rtn->GetN(), position[0], position[1], position[2] );
    }

  
  return g_rtn;
}
  
TGraph* TrackMC::GetGraphOn( string xaxis, string yaxis, double cut_min, double cut_max )
{
  auto graph_2d = GetGraph();

  Double_t *values_x = nullptr, *values_y = nullptr;
  string title = "track projection on " + xaxis + " vs " + yaxis + ";";
  for( int i=0; i<2; i++ )
    {
      auto& values = i==0 ? values_x : values_y;
      auto axis_arg = i==0 ? xaxis : yaxis;

      if( axis_arg.find( "x" ) != string::npos )
	values = graph_2d->GetX();
      else if( axis_arg.find( "y" ) != string::npos )
	values = graph_2d->GetY();
      else if( axis_arg.find( "z" ) != string::npos )
	values = graph_2d->GetZ();

      title += axis_arg + " (mm);";
    }

  TGraph* g_rtn = new TGraph( graph_2d->GetN(), values_x, values_y );
  g_rtn->SetTitle( title.c_str() );
  
  int color = kBlack;
  if( particle_name_ == "e-" )
    {
      color = kOrange + 1;
    }
  else if( particle_name_ == "e+" )
    {
      color = kAzure + 1;
    }
  else if( particle_name_ == "gamma" )
    {
      color = kGreen + 2;
    }
  
  if( IsPrimary() )
    {
      color = kRed;
      g_rtn->SetMarkerStyle( 29 ); // star
    }
  else
    {
      g_rtn->SetMarkerStyle( 20 );
      g_rtn->SetMarkerSize( 0.7 );
    }

  g_rtn->SetMarkerColorAlpha( color, 0.6 );
  g_rtn->SetLineColorAlpha( color, 0.6 );
  g_rtn->SetLineWidth( 1 );

  // apply cut for the axis not drawn
  //for( int i=0; i<graph_2d->GetN(); i++ )
  for( int i=graph_2d->GetN()-1; i>-1; i-- )
    {
      double x, y, z;
      graph_2d->GetPoint( i, x, y, z );

      // if( -888 < cut_min && cut_max < 800 )
      // 	cout <<  xaxis + yaxis << " " << i  << " " << x << " " << y << " " << z ;
      
      if( (xaxis + yaxis).find( "x" ) == string::npos ) // if x is not used...
	{
	  if( cut_min < x && x < cut_max )
	    continue;
	}
      else if( (xaxis + yaxis).find( "y" ) == string::npos )
	{
	  if( cut_min < y && y < cut_max )
	    continue;
	}
      else if( (xaxis + yaxis).find( "z" ) == string::npos )
	{
	  if( cut_min < z && z < cut_max )
	    {
	      // if( -888 < cut_min && cut_max < 800 )
	      // 	cout << endl;
	      
	      continue;
	    }
	}

      // if( -888 < cut_min && cut_max < 800 )
      // 	cout << " ---> removed" << endl;
      g_rtn->RemovePoint( i );
    }
  
  return g_rtn;
}


void TrackMC::SetMomentum( double x, double y, double z )
{
  momentum_[0] = x;
  momentum_[1] = y;
  momentum_[2] = z;
}


void TrackMC::SetPosition( double x, double y, double z )
{
  position_[0] = x;
  position_[1] = y;
  position_[2] = z;
}

void TrackMC::Print( int level )
{
  int header_width = 10;
  cout << "+" << string(38, '-') << "+" << endl;
  cout << "| " << setw(header_width) << "Event ID: " << event_id_ << endl;
  cout << "| " << setw(header_width) << "Track ID: " << id_ << endl;
  cout << "| " << setw(header_width) << "name: " << particle_name_ << endl;
  cout << "| " << setw(header_width) << "charge: " << q_ << endl;
  cout << "| " << setw(header_width) << "Primary? " << is_primary_ << endl;
  
  cout << "| " << setw(header_width) << "Total energy: " << total_energy_ << " MeV/c^2" << endl;
  cout << "| " << setw(header_width) << "Kinetic energy: " << kinetic_energy_ << " MeV/c^2" << endl;
  cout << "| " << setw(header_width) << "Track time: " << track_time_ << " ns" <<  endl;
  cout << "| " << setw(header_width) << "Track Length: " << track_length_ << " cm" <<  endl;
  cout << "| " << setw(header_width) << "#step: " << steps_.size() << endl;
  if( level > 0 )
    {
      // for( unsigned int i=0; i<vertices_.size(); i++ )
      // 	{
      // 	  cout << "| " << string( 5, ' ' )
      // 	       << "+ " << "p = ("
      // 	       << vertex_momentum_[i].first->x() << ", "
      // 	       << vertex_momentum_[i].first->y() << ", "
      // 	       << vertex_momentum_[i].first->z() << ") MeV/c " 
      // 	       << "at " << "("
      // 	       << vertex_momentum_[i].second->x() << ", "
      // 	       << vertex_momentum_[i].second->y() << ", "
      // 	       << vertex_momentum_[i].second->z() << ") cm"
      // 	       << endl;

      //}

      // if( level > 1 )
      // 	cout << "#step: " << steps_.size() << endl;
	  
    }
  
  cout << "+" << string(38, '-') << "+" << endl;
}



// void TrackMC::CalcSteps()
// {
//   cout << "void TrackMC::CalcSteps()" << endl;
//   cout << "#StepMC: " << steps_.size() << endl;

//   sort( steps_.begin(), steps_.end(), StepMC::Less );
//   cout << "after sort: " << endl;
//   for( int i=0; i<steps_.size(); i++ )
//     cout << steps_[i]->GetID() << " ";
//   cout << endl;

//   this->Print();
//   cout << " p_ = ";
//   cout << "("
//        << setw(12) << setprecision(4) << momentum_[0] << ","
//        << setw(12) << setprecision(4) << momentum_[1] << ","
//        << setw(12) << setprecision(4) << momentum_[2] << "), ";
//   cout << endl;

//   vector < double > x_abs(3);
//   vector < double > p_abs = momentum_;
//   for( int i=0; i<steps_.size(); i++ )
//     {
//       auto step = steps_[i];
//       cout << steps_[i]->GetID() << " ";
//       cout << setw(6) << setprecision(3) << step->GetStepLength() << " cm, ";

//       auto p = step->GetMomentum();
//       cout << "p = ("
// 	   << setw(8) << setprecision(2) << p[0] << ","
// 	   << setw(8) << setprecision(2) << p[1] << ","
// 	   << setw(14) << setprecision(6) << p[2] << "), ";

//       auto p_pre = step->GetMomentumPre();
//       cout << "p = ("
// 	   << setw(8) << setprecision(2) << p_pre[0] << ","
// 	   << setw(8) << setprecision(2) << p_pre[1] << ","
// 	   << setw(14) << setprecision(6) << p_pre[2] << "), ";
      
//       auto x = step->GetPosition();
//       cout << "x = ("
// 	   << setw(12) << setprecision(4) << x[0] << ","
// 	   << setw(12) << setprecision(4) << x[1] << ","
// 	   << setw(12) << setprecision(4) << x[2] << ") ";

//       auto x_pre = step->GetPositionPre();
//       cout << "x = ("
// 	   << setw(12) << setprecision(4) << x_pre[0] << ","
// 	   << setw(12) << setprecision(4) << x_pre[1] << ","
// 	   << setw(12) << setprecision(4) << x_pre[2] << ") ";

//       // for( int j=0; j<x_abs.size(); j++ )
//       // 	{
//       // 	  x_abs[j] -= dx[j];
//       // 	  p_abs[j] -= dp[j];

//       // 	}
      
//       cout << endl;

//     }
//   cout << endl;

//   // cout << "Filan x = ("
//   //      << setw(12) << setprecision(4) << x_abs[0] << ","
//   //      << setw(12) << setprecision(4) << x_abs[1] << ","
//   //      << setw(12) << setprecision(4) << x_abs[2] << ")"
//   //      << endl;

//   // cout << "Filnal p = ";
//   // cout << "("
//   //      << setw(12) << setprecision(4) << p_abs[0] << ","
//   //      << setw(12) << setprecision(4) << p_abs[1] << ","
//   //      << setw(12) << setprecision(4) << p_abs[2] << "), ";

//   cout << " p_ = ";
//   cout << "("
//        << setw(12) << setprecision(4) << momentum_[0] << ","
//        << setw(12) << setprecision(4) << momentum_[1] << ","
//        << setw(12) << setprecision(4) << momentum_[2] << "), ";

//   cout << endl;

  
// }

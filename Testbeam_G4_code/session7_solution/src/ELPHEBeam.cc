#include "ELPHEBeam.hh"

ELPHEBeam::ELPHEBeam() :
  beam_line_( "-23" ),
  production_target_( "W_200um" ),
  position_restriction_( 0 ),
  given_momentum_( 1000 * CLHEP::MeV ),
  datafile_( "beam_parameter/beam_W_400A.dat" ),
  momentum_factor_( 1.0 / 1.32 ),
  spread_factor_( 1.0 / 25.0 )
  //momentum_( 0.0 ),
  // position_( 0 ),
  // momentum_( 0 ),
  // div_num_( 1e3 ),
  // pos_x_( 0.0 ),
  // pos_y_( 0.0 )
{
  Init();
}


ELPHEBeam::ELPHEBeam( INTTMessenger* mess, std::string beam_line, std::string production_target, int position_restriction, double momentum ) :
  momentum_factor_( 1.0 / 1.32 ),
  spread_factor_( 1.0 / 25.0 ),
  given_momentum_( momentum ),
  datafile_( "beam_parameter/beam_W_400A.dat" )
  //momentum_( 0.0 ),
  // position_( 0 ),
  // momentum_( 0 ), 
  // div_num_( 1e3 ),
  // pos_x_( 0.0 ),
  // pos_y_( 0.0 )
{
  INTT_mess_ = mess;
  beam_line_ = beam_line;
  production_target_ = production_target;
  position_restriction_ = position_restriction;

  Init();
  
  Print();
}

ELPHEBeam::~ELPHEBeam()
{
  //  data_->Close();
  //  delete data_;
  //delete beam_parameter_;
}

void ELPHEBeam::Init()
{
  // momentum_ = G4ThreeVector();
  // position_ = G4ThreeVector();
  // random_gen_ = new TRandom3( 0 );

  //  ReadDatabase();

  index_ = beam_parameter_.begin();
  if( INTT_mess_->IsSmearing() )
    ReadBeamFile();
}

/*
void ELPHEBeam::ReadDatabase()
{

  data_ = new TFile( datafile_.c_str(), "READ");
  //if( data_ == nullptr )
  if( data_->IsOpen() == false )
    {
      G4cerr << std::string( 50, '=' ) << G4endl;
      G4cerr << datafile_ << " is not found" << G4endl;
      G4cerr << "Run aborted" << G4endl;
      G4cerr << std::string( 50, '=' ) << G4endl;

#ifdef G4MULTITHREADED
      auto manager = G4MTRunManager::GetRunManager();
#else
      auto manager = G4RunManager::GetRunManager();
#endif
      manager->AbortRun();
      //exit( -1 );
    }

  std::string graph_name = "momentum_vs_spread";
  if( position_restriction_ == 1 )
    {
      graph_name += "_xp_45";
    }
  else if( position_restriction_ == 2 )
    {
      graph_name += "_xp_3";
    }
  else if( position_restriction_ == 3 )
    {
      graph_name += "_xp_15";
    }

  graph_ = (TGraph*)data_->Get( graph_name.c_str() );


  //////////////////////////////////////////////////
  // 425 A W 200 um target
  /*
  std::string datafile_detail = "beam_parameter/ebl-hb/23/w425.root";
  TFile* data_detail = new TFile( datafile_detail.c_str(), "READ" );
  hist_p_x_ = (TH2D*)data_detail->Get( "h312" )->Clone(); // p vs x
  hist_p_y_ = (TH2D*)data_detail->Get( "h313" )->Clone(); // p vs y

  beam_correlation_ = new TF2( "f", "bigaus", -50, 50, -50, 50 );
  beam_correlation_->SetParameters( 1.0,                // intensity
				    -3.713, 15.28,      // mean, std dev (x)
				    0.09356, 10.37,     // mean, std dev (y)
				    -0.01433 );         // rho (correlation b/w x&y)

  beam_x_y_ = new TF2( "f_x_p", "bigaus", -50, 50, -50, 50 );
  //  beam_x_y_->SetParameters( 1.01891e+05, -3.56239e+00, 1.53247e+01, 1.01175e+03, 1.01095e+01, 7.20581e-01 );
  beam_x_y_->SetParameters( 1, -3.713, 15.28, 0.09356, 10.37, -0.01433 );
  beam_x_y_->SetNpx( 1000 );
  beam_x_y_->SetNpy( 1000 );

  beam_p_x_ = new TF2( "f_x_p", "bigaus", -50, 50, 900, 1200 );
  //  beam_p_x_->SetParameters( 1.01891e+05, -3.56239e+00, 1.53247e+01, 1.01175e+03, 1.01095e+01, 7.20581e-01 );
  beam_p_x_->SetParameters( 1, 1022, 10.95, 12.64, 25.18, 0.8643 );
  beam_p_x_->SetNpx( 1000 );
  beam_p_x_->SetNpy( 1000 );

  beam_p_y_ = new TF2( "f_y_p", "bigaus", -50, 50, 900, 1200 );
  //  beam_p_y_->SetParameters( 1.09877e+05, 2.95438e-03, 1.04175e+01, 1.01166e+03, 1.04756e+01, 1.65059e-02 );  
  beam_p_y_->SetParameters( 1.287e+05, 1012, 10.2, 0.135, 10.5, -0.01186 );
  beam_p_y_->SetNpx( 1000 );
  beam_p_y_->SetNpy( 1000 );


  // momentum distribution
  f_p_ = new TF1( "f_p", "gaus",920.085, 1026.1);
  f_p_->SetParameters(5484.99, 973.093, 10.6016);
  f_p_formula_ = "gaus";
  
  // momentum vs pT
  f_p_pt_ = new TF2( "f_p_pt", "bigaus",925.451, 1020.91, -5.691, 12.4277);
  f_p_pt_->SetParameters(4899.62, 973.18, 9.54596, 3.36835, 1.81187, -0.220089);
  f_p_pt_formula_ = "bigaus";
  
  // px vs py
  f_px_py_ = new TF2( "f_px_py", "xygaus",-17.5583, 15.8788, -11.0799, 11.1085);
  f_px_py_->SetParameters(137.26, -0.839748, 3.3437, 0.014301, 2.21884);
  f_px_py_formula_ = "xygaus";
  
  // momentum vs r
  f_p_r_ = new TF2( "f_p_r", "bigaus",917.206, 1030.02, -36.2636, 67.7288);
  f_p_r_->SetParameters(5098.29, 973.615, 11.2819, 15.7326, 10.3992, -0.268285);
  f_p_r_formula_ = "bigaus";
  
  // x vs y
  f_x_y_ = new TF2( "f_x_y", "xygaus",-89.0608, 81.6321, -53.3785, 53.6195);
  f_x_y_->SetParameters(32.88, -3.71432, 17.0693, 0.12051, 10.6998);
  f_x_y_formula_ = "xygaus";
	
  
}
*/

void ELPHEBeam::ReadBeamFile()
{
  //beamfile_path_ = "data.dat";
  //beamfile_path_ = "beam_gauss.dat";
  //  ifs_.open( beamfile_path_.c_str(), std::ios::in );
  //ifs_.open( datafile_.c_str(), std::ios::in );
  ifs_.open( INTT_mess_->GetBeamFile().data(), std::ios::in );
  if( ifs_.fail() )
    {
      G4cerr << "void ELPHEBeam::ReadBeamFile()" << G4endl;
      G4cerr << "The beam file " << beamfile_path_ << " is not found" << G4endl;
      return;
    }

  std::string line;
  while( getline( ifs_, line ) )
    {
      BeamParameter* param = new BeamParameter( line );
      beam_parameter_.push_back( param );
      // G4double value;
      // ifs_ >> value;
      // beam_data_.push_back( value );
    }

  //index_ = beam_data_.begin();
  index_ = beam_parameter_.begin();

  if( INTT_mess_->GetDebugLevel() > 0 )
    {
      G4int start = (G4int) beam_parameter_.size()
	* CLHEP::RandFlat::shootInt( std::numeric_limits<long>::min(), std::numeric_limits<long>::max() );
      //* T4SettingsFile::getInstance()->getRandom()->flat();
      index_ += start;
    }

}

G4double ELPHEBeam::GetRandomX( TH2D* hist2d, G4double y )
{
  int bin = 0;
  auto axis_y = hist2d->GetYaxis();
  for( int i=1; i<axis_y->GetNbins() + 1; i++ )
    {
      //G4cout << axis_y->GetBinLowEdge(i) << "\t" << y << G4endl;
      if( y < axis_y->GetBinUpEdge(i) )
	{
	  bin = i;
	  break;
	}
    }


  auto hist1d = hist2d->ProjectionX( "_name", bin, bin );
  //  G4cout << "Projected hist" << G4endl;
  /*
  for( int i=1; i<hist1d->GetNbinsX(); i++ )
    G4cout << i << "\t"
	   << hist1d->GetBinCenter(i) << "\t"
	   << hist1d->GetBinContent(i)
	   << G4endl;
  */  
  return hist1d->GetRandom();
}

void ELPHEBeam::GenerateNextBeam()
{
  //  momentum_ = given_momentum_ + this->GetMomentumSpread();
  //momentum_ = TRandom3::Gaus( given_momentum_, this->GetMomentumSpread() );
  //momentum_ = CLHEP::RandGauss::shoot( given_momentum_, this->GetMomentumSpread() );
  //pos_x_ = GetRandomX( hist_p_x_, momentum_ ) * CLHEP::mm;
  //pos_y_ = GetRandomX( hist_p_y_, momentum_ ) * CLHEP::mm;
  /*  
  double difference = 1e-3;

  // for momentum
  double total_momentum = f_p_->GetRandom() * CLHEP::MeV;

  TF2* f_p_pt_temp = new TF2( "f_p_pt_temp", f_p_pt_formula_.c_str(), 
			      (1.0 - difference) * total_momentum,
			      (1.0 + difference) * total_momentum,
			      f_p_pt_->GetYmin(),
			      f_p_pt_->GetYmax() );
  
  f_p_pt_temp->SetNpx( div_num_ );
  f_p_pt_temp->SetNpy( div_num_ );
  f_p_pt_temp->SetParameters( f_p_pt_->GetParameters() );

  double p_temp;
  double pt = -9999;
  f_p_pt_temp->GetRandom2( p_temp, pt );

  TF1* f_px_temp = new TF1( "f_px_temp", "gaus", -pt, pt );
  for( int i=0; i<f_px_temp->GetNpar(); i++ )
    f_px_temp->SetParameter( i, f_px_py_->GetParameter(i) );
  
  double px = f_px_temp->GetRandom();
  G4int sign = ( G4int(1) -(G4int)random_gen_->Integer( 2 )*2 );
  double py = sign * sqrt( pow( pt, 2 ) - pow( px, 2 ) ) ;
  
  // TF2* f_px_py_temp = new TF2( "f_px_py_temp", f_px_py_formula_.c_str(),
  // 			       -pt,
  // 			       pt,
  // 			       -pt,
  // 			       pt );
  // f_px_py_temp->SetParameters( f_px_py_->GetParameters() );
  
  // double px, py;
  // while( true )
  //   {
  //     f_px_py_temp->GetRandom2( px, py );
  //     px *= CLHEP::MeV;
  //     py *= CLHEP::MeV;

  //     G4cout << "px, py loop: "
  // 	     << pt << "\t"
  // 	     << sqrt( pow( px, 2 ) + pow( py, 2) ) << "\t"
  // 	     << fabs( pt - sqrt( pow( px, 2 ) + pow( py, 2) ) )
  // 	     << G4endl;
  //     if( fabs( pt - sqrt( pow( px, 2 ) + pow( py, 2) ) ) < 1e-3 )
  // 	break;

  //   }

  double pz = sqrt( pow( total_momentum, 2 ) - pow( pt, 2) );
  momentum_.set( px, py, pz );

  // position determination
  TF2* f_p_r_temp = new TF2( "f_p_r_temp", f_p_r_formula_.c_str(),
			     (1.0 - difference) * total_momentum,
			     (1.0 + difference) * total_momentum,
			     f_p_r_->GetYmin(),
			     f_p_r_->GetYmax() );
  f_p_r_temp->SetParameters( f_p_r_->GetParameters() );

  double r = -9999;
  f_p_r_temp->GetRandom2( p_temp, r );

  // TF2* f_x_y_temp = new TF2( "f_x_y_temp", f_x_y_formula_.c_str(),
  // 			     0.0,
  // 			     r,
  // 			     0.0,
  // 			     r );
  // f_x_y_temp->SetParameters( f_x_y_->GetParameters() );

  
  TF1* f_x_temp = new TF1( "f_x_temp", "gaus", -r, r );
  for( int i=0; i<f_x_temp->GetNpar(); i++ )
    f_x_temp->SetParameter( i, f_x_y_->GetParameter(i) );
  
  double x = f_x_temp->GetRandom();
  G4int sign_y = ( G4int(1) -(G4int)random_gen_->Integer( 2 )*2 );
  double y = sign * sqrt( pow( r, 2 ) - pow( x, 2 ) ) ;

  // double x, y;
  // while( true )
  //   {
  //     f_x_y_temp->GetRandom2( x, y );
  //     x *= CLHEP::mm;
  //     y *= CLHEP::mm;

  //     G4cout << "x, y loop: "
  // 	     << r << "\t"
  // 	     << sqrt( pow( x, 2 ) + pow( y, 2) ) << "\t"
  // 	     << fabs( r - sqrt( pow( x, 2 ) + pow( y, 2) ) )
  // 	     << G4endl;
  //     if( fabs( r - sqrt( pow( x, 2 ) + pow( y, 2) ) ) < 1e-3 )
  // 	break;

  //   }
    
  double z = -1.0 * CLHEP::m;
  position_.set( x, y, z );
  */
  
  //beam_correlation_->GetRandom2( pos_x_, pos_y_ );
  // beam_x_y_->GetRandom2( pos_x_, pos_y_ );
  // pos_x_ += CLHEP::mm;
  // pos_y_ += CLHEP::mm;
  
}

void ELPHEBeam::GenerateNextBeamTemp()
{

  if( index_ == beam_parameter_.end() )
    {
      index_ = beam_parameter_.begin();
      GenerateNextBeamTemp();
      
    }
  else
    {
      index_++;
      return;
    }
  
}

double ELPHEBeam::GetMomentum()
{

  // if( beam_line_ == "-30" )
  //   {
  //     return given_momentum_ * momentum_factor_;
  //   }

  // return given_momentum_;
}

double ELPHEBeam::GetMomentumSpread()
{

  // if( beam_line_ == "-30" )
  //   {
  //     return graph_->Eval( given_momentum_ * momentum_factor_ );
  //   }
  
  // return graph_->Eval( given_momentum_ );
}

void ELPHEBeam::Print( int level )
{
  G4cout << "+" << std::string( 50, '-' ) << "+" << G4endl;
  G4cout << "| ELPHEBeam class" << G4endl;
  G4cout << "|  - beam line                        : " << beam_line_ << G4endl;
  G4cout << "|  - production target                : " << production_target_ << G4endl;
  G4cout << "|  - position restriction             : " << position_restriction_ << G4endl;
  G4cout << "|  - momentum given from macro        : " << given_momentum_ << G4endl;
  //G4cout << "|  - actual momentum to be used       : " << this->GetMomentum() << G4endl;
  //G4cout << "|  - actual momentum spread to be used: " << this->GetMomentumSpread() << G4endl;
  G4cout << "|  - data file                        : " << datafile_ << G4endl;
  //G4cout << "|  - graph name                       : " << graph_->GetName() << G4endl;
  G4cout << "|-------------------------------------"   << G4endl;

  if( INTT_mess_->IsSmearing() )
    {

      G4cout << "| *** about this beam particle ***" << G4endl;
      G4cout << "|  * energy : " << this->GetBeamEnergy() << G4endl;
      G4cout << "|  * momentum vec: ("
	     << this->GetBeamMomentumDirection().x() << ", "
	     << this->GetBeamMomentumDirection().y() << ", "
	     << this->GetBeamMomentumDirection().z() << ") "
	     << G4endl;
      G4cout << "|  * position vec: ("
	     << this->GetBeamPosition().x() << ", "
	     << this->GetBeamPosition().y() << ", "
	     << this->GetBeamPosition().z() << ") "
	     << G4endl;
      G4cout << "| * Current index: " << (G4int)( index_ - beam_parameter_.begin()) << " / " << beam_parameter_.size() << G4endl;
      
    }
  
  G4cout << "+" << std::string( 50, '-' ) << "+" << G4endl;
}

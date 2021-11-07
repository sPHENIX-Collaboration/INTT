#include "ELPHEBeam.hh"

ELPHEBeam::ELPHEBeam() :
  beam_line_( "-23" ),
  production_target_( "Au_20um" ),
  position_restriction_( 0 ),
  momentum_( 800 * CLHEP::MeV ),
  momentum_factor_( 1.0 / 1.32 ),
  spread_factor_( 1.0 / 25.0 )
{
  ReadDatabase();
}


ELPHEBeam::ELPHEBeam( std::string beam_line, std::string production_target, int position_restriction, double momentum ) :
  momentum_factor_( 1.0 / 1.32 ),
  spread_factor_( 1.0 / 25.0 )
{
  beam_line_ = beam_line;
  production_target_ = production_target;
  position_restriction_ = position_restriction;
  momentum_ = momentum;
  ReadDatabase();
  
}

ELPHEBeam::~ELPHEBeam()
{
  data_->Close();
  delete data_;
}

void ELPHEBeam::ReadDatabase()
{
  datafile_ = "beam_parameter/" + production_target_ + ".root";

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
  
}

double ELPHEBeam::GetMomentum()
{

  if( beam_line_ == "-30" )
    {
      return momentum_ * momentum_factor_;
    }

  return momentum_;
}

double ELPHEBeam::GetMomentumSpread()
{

  if( beam_line_ == "-30" )
    {
      return graph_->Eval( momentum_ * momentum_factor_ );
    }
  
  return graph_->Eval( momentum_ );
}

void ELPHEBeam::Print( int level )
{
  G4cout << "+" << std::string( 50, '-' ) << "+" << G4endl;
  G4cout << "| ELPHEBeam class" << G4endl;
  G4cout << "|  - beam line                        : " << beam_line_ << G4endl;
  G4cout << "|  - production target                : " << production_target_ << G4endl;
  G4cout << "|  - position restriction             : " << position_restriction_ << G4endl;
  G4cout << "|  - momentum given from macro        : " << momentum_ << G4endl;
  G4cout << "|  - actual momentum to be used       : " << this->GetMomentum() << G4endl;
  G4cout << "|  - actual momentum spread to be used: " << this->GetMomentumSpread() << G4endl;
  G4cout << "|  - data file                        : " << datafile_ << G4endl;
  G4cout << "|  - graph name                       : " << graph_->GetName() << G4endl;

  if( level > 0 )
    graph_->Print();
  
  G4cout << "+" << std::string( 50, '-' ) << "+" << G4endl;
}

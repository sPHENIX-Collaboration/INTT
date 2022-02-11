#include "ELPHEBeam.hh"

ELPHEBeam::ELPHEBeam() :
  datafile_( "beam_parameter/beam_W_400A.dat" ),
  is_first_( true ),
  line_length_( 70 )
{
  Init();
}

ELPHEBeam::ELPHEBeam( INTTMessenger* mess ) :
  datafile_( "beam_parameter/beam_W_400A.dat" ),
  is_first_( true ),
  line_length_( 70 )
{
  INTT_mess_ = mess;
  Init();  
  
}

ELPHEBeam::~ELPHEBeam()
{
  ifs_.close();
}

void ELPHEBeam::Init()
{
  index_ = beam_parameter_.begin();

  if( INTT_mess_->IsSmearing() )
    ReadBeamFile();
  
}

void ELPHEBeam::ReadBeamFile()
{

  // no need to repeat this function because it's used as a static object
  if( is_first_ == false )
    return;
  
  ifs_.open( INTT_mess_->GetBeamFile().data(), std::ios::in );
  if( ifs_.fail() )
    {
      index_ = beam_parameter_.begin();
      G4cerr << "void ELPHEBeam::ReadBeamFile()" << G4endl;
      G4cerr << "The beam file " << beamfile_path_ << " is not found" << G4endl;
      return;
    }

  std::string line;
  while( getline( ifs_, line ) )
    {
      BeamParameter* param = new BeamParameter( line );
      beam_parameter_.push_back( param );

    }

  index_ = beam_parameter_.begin();
  if( INTT_mess_->GetDebugLevel() < 1 )
    {
      G4int start = CLHEP::RandFlat::shootInt( long(0), long(beam_parameter_.size() -1) );
      index_ += start;
    }

  is_first_ = false;
}

void ELPHEBeam::GenerateNextBeam()
{

  if( index_ == beam_parameter_.end() )
    {
      index_ = beam_parameter_.begin();
      GenerateNextBeam(); // so the first dataset is always omitted... Leave it as it's not a big problem
      
    }
  else
    {
      index_++;
      return;
    }
  
}

void ELPHEBeam::PrintLine( std::string line, std::string header, std::string footer )
{
  std::string printed_line = header + line;
  int space_num = line_length_ - ( header + line + footer ).size();

  if( space_num > 0 )
    printed_line += std::string( space_num, ' ');

  printed_line += footer;
  G4cout << printed_line << G4endl;

}

void ELPHEBeam::Print()
{
  this->PrintLine( std::string( line_length_ - 3, '-' ), " +", "+" );
  this->PrintLine( " ELPHEBeam class" );

  if( INTT_mess_->IsSmearing() )
    this->PrintLine( " - data file:" + datafile_ );
  else
    this->PrintLine( " --- Mono energy mode---" );
    
  this->PrintLine( "" );

  if( INTT_mess_->IsSmearing() && is_first_ == false )
    {

      this->PrintLine( " *** about this beam particle ***" );
      this->PrintLine( " * energy : " + to_string(this->GetBeamEnergy()) );

      std::string line = " * momentum vec: (" 
	+ to_string( this->GetBeamMomentumDirection().x() ) + ", "
	+ to_string( this->GetBeamMomentumDirection().y() ) + ", "
	+ to_string( this->GetBeamMomentumDirection().z() ) + ") ";
      this->PrintLine( line );
      
      line = " * position vec: ("
	+ to_string( this->GetBeamPosition().x() ) + ", "
	+ to_string( this->GetBeamPosition().y() ) +  ", "
	+ to_string( this->GetBeamPosition().z() ) + ") ";
      this->PrintLine( line );
      
      line = " * Current index: "
	+ to_string( (G4int)( index_ - beam_parameter_.begin()) )
	+ " / "
	+ to_string( beam_parameter_.size() );
      this->PrintLine( line );

    }

  this->PrintLine( std::string( line_length_ - 3, '-' ), " +", "+" );
}

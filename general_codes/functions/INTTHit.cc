#include "INTTHit.hh"

INTTHit::INTTHit( int adc, int ampl, int chip_id,
		  int fpga_id, int module, int chan_id,
		  int fem_id, int bco, int bco_full, int event )
{
  adc_		= adc;
  ampl_		= ampl;
  chip_id_	= chip_id;
  fpga_id_	= fpga_id;
  module_	= module;
  chan_id_	= chan_id;
  fem_id_	= fem_id;
  bco_		= bco;
  bco_full_	= bco_full;
  event_	= event;

  Init();
}


INTTHit::INTTHit( int adc, int ampl, int chip_id,
		  int fpga_id, int module, int chan_id,
		  int fem_id, int bco, int bco_full, int event,
		  int dac[8] )
{
  adc_		= adc;
  ampl_		= ampl;
  chip_id_	= chip_id;
  fpga_id_	= fpga_id;
  module_	= module;
  chan_id_	= chan_id;
  fem_id_	= fem_id;
  bco_		= bco;
  bco_full_	= bco_full;
  event_	= event;

  this->SetAllDAC( dac );
  Init();
}

void INTTHit::Init()
{
  this->cluster_geometry_.push_back( pair < int, int >( chip_id_, chan_id_) );
  this->cluster_channels_.push_back( this->chan_id_ );
  this->cluster_adc_voltages_.push_back( this->adc_voltage_ );

  if( this->IsIgnored() )
    this->SetClusteringStatus( -2 );
    
}

bool INTTHit::IsIgnored()
{
  //if( adc_ == 0 )
  //return true;

  if( this->forced_ignored_ == true )
    return true;
  
  if( 27 < chip_id_ )
    return true;

  if( 128 < chan_id_ )
    return true;

  if( ampl_ != 0 )
    return true;

  if( module_ == -1 )
    return true;
  
  if( clustering_status_ == -1 )
    return true;
  
  return false;
}

/*
INTTHit::INTTHit( const INTTHit& obj )
{

  //adc_ = -1;
}
*/

bool INTTHit::IsCluster( INTTHit* another )
{
  if( this->IsIgnored() || another->IsIgnored() )
    return false;

  if( this->bco_ != another->bco_ )
    return false;

  if( this->fem_id_ != another->fem_id_ )
    return false;

  if( this->fpga_id_ != fpga_id_ )
    return false;

  // if these hits are on the same chip, but channels are not neighboring, they are not cluster
  if( this->chip_id_ == another->chip_id_ )
    for( int i=0; i<this->cluster_channels_.size(); i++ )
      if( 1 == abs(this->cluster_channels_[i] - another->chan_id_) )
      return true;

  // if these hits are on the same channedl, but chips are not neighboring, they are not cluster
    if( this->chan_id_ == another->chan_id_ )
      if( abs(this->chip_id_ - another->chip_id_) == 1 )
	return true;

  // if these hits are on the boundary between chips facing each other and channels are 128, they are cluser
  if( abs(this->chip_id_ - another->chip_id_) == 13 )
    if( this->chan_id_ == 128 && another->chan_id_ == 128 )
      return true;
  
  return false;
}


void INTTHit::SetDAC( int id, int value )
{
  assert( -1 < id && id < 8 );
  dac_values_[id] = value;
}


void INTTHit::SetAllDAC( int values[8] )
{
  for( int i=0; i<8; i++ )
    this->SetDAC( i, values[i] );

  //  adc_voltage_ = 210.0 + this->dac_values_[this->adc_] * 4;//
  adc_voltage_ = 200.0 + this->dac_values_[this->adc_] * 4;//
  
}

void INTTHit::Clustering( INTTHit* another )
{
  if( this->IsCluster(another) == false )
    return;
  
  this->cluster_channels_.push_back( another->chan_id_ );
  this->cluster_adc_voltages_.push_back( another->adc_voltage_ );

  this->SetClusteringStatus( 1 );

  another->SetClusteringStatus( -1 );
}

INTTHit* INTTHit::MakeCluster( INTTHit* another )
{
  INTTHit* cluster = this;

  cluster->SetClusteringStatus(true);
  
  cluster->adc_ = -1;
  cluster->adc_voltage_ += another->adc_voltage_;
  cluster->cluster_geometry_.push_back( pair < int, int >( another->chip_id_, another->chan_id_ ) );
  
  return cluster;
}

void INTTHit::SetClusteringStatus( int status )
{
  // skipp if it's noise in any case
  if( this->GetClusteringStatus() != -2 )
    clustering_status_ = status;
}

void INTTHit::SetIgnored( bool status )
{
  forced_ignored_ = status;
  this->SetClusteringStatus( -2 );
}

void INTTHit::PrintInOneLine()
{
  int width_first = 8;
  int width_val = 3;
  int width_column = 1;
  int width_line = 4 * (width_first + width_val + width_column + 1) + 2;

  cout << " "
       << setw(width_first) << "module: " << setw(width_val) << module_ << "  "
       << setw(width_first) << "fem_id: " << setw(width_val) << fem_id_ << "  "
       << setw(width_first) << "fpga_id: " << setw(width_val) << fpga_id_ << "  "
       << setw(width_first) << "chip_id: " << setw(width_val) << chip_id_ << "  "
       << setw(width_first) << "chan_id: " << setw(width_val) << chan_id_ << "  "
       << setw(width_first) << "bco: " << setw(width_val) << bco_ << "  "
       << setw(width_first) << "bco_full: " << setw(width_val+3) << bco_full_ << "  "
       << setw(width_first) << "adc: " << setw(width_val) << adc_ << "  "
       << setw(width_first) << "ampl: " << setw(width_val) << ampl_
       << endl;
}

void INTTHit::Print()
{
  int width_first = 15;
  int width_val = 5;
  int width_column = 1;
  int width_line = 4 * (width_first + width_val + width_column + 1) + 2;

  cout << "+" << string(width_line - 2,  '-' ) << "+" << endl;

  cout << "| "
       << setw(width_first) << "module: " << setw(width_val) << module_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "fem_id: " << setw(width_val) << fem_id_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "fpga_id: " << setw(width_val) << fpga_id_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "chip_id: " << setw(width_val) << chip_id_
       << setw(width_column) << " " << "|"
       << endl;
  
  cout << "| "
       << setw(width_first) << "chan_id: " << setw(width_val) << chan_id_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "bco: " << setw(width_val) << bco_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "bco_full: " << setw(width_val) << bco_full_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "adc: " << setw(width_val) << adc_
       << setw(width_column) << " " << "|"
       << endl;
  
  cout << "| "
       << setw(width_first) << "adc_voltage: " << setw(width_val) << adc_voltage_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "ampl: "
       << (ampl_ == 0 ? color_red_ : color_cyan_) << setw(width_val) << ampl_ << color_cancel_
       << setw(width_column) << " " << "|";
  cout << setw(width_first) << "event: " << setw(width_val) << event_
       << setw(width_column) << " " << "|";
  cout << setw( 1*(width_first + width_val + width_column) + 3 - 3) << " "
       << "|"
       << endl;

  int width_item = 25;
  cout << "| "
       << setw(width_first) << right << "DAC config: ";
  for( int i=0; i<8; i++ )
    cout << setw(width_val) << dac_values_[i];
  cout << setw(width_line - 2 - width_first - width_val * 8 -1 ) << " "
       << "|"
       << endl;
  
  cout << "| "
       << setw(width_item) << left << "Is clustered (status)? ";

  if( this->clustering_status_ < 0 )
    cout << this->color_red_;
  else
    cout << this->color_cyan_;

  cout << setw(width_val) << left << this->clustering_status_ << this->color_cancel_      
       << setw( width_line - width_item - width_val - 3 ) << " "
       << "|"
       << endl;
    
  cout << "| "
       << setw(width_item) << "Cluster channels: ";
  for( int i=0; i<cluster_channels_.size(); i++ )
    cout << setw(width_val) << cluster_channels_[i] << " ";
  cout << setw(width_line - 2 - width_item - (width_val+1)*cluster_channels_.size() - 1) << " "
       << "|"
       << endl;

  cout << "| "
       << setw(width_item) << "Cluster ADC vols: ";
  for( int i=0; i<cluster_adc_voltages_.size(); i++ )
    cout << setw(width_val) << cluster_adc_voltages_[i] << " ";
  cout << setw(width_line - 2 - width_item - (width_val+1)*cluster_adc_voltages_.size() - 1) << " "
       << "|"
       << endl;

  cout << "| "
       << setw(width_item) << "Cluster ADC voltage: "
       << setw(width_val) << this->GetClusterADCVoltage()
       << setw(width_line - 2 - width_item - width_val - 1 ) << " "
       << "|"
       << endl;
  
  cout << "+" << string(width_line - 2,  '-' ) << "+" << endl;

  cout << right;
}

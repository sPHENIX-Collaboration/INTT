#include "INTThit.hh"

INTThit::INTThit( int adc      , int ampl     , int chip_id  , int fpga_id  ,
		  int module   ,  int chan_id  , int fem_id   , int bco      ,
		  int bco_full , int event, vector < int > dac_config )
{

    adc_	= adc;
    ampl_	= ampl;
    chip_id_	= chip_id;
    fpga_id_	= fpga_id;
    module_	= module;
    chan_id_	= chan_id;
    fem_id_	= fem_id;
    bco_	= bco;
    bco_full_	= bco_full;

    dac_ = ADC2DAC( dac_config, adc_ );

    Init();
  }

  
bool INTThit::IsNoise()
{
  bool is_noise = ampl_ != 0;
  is_noise = is_noise || chip_id_ < 1;
  is_noise = is_noise || chip_id_ > 27;
  is_noise = is_noise || chan_id_ < 0;
  is_noise = is_noise || chan_id_ > 128;
  is_noise = is_noise || adc_ < 0;
  is_noise = is_noise || adc_ > 8;
  is_noise = is_noise || module_ < 0;
  
  return is_noise;
}

void INTThit::AddForClustering( INTThit* hit )
{
  if( this->CanBeAdded( hit ) )
    {
      hits_added_.push_back( hit );
      //hit->SetAdded( true );
      hit->is_already_added_ = true;
    }
  
}

bool INTThit::Clustering()
{
  // init cluster variables with this hit information
  this->nhit_ = hits_added_.size() + 1;    
  this->dac_cluster_ = this->dac_;  
  this->chan_cluster_ = this->chan_col_ * this->dac_;
  this->chan_cluster_simple_ = this->chan_col_;

  this->max_chan_cluster_ = this->min_chan_cluster_ = this->chan_col_;
  this->max_chip_cluster_ = this->min_chip_cluster_ = this->chan_col_;

  //  chan_
  // loop over hits stored in this object to add/calc parameters
  for( const auto& hit : hits_added_ )
    {
      this->dac_cluster_ += hit->GetDac();
      this->chan_cluster_ += hit->GetChanCol() * hit->GetDac();
      this->chan_cluster_simple_ += hit->GetChanCol();


      // store max/min chip num
      this->max_chip_cluster_ = this->max_chip_cluster_ > hit->GetChipCol() ?  this->max_chip_cluster_ : hit->GetChipCol() ;
      this->min_chip_cluster_ = this->min_chip_cluster_ < hit->GetChipCol() ?  this->min_chip_cluster_ : hit->GetChipCol() ;

      // sore max/min chan num
      this->max_chan_cluster_ = this->max_chan_cluster_ > hit->GetChanCol() ?  this->max_chan_cluster_ : hit->GetChanCol() ;
      this->min_chan_cluster_ = this->min_chan_cluster_ < hit->GetChanCol() ?  this->min_chan_cluster_ : hit->GetChanCol() ;      
    }
  
  this->chan_cluster_ /= this->dac_cluster_; // the final step to get mean value
  this->chan_cluster_simple_ /= this->nhit_; // the final step to get mean value

  // starting point to get weighted std dev
  this->chan_cluster_sigma_ = pow( this->chan_col_ - this->chan_cluster_ ,2 ) * this->dac_;

  // loop again for std dev
  for( const auto& hit : hits_added_ )
    {
      this->chan_cluster_sigma_ += pow( hit->GetChanCol() - this->chan_cluster_, 2 ) * hit->GetDac();
    }

  // the final step to get weigted std dev
  this->chan_cluster_sigma_ = sqrt( this->chan_cluster_sigma_ / this->dac_cluster_ );

  // judge whether this cluster is over chip or not
  if( this->max_chan_cluster_ > 127 && this->min_chan_cluster_ <= 127 ) // y direction
    this->is_over_chip_ = true;

  
  if( hits_added_.size() == 0 )
    return false;

  
  return true;  
}

int INTThit::GetNhitWithAdc( int adc ) const
{
  int num = 0;
  if( adc_ == adc )
    num++;
  
  for( auto& hit : hits_added_ )
    if( hit->GetAdc() == adc )
      num++;

  return num;
}

// private 
void INTThit::Init()
{
  chip_col_ = chip_id_ < 14 ? chip_id_ : (chip_id_ - 13 );
  chan_col_ = chip_id_ < 14 ? chan_id_ : (255 - chan_id_ );
  nhit_ = 1;
  
  dac_cluster_ = dac_;
  //chip_center_ = chip_id_; // not ready
  chan_cluster_ = chan_id_;
  
  
  
}


void INTThit::Print( int verbosity = 0, bool is_header = false )
  {

    int width = verbosity > 0 ? 8 : 6;

    // header
    if( is_header )
      {
	width += 2;
	cout << "\e[32m";
	cout << setw( width ) << "FEM, "
	     << setw( width ) << "FPGA, "
	     << setw( width ) << "module, "
	     << setw( width ) << "chip, "
	     << setw( width ) << "ch, "
	     << setw( width ) << "ADC, "
	     << setw( width ) << "DAC, "
	     << setw( width ) << "BCO, "
	     << setw( width ) << "+full, ";
	  
	
	if( verbosity > 0 )
	  cout << setw( width ) << "ladder, "
	       << setw( width ) << "pulse, "
	       << setw( width ) << "#hit, "
	    //<< setw( width ) << "chip clst, "
	       << setw( width ) << "chan clst, "
	       << setw( width ) << "min ch, "
	       << setw( width ) << "max ch, "
	       << setw( width ) << "<ch>, "
	       << setw( width ) << "sigma(ch), "
	    //<< setw( width ) << "?, "
	       << setw( width ) << "dac clst, "
	       << setw( width ) << "over chip?, ";
	

	cout << endl;
	cout << "\e[m";

	width -= 2;
      }

    cout << setw( width ) << fem_id_ << ", "
	 << setw( width ) << fpga_id_ << ", "
	 << setw( width ) << module_ << ", "
	 << setw( width ) << chip_col_ << ", "
	 << setw( width ) << chan_col_ << ", "
	 << setw( width ) << adc_ << ", "
	 << setw( width ) << dac_ << ", "      
	 << setw( width ) << bco_ << ", "
	 << setw( width ) << bco_full_ << ", ";
      
    if( verbosity > 0 )      
      cout << setw( width ) << ladder_ << ", "
	   << setw( width ) << pulse_height_ << ", "
	   << setw( width ) << nhit_ << ", "
	   << setw( width ) << chan_cluster_ << ", "
	   << setw( width ) << min_chan_cluster_ << ", "
	   << setw( width ) << max_chan_cluster_ << ", "
	   << setw( width ) << chan_cluster_sigma_ << ", "
	   << setw( width ) << dac_cluster_ << ", "
	   << setw( width ) << is_over_chip_ << ", ";


    if( this->IsNoise() )
      cout << "noise hit";
    else
      cout << "not noise";
    
    cout << endl;
  }


bool INTThit::CanBeAdded( INTThit* hit )
{

  if( this->fem_id_ != hit->GetFemId() )
    return false;

  if( this->fpga_id_ != hit->GetFpgaId() )
    return false;
 
  if( this->module_ != hit->GetModule() )
    return false;

  if( this->chip_col_ != hit->GetChipCol() )
    return false;

  if( abs(this->chan_col_ - hit->GetChanCol()) <= 1 )
    return true;
  
  // loop over hits that already stored to judge
  for( const auto& hit_stored : hits_added_ )
    {

      // ch different <= 1 means neighbour
      if( abs(hit_stored->GetChanCol() - hit->GetChanCol() ) <= 1 )  // neighbour case
	return true;
      else // non-neighbour case
	continue;
    }
  
  return false; // it's fake
}

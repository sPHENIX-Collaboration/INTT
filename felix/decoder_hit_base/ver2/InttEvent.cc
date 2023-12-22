#pragma once

#include "InttEvent.h"

#ifdef __CINT__
ClassImp(InttHit)
ClassImp(InttEvent)
#endif // __CINT__

using namespace std;

static const int NHITS_INIT = 1000;

/////////////////////
InttHit::InttHit() {
  //cout << "ctor InttHit" << endl;
  pid = adc = ampl = chip_id
    = module = chan_id = bco = bco_full
    = evt = roc = barrel = layer
    = ladder = arm = full_fphx = full_roc
    = 0;

}

void InttHit::copy(InttHit& hit){
  //  //cout << "copy: " << hit.pid << " : " << endl;
  pid       = hit.pid;
  //  cout << "copy t1" << endl;
  adc       = hit.adc;
  ampl      = hit.ampl;
  chip_id   = hit.chip_id;
  module    = hit.module;
          
  //  cout << "copy1" << endl;
  chan_id   = hit.chan_id;
  bco       = hit.bco;
  bco_full  = hit.bco_full;

  //  cout << "copy2" << endl;
  evt       = hit.evt;
         
  roc       = hit.roc;
  barrel    = hit.barrel;
  layer     = hit.layer;
  ladder    = hit.ladder;
  arm       = hit.arm;
        
  //  cout << "copy3" << endl;
  full_fphx = hit.full_fphx;
  full_roc  = hit.full_roc;
  //  cout << "copy end" << endl;
}

bool InttHit::parameterCheck()
{
  if( pid <= 3000 || pid >= 3009 )
    return false;

  if( adc <= -1 || adc >= 9 )
    return false;
  
  if( ampl <= -1 || ampl >= 64 )
    return false;
  
  if( chip_id <= 0 || chip_id >=27 )
    return false;
  
  if ( module <= -1 || module >= 14 )
    return false;
      
  if( chan_id <= -1 || chan_id >= 129 )
    return false;
    
  if( bco <= -1 || bco >= 128 )
    return false;

  return true;
}

void InttHit::show( bool explanation_flag ){
  if(explanation_flag){
    cout << "   pid  module chip_id chan_id adc ampl clone?";
    cout << endl;
  }
  cout << "   ";
  cout << setw(6) << pid << " ";
  cout << setw(6) << module << " ";
  cout << setw(6) << chip_id << " ";
  cout << setw(6) << chan_id << " ";
  cout << setw(3) << adc << " ";
  cout << setw(4) << ampl << " ";
  cout << setw(2) << is_clone << " " ;
  cout << setw(2) << is_noise << " " ;
  cout << endl;
}

bool InttHit::is_first( const InttHit* hit1, const InttHit* hit2 )
{
  if( hit1->pid < hit2->pid )
    {
      return true;
    }
  else if( hit1->pid == hit2->pid )
    {
      if( hit1->module < hit2->module )
	{
	  return true;
	}
      else if( hit1->module == hit2->module )
	{     
	  if( hit1->chip_id < hit2->chip_id )
	    {
	      return true;
	    }
	  else if( hit1->chip_id == hit2->chip_id )
	    {	 
	      if( hit1->chan_id < hit2->chan_id )
		return true;
	    }
	}
    }
  
  return false;
}

bool InttHit::is_equal( const InttHit* hit1, const InttHit* hit2 )
{
  return (hit1->pid == hit2->pid)
    && (hit1->module == hit2->module)
    && (hit1->chip_id == hit2->chip_id)
    && (hit1->chan_id == hit2->chan_id);
}

Bool_t InttHit::IsEqualNoAdc(const TObject *obj) const
{
  const InttHit* objcp = dynamic_cast<const InttHit*>(obj);

  if( chan_id != objcp->chan_id )
    return false;
  
  if( chip_id != objcp->chip_id )
    return false;
  
  if( module != objcp->module )
    return false;
  
  if( pid != objcp->pid )
    return false;  
  
  return true;
}

Bool_t InttHit::IsEqual(const TObject *obj) const
{
  bool result = this->IsEqualNoAdc( obj );
  if( result == false )
    return false;

  const InttHit* objcp = dynamic_cast<const InttHit*>(obj);
  
  return (adc == objcp->adc);

}

Bool_t InttHit::IsSortable() const { return kTRUE;}


Int_t  InttHit::Compare(const TObject* obj) const {
  const InttHit* objcp = dynamic_cast<const InttHit*>(obj);
  if(     module < objcp->module) return -1;
  else if(module > objcp->module) return  1;
  else {
    if(     chip_id < objcp->chip_id)  return -1;
    else if(chip_id > objcp->chip_id)  return  1;
    else {
      if(     chan_id < objcp->chan_id)  return -1;
      else if(chan_id > objcp->chan_id)  return  1;

      return 0;
    }
  }
}



/////////////////////

InttEvent::InttEvent():
  evtSeq(0),
  bco(0),
  fNhits(0),
  fNhits_no_clone(0),
  fNhits_no_noise(0),
  fNhits_no_clone_noise(0),
  fhitArray(NULL),
  fhitArray_no_clone(NULL),
  fhitArray_no_noise(NULL)
{
  fhitArray = new TClonesArray("InttHit", NHITS_INIT);
  fhitArray_no_clone = new TClonesArray("InttHit", NHITS_INIT);
  fhitArray_no_noise = new TClonesArray("InttHit", NHITS_INIT);
}

InttEvent::~InttEvent(){
  if(fhitArray!=NULL) delete fhitArray;
  if(fhitArray_no_clone!=NULL) delete fhitArray_no_clone;
  if(fhitArray_no_noise!=NULL) delete fhitArray_no_noise;
  //cout << "dtor InttEvent" << endl;
}

InttHit* InttEvent::addHit( TClonesArray* hit_array, InttHit* hit ){
  //cout << "nhits: " << fNhits << endl;
  TClonesArray& hitArray = *hit_array;
  const auto num = hit_array->GetEntries();
  InttHit* hitnew = new(hitArray[ num ]) InttHit();
  //hitnew = hit;
  hitArray[ num ] = hit;
  //cout << "nhits: before return " << fNhits << endl;
  return hitnew;
}

InttHit* InttEvent::addHit(){
  //cout << "nhits: " << fNhits << endl;
  TClonesArray& hitArray = *fhitArray;
  InttHit* hitnew = new(hitArray[fNhits++]) InttHit();
  
  //cout << "nhits: before return " << fNhits << endl;
  return hitnew;
}

int InttEvent::getNHits(){
  return fNhits;//m_hitArray->GetEntries();
}

int InttEvent::getNHits( bool without_clone, bool without_noise ){
  if( without_clone && without_noise )
    return fNhits_no_clone_noise;
  else if( without_clone && !without_noise )
    return fNhits_no_clone;
  else if( !without_clone && without_noise )
    return fNhits_no_noise;
  
  return fNhits;
}

int InttEvent::getNHitsNoClone(){
  return getNHits( true, false );
}

int InttEvent::getNHitsNoNoise(){
  return getNHits( false, true );
}

int InttEvent::getNHitsNoCloneNoise(){
  return getNHits( true, true );
}


InttHit* InttEvent::getHitTemp(const int ihit, bool without_clone, bool without_noise)
{
  // For example
  // 
  // --------------------- List of hits ---------------------
  //  index     pid  module    chip    chan  clone?  noise?  index(no clone)
  //      0    3004       1      36       1       0       0                0
  //      1    3004       1      36       1       1       0          ignored
  //      2    3007       1       0      11       0       0                1
  //      3    3007       1       0      11       1       0          ignored
  
  if( ihit > getNHits(without_clone, without_noise ) )
    return NULL;
  
  if( !without_clone && !without_noise ) // only false && false
    return (InttHit*)fhitArray->UncheckedAt( ihit );
  else if( !without_clone && without_noise ) // no clone and noise required and it's not a clone and a noise
    return (InttHit*)fhitArray_no_noise->UncheckedAt( ihit );
  else if( without_clone && !without_noise ) // no clone and noise required and it's not a clone and a noise
    return (InttHit*)fhitArray_no_clone->UncheckedAt( ihit );
  else if( without_clone && without_noise ) // no clone and noise required and it's not a clone and a noise
    {
      cerr << "InttHit* InttEvent::getHit( const int ihit, bool without_clone = true, bool without_noise = true ) " << endl;
      cerr << "is not ready...." << endl;
      return NULL;
    }

  cerr << "InttHit* InttEvent::getHit( const int ihit, bool without_clone, bool without_noise)" << endl;
  cerr << "something wrong" << endl;
  return nullptr;
}

InttHit* InttEvent::getHit(const int ihit, bool without_clone, bool without_noise)
{
  // For example
  // 
  // --------------------- List of hits ---------------------
  //  index     pid  module    chip    chan  clone?  noise?  index(no clone)
  //      0    3004       1      36       1       0       0                0
  //      1    3004       1      36       1       1       0          ignored
  //      2    3007       1       0      11       0       0                1
  //      3    3007       1       0      11       1       0          ignored
  
  if( ihit > getNHits(without_clone, without_noise ) )
    return NULL;

  int counter = 0;
  for( int i=0; i<fhitArray->GetEntries(); i++ )
    {
      auto hit = (InttHit*)fhitArray->UncheckedAt( i );
      // cout << "loop " << i << " ";
      //hit->show();

      //cout << "GetHit\t" << "checking " << i << " th hit" << endl;
      //hit->show();
      //      cout << without_noise << "\t" << hit->is_noise << endl;
      if( !without_clone && !without_noise ) // only false && false
	return hit;

      if( without_clone && without_noise && !hit->is_clone && !hit->is_noise ) // no clone and noise required and it's not a clone and a noise
	{
	  counter++;
	  if( ihit == (counter-1) )
	    return hit;
	}
      else if( without_clone && !hit->is_clone ) // no clone required and it's not a clone
	{
	  counter++;	      
	  if( ihit == (counter-1) )
	    return hit;
	  
	}
      else if( without_noise && !hit->is_noise ) // no noise required and it's not a noise
	{
	  counter++;
	  if( ihit == (counter-1) )
	    return hit;
	}
      // else
      // 	{
      // 	  cout << "here?" << endl;

      // 	}
      
    }
  
  cerr << "InttHit* InttEvent::getHit( const int ihit, bool without_clone, bool without_noise)" << endl;
  cerr << "something wrong" << endl;
  return nullptr;
}

InttHit* InttEvent::getHit(const int ihit){
  return (ihit<getNHits()) ? (InttHit*)fhitArray->UncheckedAt(ihit) : NULL;
}

void InttEvent::clear() {
  fhitArray->Clear(); 
  fhitArray_no_clone->Clear(); 
  fhitArray_no_noise->Clear(); 
  fNhits = 0;
  fNhits_no_clone = 0;
  fNhits_no_noise = 0;
  fNhits_no_clone_noise = 0;

  evtSeq = 0;
  bco = 0;
};

void InttEvent::copy(InttEvent* org) {
  if(org==NULL) return;

  clear();

  evtSeq = org->evtSeq;
  bco    = org->bco;

  for(int ihit=0; ihit<org->getNHits(); ihit++){
    InttHit* hitnew = addHit();
    InttHit* hit    = org->getHit(ihit);
    hitnew->copy(*hit);
    //cout << "debug hit: " << endl;
    //hitnew->show();
    //hit->show();
  }
};

void InttEvent::show() {
  cout << "Evt : " << evtSeq << " 0x" << hex << bco << dec << endl;
  cout << "  Nhits : " << fNhits << endl;

  for(int ihit=0; ihit<fNhits; ihit++){
    InttHit* hit = getHit( ihit );
    hit->show( (ihit==0) );
  }
};

void InttEvent::showList( bool without_clone, bool without_noise )
{

  int width = 8;
  string header = "List of hits";
  if( without_clone )
    header += " without clones";
  if( without_noise )
    header += " without noises";
  
  string line_header( (width * 7 - header.size() - 2) / 2 , '-' );
  cout << line_header
       << " " << header << " "
       << line_header
       << endl;
  
  cout << setw( width ) << "index"
       << setw( width ) << "pid"
       << setw( width ) << "module"
       << setw( width ) << "chip"
       << setw( width ) << "chan"
       << setw( width ) << "clone?"
       << setw( width ) << "noise?"
       << endl;

  cout << "#hit = " << getNHits( without_clone, without_noise) << endl;
  
  for( int i=0; i<getNHits( without_clone, without_noise) ; i++ )
    {
      auto hit = getHit( i, without_clone, without_noise );
      
      cout << setw( width ) << i
	   << setw( width ) << hit->pid
	   << setw( width ) << hit->module
	   << setw( width ) << hit->chip_id
	   << setw( width ) << hit->chan_id
	   << setw( width ) << hit->is_clone
	   << setw( width ) << hit->is_noise
	   << endl;
    }
  string line_footer( width * 7, '-' );
  cout << line_footer << endl;

}


void InttEvent::showList()
{
  this->showList( false, false );
}

void InttEvent::sort() {
  fhitArray->Sort();
  fhitArray_no_clone->Sort();
  fhitArray_no_noise->Sort();

};

void InttEvent::checkCloneHit()
{

  const int Nhits = this->getNHits();
  for( int i=0; i<Nhits; i++ )
    {
      auto hit1 = this->getHit( i );
      hit1->setCloneFlag( false ); // need to be init again to overwrite status of the hit with the same index in the previous event
      for( int j=i+1; j<Nhits; j++ )
	{
	  auto hit2 = this->getHit( j );
	  hit2->setCloneFlag( hit2->IsEqual( hit1 ) );

	} // end of for( int j=i+1; j<Nhits; j++ )
      
    } // for( int i=0; i<Nhits; i++ )

  // set the number of no clone hit
  fNhits_no_clone = 0;
  for( int i=0; i<Nhits; i++ )
    {
      auto hit = this->getHit( i );
      if( hit->is_clone == false )
	{	  
	  fNhits_no_clone++;
	}
    }      

}

void InttEvent::checkNoiseHit()
{

  const int Nhits = this->getNHits();
  for( int i=0; i<Nhits; i++ )
    {
      auto hit1 = this->getHit( i );
      
      for( auto& noise : noise_hits )
	{
	  hit1->setNoiseFlag( false );
	  if( hit1->IsEqualNoAdc( noise ) == true )
	    {

	      hit1->setNoiseFlag( true );
	      break;
	    }
	  
	}
      
    }

  // set the number of no noise hits
  fhitArray_no_noise->Clear( "C" ); 
  //fNhits_no_noise = 0;
  for( int i=0; i<Nhits; i++ )
    {
      auto hit = this->getHit( i );
      if( hit->is_noise == false )
	{
	  addHit( fhitArray_no_noise , hit );
	  //fNhits_no_noise++;
	}
    }      

  fNhits_no_noise = fhitArray_no_noise->GetEntries();
  
}

bool InttEvent::setNoisyMap( string noisy_map_arg )
{
  noisy_map = noisy_map_arg;
  cout << "InttEvent::setNoisyMap\t" << noisy_map << endl;
  ifstream ifs( noisy_map );
  if( ifs.fail() )
    {
      cerr << "bool InttEvent::setNoisyMap( string noisy_map_arg )" << endl;
      cerr << noisy_map << " cannot be opened." << endl;
      cerr << "Nothing imported." << endl;
      return false;

    }

  string line;
  while( getline( ifs, line ) )
    {
      if( line[0] == '#' )
	continue;

      stringstream ss( line );
      int pid, module, chip_id, chan_id;
      ss >> pid >> module >> chip_id >> chan_id;
      
      InttHit* hit = new InttHit();
      // Take only some parameters. Those are enough for hot channel estimation
      hit->pid     = pid;
      hit->chip_id = chip_id;
      hit->module  = module;
      hit->chan_id = chan_id;
      noise_hits.push_back( hit );
    }

  return true;
}

void InttEvent::showNoisyChannels()
{
  cout << "------------------------------------" << endl;
  cout << " List of noisy channels in this run" << endl;
  for( auto& hit : noise_hits )
    hit->show();
  cout << endl;
}

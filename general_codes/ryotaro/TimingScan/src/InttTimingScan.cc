#include "InttTimingScan.h"
#include <bitset>

InttTimingScan::InttTimingScan(const std::string &name, int run):SubsysReco(name){
  std::cout << "InttTimingScan(const std::string &name) Calling the constructor" << std::endl;

  run_ = run;
  my_event_counter_=0;
};

InttTimingScan::~InttTimingScan(){
  std::cout << "InttTimingScan(const std::string &name) Calling the deconstructor" << std::endl;
};


int InttTimingScan::Init(PHCompositeNode *topNode){
  std::cout << "InttTimingScan::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run_);
  // std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d_felix7.root", run_);
  output_file_ = new TFile(filename.c_str(),"recreate");
  if ( !output_file_->IsOpen()){
    std::cerr << "ERROR : File '" << filename << "' not found or could not be opend." << std::endl;
    std::exit(1);
  }

  h_bco_diff_ = new TH1D("h_bco_diff", "h_bco_diff", 128, 0, 128);
  
  t_output_ = new TTree("tree", "tree");
  t_output_->Branch("bco_diff", &branch_bco_diff_, "bco_diff/s"); // "/s" means short integer or 16 bit unsigned int.

  if (mode_ == Mode::RawHitOnly) {
    this->PrepareHotChannelMap();
  }

  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::InitRun(PHCompositeNode *topNode){
  std::cout << "InttTimingScan::InitRun(PHCompositeNode *topNode) Initializing ... " << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::process_event(PHCompositeNode *topNode){
  switch (mode_) {
    case Mode::RawHitOnly:
      process_event_inttrawhit_only(topNode);
      break;
    case Mode::RawPlusTrkr:
      process_event_raw_plus_trkrhit(topNode);
      break;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTimingScan::process_event_inttrawhit_only(PHCompositeNode *topNode) {
  my_event_counter_++;
  if (my_event_counter_ % 200 == 0) { std::cout << "event id: " << my_event_counter_ << std::endl; }


  switch (daq_mode_) {
    case DaqMode::Official:
      for (int i=0; i<8; i++) {
        std::string node_name = Form("%s_%d",node_name_intt_raw_hit_.c_str(), i);
        node_inttrawhit_map_ = findNode::getClass<InttRawHitContainerv2>(topNode, node_name.c_str());
        this->FillHist(node_inttrawhit_map_);
      }
      break;    
    case DaqMode::Local:
      node_inttrawhit_map_ = findNode::getClass<InttRawHitContainerv2>(topNode,"INTTRAWHIT");
      this->FillHist(node_inttrawhit_map_);
      break;
  }

  /* This part is ported to FillHist() */
  // for ( unsigned int i = 0; i < node_inttrawhit_map_->get_nhits(); i++ ) {
  //   InttRawHit* hit = node_inttrawhit_map_->get_hit(i);

  //   /* some conversions needed to adjust to value range of hot_channel_map_ */
  //   int felix_server = hit->get_packetid() - 3001;
  //   int felix_channel = hit->get_fee();
  //   int chip = (hit->get_chip_id() - 1) % 26;
  //   int channel = hit->get_channel_id();

  //   if (hot_channel_map_.size() == 0) {std::cerr<<"map size zero"<<std::endl; exit(1);}
  //   bool is_from_hot_channel = hot_channel_map_.find(Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel)) != hot_channel_map_.end();
  //   if ( is_from_hot_channel ) {
  //     // std::cout << "from hot channel: " <<  Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel) << std::endl;
  //     continue; /* move to next hit */
  //   }

  //   uint16_t fphx_bco = hit->get_FPHX_BCO();
  //   uint64_t bco_full = hit->get_bco();
  //   // uint16_t bco_diff = 0;

  //   /* ここが変な振る舞いの原因? */
  //   // if( (uint16_t)(bco_full & 0x7f)  > (uint16_t)(fphx_bco) ){
  //   //   bco_diff = ( (uint16_t)fphx_bco + 128 ) - (uint16_t)((bco_full) & 0x7f );
  //   // } else {
  //   //   bco_diff = (uint16_t)fphx_bco - (uint16_t)(bco_full & 0x7f );
  //   // }
  //   uint64_t bco_diff = (fphx_bco - ( (bco_full) & 0x7fU) + 128) % 128;
    
  //   h_bco_diff_->Fill(bco_diff);
    
  //   branch_bco_diff_ = bco_diff;
  //   t_output_->Fill(); /* Fills a bco_diff to the TTree */
  // }

  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::process_event_raw_plus_trkrhit(PHCompositeNode *topNode){
  my_event_counter_++;

  if (my_event_counter_ % 200 == 0) { std::cout << "event id: " << my_event_counter_ << std::endl; }
  // std::cout << "event id: " << my_event_counter_ << std::endl;

  node_trkrhitset_map_ = findNode::getClass<TrkrHitSetContainer>(topNode,"TRKR_HITSET"); // 返り値の型は TrkrHitSetContainer ??
  node_inttrawhit_map_ = findNode::getClass<InttRawHitContainerv2>(topNode,"INTTRAWHIT");
  
  /* This can be omitted, I think. We can use node_inttrawhit_map_->get_bco() instead.  */
  // InttEventInfo* node_intteventheader_map_ = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");
  // auto bco_full = node_intteventheader_map_->get_bco_full();
  auto bco_full = node_inttrawhit_map_->get_hit(0)->get_bco();

  auto bco_event_counter_pair = this->GetBcoEventCounter();
  for (int i=0; i<bco_event_counter_pair.size(); i++) {
    uint16_t fphx_bco = bco_event_counter_pair[i].first;
    uint16_t bco_diff = 0;
    
    if( (uint16_t)(bco_full & 0x7f)  > (uint16_t)(fphx_bco) ){
      bco_diff = ( (uint16_t)fphx_bco + 128 ) - (uint16_t)((bco_full) & 0x7f );
    } else {
      bco_diff = (uint16_t)fphx_bco - (uint16_t)(bco_full & 0x7f );
    }
    
    h_bco_diff_->Fill(bco_diff);
    
    branch_bco_diff_ = bco_diff;
    t_output_->Fill(); /* Fills a bco_diff to the TTree */
 
    std::cout << "event id: " << my_event_counter_ << std::endl;
    
    //std::cout << std::bitset<32>(fphx_bco+128) << '-' << std::bitset<32>((bco_full)&(0x7f)) << '=' << std::bitset<32>( bco_diff )  << std::endl;
  }
  

  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::ResetEvent(PHCompositeNode *topNode){
  // std::cout << "InttTimingScan::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;

};

int InttTimingScan::EndRun(const int runnumber){
  std::cout << "InttTimingScan::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::End(PHCompositeNode *topNode){
  std::cout << "InttTimingScan::End(PHCompositeNode *topNode) This is the End..." << std::endl;


  output_file_->cd();
  h_bco_diff_->Scale( 1.0 / my_event_counter_);
  h_bco_diff_->Write();
  t_output_->Write();

  output_file_->Close();

  //h_bco_diff_->SaveAs("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/TimingScan/output/image/run.png");

  return Fun4AllReturnCodes::EVENT_OK;
};

int InttTimingScan::Reset(PHCompositeNode * /*topNode*/){
  std::cout << "InttTimingScan::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

void InttTimingScan::Print(const std::string &what) const
{
  std::cout << "InttTimingScan::Print(const std::string &what) const Printing info for " << what << std::endl;
}



















































std::vector< std::pair<uint16_t, int> > InttTimingScan::GetBcoEventCounter(){
  std::cout << "GetBcoEventCounter Called." << std::endl;

  std::vector< std::pair< InttNameSpace::Online_s, unsigned int> > online_hits = this->GetHits( node_trkrhitset_map_->getHitSets() );
  std::vector<InttRawHit*> raw_hits = this->GetRawHitsWithoutClone();

  std::vector< std::pair<uint16_t, int> > rtn;

  
  // if( raw_hits.size() > 20 )
  //   return rtn;
  InttDacMap* dac_map = new InttDacMap();
  
  int counter = 0;
  for( int i=0; i<online_hits.size(); i++ ){
    ////////////////////////////////////////////
    // struct Online_s                        //
    //   int lyr, int ldr,  int arm           //
    //   int chp, int chn                     //
    ////////////////////////////////////////////
    auto online_hit = online_hits[i].first;
    auto dac = online_hits[i].second;

    ////////////////////////////////////////////
    // struct RawData_s:                      //
    //   int felix_server, int felix_channel  //
    //   int chip,         int channel,       //
    ////////////////////////////////////////////
    // InttNameSpace::RawData_s
    auto raw_data  = InttNameSpace::ToRawData( online_hit );
      
    ////////////////////////////////////////////
    // struct Offline_s                       //
    //   int layer,     int ladder_phi,       //
    //   int ladder_z,  int strip_x,          //
    //   int strip_y,                         //
    ////////////////////////////////////////////
    // InttNameSpace::Offline_s
    auto offline_hit = InttNameSpace::ToOffline( online_hit );

    // consistency confirmed, no need anymore
    // if( online_hit.chp != raw_data.chip
    // 	  || online_hit.chn != raw_data.channel )
    // 	{
      
    // 	  cout << setw(4) << online_hit.chp
    // 	       << setw(5) << online_hit.chn
    // 	       << endl;
	  
    // 	  cout << setw(4) << raw_data.chip
    // 	       << setw(5) << raw_data.channel
    // 	       << " | "
    // 	       << setw(3) << raw_data.felix_server
    // 	       << setw(5) << raw_data.felix_channel
    // 	       << endl;
	  
    // 	  cout << "---------------------------------------------------" << endl;
    // 	}

    // cout << "TrkrHit: "
    // 	   << setw(4) << i
    // 	   << ": " 
    // 	   << setw(3) << raw_data.felix_server
    // 	   << setw(5) << raw_data.felix_channel
    // 	   << setw(4) << raw_data.chip
    // 	   << setw(6) << raw_data.channel
    // 	   << setw(4) << dac
    // 	   << " vs ";
	  //	   << endl;
    std::vector<int> used_index( raw_hits.size() );
     
    for( int j=0; j<raw_hits.size(); j++ ){
	  
	    auto raw_hit = raw_hits[j];

	    int chan_raw		= raw_hit->get_channel_id();
	    // first selections
	    if( chan_raw != raw_data.channel ) continue;
	  
	    // uint16_t InttRawHit::get_chip_id
	    int chip_raw		= raw_hit->get_chip_id();    
	    if(chip_raw  > 26){
        chip_raw = chip_raw - 26;
      }
	    chip_raw--; // to change from 1-base to 0-base
	    if( chip_raw != raw_data.chip ) continue;

	    int felix_raw		= raw_hit->get_packetid() - 3001;
	    int felix_ch_raw	= raw_hit->get_fee();
	    bool is_same = (felix_raw == raw_data.felix_server
			  || felix_ch_raw == raw_data.felix_channel );
			  // || adc_raw == adc );

	    if( is_same == false ) continue;

	    auto adc_raw		= raw_hit->get_adc();
	    /*
	    virtual unsigned short GetDAC(const uint& felix_server,
                                const uint& felix_channel,
                                const uint& chip,
                                const uint& channel,
                                const uint& adc);
      */
	    auto dac_raw = dac_map->GetDAC( felix_raw, felix_ch_raw, chip_raw, chan_raw, adc_raw );
	    if( dac_raw != dac ) continue;
	  
	    auto bco_raw		= raw_hit->get_FPHX_BCO();
      //if (bco_raw > 256) std::cout << bco_raw << std::endl;
	    int event_counter_raw	= raw_hit->get_event_counter(); // uint32_t IttRawHit::get_event_counter()
	    std::pair<uint16_t, int> bco_event_counter(bco_raw, event_counter_raw);
	    
      /* before */
      rtn.push_back( bco_event_counter );
	    counter++;

      /* after */
      // u_int32_t felix_server = raw_hit->get_packetid() - 3001;
      // if (felix_server == 7) {
      //   std::cout << felix_server;
      //   rtn.push_back( bco_event_counter );
      //   counter++;
      // }
      
     
      //if( felix_raw == raw_data.felix_server )
	    //	  if( felix_ch_raw == raw_data.felix_channel && felix_ch_raw != 0 )
	      // cout << "RawHit: "
	      // 	 << setw(4) << j
	      // 	 << ": " 
	      // 	 << setw(3) << felix_raw
	      // 	 << setw(5) << felix_ch_raw
	      // 	 << setw(4) << chip_raw
	      // 	 << setw(6) << chan_raw
	      // 	 << setw(4) << adc_raw
	      // 	 << endl;
	  		
	    // cout << "( "
	    //      << setw(3) << bco_raw << ", " << setw(1) << adc_raw << "-> " << dac
	    //      << "), ";
	    //used_index.push_back( j );
	    //raw_hits.erase( raw_hits.begin() + j );
	    //break;
	  }
      //      cout << endl;
      
  }
  

  // cout << counter << " / " << online_hits.size() << " found their corresponding raw hit while there are "
  //      << raw_hits.size() << " raw hits in this event." 
  //      << endl;
  return rtn;
}

std::vector<InttRawHit*> InttTimingScan::GetRawHitsWithoutClone(){
  std::cout << "GetRawHitsWithoutClone() Called." << std::endl;

  auto hits = this->GetRawHits();
  std::vector<InttRawHit*> rtn;

  for( int i=0; i<hits.size(); i++ ){

    bool is_same_found = false;
    for( int j=i+1; j<hits.size(); j++ ){

      is_same_found = is_same_found || this->IsSame( hits[i], hits[j] );
      if( is_same_found == true ){
        break;
      }

    } // end of for( hit_j )

    // if this hit is still unique (same hit is not found), keep it
    if( is_same_found == false ){
      rtn.push_back( hits[i] );
    }

  } // end of for( hit_i )

  
  return rtn;
}

std::vector<InttRawHit*> InttTimingScan::GetRawHits(){
  
  std::vector<InttRawHit*> hits;
  auto raw_hit_num = node_inttrawhit_map_->get_nhits();
  for (unsigned int i = 0; i < raw_hit_num; i++){
    auto hit = node_inttrawhit_map_->get_hit(i);
    hits.push_back( hit );
  }
  
  return hits;
}


// Create a easy-to-use object from tracker hit sets
std::vector< std::pair<InttNameSpace::Online_s, unsigned int> > InttTimingScan::GetHits(TrkrHitSetContainer::ConstRange hitsets){
  std::cout << "GetHits(TrkrHitSetContainer::ConstRange hitsets) Called." << std::endl;
  
  std::vector< std::pair<InttNameSpace::Online_s, unsigned int> > hits_information;
  auto current = hitsets.first;
  while( current != hitsets.second ){
    auto hitset = (*current).second;

    //TrkrDefs::printBits( (*current).first );

    auto layer = TrkrDefs::getLayer( (*current).first );
    // layer: 0, 1, 2   : MVTX
    //        3, 4, 5, 6: INTT
    //        6<        : TPC

    // Take hits from only INTT
    if( !(3<= layer && layer <= 6) )	continue;
      
    auto detector_id = TrkrDefs::getTrkrId( (*current).first );
    auto phi         = InttDefs::getLadderPhiId( (*current).first ); //TrkrDefs::getPhiElement( (*current).first );
    auto z           = InttDefs::getLadderZId( (*current).first ); // TrkrDefs::getZElement( (*current).first );
    // cout << "-----> "
    // 	   << int(detector_id) << "\t" << int(layer) << "\t"
    // 	   << int(phi) << "\t" << int(z) ;
    // 	//<< endl;
    // cout << " + " << (*current).first << "\t"
    // 	   << hitset->size()
    // 	   << endl;

    auto hits = hitset->getHits(); // ConstRange = std::pair< ConstIterator, ConstIterator >
    auto current2 = hits.first; // ConstIterator = Map::const_iterator, here, Map = std::map < TrkrDefs::hitkey, TrkrHit* >, This Map is different from the Map for TrkrHitSetContainer!!!
    while( current2 != hits.second ){
	    auto key = (*current2).first;
	    auto hit = (*current2).second;

      auto col = InttDefs::getCol( key );
	    auto row = InttDefs::getRow( key );

	    InttNameSpace::Offline_s offline_hit;
	    offline_hit.layer = layer;
	    offline_hit.ladder_phi = phi;
	    offline_hit.ladder_z = z;
	    offline_hit.strip_x = row;
	    offline_hit.strip_y = col;

	    InttNameSpace::Online_s online_hit = InttNameSpace::ToOnline( offline_hit );
	  
	    auto hit_info = std::pair < InttNameSpace::Online_s, unsigned int >( online_hit, hit->getAdc() );
	    hits_information.push_back( hit_info );
  
	    current2++;
	  }
      
    current++;
  }

  return hits_information;
};

bool InttTimingScan::IsSame(InttRawHit* hit1, InttRawHit* hit2){

  if( hit1->get_word() != hit2->get_word() ) // only this might be enough
    return false;
  else if( hit1->get_bco() != hit2->get_bco() )
    return false;
  else if( hit1->get_FPHX_BCO() != hit2->get_FPHX_BCO() )
    return false;
  else if( hit1->get_channel_id() != hit2->get_channel_id() )
    return false;
  else if( hit1->get_chip_id() != hit2->get_chip_id() )
    return false;
  else if( hit1->get_fee() != hit2->get_fee() )
    return false;
  else if( hit1->get_packetid() != hit2->get_packetid() )
    return false;
  else if( hit1->get_adc() != hit2->get_adc() )
    return false;
  else if( hit1->get_full_FPHX() != hit2->get_full_FPHX() )
    return false;
  // else if( hit1->get_full_ROC() != hit2->get_full_ROC() ) // maybe no value is assigned?
  //   return false;
  // else if( hit1->get_amplitude()!= hit2->get_amplitude()) // maybe no value is assigned?
  //   return false;

  // hits pass all comparison are the same
  return true;
};


void InttTimingScan::PrepareHotChannelMap(){
    std::cout << "InttTimingScan::PrepareHotChannelMap called" << std::endl;
  TFile* input_file_hot_channel_map = TFile::Open(this->file_path_to_hot_channel_map_.c_str());
  if (!input_file_hot_channel_map)  {
    std::cout << "InttTimingScan::PrepareHotChannelMap - hot channel file not found" << std::endl;
    exit(EXIT_FAILURE);
  }

  TTree* tree_hot_channel = (TTree*)input_file_hot_channel_map->Get(tree_name_hot_channel_.c_str());

  int IID;             
  int Ichannel;        
  int Ichip;           
  int Ifelix_channel;  
  int Ifelix_server;   
  int Iflag;

  tree_hot_channel->SetBranchAddress("IID",&IID);
  tree_hot_channel->SetBranchAddress("Ichannel",&Ichannel);
  tree_hot_channel->SetBranchAddress("Ichip",&Ichip);
  tree_hot_channel->SetBranchAddress("Ifelix_channel",&Ifelix_channel);
  tree_hot_channel->SetBranchAddress("Ifelix_server",&Ifelix_server);
  tree_hot_channel->SetBranchAddress("Iflag",&Iflag);

  // std::map<std::string,std::string> hot_channel_map; /* key はともかく、value を string にする必要はないような*/

  for (int i=0; i<tree_hot_channel->GetEntries(); i++) {
    tree_hot_channel->GetEntry(i);
    this->hot_channel_map_[Form("%d_%d_%d_%d",Ifelix_server,Ifelix_channel,Ichip,Ichannel)] = Form("%d",Iflag);
  }
};

void InttTimingScan::FillHist(InttRawHitContainerv2* container_inttrawhit) {
  for ( unsigned int i = 0; i < container_inttrawhit->get_nhits(); i++ ) {
    InttRawHit* hit = container_inttrawhit->get_hit(i);

    /* some conversions needed to adjust to value range of hot_channel_map_ */
    int felix_server = hit->get_packetid() - 3001;
    int felix_channel = hit->get_fee();
    int chip = (hit->get_chip_id() - 1) % 26;
    int channel = hit->get_channel_id();

    if (hot_channel_map_.size() == 0) {std::cerr<<"map size zero"<<std::endl; exit(1);}
    bool is_from_hot_channel = hot_channel_map_.find(Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel)) != hot_channel_map_.end();
    if ( is_from_hot_channel ) {
      // std::cout << "from hot channel: " <<  Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel) << std::endl;
      continue; /* move to next hit */
    }

    uint16_t fphx_bco = hit->get_FPHX_BCO();
    uint64_t bco_full = hit->get_bco();
    // uint16_t bco_diff = 0;

    /* ここが変な振る舞いの原因? */
    // if( (uint16_t)(bco_full & 0x7f)  > (uint16_t)(fphx_bco) ){
    //   bco_diff = ( (uint16_t)fphx_bco + 128 ) - (uint16_t)((bco_full) & 0x7f );
    // } else {
    //   bco_diff = (uint16_t)fphx_bco - (uint16_t)(bco_full & 0x7f );
    // }
    uint64_t bco_diff = (fphx_bco - ( (bco_full) & 0x7fU) + 128) % 128;
    
    h_bco_diff_->Fill(bco_diff);
    
    branch_bco_diff_ = bco_diff;
    t_output_->Fill(); /* Fills a bco_diff to the TTree */
  }
}
// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTT_MBD_V2_H
#define INTT_MBD_V2_H

#include <fun4all/SubsysReco.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <ffaobjects/EventHeader.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <phhepmc/PHHepMCGenHelper.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop
#include <HepMC/GenParticle.h>
#include <HepMC/HeavyIon.h> // for HeavyIon
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h>

#include <g4eval/SvtxClusterEval.h>
#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxHitEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <phool/getClass.h>

#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <intt/CylinderGeomIntt.h>

#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>
#include <phool/getClass.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>
#include <trackbase/InttEventInfo.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
// #include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/MbdVertexMapv1.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <mbd/MbdGeom.h>

#include <ffarawobjects/Gl1Packetv2.h>

//add by manami
#include <cdbobjects/CDBTTree.h>
#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/EventplaneinfoMap.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TVector3.h>
#include <TDatabasePDG.h>
#include <TParticle.h>
#include <TParticlePDG.h>
#include <TH2.h>

class Gl1Packet;
class PHCompositeNode;
class SvtxTrack;
class SvtxTrackMap;
class TrkrHitSetContainer;
class InttEventInfo;
class MinimumBiasInfo;
class PHHepMCGenEvent;
class PHHepMCGenEventMap;
class PHHepMCGenHelper;
class TrkrClusterContainer;
class CentralityInfo;
class MbdPmtHit;
class MbdOut;
class MbdPmtContainer;
class MbdVertex;
class MbdVertexMap;
class MbdGeom;
class EventplaneInfoMap;
class Eventplaneinfo;

class InttMbdv2 : public SubsysReco
{
  public:
    InttMbdv2(const std::string &name = "InttMbdv2Analyzer",    //
               const std::string &outputfile = "INTTdNdEta.root", //
               const bool &isData = false);

    ~InttMbdv2() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
     */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
     */
    int InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
     */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

    void GetHEPMC(bool b) { _get_hepmc_info = b; }

    void GetRecoCluster(bool b) { _get_reco_cluster = b; }

    void GetINTTdata(bool b) { _get_intt_data = b; }

    void GetCentrality(bool b) { _get_centrality = b; }

    void GetPMTInfo(bool b) { _get_pmt_info = b; }

    void GetInttRawHit(bool b) { _get_inttrawhit = b; }

    void GetTrkrHit(bool b) { _get_trkr_hit = b; }

    void GetPHG4(bool b) { _get_phg4_info = b; }

    void GetEPDInfo(bool b) { _get_epd_info = b; } //add by manami
    
    void GetEventPlaneInfo(bool b) { _get_EP_info = b; } //add by manami

    void set_Mbd_vtx_cut(float vtx_cut){ _vtx_cut = vtx_cut; }
    
    void set_Epd_charge_cut(float charge_cut){ _epd_charge_cut = charge_cut; }

  private:
    void ResetVectors();
    void GetHEPMCInfo(PHCompositeNode *topNode);
    void GetRecoClusterInfo(PHCompositeNode *topNode);
    void GetTruthClusterInfo(PHCompositeNode *topNode);
    void GetCentralityInfo(PHCompositeNode *topNode);
    void GetInttRawHitInfo(PHCompositeNode *topNode);
    void GetTrkrHitInfo(PHCompositeNode *topNode);
    void GetPHG4Info(PHCompositeNode *topNode);

    long long live_trigger_decimal; // note : added by CW
    Gl1Packet *p_gl1; // note : added by CW
    void GetLiveTrigger(PHCompositeNode *topNode); // note : added by CW
    static std::vector<int> prepare_trigger_vec(long long trigger_input); // note : added by CW

    bool _get_hepmc_info;
    bool _get_truth_cluster;
    bool _get_reco_cluster;
    bool _get_centrality;
    bool _get_intt_data;
    bool _get_inttrawhit;
    bool _get_trkr_hit;
    bool _get_phg4_info;
    bool _get_pmt_info;
    bool _get_epd_info;
    bool _get_EP_info;

    unsigned int eventNum = 0;
    std::string _outputFile;
    bool IsData;

    //TH2S *bcodiff;
    // int InputFileListIndex;
    // int NEvtPerFile;

    /*
    TH2S *hitMap_43;  //add by Manami
    TH2S *hitMap_46;  //add by Manami
    TH2S *hitMap_41;  //add by Manami

    int clusterNum_43 = 0;
    int clusterNum_46 = 0;
    int clusterNum_41 = 0;
    */

    //TH2S *hitMap[5];
    //int clusNum[5]={0,0,0,0,0};

    TTree *outtree;
    int event_, evt_sequence_;
    uint64_t intt_bco;
    // Centrality and MBD stuff
    float centrality_bimp_;
    float centrality_impactparam_;
    float centrality_mbd_;
    float centrality_mbdquantity_;
    int ncoll_;
    int npart_;
    UShort_t clk;
    UShort_t femclk;
    Short_t mbd_north_npmt;
    Short_t mbd_south_npmt;
    float mbd_south_charge_sum;
    float mbd_north_charge_sum;
    float mbd_charge_sum;
    float mbd_charge_asymm;
    float mbd_z_vtx;
    //float m_pmt_q[128];
    bool is_min_bias;
    bool is_min_bias_wozdc;
    std::vector<float> MbdPhi_;  //add by Manami
    std::vector<float> m_pmt_q;  //add by Manami

    //add by manami
    std::vector<float> EpdPhis_;
    std::vector<float> EpdPhin_;
    std::vector<float> EpdPhis_charge;
    std::vector<float> EpdPhin_charge;
    std::string m_sEPDMapName;
    std::string m_sEPDfieldname; 
    CDBTTree *cdbttree{nullptr};
    std::vector<unsigned int> vkey;
    unsigned int key{999};	

    float _vtx_cut{20.0};
    float _epd_charge_cut{1.0};

    float EPQx_south;
    float EPQy_south;
    float EPQx_north;
    float EPQy_north;
	    
    float psi2s_raw;
    float psi2n_raw;

    float Epds_charge;
    float Epdn_charge;

    // Truth primary vertex information
    float TruthPV_trig_x_;
    float TruthPV_trig_y_;
    float TruthPV_trig_z_;
    int NTruthVtx_;

    // HepMC information - final state particles
    int NHepMCFSPart_;
    int signal_process_id_;
    std::vector<float> HepMCFSPrtl_Pt_;
    std::vector<float> HepMCFSPrtl_Eta_;
    std::vector<float> HepMCFSPrtl_Phi_;
    std::vector<float> HepMCFSPrtl_E_;
    std::vector<float> HepMCFSPrtl_prodx_;
    std::vector<float> HepMCFSPrtl_prody_;
    std::vector<float> HepMCFSPrtl_prodz_;
    std::vector<int> HepMCFSPrtl_PID_;

    // Reconstructed cluster information
    int NClus_;
    int NClus_Layer1_;
    std::vector<int> ClusLayer_;
    std::vector<int> ClusHitcount_;
    std::vector<int> ClusTimeBucketId_;
    std::vector<float> ClusX_;
    std::vector<float> ClusY_;
    std::vector<float> ClusZ_;
    std::vector<float> ClusR_;
    std::vector<float> ClusPhi_;
    std::vector<float> ClusEta_;

    std::vector<float> ClusLocalX_;
    std::vector<float> ClusLocalY_;

    std::vector<unsigned int> ClusAdc_;
    std::vector<float> ClusPhiSize_;
    std::vector<float> ClusZSize_;
    std::vector<uint8_t> ClusLadderZId_;
    std::vector<uint8_t> ClusLadderPhiId_;
    std::vector<uint32_t> ClusTrkrHitSetKey_;

    // Truth cluster information
    int NTruthLayers_;
    std::vector<int> ClusTruthCKeys_;
    std::vector<int> TruthClusPhiSize_;
    std::vector<int> TruthClusZSize_;
    std::vector<int> PrimaryTruthClusPhiSize_;
    std::vector<int> PrimaryTruthClusZSize_;
    std::vector<int> ClusNG4Particles_;
    std::vector<int> ClusNPrimaryG4Particles_;
    std::vector<int> TruthClusNRecoClus_;
    std::vector<int> PrimaryTruthClusNRecoClus_;

    // InttRawHit information
    int NInttRawHits_;
    std::vector<uint64_t> InttRawHit_bco_;
    std::vector<uint32_t> InttRawHit_packetid_;
    std::vector<uint32_t> InttRawHit_word_;
    std::vector<uint16_t> InttRawHit_fee_;
    std::vector<uint16_t> InttRawHit_channel_id_;
    std::vector<uint16_t> InttRawHit_chip_id_;
    std::vector<uint16_t> InttRawHit_adc_;
    std::vector<uint16_t> InttRawHit_FPHX_BCO_;
    std::vector<uint16_t> InttRawHit_full_FPHX_;
    std::vector<uint16_t> InttRawHit_full_ROC_;
    std::vector<uint16_t> InttRawHit_amplitude_;

    // TrkrHit information
    int NTrkrhits_;
    std::vector<uint16_t> TrkrHitRow_;
    std::vector<uint16_t> TrkrHitColumn_;
    std::vector<uint16_t> TrkrHitADC_;
    std::vector<uint8_t> TrkrHitLadderZId_;
    std::vector<uint8_t> TrkrHitLadderPhiId_;
    std::vector<uint8_t> TrkrHitLayer_;
    std::vector<int>     TrkrHitTimeBucketId_; // note : added by CW

    // PHG4 information (from all PHG4Particles)
    int NPrimaryG4P_;
    int NPrimaryG4P_promptChargeadron_;
    std::vector<float> PrimaryG4P_Pt_;
    std::vector<float> PrimaryG4P_Eta_;
    std::vector<float> PrimaryG4P_Phi_;
    std::vector<float> PrimaryG4P_E_;
    std::vector<int> PrimaryG4P_PID_;
    std::vector<TString> PrimaryG4P_ParticleClass_;
    std::vector<bool> PrimaryG4P_isStable_;
    std::vector<double> PrimaryG4P_Charge_;
    std::vector<bool> PrimaryG4P_isChargeHadron_;

    // note : added by CW,
    std::vector<int> live_trigger_vec;

    EventHeader *eventheader = nullptr;
    InttEventInfo *intteventinfo = nullptr;

    PHHepMCGenEventMap *m_geneventmap = nullptr;
    PHHepMCGenEvent *m_genevt = nullptr;

    SvtxEvalStack *svtx_evalstack = nullptr;
    SvtxTruthEval *truth_eval = nullptr;
    SvtxClusterEval *clustereval = nullptr;
    SvtxHitEval *hiteval = nullptr;

    TrkrClusterContainerv4 *dst_clustermap = nullptr;
    TrkrClusterHitAssoc *clusterhitmap = nullptr;
    InttRawHitContainer *inttrawhitcontainer = nullptr;
    TrkrHitSetContainer *hitsets = nullptr;
    ActsGeometry *_tgeometry = nullptr;
    PHG4CylinderGeomContainer *_intt_geom_container = nullptr;
    PHG4TruthInfoContainer *m_truth_info = nullptr;
    CentralityInfo *m_CentInfo = nullptr;
    MinimumBiasInfo *_minimumbiasinfo = nullptr;
    MbdOut *m_mbdout = nullptr;
    MbdPmtContainer *m_mbdpmtcontainer = nullptr;
    GlobalVertexMap *m_glbvtxmap = nullptr;
    GlobalVertex *m_glbvtx = nullptr;
    MbdVertexMapv1 *m_mbdvtxmap = nullptr;
    MbdVertex *m_mbdvtx = nullptr;
    MbdGeom *mbdgeom = nullptr;
};

#endif // INTT_MBD_V2_H

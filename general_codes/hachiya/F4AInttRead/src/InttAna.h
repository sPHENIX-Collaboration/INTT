#ifndef INTTANA_H__
#define INTTANA_H__

#include <fun4all/SubsysReco.h>
#include <trackbase/ActsGeometry.h>
//
#include <string>
#include <vector>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;
class TrkrCluster;

/// Definition of this analysis module class
class InttAna : public SubsysReco
{
 public:
  struct evtbco_info{
    int      evt_gl1 {-1};
    uint     evt_intt{ 0};
    int      evt_mbd {-1};
    uint64_t bco_gl1 {0};
    uint64_t bco_intt{0};
    uint64_t bco_mbd {0};

    void clear() {
      evt_gl1  = -1;
      evt_intt =  0;
      evt_mbd  = -1;
      bco_gl1  =  0;
      bco_intt =  0;
      bco_mbd  =  0;
    };

    void copy(const evtbco_info& info) {
      evt_gl1  = info.evt_gl1 ;
      evt_intt = info.evt_intt;
      evt_mbd  = info.evt_mbd ;
      bco_gl1  = info.bco_gl1 ;
      bco_intt = info.bco_intt;
      bco_mbd  = info.bco_mbd ;
    };
  };

  struct cluspair {
    double p1_ang;
    double p2_ang;
    double p1_r;
    double p2_r;
    double p1_x;
    double p2_x;
    double p1_y;
    double p2_y;
    double p1_z;
    double p2_z;
    double p1_the;
    double p2_the;
    double dca_p0;
    double len_p0;

    double cx, cy, radius;
    double px, py, pz, pt;
    double phi, theta;
    int    charge;

    double slope_xy; // straight line fit
    double offset_xy;
    double chi2_xy;

    double slope_rz; // straight line fit
    double offset_rz;
    double chi2_rz;

    double prj_emc[3];
    double pos_emc[3];


    TrkrCluster* p1;
    TrkrCluster* p2;

  };

 public:
  /// Constructor
  InttAna(const std::string &name = "InttAna",
          const std::string &fname = "AnaTutorial.root");

  // Destructor
  virtual ~InttAna();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


  void setInttRawData(InttRawData *rawModule) { _rawModule = rawModule; }

  void readRawHit(PHCompositeNode *);

  void setBeamCenter(float x=0, float y=0) {xbeam_=x; ybeam_=y;}
  void setMvtxOffset(float x=0, float y=0) {xmvtx_=x; ymvtx_=y;}
  void useSimVtx(const bool& flag) { m_useSimVtx=flag; }

 public:
  double m_xvertex, m_yvertex, m_zvertex;
  std::vector<cluspair> vcluspair;

 private:
  void linefitXYplane(cluspair& clspair, Acts::Vector3& beamspot);
  void linefitRZplane(cluspair& clspair, Acts::Vector3& beamspot);

  double linefit(std::vector<double>& vx, std::vector<double>& vy, std::vector<double>& vey, double& slope, double& offset); // return; chi2

  void calculateMomentum(cluspair& clspair, Acts::Vector3& pos, int order=0); // 0:pos=beamspot, 2:outer detector
  void calculate2DMomentum(std::vector<double>& x,std::vector<double>& y, 
                           double& cx, double& cy, double& radius, double& px, double& py, int& charge);
  void  calculatepZ(const double &pt, const double& slope_rz, double& pz);

  void projectTrack(cluspair& clspair, Acts::Vector3& beamspot, const double& R, double &prjx, double& prjy, double& prjz);
  void projectToXY(cluspair& clspair, Acts::Vector3& beamspot, const double& R, double &prjx, double& prjy);
  void projectToRZ(cluspair& clspair, Acts::Vector3& beamspot, const double& R, double& prjz);

  float calc_pair(
                float pxp, float pyp, float pzp, float Mp,
		float pxm, float pym, float pzm, float Mm,
		float& Mee,  float& px,   float& py,   float& pz, float& pt,
		float& thev, float& ptep, float& ptem, float& phiv);

 private:
  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  TH1*   h_dca2d_zero; 
  TH2*   h2_dca2d_zero; 
  TH2*   h2_dca2d_len; 
  TH1*   h_zvtx;
  TNtuple*  h_ntp_clus; 
  TNtuple*  h_ntp_emcclus; 
  TNtuple*  h_ntp_mvtxclus; 
  //TTree*  h_t_clus; 
  TNtuple*  h_ntp_cluspair; 
  TNtuple*  h_ntp_emccluspair{nullptr}; 
  TNtuple*  h_ntp_mvtxcluspair{nullptr}; 
  TNtuple*  h_ntp_mvtxpair{nullptr}; 
  TNtuple*  h_ntp_intttrk{nullptr}; 
  TNtuple*  h_ntp_trkpair{nullptr}; 
  TNtuple*  h_ntp_evt; 
  TTree*  h_t_evt_bco; 

  float xbeam_{0};
  float ybeam_{0};

  float xmvtx_{0};
  float ymvtx_{0};

  bool  m_useSimVtx{false};

  evtbco_info m_evtbcoinfo;
  evtbco_info m_evtbcoinfo_prev;

  TH1* h_zvtxseed_;

};

#endif

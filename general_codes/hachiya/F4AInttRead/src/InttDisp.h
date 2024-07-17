#ifndef INTTDISP_H__
#define INTTDISP_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>
#include <string>
#include <vector>

#include <intt/InttMapping.h>


/// Class declarations for use in the analysis module
class PHCompositeNode;
class InttAna;

class InttRawHit;
class InttRawHitContainer;

class TH1;
class TGraph;
class TEllipse;
class TLine;

/// Definition of this analysis module class
class InttDisp : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttDisp(const std::string &name = "InttDisp");

  // Destructor
  virtual ~InttDisp();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void setAnaModule(InttAna* inttana) { m_inttana = inttana;}

  bool calcProjAndAngle(double cx, double cy, double radius, double px, double py, double projRadius,
                        double &prjx, double& prjy, double& ang0, double& ang1);


  void analyzeEMC(PHCompositeNode *);
  void analyzeHepMC(PHCompositeNode *);
  void analyzePHG4(PHCompositeNode *);

 private:
   InttAna* m_inttana{nullptr};

   TGraph* m_gxy{nullptr};
   TGraph* m_grz{nullptr};
   TGraph* m_gvtx_xy{nullptr};
   TGraph* m_gvtx_rz{nullptr};
   TGraph* m_gxy_emc{nullptr};
   TGraph* m_grz_emc{nullptr};
   TGraph* m_gphiz_emc{nullptr};
   TGraph* m_gphiz_emcprj{nullptr};

   TGraph* m_gxy_emctru{nullptr};
   TGraph* m_grz_emctru{nullptr};

   std::vector<TEllipse*> m_vtrk_xy;
   std::vector<TEllipse*> m_vtrutrk_xy;
   std::vector<TEllipse*> m_vtrutrk_xy_emc;

   std::vector<TLine*> m_vtrk_rz;
   std::vector<TLine*> m_vtrutrk_rz;

 protected:

};

#endif

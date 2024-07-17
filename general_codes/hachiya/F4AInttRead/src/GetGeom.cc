#include "GetGeom.h"

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <intt/CylinderGeomIntt.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>


/// ROOT includes
#include <TFile.h>
#include <TTree.h>

/// C++ includes
#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
GetGeom::GetGeom(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , m_outfilename(filename)
{
}

/**
 * Destructor of module
 */
GetGeom::~GetGeom()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int GetGeom::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in GetGeom" << std::endl;
  }

 // m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int GetGeom::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in GetGeom" << std::endl;
  }

  PHG4CylinderGeomContainer* m_geomContainerIntt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
  if (!m_geomContainerIntt)
  {
    std::cout << PHWHERE << "CYLINDERGEOM_INTT node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  ActsGeometry *m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
  {
    std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  static int ievent=0;

  if(ievent>0) return 0;

  std::vector<double> ladderALocationX;
  std::vector<double> ladderALocationY;
  std::vector<double> ladderALocationZ;

  std::vector<double> ladderBLocationX;
  std::vector<double> ladderBLocationY;
  std::vector<double> ladderBLocationZ;
  uint8_t type;
  uint8_t ladder_index_max;
  int time_bucket = 0; 

  for(uint8_t lyr=3;lyr<7;lyr++){

    //number of ladders
    switch(lyr){
      case 3:
        ladder_index_max=12;
        break;
      case 4:
        ladder_index_max=12;
        break;
      case 5:
        ladder_index_max=16;
        break;
      case 6:
        ladder_index_max=16;
        break;
    }
    std::cout<<"ladder_index_max ="<<ladder_index_max<<std::endl;


    auto m_geom = dynamic_cast<CylinderGeomIntt*>(m_geomContainerIntt->GetLayerGeom(lyr));

    //typeA position
    type = 0;
    for(uint8_t ladder_index=0;ladder_index<ladder_index_max;ladder_index++){
      auto genhitkeyintt = InttDefs::genHitSetKey(lyr,type,ladder_index,time_bucket);
      auto surfintt = m_tGeometry->maps().getSiliconSurface(genhitkeyintt);
      double ladderLocation[3] = {0.,0.,0.};
      m_geom->find_segment_center(surfintt,m_tGeometry,ladderLocation);
      //cout << "ladderLocationA=(" << ladderLocation[0] << "," << ladderLocation[1] << "," << ladderLocation[2] << ")" << endl;

      ladderALocationX.push_back(ladderLocation[0]);
      ladderALocationY.push_back(ladderLocation[1]);
      ladderALocationZ.push_back(ladderLocation[2]);
      //cout<<"(lyr,ladder_index)=("<<unsigned(lyr)<<","<<unsigned(ladder_index)<<")"<<endl;
    }

    type = 2;
    for(uint8_t ladder_index=0;ladder_index<ladder_index_max;ladder_index++){
      auto genhitkeyintt = InttDefs::genHitSetKey(lyr,type,ladder_index,time_bucket);
      auto surfintt = m_tGeometry->maps().getSiliconSurface(genhitkeyintt);
      double ladderLocation[3] = {0.,0.,0.};
      m_geom->find_segment_center(surfintt,m_tGeometry,ladderLocation);
      //cout << "ladderLocationA=(" << ladderLocation[0] << "," << ladderLocation[1] << "," << ladderLocation[2] << ")" << endl;

      ladderALocationX.push_back(ladderLocation[0]);
      ladderALocationY.push_back(ladderLocation[1]);
      ladderALocationZ.push_back(ladderLocation[2]);
      //cout<<"(lyr,ladder_index)=("<<unsigned(lyr)<<","<<unsigned(ladder_index)<<")"<<endl;
    }

    //typeB position
    type = 1;
    for(uint8_t ladder_index=0;ladder_index<ladder_index_max;ladder_index++){
      auto genhitkeyintt = InttDefs::genHitSetKey(lyr,type,ladder_index,time_bucket);
      auto surfintt = m_tGeometry->maps().getSiliconSurface(genhitkeyintt);
      double ladderLocation[3] = {0.,0.,0.};
      m_geom->find_segment_center(surfintt,m_tGeometry,ladderLocation);
      //cout << "ladderLocationB=(" << ladderLocation[0] << "," << ladderLocation[1] << "," << ladderLocation[2] << ")" << endl;

      ladderBLocationX.push_back(ladderLocation[0]);
      ladderBLocationY.push_back(ladderLocation[1]);
      ladderBLocationZ.push_back(ladderLocation[2]);
      //cout<<"(lyr,ladder_index)=("<<unsigned(lyr)<<","<<unsigned(ladder_index)<<")"<<endl;
    }


    type = 3;
    for(uint8_t ladder_index=0;ladder_index<ladder_index_max;ladder_index++){
      auto genhitkeyintt = InttDefs::genHitSetKey(lyr,type,ladder_index,time_bucket);
      auto surfintt = m_tGeometry->maps().getSiliconSurface(genhitkeyintt);
      double ladderLocation[3] = {0.,0.,0.};
      m_geom->find_segment_center(surfintt,m_tGeometry,ladderLocation);
      //cout << "ladderLocationB=(" << ladderLocation[0] << "," << ladderLocation[1] << "," << ladderLocation[2] << ")" << endl;

      ladderBLocationX.push_back(ladderLocation[0]);
      ladderBLocationY.push_back(ladderLocation[1]);
      ladderBLocationZ.push_back(ladderLocation[2]);
      //cout<<"(lyr,ladder_index)=("<<unsigned(lyr)<<","<<unsigned(ladder_index)<<")"<<endl;
    }

  }

  TFile * location = TFile::Open("segmentlocation_20240220.root","RECREATE");

  location->WriteObject(&ladderALocationX, "segmentALocationX");
  location->WriteObject(&ladderALocationY, "segmentALocationY");
  location->WriteObject(&ladderALocationZ, "segmentALocationZ");

  location->WriteObject(&ladderBLocationX, "segmentBLocationX");
  location->WriteObject(&ladderBLocationY, "segmentBLocationY");
  location->WriteObject(&ladderBLocationZ, "segmentBLocationZ");

  location->Close();

  ievent++;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int GetGeom::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending GetGeom analysis package" << std::endl;
  }


  return 0;
}


#include "InttBCOFilterv1.h"
#include <cdbobjects/CDBHistos.h>
#include <ffamodules/CDBInterface.h>
#include <iostream>  
#include <map>

R__LOAD_LIBRARY(libcdbobjects.so)

bool InttBCOFilterv1::LoadCDBFile( std::string const& filename )
{
  //Example BCO CDB file
 //std::string filename = "/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/CDB/2023/cdb_bco_20869.root";
  CDBTTree cdbttree(filename);
  cdbttree.LoadCalibrations();
  Long64_t N = cdbttree.GetSingleIntValue("size");
  for (Long64_t n = 0; n < N; ++n)
  {
    int felix_server = 0;   // 0~7
    int felix_channel = 0;  // 0~13
    int bco_diff = 0;        
    felix_server = cdbttree.GetIntValue(n, "felix_server");
    felix_channel = cdbttree.GetIntValue(n, "felix_channel");
    bco_diff = cdbttree.GetIntValue(n, "bco_diff");
    unsigned int felix_key = felix_channel + felix_server * 14;
    m_InttBCOMap.insert(make_pair(felix_key, bco_diff));
  }
  return true;
}

bool InttBCOFilterv1::isBCOPeakTest(int felix_server,int felix_channel,int testvalue)
{
  int bco_diff = testvalue; 
  unsigned int felix_key = felix_channel + felix_server * 14;
  int bco_peak = m_InttBCOMap.find(felix_key)->second;
  int bco_minus = bco_peak-1;
  if(bco_minus==-1) bco_minus = 127;
  int bco_plus = bco_peak+1;
  if(bco_plus==128) bco_plus = 0;
  
  if(bco_diff==bco_peak || bco_diff==bco_minus || bco_diff==bco_plus) return true;
  else 
  return false;
}
bool InttBCOFilterv1::isBCOPeak(Long64_t bco_full_, int bco_, int felix_server,int felix_channel)
{
  int bco_diff = (bco_full_ & 0x7F) - bco_;
  if (bco_diff < 0)
  { 
    bco_diff=bco_diff + 128;
  }
  unsigned int felix_key = felix_channel + felix_server * 14;
  int bco_peak = m_InttBCOMap.find(felix_key)->second;
  int bco_minus = bco_peak-1;
  if(bco_minus==-1) bco_minus = 127;
  int bco_plus = bco_peak+1;
  if(bco_plus==128) bco_plus = 0;
  
  if(bco_diff==bco_peak || bco_diff==bco_minus || bco_diff==bco_plus) return true;
  else 
  return false;
}
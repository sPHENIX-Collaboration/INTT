#include "InttDeadMapTest.h"
#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>
#include <iostream>  
#include <map>

R__LOAD_LIBRARY(libcdbobjects.so)

bool InttDeadMapTest::LoadCDBFile( std::string const& filename )
{
  //Example HotDeadMap CDB file
 // std::string filename = "/sphenix/tg/tg01/commissioning/INTT/QA/hotdeadmap/CDB/2023/InttHotDeadMap_cdb-00020869_30.root";
  CDBTTree cdbttree(filename);
  cdbttree.LoadCalibrations();
  Long64_t N = cdbttree.GetSingleIntValue("size");
  for (Long64_t n = 0; n < N; ++n)
  {
    int felix_server = 0;   // 0~7
    int felix_channel = 0;  // 0~13
    int chip = 0;           // 0~25
    int channel = 0;        // 0~127
    int flag = 0;           // channel flag 0 : Good; Hot : 8; Cold : 4; Half : 2; Dead : 1
    felix_server = cdbttree.GetIntValue(n, "felix_server");
    felix_channel = cdbttree.GetIntValue(n, "felix_channel");
    chip = cdbttree.GetIntValue(n, "chip");  // offline RawData_s 0~25
    channel = cdbttree.GetIntValue(n, "channel");
    flag = cdbttree.GetIntValue(n, "flag");
    unsigned int chan_key = channel + chip * 128 + felix_channel * 128 * 26 + felix_server * 14 * 26 * 128;
    m_InttHotDeadMap.insert(make_pair(chan_key, flag));
  }
  return true;
}

int InttDeadMapTest::isBad(const int felix_server,
                         const int felix_channel, const int chip,
                         const int channel) const
{
  unsigned int chan_key = channel + chip * 128 + felix_channel * 128 * 26 + felix_server * 14 * 26 * 128;
  return m_InttHotDeadMap.find(chan_key)->second;
}
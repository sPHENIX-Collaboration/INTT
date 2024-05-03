#ifndef INTTBCOFILTERV1_H
#define INTTBCOFILTERV1_H

#include <iostream>
#include <map>

class InttBCOFilterv1
{
 public:
  InttBCOFilterv1() {}
  ~InttBCOFilterv1() {}
  typedef std::map<unsigned int, int> Map;  //<key,flag>
  // key : felix_channel+felix_server*14;
  bool LoadCDBFile(std::string const& filename);
  bool isBCOPeak(Long64_t bco_full_, int bco_, int felix_server_,int felix_channel_);
  bool isBCOPeakTest(int felix_server_,int felix_channel_,int testvalue);

 private:
  Map m_InttBCOMap;
};
#endif
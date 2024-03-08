#ifndef INTTDEADMAPTEST_H
#define INTTDEADMAPTEST_H

#include <iostream>
#include <map>

class InttDeadMapTest
{
 public:
  InttDeadMapTest() {}
  ~InttDeadMapTest() {}
  typedef std::map<unsigned int, int> Map;  //<key,flag>
  // key : channel+chip*128+felix_channel*128*26+felix_server*14*26*128;
  bool LoadCDBFile(std::string const& filename);
  int isBad(const int felix_server,
            const int felix_channel, const int chip,
            const int channel) const;

 private:
  Map m_InttHotDeadMap;
};
#endif
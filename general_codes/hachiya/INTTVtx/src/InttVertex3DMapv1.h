#ifndef INTT_INTTVERTEX3DMAPV1_H
#define INTT_INTTVERTEX3DMAPV1_H

#include "InttVertex3DMap.h"

#include "InttVertex3D.h"

#include <cstddef>  // for size_t
#include <iostream>
#include <map>

class InttVertex3DMapv1 : public InttVertex3DMap
{
 public:
  InttVertex3DMapv1() = default;
  ~InttVertex3DMapv1() override;

  void identify(std::ostream& os = std::cout) const override;
  void Reset() override { clear(); }
  int isValid() const override { return 1; }

  bool empty() const override { return _map.empty(); }
  size_t size() const override { return _map.size(); }
  size_t count(unsigned int idkey) const override { return _map.count(idkey); }
  void clear() override;

  const InttVertex3D* get(unsigned int idkey) const override;
  InttVertex3D* get(unsigned int idkey) override;
  InttVertex3D* insert(InttVertex3D* vertex) override;
  size_t erase(unsigned int idkey) override
  {
    delete _map[idkey];
    return _map.erase(idkey);
  }

  ConstIter begin() const override { return _map.begin(); }
  ConstIter find(unsigned int idkey) const override { return _map.find(idkey); }
  ConstIter end() const override { return _map.end(); }

  Iter begin() override { return _map.begin(); }
  Iter find(unsigned int idkey) override { return _map.find(idkey); }
  Iter end() override { return _map.end(); }

 private:
  std::map<unsigned int, InttVertex3D*> _map;

  ClassDefOverride(InttVertex3DMapv1, 1);
};

#endif  // INTT_INTTVERTEXMAPV1_H

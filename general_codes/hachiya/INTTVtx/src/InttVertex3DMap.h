#ifndef INTT_INTTVERTEX3DMAP_H
#define INTT_INTTVERTEX3DMAP_H

#include <phool/PHObject.h>

#include <cstddef>  // for size_t
#include <iostream>
#include <map>

class InttVertex3D;

class InttVertex3DMap : public PHObject
{
 public:
  typedef std::map<unsigned int, InttVertex3D*>::const_iterator ConstIter;
  typedef std::map<unsigned int, InttVertex3D*>::iterator Iter;

  ~InttVertex3DMap() override {}

  void identify(std::ostream& os = std::cout) const override { os << "InttVertex3DMap base class" << std::endl; }
  int isValid() const override { return 0; }

  virtual bool empty() const { return true; }
  virtual size_t size() const { return 0; }
  virtual size_t count(unsigned int /*idkey*/) const { return 0; }
  virtual void clear() {}

  virtual const InttVertex3D* get(unsigned int /*idkey*/) const { return nullptr; }
  virtual InttVertex3D* get(unsigned int /*idkey*/) { return nullptr; }
  virtual InttVertex3D* insert(InttVertex3D* /*vertex*/) { return nullptr; }
  virtual size_t erase(unsigned int /*idkey*/) { return 0; }

  virtual ConstIter begin() const;
  virtual ConstIter find(unsigned int idkey) const;
  virtual ConstIter end() const;

  virtual Iter begin();
  virtual Iter find(unsigned int idkey);
  virtual Iter end();

 protected:
  InttVertex3DMap() {}

 private:
  ClassDefOverride(InttVertex3DMap, 1);
};

#endif  // INTT_INTTVERTEXMAP_H

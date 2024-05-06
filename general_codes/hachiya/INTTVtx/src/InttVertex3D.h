#ifndef INTT_INTTVERTEX3D_H
#define INTT_INTTVERTEX3D_H

#include <globalvertex/Vertex.h>

#include <cmath>
#include <iostream>

class InttVertex3D : public Vertex
{
 public:
  ~InttVertex3D() override {}

  // PHObject virtual overloads

  void identify(std::ostream& os = std::cout) const override { os << "InttVertex3D base class" << std::endl; }
  PHObject* CloneMe() const override { return nullptr; }
  int isValid() const override { return 0; }

  // vertex info

  virtual unsigned int get_id() const override { return 0xFFFFFFFF; }
  virtual void set_id(unsigned int)   override {}

  virtual float get_x() const override { return NAN; }
  virtual void  set_x(float)  override {}

  virtual float get_y() const override { return NAN; }
  virtual void  set_y(float)  override {}

  virtual float get_z() const override { return NAN; }
  virtual void  set_z(float)  override {}

  virtual float get_position(unsigned int) const  override { return NAN; }
  virtual void  set_position(unsigned int, float) override {}

  virtual float get_error(unsigned int, unsigned int) const  override { return NAN; }
  virtual void  set_error(unsigned int, unsigned int, float) override {}

 protected:
  InttVertex3D() {}

 private:
  ClassDefOverride(InttVertex3D, 1);
};

#endif

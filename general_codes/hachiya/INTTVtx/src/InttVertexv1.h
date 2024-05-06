#ifndef INTT_INTTVERTEXV1_H
#define INTT_INTTVERTEXV1_H

#include "InttVertex.h"

#include <iostream>

class PHObject;

class InttVertexv1 : public InttVertex
{
 public:
  InttVertexv1();
  ~InttVertexv1() override = default;

  // PHObject virtual overloads

  void identify(std::ostream& os = std::cout) const override;
  void Reset() override { *this = InttVertexv1(); }
  int isValid() const override;
  PHObject* CloneMe() const override { return new InttVertexv1(*this); }

  // vertex info

  unsigned int get_id() const override { return _id; }
  void set_id(unsigned int id) override { _id = id; }

  float get_x() const override { return _pos[0]; }
  void set_x(float x) override { _pos[0] = x; }

  float get_y() const override { return _pos[1]; }
  void set_y(float y) override { _pos[1] = y; }

  float get_z() const override { return _pos[2]; }
  void set_z(float z) override { _pos[2] = z; }

  float get_position(unsigned int coor) const override { return _pos[coor]; }
  void set_position(unsigned int coor, float xi) override { _pos[coor] = xi; }

  float get_error(unsigned int i, unsigned int j) const override;        //< get vertex error covar
  void set_error(unsigned int i, unsigned int j, float value) override;  //< set vertex error covar

 private:
  unsigned int covar_index(unsigned int i, unsigned int j) const;

 private:
  unsigned int _id = std::numeric_limits<unsigned int>::max();  //< unique identifier within container
  float _pos[3] = {};                                           //< collision position x,y,z
  float _err[6] = {};                                      //< error covariance matrix (+/- cm^2)


  ClassDefOverride(InttVertexv1, 1);
};

#endif

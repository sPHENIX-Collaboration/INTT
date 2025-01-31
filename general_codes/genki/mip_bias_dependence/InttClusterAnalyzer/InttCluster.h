#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <TVector3.h>

class InttCluster
{
 private:
  // member variables
  int layer_	= -1; // layer ID that this cluster belongs to ( 0 -- 3 )
  TVector3 position_; // cluster position in the lab frame (cm)
  double adc_	= 0.0;
  int size_phi_ = -1;
  int size_z_	= -1;
  double theta_ = -9999.9; // polar angle (0 - pi radian)
  double phi_	= -9999.9;
  double eta_	= -9999.9; // eta=-ln (tan (theta/2)) 

  // parameters depends on vertex position
  TVector3 position_vertex_; // vertex position in the lab frame (cm)
  TVector3 position_relative_; // relative position of this cluster wrt the vertex position ( <-- it's (0, 0, 0))
  double theta_relative_	= -9999.9;
  double phi_relative_		= -9999.9;
  double eta_relative_		= -9999.9;

  // member funcitons
  void Init();

 public:
  InttCluster();
  InttCluster( double x, double y, double z );

  void Calc(); // run this function before use

  ////////////////////////////////////////////////////////
  // Get functions
  ////////////////////////////////////////////////////////
  int		GetLayer()	{ return layer_; };
  TVector3	GetPosition()	{ return position_; };
  double	GetAdc()	{ return adc_; };
  int		GetSizePhi()	{ return size_phi_; };
  int		GetSizeZ()	{ return size_z_; };
  double	GetTheta()	{ return theta_; };
  double	GetPhi()	{ return phi_; };
  double	GetEta()	{ return eta_; };
  
  // parameters depends on vertex position
  TVector3	GetVertexPosition()	{ return position_vertex_; };
  TVector3	GetPositionRelative()	{ return position_relative_; };
  double	GetThetaRelative()	{ return theta_relative_; };
  double	GetPhiRelative()	{ return phi_relative_; };
  double	GetEtaRelative()	{ return eta_relative_; };
  
  ////////////////////////////////////////////////////////
  // Set functions
  ////////////////////////////////////////////////////////
  void SetAdc			( double val )		{ adc_ = val; };
  void SetLayer			( int num )		{ layer_ = num; };
  void SetPosition		( double x, double y, double z );
  void SetSizePhi		( int num )		{ size_phi_ = num; };
  void SetSizeZ			( int num )		{ size_z_ = num; };

  void SetVertex		( TVector3 vec )	{ position_vertex_ = vec; };
  void SetVertexPosition	( double x, double y, double z );
  void SetVertexZ		( double z )		{ position_vertex_.SetZ( z ); };

  void Print();
};

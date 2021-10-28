//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//*   - the detailed geant4 simulation for Tesla -      *
//*                                                     *
//* For more information about Mokka, please, go to the *
//*                                                     *
//*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
//*                                                     *
//*    Mokka home page.                                 *
//*                                                     *
//*******************************************************
//
// $Id: CGADefs.h,v 1.20 2006/05/23 09:14:54 musat Exp $
// $Name: mokka-07-00 $
//
// History
// first implementation for the 
// Mokka Common Geometry Access (CGA) by 
// Gabriel Musat (musat@poly.in2p3.fr), July 2002
//
// see CGA documentation at 
// http://polype.in2p3.fr/geant4/tesla/www/mokka/
//        software/doc/CGADoc/CGAIndex.html
//-------------------------------------------------------

#ifndef CGADefs_h
#define CGADefs_h 1

#include <string>
#include <vector>

using namespace std;

#define INSTANTIATE(x) x the##x;

typedef struct {
	double X;
	double Y;
	double Z;
	int GRZone;
}cell_info;

typedef struct {
        int id0;
        int id1;
}cell_ids;

string CGAWhereAmIForJava(double * pos);
void CGAGetStepsForJava(vector<string> &volNames, vector<string> &matNames,
		vector<double> & distance,
		vector<double>& x, vector<double>& y, vector<double>& z,
		vector<double>& nbX0, vector<double> &nInterLen);
void CGAGetVolumeDataForJava(string nomVol, vector<double *> &distance,
		vector<double> &x, vector<double> &y, vector<double> &z,
		vector<double *> &nbX0, vector<double*> &nInterLen);
#ifdef __cplusplus
extern "C" {
#endif

void CGASetTBConfigAngle(float angle);
void CGAInit(const char* steering, const char *model, const char *setup, const char *host, const char *user, const char *passwd);
void CGABeamOn(double *initial, double *final, double *direction,
                char * particule, float energie, int nbPart);
void CGAWhereAmI(double * pos, char *nomVol, int NOMVOLLEN);
std::string CGAGetMaterialName(double pos[]);
double CGAGetDensity(double pos[]);
double CGAGetTemperature(double pos[]);
double CGAGetPressure(double pos[]);
double CGAGetRadLen(double pos[]);
double CGAGetIntLen(double pos[]);
void CGAGetSteps(char ** nomVol, char **nomMat, double * parcours,
                double **preSteps, double * nbX0, double *nInterLen, 
		int * nSteps, int *OKFlag, int nomVolLen, int nomMatLen);
void CGAGetVolumeData(char * nomVol, double * parcours,
                double **preSteps, double * nbX0, double *nInterLen, 
		int * nSteps, int * OKFlag);
void CGASetSD(int flag);
cell_info CGACellIndex(cell_ids cellID);
cell_ids CGAGetCellId(double x, double y, double z, int &flag,
	double xDir=0, double yDir=0, double zDir=1);
double CGAGetBdl(double start[3], double end[3]);
double CGAGetEdl(double start[3], double end[3]);
std::vector<double> CGAGetB(double position[3]);
std::vector<double> CGAGetE(double position[3]);
std::vector<std::string> CGAGetListOfLogicalVolumes(double position[3]);
std::vector<std::string> CGAGetListOfPhysicalVolumes(double position[3]);
std::string CGAGetRegionName(double position[3]);
std::vector<double> CGAGetLocalPosition(double position[3]);
bool CGAIsCalorimeter(double position[3]);
bool CGAIsTracker(double position[3]);
#ifdef __cplusplus
}
#endif

#endif //CGADefs_h

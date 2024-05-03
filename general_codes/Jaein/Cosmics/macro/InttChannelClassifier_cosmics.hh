#pragma once

#include <cdbobjects/CDBTTree.h>
#include "constant_values.hh"
#include "Classifier.hh"

using namespace std;

//////////////
// Funtions //
//////////////
// double SingleGaussianFit(TH1D *hist, double &mean1, double &sigma1); 
// //Function to do fitting return value : value for HotChannel cut

// void GetFirstFilledBinCenter(TH1D *hist, double &a, double &b);      
// //Function to find bin center not used for now.                                                                    

/////////////////////
//Global parameters//
/////////////////////
int chip = 26;
int mod = 14;
double sig_cut = 3.0;
bool Writecsv = false;
//chip : # of chips on each half ladder(defalt : 26)
//mod : # of moudles for each felix server(default : 14)
//sig_cut : sigma cut for hot/cold channel determination default : 4.0
//Writecsv : flag to write csv file (default : false )


/////////////
//File Path//
/////////////
std::string map_input_path =  "./";
std::string root_output_path = "./";
std::string cdb_output_path = "./";
//std::string csv_output_path = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/HitMap/GaussianClassifier/macro/";
//map_input_path : location of the HitMap file 
//root_output_path : output file path
//cdb_output_path : cdb output file path 
//csv_output_path : csv output file path (used for Grafana online monitoring)

// struct Half_Chip {
//   int felix_id_;
//   int module_id_;
//   int chip_id_;
// };

////////////////////////////
//List of Half entry chips//
////////////////////////////
// std::vector<Half_Chip> half_chips = // Chip number 0~25
//     {
//         // Felix 0
//         {0, 7, 14},
//         // Felix 2
//         {2, 9, 15},
//         // Felix 3
//         {3, 13, 20},
//         {3, 13, 22},
//         {3, 13, 24},
//         // Felix 7
//         {7, 0, 0},
//         {7, 0, 1},
//         {7, 0, 2},
//         {7, 0, 3},
//         {7, 0, 4},
//         {7, 0, 13},
//         {7, 0, 14},
//         {7, 0, 15},
//         {7, 0, 16},
//         {7, 0, 17},
//         {7, 0, 6},
//         {7, 0, 8},
//         {7, 0, 10},
//         {7, 0, 12},
//         {7, 0, 19},
//         {7, 0, 21},
//         {7, 0, 23},
//         {7, 0, 25},
//         {7, 1, 0},
//         {7, 1, 1},
// };

R__LOAD_LIBRARY(libcdbobjects.so)

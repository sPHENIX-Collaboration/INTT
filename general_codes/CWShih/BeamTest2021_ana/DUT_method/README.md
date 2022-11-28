# INTT BeamTest 2021 
## Detection efficiency study
The detection efficiency is now done by the DUT method (geometry method)
The function of the codes are described below
1. interface_v5.C -> this code is for the MC, to convert the data format of MC to data
2. filter_v4.C    -> this code is for filtering the "clone hits", and do some preliminary data check
3. track_single_cluster_v1.C -> this code is for calculating the clusters and output a root file
4. DUT_test_v1.h -> this code includes the functions to calculate the detection efficiency
5. par_init.h  -> this code provides the default parameter and also the function to update the parameter
6. run89_data_DUT_effi.C -> this code include the DUT_test_v1.h call the function and calculate the efficiency
7. double_gaus.h -> this code has the function of the double guassian, it also has the ability to calculate the three-sigma width

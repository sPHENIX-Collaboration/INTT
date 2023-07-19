# INTT Alignment

This is a repository for analyzing survey data
	* Survey data is stored as .txt files under dat/
	* Specialized classes read these files
	* Align transforms of INTT sensors are computed as augmented transformation matrices
	* Key parameters are written to other .txt files

##Usage
Run `sh get_transforms.sh` to produce `dat/transforms.txt`, a human-readable file with the augmented matrices for sensor to global transforms computed. Check the macro before running to make sure that the `PATH` variable is correct on your system.

The columns are the coordinate axes of the sensor, which are normalized with components in sPHENIX global coordinates. The last column is the translation and the last row is the augmented row.

##Debugging
Run `sh check_transforms.sh` to start an interactive ROOT session that will draw color-coded sensors or ladders

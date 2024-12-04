# Bias voltage dependence of the MIP peak
The silicon module of INTT is supposed to have 100 V bias for the proper operation. Here is the analysis codes for the study of bias voltage dependence of the MIP peak position.
See the wiki for more details.

## Runs
6 runs were taken **in the local mode** in the commissioning phase with AuAu beams in 2024 with 3 types of bias voltages: 100 V, 75 V, and 50 V.
See [INTT run log](https://docs.google.com/spreadsheets/d/175Z06nDFWACKIrvqN_R43ABLtRfE6r23u9CZtVuwDHg/edit?gid=1459536224#gid=1459536224).

## Analysis procedure
1. Decode and event combining by ourselves as data was taken in the local mode. DSTs with InttRawHit are made.
2. Make hot channel CDB
3. Make BCO diff selection CDB if needed
4. Convert InttRawHit and clusterize them using the DSTs generated in Step 1. The hot channel CDB from Step 2 and BCO diff CDB from Step 3 can be applied at this step. DSTs with TrkrCluster is made.
5. Run Fun4All_intt_vertex_xy.C to get the averaged vertex position in the x and y coordinates.
6. Analyze the DST made in Step 4 with the analysis module InttClusterAnalyzer.
7. More analysis with tree_analysis.cc (more implementation is needed).

## Vertex reconstruction
Because the data was taken in the local mode, information from other subsystems is unavailable. We need to reconstruct the collision vertex by ourselves.

### X and Y coordinates
InttXYVertexFinder ([.h](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/packages/intt/InttXYVertexFinder.h) and [.cc](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/packages/intt/InttXYVertexFinder.cc) ) is used.
The results are accessed with InttVertexMapv1 in the analysis module InttVertexXY ([.h](https://github.com/nukazuka/INTT/blob/main/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttVertexXY.h) and [.cc](https://github.com/nukazuka/INTT/blob/main/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttVertexXY.cc) ) [here](https://github.com/nukazuka/INTT/blob/4890cdfc52dd56aaf68a347b9a774f9d30f73d37/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttVertexXY.cc#L35).
I need to confirm how to use InttXYVertexFinder with someone...

### Z coordinate
InttZVertexFinder ([.h](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/packages/intt/InttXYVertexFinder.h) and [.cc](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/packages/intt/InttXYVertexFinder.cc)) was used.
The beam center in the x and y axis was taken from the results in the previous step.
The results are accessed with InttVertexMapv1 in the analysis module InttClusterAnalyzer ([.h](https://github.com/nukazuka/INTT/blob/main/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttClusterAnalyzer.h) and [.cc](https://github.com/nukazuka/INTT/blob/main/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttClusterAnalyzer.cc)).
What kind of algorithm is used in InttZVertexFiner? What about the performance?

## Analysis module

### InttVertexXY
It's used in step 5 to determine the beam center in the x and y directions.
The Fun4All macro Fun4All_intt_vertex_xy.C uses this module.
You need to register InttXYVertexFinder for this module.
The results are saved into ./results/vertex_xy/.
Users cannot change the output directory for the moment.
.txt just contains the beam center of the run in cm.
.root is made for more studies, but nothing is written so far.

### InttClusterAnalyzer
It's used in step 6.
The Fun4All macro Fun4All.C uses this module.
You need to register InttZVertexFinder for this module.
What is done by this module is
1. Init:
  1. The output file name is determined.
  2. The output ROOT file is opened, and a TTree is made for the output.
  3. A branch of [InttEvent](https://github.com/nukazuka/INTT/blob/main/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttEvent.h) is made.
2. Event loop:
  1. Nodes are obtained in [GetNode](https://github.com/nukazuka/INTT/blob/4890cdfc52dd56aaf68a347b9a774f9d30f73d37/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttClusterAnalyzer.cc#L42) function.
  2. Run number and event ID are set to the InttEvent object.
  3. Analysis related to the vertex is done in [VertexAnalysis](https://github.com/nukazuka/INTT/blob/4890cdfc52dd56aaf68a347b9a774f9d30f73d37/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttClusterAnalyzer.cc#L166) function. The following parameters are taken from InttVertexMapv1 and set to the InttEvent object.
     - vertex coordinate
     - Chi2/NDF of z vertex fitting
     - a flag to tell whether this vertex reconstruction is good (true) or bad (false)
  4. Cluster loop is done in [process_event](https://github.com/nukazuka/INTT/blob/4890cdfc52dd56aaf68a347b9a774f9d30f73d37/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttClusterAnalyzer.cc#L107) function.
     1. Loop over 4 INTT layers (inner layer of inner barrel, outer layer of inner barrel, inner layer of outer barrel, and outer layer of outer barrel)
     2. Get clusters that belong to the INTT layer
     3. The global position of the cluster is taken with Acts.
     4. InttCluster object is created. The cluster parameters are set to this object.
     5. The InttCluster object is added to the InttEvent object
  5. Theta, phi, and eta are calculated by [InttEvent::Calc](https://github.com/nukazuka/INTT/blob/4890cdfc52dd56aaf68a347b9a774f9d30f73d37/general_codes/genki/mip_bias_dependence/InttClusterAnalyzer/InttEvent.cc#L30) after the cluster loop.

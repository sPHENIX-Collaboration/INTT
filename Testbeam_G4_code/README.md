v1 : contents INTT modules * 4 (silicon sensor + HDI+ FPHX chips + carbon cooling plate) + trigger scintillator*2 


v2 : replace carbon cooling plate by stave


The procedures to run the G4 (under the G4 environment) : 
1. decompress session7_solution_vX.zip
2. mkdir session7_solution_vX_build
3. cd session7_solution_vX_build
4. cmake ../session7_solution_vX
5. make       #compile
6. ./example      #this command is to open the TBrowser 

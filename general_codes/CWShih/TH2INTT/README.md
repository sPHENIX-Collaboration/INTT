# TH2INTT

## Quick introduction
* A tool aiming to have a better way to present the overall INTT status, up to half-ladder (HL) level. 
* Inherit from the ROOT::TH2Poly
* The examaple plot can be seen in output_example_1.png
* The arrows in the plot show the sPHENIX coordinate, view from North to South
* The ladder location is according to https://wiki.sphenix.bnl.gov/index.php/INTT_Barrel

## How to use it
* It's a class. So just like how you use the ROOT::TH2F. For example : 
```
#include "TH2INTT.h"
TH2INTT * INTT_effi = new TH2INTT(); #No variable in the parentheses.
```

### Ways to fill the bin (half-ladder) content  
#### One thing different from the TH2F, you CAN NOT use "TH2::Fill" to fill the content
#### Instead, here we use "SetContent". You can determine the HL to be set by giving the "ladder name + directions" or the "server + Felix channel". Examples are shown below.
 
1. void SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content) 
* side : 0 -> South, 1 -> North
```
 INTT_effi -> SetLadderIContent(1,1,10,1, 300); // B1L110N, content : 300
```
 
2. void SetLadderSContent(TString ladder_name, double content)
```
 INTT_effi -> SetLadderSContent(“B1L110N”, 9.8); // B1L110N, content : 9.8
```

3. void SetSerFCIContent(int server_id, int FC_id, double content) 
```
INTT_effi -> SetSerFCIContent(4,3,100); // intt4_FC3, content : 100
```

4. void SetSerFCSContent(TString server_FC, double content)
```
INTT_effi -> SetSerFCSContent(“intt4_3”,20) // intt4_FC3, content : 20
```

### Ways to get content 
* Same ways as how you do the SetContent. Just change the "Set" of functions mentioned above to "Get". For example : 
void GetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side) 
```
 INTT_effi -> GetLadderIContent(1,1,10,1); // B1L110N, return content : XXX
```
### Name, Label, Legned
As this function is inherited from the ROOT::TH2Poly, you can simply do 
```
INTT_effi -> SetTitle();
INTT_effi -> GetXaxis() -> SetTitle();
INTT_effi -> GetYaxis() -> SetTitle();
```
Also, add the legend, text or other stuff same way as how do you it with TH2F.  

### Draw
```
INTT_effi -> Draw("colz0");
```

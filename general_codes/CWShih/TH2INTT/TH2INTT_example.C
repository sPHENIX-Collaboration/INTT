#include "TH2INTT.h"

void TH2INTT_example()
{
    // note : recommended size of the canvas (as least the ratio, can be followed)
    TCanvas * c1 = new TCanvas("","",1780,800); 
    c1 -> cd();

    // note : declare the class
    TH2INTT * class_test = new TH2INTT();
    
    // note : the following are the commands to fill or assign the value of each half-ladder
    // note : Currently four ways are offered to fill the half-ladder value
    // note : void SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content) <----- this one is used in this example
    // note : void SetLadderSContent(TString ladder_name, double content)
    // note : void SetSerFCIContent(int server_id, int FC_id, double content)
    // note : void SetSerFCSContent(TString server_FC, double content)
    // note : For "int side" : 0 -> South, 1 -> North

    for (int i = 0; i < 12; i++)
    {
        class_test -> SetLadderIContent(0,0,i,0, (i+1)*20);
    }

    for (int i = 0; i < 16; i++)
    {
        class_test -> SetLadderIContent(1,0,i,0, (i+1)*20);
    }

    class_test -> SetLadderIContent(0,0,11,1, 200);
    class_test -> SetLadderIContent(0,0,10,1, 200);
    class_test -> SetLadderIContent(0,0,9,1, 200);
    class_test -> SetLadderIContent(0,1,11,1, 200);
    class_test -> SetLadderIContent(0,1,0,1, 200);
    class_test -> SetLadderIContent(1,0,15,1, 200);
    class_test -> SetLadderIContent(1,1,15,1, 200);
    
    // note : Draw the plot
    class_test->Draw("colz0");
    // note : set the title, just like how you treat with TH2-class family
    class_test -> SetTitle("Class TH2INTT, plot example");
}

// SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content)
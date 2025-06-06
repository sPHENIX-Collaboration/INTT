#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
//#include <fun4allraw/SingleInttInput.h>
#include <fun4allraw/SingleInttPoolInput.h>
//#include <fun4allraw/Fun4AllEvtInputPoolManager.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <inttread/InttCombinedRawDataConverterEB.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libinttread.so)

//SingleInttInput *sngl[9]{};

void Fun4All_Intt_Combiner(int nEvents = 10000,
		      const string &input_file00 = "intt0.list",
		      const string &input_file01 = "intt1.list",
		      const string &input_file02 = "intt2.list",
		      const string &input_file03 = "intt3.list",
		      const string &input_file04 = "intt4.list",
		      const string &input_file05 = "intt5.list",
		      const string &input_file06 = "intt6.list",
		      const string &input_file07 = "intt7.list"
)
{
  vector<string> infile;
  infile.push_back(input_file00);
  //infile.push_back(input_file01);
  //infile.push_back(input_file02);
  //infile.push_back(input_file03);
  //infile.push_back(input_file04);
  //infile.push_back(input_file05);
  //infile.push_back(input_file06);
  //infile.push_back(input_file07);

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",20445);
  //
  //
  InttCombinedRawDataConverterEB *inttdecode = new InttCombinedRawDataConverterEB();
  inttdecode->SetOutputFile("evt.root");
  se->registerSubsystem(inttdecode);
  
  
  
  
  
//  Fun4AllEvtInputPoolManager *in = new Fun4AllEvtInputPoolManager("Comb");
  Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
//  in->Verbosity(10);
  int i=0;
  for (auto iter : infile)
  {
    //SingleInttInput *sngl= new SingleInttInput("INTT_" + to_string(i));
    SingleInttPoolInput *sngl= new SingleInttPoolInput("INTT_" + to_string(i));
//    sngl->Verbosity(3);
    sngl->AddListFile(iter);
    //in->registerStreamingInput(sngl, Fun4AllEvtInputPoolManager::INTT);
    in->registerStreamingInput(sngl, Fun4AllStreamingInputManager::INTT);
    i++;
  }
  se->registerInputManager(in);



  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out","test.root");
  // se->registerOutputManager(out);

   se->run(nEvents);

   se->End();
   delete se;
   gSystem->Exit(0);
}

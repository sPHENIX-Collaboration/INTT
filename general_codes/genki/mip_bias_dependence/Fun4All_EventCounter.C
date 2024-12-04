// Fun4All headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

// #include <ffamodules/FlagHandler.h>
// #include <ffamodules/HeadReco.h>
// #include <ffamodules/SyncReco.h>
// #include <ffamodules/CDBInterface.h>

// #include <phool/PHRandomSeed.h>
// #include <phool/recoConsts.h>

#include <EventCounter.h>
// Loading libraries
R__LOAD_LIBRARY( libfun4all.so )
R__LOAD_LIBRARY( libffarawobjects.so )  
R__LOAD_LIBRARY( libEventCounter.so )

int Fun4All_EventCounter( int run = 54688 )
{

  // File path format: /sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00054688_special.root
  const string kData_dir = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/";
  string data_name = "DST_beam_intt-000" + to_string( run ) + "_no_hot_special.root";
  string data = kData_dir + data_name;
  
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  
  // Read DST
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->Verbosity( 1 );
  in->fileopen( data );
  // in->AddListFile(inputfile); // to read a list of files, use it. A path to the text file needs to be given.
  se->registerInputManager( in );

  EventCounter* ev = new EventCounter();
  se->registerSubsystem( ev );
  
  se->run( 0 );
  se->End();

  cout << data << "\t" << ev->GetEventNum() << endl;
  delete se;
  
  gSystem->Exit(0);
  return 0;
}

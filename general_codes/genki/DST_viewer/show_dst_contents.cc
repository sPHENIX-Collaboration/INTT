#include <fun4all/Fun4AllServer.h>

#include <fun4all/Fun4AllDstInputManager.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY( libffarawobjects.so )  // OK

int show_dst_contents( const string &data = "/sphenix/lustre01/sphnxpro/physics/slurp/streaming/physics/inttonlyrun_00051100_00051200/DST_INTT_EVENT_run2pp_new_2024p002-00051100-00000.root" )
{

  Fun4AllServer *se = Fun4AllServer::instance();
  
  auto *in = new Fun4AllDstInputManager("DSTin");
  in->fileopen( data ); // this is an old method but supports skip function
  se->registerInputManager(in);

  se->run( 1 );
  delete se;

  gSystem->Exit(0);
  return 0;
}

#include "Fun4All_Intt_MixupQA.hh"


void Fun4All_Intt_MixupQA(int run_num = 40741, int nevents = 0 ) {
  
  Fun4AllServer *se = Fun4AllServer::instance(); 
  // se->Verbosity(5);

  string run_type = GetRunType( run_num );
  string run_num_str = GetRunNum8digits( run_num );
  string in_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041668_special.root";

  cout << in_file << endl;
  
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  rc->set_IntFlag("RUNNUMBER", run_num );

  //--input
  Fun4AllInputManager *in = new Fun4AllDstInputManager( "DSTin" );
  in->Verbosity( 2 );
  in->fileopen( in_file );
  se->registerInputManager( in );

  InttMixupQA *inttmixqa = new InttMixupQA("inttmixqa", run_num);
  
  se->registerSubsystem(inttmixqa);
  
  se->run( nevents );
  se->End();

  delete se;
}

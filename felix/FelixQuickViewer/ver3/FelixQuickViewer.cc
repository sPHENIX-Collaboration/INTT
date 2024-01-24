#define __CINT__ // define it if it's run by root command

#include "HistMaker.hh"
#include "Plotter.hh"

//int main( int argc, char* argv[] )
//int main()
int FelixQuickViewer( string data )
{

  HistMaker* hm = new HistMaker( data );
  hm->Print();
  hm->Process();
  hm->SaveHists();
  
  return 0;
}

#define __CINT__ // define it if it's run by root command
#include "Viewer.hh"


//int main( int argc, char* argv[] )
//int main()
int FelixQuickViewer( string data )
{

  Viewer* view = new Viewer( data );
  view->Print();
  view->Process();
  view->SaveHists();
  return 0;
}

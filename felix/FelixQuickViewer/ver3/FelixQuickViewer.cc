#include <cstring>
#include <iostream>

//#include "TApplication.h"

#include "BaseClass.hh"
#include "HistMaker.hh"
#include "Viewer.hh"
//#include "Plotter.hh"

#ifndef CLING
int FelixQuickViewer( string data = "calib_intt0-12345678-0000.root", string run_type = "calibration", bool skip_hist_generation = false, int year=2024 )
{
  int argc = 3;
  char* argv[data.size()];
  
  
#else

int main( int argc, char* argv[] )
{  
  std::string data = argv[1];
  
#endif
  
  if( argc <= 1 )
    {
      std::cerr << "No data path is given." << std::endl;
      std::cerr << "Usage: " << argv[0] << " [data path]" << std::endl;
      return 0;
    }  

  //TApplication app( "app", &argc, argv );

  HistMaker* hm = new HistMaker( data, year );

  if( skip_hist_generation == false )
    {
      hm->Print();
      hm->Process();
      hm->SaveHists();
    }

  //  Viewer* vw = new Viewer( hm->GetOutput() );
  Viewer* vw = new Viewer( data, year );
  vw->SetRunType( run_type );
  vw->Draw();
  
  //  TRootBrowser* imp = (TRootBrowser*)a->GetBrowserImp();
  //  imp->Connect( imp, "Refresh()", "TApplication", &app, "Terminate()" );
  //  imp->Connect( imp, "CloseWindow()", "TApplication", &app, "Terminate()" );
  //  app.Run();
  
  return 0;
}

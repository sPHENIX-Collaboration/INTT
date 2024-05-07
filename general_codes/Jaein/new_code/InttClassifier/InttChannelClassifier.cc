#include "InttChannelClassifier.hh"

/*!
  @brief A macro to creat a hot channel map using a hitmap
  @author J. Hwang (Korea Univ.)
 */
void InttChannelClassifier(int runnumber = 41047) //runnumber
{
  
  
  ////////////////////////////////////////
  // Load HitMap                        //
  ////////////////////////////////////////
  InttClassifier* cl = new InttClassifier( runnumber , 5.0);
  if( true )
    {
      cl->SetQaRootOutputDir( "./" );
      cl->SetQaPdfOutputDir( "./" ); 
      cl->SetCdcOupoutDir( "./" ); 
    }

  cl->ProcessBeam();
  cl->WriteResults();
  cl->Print();
  return;
}

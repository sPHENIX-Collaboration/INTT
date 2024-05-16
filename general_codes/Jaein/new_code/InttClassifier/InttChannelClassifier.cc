#include "InttChannelClassifier.hh"

/*!
  @brief A macro to creat a hot channel map using a hitmap
  @author J. Hwang (Korea Univ.)
 */
void InttChannelClassifier(int runnumber = 41047, int event_num=0 ) //runnumber
{
  
  InttClassifier* cl = new InttClassifier( runnumber , 5.0);
  if( true ) // for debugging
    {
      cl->SetHitmapDir( kIntt_hitmap_dir + "root/" );
      cl->SetQaRootOutputDir( "./" );
      cl->SetQaPdfOutputDir( "./" ); 
      cl->SetCdcOupoutDir( "./" );
      cl->SetOfficialDstFlag( false );
    }

  //cl->SetOutputTag( "test" );

  // for some special porpose, temp
  if( event_num > 0 )
    string tag = "_special_" + to_string( event_num ) ;

  cl->SetFileSuffix( tag );
  cl->SetFittingMode( "double_gaus" );
  cl->Init();

  // Launch the analysis part
  cl->ProcessBeam();

  // Use this function to writte results to files
  cl->WriteResults();
  
  // Some conditions are shown.
  cl->Print();
  return;
}

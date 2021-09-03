#include "INTTHit.hh"

int INTTHit_test()
{

  int DACs[8] = { 15, 23, 60, 98, 135, 173, 210, 248 };
  vector < INTTHit* > hits;
  /*
  //                           adc,ampl,chip,fpga,modl,chan, fem, bco,bcof, eve
  hits.push_back( new INTTHit(   1,   0,   3,   1,   1,   5,   1,   1, 100,   1, DACs ) ); // 0
  hits.push_back( new INTTHit(   1,   0,   3,   1,   1,   6,   1,   1, 100,   1, DACs ) ); // 1
  hits.push_back( new INTTHit(   1,   0,   7,   1,   1, 120,   1,   1, 100,   1, DACs ) ); // 2
  hits.push_back( new INTTHit(   1,   0,  15,   1,   1,   5,   1,   1, 100,   1, DACs ) ); // 3
  hits.push_back( new INTTHit(   1,   0,   7,   1,   1, 121,   1,   1, 100,   1, DACs ) ); // 4
  hits.push_back( new INTTHit(   1,   0,   7,   1,   1, 119,   1,   1, 100,   1, DACs ) ); // 5
  hits.push_back( new INTTHit(   1,   0,   7,   1,   1, 118,   1,   1, 100,   1, DACs ) ); // 6
  hits.push_back( new INTTHit(   1,   0,  15,   1,   1,   6,   1,   1, 100,   1, DACs ) ); // 7
  hits.push_back( new INTTHit(   1,   0,   7,   1,   1, 117,   1,   1, 100,   1, DACs ) ); // 8
  hits.push_back( new INTTHit(   1,   0,  25,   1,   1,  45,   1,   1, 100,   1, DACs ) ); // 9
  */
  
  hits.push_back( new INTTHit(0, 0, 18, 0, 6, 93, 8, 52, 31412, -1, DACs ) );
  hits.push_back( new INTTHit(4, 0, 18, 0, 6, 94, 8, 52, 31412, -1, DACs ) );
  hits.push_back( new INTTHit(3, 0, 18, 0, 6, 95, 8, 52, 31412, -1, DACs ) );
  hits.push_back( new INTTHit(2, 0, 18, 0, 6, 96, 8, 52, 31412, -1, DACs ) );
  hits.push_back( new INTTHit(0, 0, 18, 0, 6, 97, 8, 52, 31412, -1, DACs ) );

  cout << "  ";
  for( int i=0; i<hits.size(); i++ )
    cout << i << " ";
  cout << endl;
  cout << string( 2 * 11 , '-' ) << endl;
  vector < vector < int > > groups(10);
  for( int i=0; i<hits.size(); i++ )
    {

      //if( hits[i]->GetClusteringStatus() == -1 )
      //continue;
      
      cout << i << "|" << string( (i+1) * 2 , ' ' );
      for( int j=i+1; j<hits.size(); j++ )
	{
	  bool flag = hits[i]->IsCluster( hits[j] );

	  if( flag )
	    cout << "\033[31m";
	  else
	    cout << "\033[36m";

	  cout << flag << " \033[m";
	  
	  if( flag == true )
	    {
	      hits[i]->Clustering( hits[j] );
	    }
	}
      cout << endl;
    }

  cout << "Hits finally: ";
  for( int i=0; i<hits.size(); i++ )
    if( hits[i]->GetClusteringStatus() != -1 )
      cout << i << " ";
  cout << endl;
  
  for( const auto& hit : hits )
    if( hit->GetClusteringStatus() != -1 )
      hit->Print();
  
  return 0;
}

    

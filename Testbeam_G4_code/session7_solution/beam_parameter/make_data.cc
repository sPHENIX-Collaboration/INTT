#define __CINT__

int make_data( string data = "Au_20um.tsv" )
{
  // setting of the output file
  string output = data.substr( 0, data.find_last_of( "." ) ) + ".root";
  TFile* tf = new TFile( output.c_str(), "RECREATE" );
  
  ifstream ifs( data.c_str() );
  if( ifs.fail() )
    {
      cerr << data << " is not found" << endl;
      return -1;
    }

  string line;

  // skip headers
  getline( ifs, line ); //  cout << line << endl;
  getline( ifs, line ); //  cout << line << endl;

  // setting of the graphs to be output
  const int kGraph_num = 4;
  vector < TGraph* > graphs;

  // init graphs
  for( int i=0; i<kGraph_num; i++ )
    {
      graphs.push_back( new TGraph() );
    }

  graphs[0]->SetName( "momentum_vs_spread" );
  graphs[1]->SetName( "momentum_vs_spread_xp_15" );
  graphs[2]->SetName( "momentum_vs_spread_xp_3" );
  graphs[3]->SetName( "momentum_vs_spread_xp_45" );

  // loop over the data file by line
  while( getline( ifs, line ) )
    {
      stringstream ss(line);

      string stemp;
      ss >> stemp; // current for magnet, no need

      for( int i=0; i<kGraph_num ; i++ )
	{
	  double momentum, spread, dtemp;
	  ss >> momentum >> spread >> dtemp; // dtemp = spread / momentum
	  cout << momentum << "\t" << spread << endl;

	  graphs[i]->SetPoint( graphs[i]->GetN(), momentum, spread );
	}

    }


  for( int i=0; i<kGraph_num; i++ )
    tf->WriteTObject( graphs[i], graphs[i]->GetName() );

  ifs.close();
  tf->Close();
  return 0;
}

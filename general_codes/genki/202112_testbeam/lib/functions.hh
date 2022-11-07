#pragma once

#ifdef __linux__
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh" // for ?

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include <vector>

int global_numbering = 0;

int ADC2DAC( vector < int >& dac_config, int adc )
{

  assert( -1<adc && adc<8 );

  return dac_config[ adc ];
}

template < class T >
void ClearVector( vector < T >& values )
{
  values.erase( values.begin(), values.end() );
}

int GetHotChip( TTree* tr, int module, string additional_cut = "" )
{
  string cut = string( "module==" ) + to_string( module );
  if( additional_cut != "" )
    cut += "&& " + additional_cut;

  string hist_name = "GetHotChip_" + to_string( global_numbering );  
  auto hist = new TH1D( hist_name.c_str(), "title", 13, 1, 14 );
  string expression = "chip_id>>" + hist_name;
  tr->Draw( expression.c_str(), cut.c_str(), "goff" );

  //tr, "", 13, 1, 14, "chip_id", cut );
  vector < int > contents_vec;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      contents_vec.push_back( hist->GetBinContent(i) );
    }
  
  int index = max_element( contents_vec.begin(), contents_vec.end() ) - contents_vec.begin() + 1;

  global_numbering++;
  return index;
}

template < class T >
void PushBack( vector < pair < TObject*, string > >& obj_saved, T* obj, std::initializer_list < string > list )
{

  string name = "";
  for( string name_arg : list )
    name += name_arg + "_";

  cout << name.substr( 0, name.size() - 1 )  << endl;
  pair < TObject*, string > this_pair( (TObject*)obj, name.substr( 0, name.size() - 1 ) );
  obj_saved.push_back( this_pair );
}

template < class T >
string GetCutForLaddersEachValue( int num, string expression, vector < T >& values )
{
  if( expression.find( "[" ) != string::npos )
    return expression;
  
  stringstream rtn;
  for( int i=0; i<num; i++ )
    rtn << expression << "[" << i <<  "] == " << values[i] << " && ";

  
  return rtn.str().substr( 0, rtn.str().size() - 3);
}

template < class T >
string GetCutForLadders( int num, string expression, T value )
{
  vector < T > values(num, value);
  return GetCutForLaddersEachValue( num, expression, values );
}

string CutArgumentProcess( string cut_arg )
{
  stringstream ss( cut_arg );
  vector < pair < string, string > > expressions;
  string stemp = "", stemp2 = "";
  while( ss >> stemp2 )
    {
      if( stemp2 != "&&" && stemp2 != "||" )
	{
	  stemp += stemp2;
	}
      else
	{
	  expressions.push_back( pair < string, string >(stemp, stemp2) );
	  stemp = "";
	}
    }

  // the last piece
  expressions.push_back( pair < string, string >(stemp, "") );

  string rtn = "";
  vector < string > ope_list;
  ope_list.push_back( "==" );
  ope_list.push_back( "!=" );
  ope_list.push_back( ">=" );
  ope_list.push_back( "<=" );
  ope_list.push_back( "<" );
  ope_list.push_back( ">" );

  for( auto& expression_pair : expressions )
    {

      if( expression_pair.first.find( "[" ) != string::npos )
	{
	  rtn += "(" + expression_pair.first + expression_pair.second + ")";
	  continue;
	}
      
      string ope = ""; // operator
      for( auto& it : ope_list )
	if( expression_pair.first.find( it ) != string::npos )
	  ope = it;

      // divide the expression in the argument (e.g. dac>30  -> dac and 30)
      string expression = expression_pair.first.substr( 0, expression_pair.first.find( ope ) );
      string value = expression_pair.first.substr( expression_pair.first.find( ope ) + ope.size() );

      string cut = GetCutForLadders( 3, expression, value );
      cut = "(" + Replace( cut, "==", ope ) + ")" + expression_pair.second ;

      rtn += cut;
    }

  return rtn;
}

bool Get3SigmaRange( TF1* f, double& lower, double& upper )
{

  double xmin = f->GetXmin();
  double xmax = f->GetXmax();

  //double mean = f->Mean( xmin, xmax ); // usuless, It's fine in CINT, but returns crazy value in macro
  double mean = f->GetParameter(1);
  double whole_area = f->Integral( xmin, xmax );
  
  double division = 1e5;
  double step = (xmax - xmin ) / division;

  for( int i=0; i<int(division); i++ )
    {
      double xmin_current = mean - i * step;
      double xmax_current = mean + i * step;

      double area = f->Integral( xmin_current, xmax_current );

      if( area / whole_area > 0.9973 )
	{
	  lower = xmin_current;
	  upper = xmax_current;

	  return true;
	}

    }

  return false;
}

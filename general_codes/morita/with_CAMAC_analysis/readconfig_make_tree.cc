//20211119
//MiuMorita
//edit readconfig_make_tree.cc based on readconfig.cc made by A.Suzuki

#ifndef READCONFIG_CC
#define READCONFIG_CC

#include <map>

using namespace std;

class ReadConfig
{
 public :
    ReadConfig()
    { 
    }

 //bool read(const char* configfile = "effana.ini");
 bool read( const char* configfile = "make_tree_new.ini" );

 TString getParameter(  const char *parname, bool &status );
 int     getParameteri( const char *parname, bool &status );
 double  getParameterf( const char *parname, bool &status );

 private :
    map<TString, TString> m_fmap;
};
 
//bool ReadConfig::read(const char* configfile = "effana.ini"){
//bool ReadConfig::read(const char* configfile = "make_tree_new.ini"){
bool ReadConfig::read( const char* configfile )
{

 ifstream fin( configfile );
 if( !fin.is_open() )
 {
    cout << "Empty" << endl;
    return false;
 }
  
 TString sline;
 
 while( sline.ReadLine( fin ) )
 {
    sline.ReplaceAll( " ", "" );

    //cout << sline << endl;
    //cout <<  sline.Index("//",0) << endl;

    int sindex = sline.Index( "//", 0 );
    
	 TString sbody = ( sindex > 0 ? TString( sline, sline.Index( "//", 0 ) ) : TString( sline ) );

    //cout << sbody << endl;
    
    TObjArray *objarray = sbody.Tokenize( "=" );  

    //cout << sbody << " " << objarray->LastIndex() << endl;
    //cout << ((TObjString* )(objarray->At(0)))->GetString() << endl;
    //cout << ((TObjString* )(objarray->At(1)))->GetString() << endl;

    TString skey = ( ( TObjString* )( objarray -> At( 0 ) ) ) -> GetString();
    TString sval = ( ( TObjString* )( objarray -> At( 1 ) ) ) -> GetString();    
    skey.ToUpper();
    sval.ToUpper();
    //TString sval = "aaa";

    m_fmap.insert( make_pair( skey, sval ) );
    //cout << m_fmap.size() << endl;
    
    if( sline.Index( "//", 0 ) == 0 )
	 {
      //cout << "comment" << endl;
    }
    //cout << sline << endl;
 }

 return true;

}

TString ReadConfig::getParameter( const char *parname, bool &status )
{
  TString sparname( parname );
  sparname.ToUpper();
  map<TString, TString>::iterator itr = m_fmap.find( sparname );

  status = ( itr != m_fmap.end() );
  
  if( status )
  {
    cout << itr -> first << " " << itr -> second << endl;
    return itr -> second;
  }
  
  return TString();
  
}

int ReadConfig::getParameteri( const char *parname, bool &status )
{
  TString spar = getParameter( parname, status );
  if( status )return spar.Atoi();
  return -9999;
}

double ReadConfig::getParameterf( const char *parname, bool &status )
{
  TString spar = getParameter( parname, status );
  if( status )return spar.Atof();
  return -9999;
}

void readconfig()
{
  ReadConfig rc;
  //rc.read("effana.ini");
  rc.read( "make_tree_new.ini" );

  bool status;
  TString spar = rc.getParameter("SINGLE_HIT_PER_CHIP_DENOMINATOR", status);
  //cout << spar << " " << status << endl;

}

#endif

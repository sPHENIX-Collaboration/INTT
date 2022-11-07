/*! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  @file NoisyChannelList.hh
  @brief Class to treat noisy channels.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once


class NoisyChannel
{
public:
  int run_ = -1;
  string name_ = "";
  int module_ = -1;
  int chip_ = -1;
  int channel_ = -1;

  NoisyChannel(){};
  NoisyChannel( string run, string name, string item )
  {
    run_ = stoi( run );
    name_ = name;
    
    if( item == "none" )
      return;
	
    int index_first = item.find_first_of("-");
    int index_last = item.find_last_of("-");
    
    module_ = stoi( item.substr(0, index_first ));
    chip_ = stoi( item.substr(index_first+1, index_last-2 ) );
    channel_ = stoi( item.substr(index_last+1, item.size() ) );

  }

  bool IsData()
  {
    if( run_ == -1 || name_ == "" || module_ == -1 || chip_ == -1 || channel_ == -1 )
      return false;
    return true;
  }
  
  string  Print( bool do_print = true )
  {
    stringstream rtn;
    if( !this->IsData() )
      return "";
    
    rtn << "Run" << setw(2) << run_ << "  "
	<< setw(33) << name_ << "  "
	<< "module" << setw(1) << module_ << ", "
	<< "chip" << setw(2) << chip_ << ", "
	<< "ch" << setw(3) << channel_;
    if( do_print )
      cout << rtn.str() << endl;

    return rtn.str();
  };
};

class NoisyChannelList
{
public:
  NoisyChannelList(){};
  NoisyChannelList( string data );

  vector < NoisyChannel* > GetList( int run );
  vector < NoisyChannel* > GetListWithChipColumn( int run );
  string GetCut( int run );
  string GetCutWithChipColumn( int run );
  
  void SetLineLnegth( int num ){ line_length_ = num;};
  void Print();
  
private:

  int line_length_ = 100;
  string data_ = "";

  vector < NoisyChannel* > noisy_list_;
  
  void Init();
  string GetCutPrivate( vector < NoisyChannel* >& list );

  template < class T >
  //  static
  void PrintVector( T& values, string separator = " ", string last = "\n" );

  
  void PrintLine( string header = "header", string words = "words" , char fill_with = ' ',
		  string starting = "| ", string ending = "|" );
};


void NoisyChannelList::Init()
{

  ifstream ifs( data_.c_str() );
  if( ifs.fail() )
    {
      cerr << data_ << " is not found." << endl;
      return;
    }
  
  string line;
  getline( ifs, line ); // for header

  while( getline( ifs, line ) )
    {
      stringstream ss(line);
      string run, name;
      ss >> run >> name;

      string item;
      while( ss >> item )      
	noisy_list_.push_back( new NoisyChannel( run, name, item ) );
    }

  ifs.close();
}

NoisyChannelList::NoisyChannelList( string data )
{
  data_ = data;
  Init();
}


vector < NoisyChannel* > NoisyChannelList::GetList( int run )
{
  vector < NoisyChannel* > rtn;
  for( auto& noisy : noisy_list_ )
    {
      if( !noisy->IsData() )
	continue;

      if( noisy->run_ != run )
	continue;

      rtn.push_back( noisy );

    }

  return rtn;
}

vector < NoisyChannel* > NoisyChannelList::GetListWithChipColumn( int run )
{
  vector < NoisyChannel* > rtn = this->GetList( run );
  for( auto& noisy : rtn )
    {
      bool flag = noisy->chip_ < 14;
      noisy->chip_ = ( flag  ? noisy->chip_ : noisy->chip_ -13 );
      noisy->channel_ = ( flag ? noisy->channel_ : noisy->channel_ + 128);
    }
  
  return rtn;
}

string NoisyChannelList::GetCutPrivate( vector < NoisyChannel* >& list )
{
  string cut = "";

  for( auto& noisy : list )
    {
      string this_cut = string("(")
	+ "module==" + to_string( noisy->module_ )
	+ " && " + "chip_id==" + to_string( noisy->chip_ )
	+ " && " + "chan_id==" + to_string( noisy->channel_ )
	+ ")";
      
      if( cut != "" )
	this_cut = " && " + this_cut;

      cut += this_cut;
    }

  if( cut != "" )
    {
      if( cut.find( ") && (" ) != string::npos )
	cut = "(" + cut + ")";

      cut = "!" + cut;
    }
  
  return cut;
}

string NoisyChannelList::GetCut( int run )
{
  auto list = this->GetList( run );
  return this->GetCutPrivate( list );
}
  
string NoisyChannelList::GetCutWithChipColumn( int run )
{
  auto list = this->GetListWithChipColumn( run );
  return this->GetCutPrivate( list );
}

template < class T >
void NoisyChannelList::PrintVector( T& values, string separator, string last )
{
  for( auto& value : values )
    cout << value << separator;

  cout << last;
}

void NoisyChannelList::PrintLine( string header, string words, char fill_with, string starting, string ending  )
{

  int filling_num = line_length_ - (starting.size() + header.size() + words.size() + ending.size() );
  if( header.size() != 0 )
    filling_num--;
  
  cout << starting
       << header
       << (header.size() == 0? "" : " ")
       << words
       << string( filling_num, fill_with )
       << ending
       << endl;

}

void NoisyChannelList::Print()
{
  int width = 5;
  this->PrintLine( "----- NoisyChannelList", "", '-', "+", "+" );

  this->PrintLine( "Data:", data_ );
  for( auto& noisy : noisy_list_ )
    if( noisy->Print( false )  != "" )
      this->PrintLine( "Noise:", noisy->Print( false ) );
  
  this->PrintLine( "", "", '-', "+", "+" );
}

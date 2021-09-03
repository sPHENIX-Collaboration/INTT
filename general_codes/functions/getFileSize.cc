#include "getFileSize.hh"

// returns the size of file
int getFileSize(std::ifstream& f) {
  f.seekg(0, std::ifstream::end);

  if (f.fail()) {
    std::ifstream::iostate state = f.rdstate();
    std::cout << "error seeking to end, read state = " << state << std::endl;
    if (state & std::ios::eofbit)  std::cout << " eof bit set" << std::endl;
    if (state & std::ios::failbit) std::cout << " fail bit set" << std::endl;
    if (state & std::ios::badbit)  std::cout << " bad bit set" << std::endl;
    return 0;
  }

  int size = f.tellg();
  if (size < 0) {
    std::ifstream::iostate state = f.rdstate();
    std::cout << "error in tellg, read state = " << state << std::endl;
    if (state & std::ios::eofbit)
      std::cout << " eof bit set" << std::endl;

    if (state & std::ios::failbit)
      std::cout << " fail bit set" << std::endl;

    if (state & std::ios::badbit)
      std::cout << " bad bit set" << std::endl;
    
    return 0;
  }
  
  // Return the file stream to the start of the file
  f.seekg(0, std::ifstream::beg);
  
  return size;
}

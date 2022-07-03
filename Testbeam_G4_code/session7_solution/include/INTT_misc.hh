// #ifndef INTT_misc_h
// #define INTT_misc_h 1
#pragma once

#include <string>

namespace INTT_misc
{
  const std::string black = "\033[30;1m";
  //const std::string red = "\033[31;1m";
  const std::string red = "\033[31;1m";
  //const char* red = "\033[31;1m";
  const std::string green = "\033[32;1m";
  const std::string yellow = "\033[33;1m";
  const std::string blue = "\033[34;1m";
  const std::string pink = "\033[35;1m";
  const std::string cyan = "\033[36;1m";
  const std::string white = "\033[37;1m";

  //const std::string close = "\[\e[;1m";
  const std::string close = "\033[0m";

}



//#endif // INTT_misc_h

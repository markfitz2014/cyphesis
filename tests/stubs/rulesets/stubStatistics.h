// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubStatistics_custom.h file.

#ifndef STUB_RULESETS_STATISTICS_H
#define STUB_RULESETS_STATISTICS_H

#include "rulesets/Statistics.h"
#include "stubStatistics_custom.h"

#ifndef STUB_Statistics_Statistics
//#define STUB_Statistics_Statistics
   Statistics::Statistics(Character & chr)
    : m_script(nullptr)
  {
    
  }
#endif //STUB_Statistics_Statistics

#ifndef STUB_Statistics_get
//#define STUB_Statistics_get
  float Statistics::get(const std::string & name)
  {
    return 0;
  }
#endif //STUB_Statistics_get

#ifndef STUB_Statistics_get_default
//#define STUB_Statistics_get_default
  float Statistics::get_default(const std::string & name)
  {
    return 0;
  }
#endif //STUB_Statistics_get_default

#ifndef STUB_Statistics_increment
//#define STUB_Statistics_increment
  void Statistics::increment(const std::string & name, OpVector & res)
  {
    
  }
#endif //STUB_Statistics_increment


#endif
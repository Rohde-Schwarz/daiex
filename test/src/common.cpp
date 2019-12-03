#include "common.h"

#include <fstream>

using namespace std;

std::string Common::TestDataDir;
std::string Common::TestOutputDir;

std::mutex Common::localeLock_;

bool Common::binCompareFiles(const std::string& file1, const std::string& file2)
{
  // Open both files and compare content
  ifstream f1(file1, ifstream::binary | ifstream::ate);
  ifstream f2(file1, ifstream::binary | ifstream::ate);

  if (f1.fail() || f2.fail())
  {
    return false;
  }

  if (f1.tellg() != f2.tellg()) 
  {
    return false; 
  }

  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>(f2.rdbuf()));
}

#include "gtest/gtest.h"

#include "common.h"
#include <iostream>

int main(int argc, char **argv)
{
  if (argc >= 3)
  {
    Common::TestDataDir = argv[1];
    Common::TestOutputDir = argv[2];
  }
  else
  {
    std::cout << "wrong usage. Either call\n";
    std::cout << "daiex TEST_DATA_DIR TEST_OUTPUT_DIR or\n";
    std::cout << "daiex TEST_DATA_DIR TEST_OUTPUT_DIR --GTEST_PARAM\n";
    exit(0);
  }

  if (Common::TestDataDir.empty())
  {
    std::cout << "TestDataDir not defined. Aborting tests.\n";
    exit(-1);
  }

#ifdef _WIN32
  if (Common::TestDataDir.back() != '\\' && Common::TestDataDir.back() != '/')
  {
    Common::TestDataDir += '\\';
  }

  if (Common::TestOutputDir.back() != '\\' && Common::TestOutputDir.back() != '/')
  {
    Common::TestOutputDir += '\\';
  }

  mkdir(Common::TestOutputDir.c_str());
#else
  if (Common::TestDataDir.back() != '/') 
  {
    Common::TestDataDir += '/';
  }

  if (Common::TestOutputDir.back() != '/') 
  {
    Common::TestOutputDir += '/';
  }

  mkdir(Common::TestOutputDir.c_str(), 777);
#endif

  std::cout << "Using data dir: " << Common::TestDataDir << "\n";
  std::cout << "Using tmp dir: " << Common::TestOutputDir << "\n";

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

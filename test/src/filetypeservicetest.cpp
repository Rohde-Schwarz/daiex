#include "gtest/gtest.h"

#include <string>
#include <vector>

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class FileTypeServiceTests : public ::testing::Test
{
};

class FileTypeTests : public ::testing::TestWithParam<rohdeschwarz::mosaik::dataimportexport::FileType>
{
};

TEST_F(FileTypeServiceTests, TestPossibleFileFormats)
{
  vector<FileType> availableFormats = FileTypeService::getPossibleFileFormats();
  ASSERT_EQ(availableFormats.size(), 8);
  ASSERT_EQ(availableFormats[0], FileType::IQW);
  ASSERT_EQ(availableFormats[1], FileType::Iqtar);
  ASSERT_EQ(availableFormats[2], FileType::Csv);
  ASSERT_EQ(availableFormats[3], FileType::Matlab4);
  ASSERT_EQ(availableFormats[4], FileType::Matlab73);
  ASSERT_EQ(availableFormats[5], FileType::IQX);
  ASSERT_EQ(availableFormats[6], FileType::WV);
  ASSERT_EQ(availableFormats[7], FileType::AID);
}

TEST_P(FileTypeTests, TestFileExtensionAndFilter)
{
  FileType fileType = GetParam();

  string filter = FileTypeService::getFileFilter(fileType);
  string ext = FileTypeService::getFileExtension(fileType);
  
  size_t found = filter.find(ext);
  ASSERT_NE(found, string::npos);
}

INSTANTIATE_TEST_CASE_P(FileTypeTests, FileTypeTests,
                        ::testing::Values(FileType::IQW, FileType::Iqtar, FileType::Csv, FileType::Matlab4, FileType::Matlab73));
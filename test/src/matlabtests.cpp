#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

template<typename T, MatlabVersion mat>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const MatlabVersion Mat = mat; 
};

template<typename T>
class MatlabFormatTests : public ::testing::Test
{
};

class MatlabTests : public ::testing::Test
{
};

typedef ::testing::Types<
  Container<float, MatlabVersion::Mat4>,
  Container<double, MatlabVersion::Mat4>,
  Container<float, MatlabVersion::Mat73>,
  Container<double, MatlabVersion::Mat73>
> MyTypes;
TYPED_TEST_CASE(MatlabFormatTests, MyTypes);

TEST_F(MatlabTests, CheckTempDir)
{
  const string filename = Common::TestOutputDir + "CheckDefaultTempDir.mat";
  string tempDir;
#ifdef _WIN32
  tempDir = "c:/temp/"; // trailing slash will be removed by libdai to check if folder exists
#else
  tempDir = "/tmp/";
#endif

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  Iqw writeFile(filename);

  auto ret = writeFile.setTempDir(tempDir);

  ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

   string setTempDir = writeFile.getTempDir() + "/";
  ASSERT_EQ(tempDir, setTempDir);

  remove(filename.c_str());
}

TEST_F(MatlabTests, ChangeTempDirAfterWriterOpen)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.mat";
  string tempDir;
#ifdef _WIN32
  tempDir = "c:/";
#else
  tempDir = "/tmp/";
#endif

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));


  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.setTempDir(tempDir);
  ASSERT_EQ(ret, ErrorCodes::WriterAlreadyInitialized);

  remove(filename.c_str());
}

TEST_F(MatlabTests, UnknownTempPath)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.mat";
  string tempDir;
#ifdef _WIN32
  tempDir = "c:/ABCDEFG";
#else
  tempDir = "/ABCDEFG/";
#endif

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IqTar writeFile(filename);
  auto ret = writeFile.setTempDir(tempDir);
  ASSERT_EQ(ret, ErrorCodes::FolderNotFound);

  remove(filename.c_str());
}

TYPED_TEST(MatlabFormatTests, ReadGenuineMatFiles)
{
  MatlabVersion matVersion = TypeParam::Mat;
  string versionString;
  if (matVersion == MatlabVersion::Mat4)
  {
    versionString = "v4";
  }
  else if (matVersion == MatlabVersion::Mat73)
  {
    versionString = "v73";
  }

  const string filename = Common::TestDataDir + "ReadGenuineMatFiles_" + versionString + ".mat";

  IqMatlab file(filename);
  
  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ("RealerKanal1", arrayNames[0]);
  ASSERT_EQ("RealerKanal2", arrayNames[1]);

  vector<ChannelInfo> channelInfos;
  map<string, string> metadata;
  ret = file.getMetadata(channelInfos, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  
  ASSERT_EQ(7, metadata.size());
  ASSERT_EQ(2, channelInfos.size());

  ASSERT_EQ(0.1, channelInfos[0].getFrequency());
  ASSERT_EQ(1.1, channelInfos[0].getClockRate());
  ASSERT_EQ(0.2, channelInfos[1].getFrequency());
  ASSERT_EQ(2.2, channelInfos[1].getClockRate());

  auto l = file.getArraySize(arrayNames[0]);
  ASSERT_EQ(1001, l);
  l = file.getArraySize(arrayNames[1]);
  ASSERT_EQ(1001, l);

  vector<typename TypeParam::Dt> values(l);
  ret = file.readArray(arrayNames[0], values, l);
  ASSERT_EQ(ret, ErrorCodes::Success);
  for (size_t i = 0; i < l; ++i)
  {
    ASSERT_EQ(values[i], i + 1);
  }

  file.close();
}

TEST_F(MatlabTests, ReadGenuineMat70)
{
  const string filename = Common::TestDataDir + "ReadGenuineMatFiles_v7.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ("RealerKanal1", arrayNames[0]);
  ASSERT_EQ("RealerKanal2", arrayNames[1]);

  vector<ChannelInfo> channelInfos;
  map<string, string> metadata;
  ret = file.getMetadata(channelInfos, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(7, metadata.size());
  ASSERT_EQ(2, channelInfos.size());

  ASSERT_EQ(0.1, channelInfos[0].getFrequency());
  ASSERT_EQ(1.1, channelInfos[0].getClockRate());
  ASSERT_EQ(0.2, channelInfos[1].getFrequency());
  ASSERT_EQ(2.2, channelInfos[1].getClockRate());

  auto l = file.getArraySize(arrayNames[0]);
  ASSERT_EQ(1001, l);
  l = file.getArraySize(arrayNames[1]);
  ASSERT_EQ(1001, l);

  vector<float> values(l);
  ret = file.readArray(arrayNames[0], values, l);
  ASSERT_EQ(ret, ErrorCodes::Success);
  for (size_t i = 0; i < l; ++i)
  {
    ASSERT_EQ(values[i], i + 1);
  }
}

TEST_F(MatlabTests, ReadGenuineMat60)
{
  const string filename = Common::TestDataDir + "ReadGenuineMatFiles_v6.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ("RealerKanal1", arrayNames[0]);
  ASSERT_EQ("RealerKanal2", arrayNames[1]);

  vector<ChannelInfo> channelInfos;
  map<string, string> metadata;
  ret = file.getMetadata(channelInfos, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(7, metadata.size());
  ASSERT_EQ(2, channelInfos.size());

  ASSERT_EQ(0.1, channelInfos[0].getFrequency());
  ASSERT_EQ(1.1, channelInfos[0].getClockRate());
  ASSERT_EQ(0.2, channelInfos[1].getFrequency());
  ASSERT_EQ(2.2, channelInfos[1].getClockRate());

  auto l = file.getArraySize(arrayNames[0]);
  ASSERT_EQ(1001, l);
  l = file.getArraySize(arrayNames[1]);
  ASSERT_EQ(1001, l);

  vector<float> values(l);
  ret = file.readArray(arrayNames[0], values, l);
  ASSERT_EQ(ret, ErrorCodes::Success);
  for (size_t i = 0; i < l; ++i)
  {
    ASSERT_EQ(values[i], i + 1);
  }

  file.close();
}
TEST_F(MatlabTests, QueryMat)
{
  const string filename = Common::TestDataDir + "QueryMat.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::InvalidFormatOfIqMatlabContent);
  ASSERT_EQ(0, arrayNames.size());

  file.matchArrayDimensions(50, 50, false, arrayNames);
  ASSERT_EQ(1, arrayNames.size());

  auto rows = file.getNofRows(arrayNames[0]);
  ASSERT_EQ(103, rows);

  auto cols = file.getNofCols(arrayNames[0]);
  ASSERT_EQ(1009, cols);

  file.close();
}

TEST_F(MatlabTests, UninitializedQueryMatExactCol)
{
  const string filename = Common::TestDataDir + "QueryMat.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  file.matchArrayDimensions(11, 10, true, arrayNames);
  ASSERT_EQ(1, arrayNames.size());

  auto rows = file.getNofRows(arrayNames[0]);
  ASSERT_EQ(10, rows);

  auto cols = file.getNofCols(arrayNames[0]);
  ASSERT_EQ(11, cols);

  file.close();
}

TEST_F(MatlabTests, UninitializedQueryInvalidIndices)
{
  const string filename = Common::TestDataDir + "QueryMat.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  file.matchArrayDimensions(11, 10, true, arrayNames);
  ASSERT_EQ(1, arrayNames.size());

  vector<double> values;
  auto ret = file.readRawArray(arrayNames[0], 200, 2, values);
  ASSERT_EQ(ret, ErrorCodes::InvalidColumnIndex);

  ret = file.readRawArray(arrayNames[0], 2, 1000, values);
  ASSERT_EQ(ret, ErrorCodes::InvalidDataInterval);

  ret = file.readRawArray(arrayNames[0], 2, 2, values, 1000);
  ASSERT_EQ(ret, ErrorCodes::InvalidDataInterval);

  file.close();
}

TEST_F(MatlabTests, UninitializedQueryDataFromDouble)
{
  const string filename = Common::TestDataDir + "QueryMat.mat";

  IqMatlab file(filename);

  vector<string> arrayNames;
  file.matchArrayDimensions(1009, 103, true, arrayNames);
  ASSERT_EQ(1, arrayNames.size());

  vector<double> values;
  auto ret = file.readRawArray(arrayNames[0], 4, 10, values, 40);
  ASSERT_EQ(ret, ErrorCodes::Success);

  for (int i = 0; i < 10; ++i)
  {
    ASSERT_EQ(values[i], i + 1);
  }

  file.close();
}

TEST_F(MatlabTests, UninitializedQueryDataFromSingle)
{
  const string filename = Common::TestDataDir + "QueryMat.mat";

  IqMatlab file(filename);

  vector<float> values;
  auto ret = file.readRawArray("singlemat", 2, 5, values, 3);
  ASSERT_EQ(ret, ErrorCodes::Success);

  for (int i = 0; i < 5; ++i)
  {
    ASSERT_EQ(values[i], i + 1);
  }

  file.close();
}

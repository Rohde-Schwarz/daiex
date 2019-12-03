#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

template<typename T, FileType ft, int realChannels>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const int nofRealChannels = realChannels;
  static const FileType Ft = ft; 
};

template<typename T>
class ReadRealTest : public ::testing::Test
{
public:
  map<string, string> metaDataWrite_;

protected:
  void SetUp()
  {
    this->metaDataWrite_.insert(make_pair("Key1<>", "Wert1"));
    this->metaDataWrite_.insert(make_pair("2 Key", "Wert2"));
    this->metaDataWrite_.insert(make_pair("Rohde&Schwarz", "Wert3"));
  }
};

typedef ::testing::Types<
  Container<float, FileType::Iqtar, 1>,
  Container<double, FileType::Iqtar, 1>,
  Container<float, FileType::Iqtar, 2>,
  Container<double, FileType::Iqtar, 2>,
  Container<float, FileType::Iqtar, 3>,
  Container<double, FileType::Iqtar, 3>,
  #ifndef __APPLE__
  Container<float, FileType::Matlab4, 1>,
  Container<double, FileType::Matlab4, 1>,
  Container<float, FileType::Matlab4, 2>,
  Container<double, FileType::Matlab4, 2>,
  Container<float, FileType::Matlab4, 3>,
  Container<double, FileType::Matlab4, 3>,
  Container<float, FileType::Matlab73, 1>,
  Container<double, FileType::Matlab73, 1>,
  Container<float, FileType::Matlab73, 2>,
  Container<double, FileType::Matlab73, 2>,
  Container<float, FileType::Matlab73, 3>,
  Container<double, FileType::Matlab73, 3>,
  #endif
  Container<float, FileType::Csv, 1>,
  Container<double, FileType::Csv, 1>,
  Container<float, FileType::Csv, 2>,
  Container<double, FileType::Csv, 2>,
  Container<float, FileType::Csv, 3>,
  Container<double, FileType::Csv, 3>
> MyTypes;

TYPED_TEST_CASE(ReadRealTest, MyTypes);


TYPED_TEST(ReadRealTest, WriteRealArrayReadArrayAndChannel)
{
  const string filename = Common::TestOutputDir + "WriteRealChannelReadArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::nofRealChannels;
  const FileType fileType = TypeParam::Ft;

  vector<vector<typename TypeParam::Dt>> iqValues;
  iqValues.reserve(nofChannels);

  vector<ChannelInfo> channelInfos;
  channelInfos.reserve(nofChannels);

  Common::initVector(iqValues, nofChannels, 6);

  for (size_t i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 12, 12));
  }

  auto file = FileTypeService::create(filename, fileType)	;
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Real, nofChannels, "Mosaik DAI", "Real data", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), nofChannels);

  for (size_t channel = 0; channel < nofChannels; ++channel)
  {
    long size = file->getArraySize(arrayNames[channel]);

    for (size_t startIdx = 0; startIdx < size; ++startIdx)
    {
      int values2Read = (int)size - startIdx;
      if (startIdx == 0)
      {
        values2Read = 3;
      }
      else if (values2Read % 2 == 0)
      {
        values2Read = values2Read / 2;
      }

      vector<typename TypeParam::Dt> values(values2Read);

      ret = file->readChannel(arrayNames[channel], values, values2Read, startIdx);
      ASSERT_EQ(ret, ErrorCodes::Success);

      for (size_t idx = 0; idx < values2Read; ++idx)
      {
        ASSERT_NEAR(values[idx], iqValues[channel][startIdx + idx], 0.00001) << "unexpected value";
      }

      ret = file->readArray(arrayNames[channel], values, values2Read, startIdx);
      ASSERT_EQ(ret, ErrorCodes::Success);

      for (size_t idx = 0; idx < values2Read; idx++)
      {
        ASSERT_NEAR(values[idx], iqValues[channel][startIdx + idx], 0.00001) << "Unexpected value";
      }
    }
  }

  delete file;

  remove(filename.c_str());
}

TYPED_TEST(ReadRealTest, WriteRealChannelReadArrayAndChannel)
{
  const string filename = Common::TestOutputDir + "WriteRealChannelReadArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::nofRealChannels;
  const FileType fileType = TypeParam::Ft;

  vector<vector<typename TypeParam::Dt>> iqValues;
  iqValues.reserve(nofChannels);

  vector<ChannelInfo> channelInfos;
  channelInfos.reserve(nofChannels);

  Common::initVector(iqValues, nofChannels, 6);

  for (size_t i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 12, 12));
  }

  auto file = FileTypeService::create(filename, fileType)	;
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Real, nofChannels, "Mosaik DAI", "Real data", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), nofChannels);

  for (size_t channel = 0; channel < nofChannels; ++channel)
  {
    long size = file->getArraySize(arrayNames[channel]);

    for (size_t startIdx = 0; startIdx < size; ++startIdx)
    {
      int values2Read = (int)size - startIdx;
      if (startIdx == 0)
      {
        values2Read = 3;
      }
      else if (values2Read % 2 == 0)
      {
        values2Read = values2Read / 2;
      }

      vector<typename TypeParam::Dt> values(values2Read);

      ret = file->readChannel(arrayNames[channel], values, values2Read, startIdx);
      ASSERT_EQ(ret, ErrorCodes::Success);

      for (size_t idx = 0; idx < values2Read; ++idx)
      {
        ASSERT_NEAR(values[idx], iqValues[channel][startIdx + idx], 0.00001) << "unexpected value";
      }

      ret = file->readArray(arrayNames[channel], values, values2Read, startIdx);
      ASSERT_EQ(ret, ErrorCodes::Success);

      for (size_t idx = 0; idx < values2Read; idx++)
      {
        ASSERT_NEAR(values[idx], iqValues[channel][startIdx + idx], 0.00001) << "Unexpected value";
      }
    }
  }

  delete file;

  remove(filename.c_str());
}
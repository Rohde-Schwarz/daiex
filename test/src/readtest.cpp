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

template<typename T, FileType ft>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const FileType Ft = ft; 
};

template<typename T>
class ReadTest : public ::testing::Test
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
  Container<float, FileType::Iqtar>,
  Container<double, FileType::Iqtar>,
  #ifndef __APPLE__
  Container<float, FileType::Matlab4>,
  Container<double, FileType::Matlab4>,
  Container<float, FileType::Matlab73>,
  Container<double, FileType::Matlab73>,
  #endif
  Container<float, FileType::Csv>,
  Container<double, FileType::Csv>
> MyTypes;

TYPED_TEST_CASE(ReadTest, MyTypes);


TYPED_TEST(ReadTest, WriteReadThreeChannelDataInterleaveComplex)
{ 
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "ReadThreeChannelDataInterleaved." + FileTypeService::getFileExtension(TypeParam::Ft);

  vector<vector<typename TypeParam::Dt>> iqdata;
  Common::initVector(iqdata, 3, 6);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 133.0, 155.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 133.0, 155.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal3", 133.0, 155.0));

  auto writeFile = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)writeFile)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)writeFile)->setMatlabVersion(MatlabVersion::Mat73);
  }

  int returnCode = writeFile->writeOpen(IqDataFormat::Complex, channelInfosWrite.size(), "Mosaik DataImportExport", "3 channel, complex", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = writeFile->appendChannels(iqdata);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = writeFile->close();
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  delete writeFile;

  auto readFile = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  vector<string> arrayNames;
  returnCode = readFile->readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  for (int i = 0; i < 3; ++i)
  {
    string name = "Kanal" + to_string(i+1);

    long size = readFile->getArraySize(name + "_I") * 2;
    vector<typename TypeParam::Dt> values(size);

    returnCode = readFile->readChannel(name, values, size);
    ASSERT_EQ(returnCode, ErrorCodes::Success);
    Common::almostEqual(iqdata[i], values);
  }

  readFile->close();
  delete readFile;
  remove(filename.c_str());
}

TYPED_TEST(ReadTest, WriteReadThreeChannelDataInterleaveComplex32As64And64As32)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteReadThreeChannelDataInterleaveComplex32As64And64As32." + FileTypeService::getFileExtension(TypeParam::Ft);

  vector<vector<typename TypeParam::Dt>> iqdata;
  Common::initVector(iqdata, 3, 6);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 133.0, 155.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 133.0, 155.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal3", 133.0, 155.0));

  auto writeFile = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)writeFile)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)writeFile)->setMatlabVersion(MatlabVersion::Mat73);
  }

  int returnCode = writeFile->writeOpen(IqDataFormat::Complex, channelInfosWrite.size(), "Mosaik DataImportExport", "3 channel, complex", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = writeFile->appendChannels(iqdata);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = writeFile->close();
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  delete writeFile;

  auto readFile = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  vector<string> arrayNames;
  returnCode = readFile->readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  for (int i = 0; i < 3; ++i)
  {
    string name = "Kanal" + to_string(i+1);

    long size = readFile->getArraySize(name + "_I") * 2;
    
    if (std::is_same<typename TypeParam::Dt, float>::value)
    {
      vector<double> values(size);

      returnCode = readFile->readChannel(name, values, size);
      ASSERT_EQ(returnCode, ErrorCodes::Success);
      Common::almostEqual(iqdata[i], values);
    }
    else
    {
      vector<float> values(size);

      returnCode = readFile->readChannel(name, values, size);
      ASSERT_EQ(returnCode, ErrorCodes::Success);
      Common::almostEqual(iqdata[i], values);
    }
  }

  readFile->close();
  delete readFile;
  remove(filename.c_str());
}
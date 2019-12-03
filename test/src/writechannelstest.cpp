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

template<typename T, FileType ft, int nofChannels>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const int writtenofChannels = nofChannels;
  static const FileType Ft = ft; 
};

template<typename T>
class WriteChannelsTest : public ::testing::Test
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
  Container<float, FileType::Iqtar, 4>,
  Container<double, FileType::Iqtar, 4>,
  #ifndef __APPLE__
  Container<float, FileType::Matlab4, 1>,
  Container<double, FileType::Matlab4, 1>,
  Container<float, FileType::Matlab4, 2>,
  Container<double, FileType::Matlab4, 2>,
  Container<float, FileType::Matlab4, 3>,
  Container<double, FileType::Matlab4, 3>,
  Container<float, FileType::Matlab4, 4>,
  Container<double, FileType::Matlab4, 4>,
  Container<float, FileType::Matlab73, 1>,
  Container<double, FileType::Matlab73, 1>,
  Container<float, FileType::Matlab73, 2>,
  Container<double, FileType::Matlab73, 2>,
  Container<float, FileType::Matlab73, 3>,
  Container<double, FileType::Matlab73, 3>,
  Container<float, FileType::Matlab73, 4>,
  Container<double, FileType::Matlab73, 4>,
  #endif
  Container<float, FileType::Csv, 1>,
  Container<double, FileType::Csv, 1>,
  Container<float, FileType::Csv, 2>,
  Container<double, FileType::Csv, 2>,
  Container<float, FileType::Csv, 3>,
  Container<double, FileType::Csv, 3>,
  Container<float, FileType::Csv, 4>,
  Container<double, FileType::Csv, 4>
> MyTypes;

TYPED_TEST_CASE(WriteChannelsTest, MyTypes);


TYPED_TEST(WriteChannelsTest, WriteXArraysReadComplexArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXArraysReadComplexArrayAndChannel" + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 2 * nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 66.6, 11.1));
  }
  
  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, iqValues.size(), "MOSAIK DAI", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file; 

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NE(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadata;
  ret = file->getMetadata(channelInfosRead, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadata.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadata.size(), 8) << "Unexpected number of metadata entries";
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadata.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadata[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string IName = "Kanal" + to_string(channel) + "_I";
    string QName = "Kanal" + to_string(channel) + "_Q";

    auto countIValues = file->getArraySize(IName);
    vector<typename TypeParam::Dt> IValues(countIValues);
    ret = file->readArray(IName, IValues, countIValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(IValues, iqValues[2 * channel]);

    auto countQValues = file->getArraySize(QName);
    vector<typename TypeParam::Dt> QValues(countQValues);
    ret = file->readArray(QName, QValues, countQValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(QValues, iqValues[2 * channel + 1]);

    string chName = "Kanal" + to_string(channel);

    auto countValues = file->getArraySize(chName + "_I") * 2;
    vector<typename TypeParam::Dt> values(countValues);
    ret = file->readChannel(chName, values, countValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    for (auto i = 0; i < countValues / 2; ++i)
    {
      ASSERT_NEAR(values[i * 2], iqValues[channel * 2][i], 0.0001);
      ASSERT_NEAR(values[i * 2 + 1], iqValues[channel * 2 + 1][i], 0.0001);
    }
  }

  file->close();
  delete file;

  remove(filename.c_str());
}

TYPED_TEST(WriteChannelsTest, WriteXChannelsReadComplexArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXChannelsReadComplexArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 66.6, 11.1));
  }

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, iqValues.size(), "MOSAIK DAI", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file; 

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NE(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadata;
  ret = file->getMetadata(channelInfosRead, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadata.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadata.size(), 8) << "Unexpected number of metadata entries";
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadata.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadata[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string IName = "Kanal" + to_string(channel) + "_I";
    string QName = "Kanal" + to_string(channel) + "_Q";

    auto countIValues = file->getArraySize(IName);
    vector<typename TypeParam::Dt> IValues(countIValues);
    ret = file->readArray(IName, IValues, countIValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    auto countQValues = file->getArraySize(QName);
    vector<typename TypeParam::Dt> QValues(countQValues);
    ret = file->readArray(QName, QValues, countQValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    for (auto i = 0; i < countIValues; ++i)
    {
      ASSERT_NEAR(IValues[i], iqValues[channel][2*i], 0.0001);
      ASSERT_NEAR(QValues[i], iqValues[channel][2*i + 1], 0.0001);
    }

    string chName = "Kanal" + to_string(channel);

    auto countValues = file->getArraySize(chName + "_I") * 2;
    vector<typename TypeParam::Dt> values(countValues);
    ret = file->readChannel(chName, values, countValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(values, iqValues[channel]);
  }

  file->close();
  delete file;

  remove(filename.c_str());
}

TYPED_TEST(WriteChannelsTest, WriteXArraysReadPolarArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXArraysReadPolarArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 2 * nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 66.6, 11.1));
  }

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Polar, iqValues.size(), "MOSAIK DAI", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file; 

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NE(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadata;
  ret = file->getMetadata(channelInfosRead, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadata.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadata.size(), 8) << "Unexpected number of metadata entries";
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadata.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadata[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string IName = "Kanal" + to_string(channel) + "_I";
    string QName = "Kanal" + to_string(channel) + "_Q";

    auto countIValues = file->getArraySize(IName);
    vector<typename TypeParam::Dt> IValues(countIValues);
    ret = file->readArray(IName, IValues, countIValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(IValues, iqValues[2 * channel]);

    auto countQValues = file->getArraySize(QName);
    vector<typename TypeParam::Dt> QValues(countQValues);
    ret = file->readArray(QName, QValues, countQValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(QValues, iqValues[2 * channel + 1]);

    string chName = "Kanal" + to_string(channel);

    auto countValues = file->getArraySize(chName + "_I") * 2;
    vector<typename TypeParam::Dt> values(countValues);
    ret = file->readChannel(chName, values, countValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    for (auto i = 0; i < countValues / 2; ++i)
    {
      ASSERT_NEAR(values[i * 2], iqValues[channel * 2][i], 0.0001);
      ASSERT_NEAR(values[i * 2 + 1], iqValues[channel * 2 + 1][i], 0.0001);
    }
  }

  file->close();
  delete file;
  

  remove(filename.c_str());
}

TYPED_TEST(WriteChannelsTest, WriteXChannelsReadPolarArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXChannelsReadPolarArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("Kanal" + to_string(i), 66.6, 11.1));
  }

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, iqValues.size(), "MOSAIK DAI", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file; 

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NE(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadata;
  ret = file->getMetadata(channelInfosRead, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadata.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadata.size(), 8) << "Unexpected number of metadata entries";
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadata.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadata[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string IName = "Kanal" + to_string(channel) + "_I";
    string QName = "Kanal" + to_string(channel) + "_Q";

    auto countIValues = file->getArraySize(IName);
    vector<typename TypeParam::Dt> IValues(countIValues);
    ret = file->readArray(IName, IValues, countIValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    auto countQValues = file->getArraySize(QName);
    vector<typename TypeParam::Dt> QValues(countQValues);
    ret = file->readArray(QName, QValues, countQValues);
    ASSERT_EQ(ret, ErrorCodes::Success);

    for (auto i = 0; i < countIValues; ++i)
    {
      ASSERT_NEAR(IValues[i], iqValues[channel][2*i], 0.0001);
      ASSERT_NEAR(QValues[i], iqValues[channel][2*i + 1], 0.0001);
    }

    string chName = "Kanal" + to_string(channel);

    auto countValues = file->getArraySize(chName + "_I") * 2;
    vector<typename TypeParam::Dt> values(countValues);
    ret = file->readChannel(chName, values, countValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(values, iqValues[channel]);
  }

  file->close();
  delete file;

  remove(filename.c_str());
}

TYPED_TEST(WriteChannelsTest, WriteXChannelRealReadArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXChannelRealReadArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("RealerKanal" + to_string(i), 66.6, 11.1));
  }

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Real, nofChannels, "App", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = file->getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), nofChannels);
  
  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadataRead.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadataRead.size(), 8) << "Unexpected number of metadata entries";
  }
  
  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string chName = channelInfosRead[channel].getChannelName();

    long countKValues = file->getArraySize(chName);
    vector<typename TypeParam::Dt> readValues(countKValues);

    // read as array
    ret = file->readArray(chName, readValues, countKValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readValues, iqValues[channel]);

    // read as channel
    ret = file->readChannel(chName, readValues, countKValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readValues, iqValues[channel]);
  }

  file ->close();
  delete file;

  remove(filename.c_str());
}

TYPED_TEST(WriteChannelsTest, WriteXArrayRealReadArrayAndChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "WriteXArrayRealReadArrayAndChannel." + FileTypeService::getFileExtension(TypeParam::Ft);
  const int nofChannels = TypeParam::writtenofChannels;

  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, nofChannels, 6);

  vector<ChannelInfo> channelInfos;
  for (auto i = 0; i < nofChannels; ++i)
  {
    channelInfos.push_back(ChannelInfo("RealerKanal" + to_string(i), 66.6, 11.1));
  }

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Real, nofChannels, "App", "Comment", channelInfos, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  file = FileTypeService::create(filename, TypeParam::Ft);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), nofChannels);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = file->getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), nofChannels);

  //5 standard entries + 3 user specific
  if (fileType == FileType::Matlab4 || fileType == FileType::Matlab73 || fileType == FileType::Csv) // no scaling factor
  {
    ASSERT_EQ(metadataRead.size(), 7) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(metadataRead.size(), 8) << "Unexpected number of metadata entries";
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfos.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfos[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfos[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfos[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  for (auto channel = 0; channel < nofChannels; ++channel)
  {
    string chName = channelInfosRead[channel].getChannelName();

    long countKValues = file->getArraySize(chName);
    vector<typename TypeParam::Dt> readValues(countKValues);

    // read as array
    ret = file->readArray(chName, readValues, countKValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readValues, iqValues[channel]);

    // read as channel
    ret = file->readChannel(chName, readValues, countKValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readValues, iqValues[channel]);
  }

  file->close();
  delete file;

  remove(filename.c_str());
}
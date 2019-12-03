#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "dataimportexport.h"
#include "constants.h"
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
class WriteTest : public ::testing::Test
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

TYPED_TEST_CASE(WriteTest, MyTypes);


TYPED_TEST(WriteTest, WriteOneArrayPolarFromDrehzeigerPolar)
{
  FileType fileType = TypeParam::Ft;
  const string drehzeiger = Common::TestDataDir + "DrehzeigerPolar.dat";
  const string filename = Common::TestOutputDir + "WriteOneChannelPolarFromDrehzeigerPolar." + FileTypeService::getFileExtension(TypeParam::Ft);

  vector<vector<double>> iqdata;
  vector<double> ivalues;
  vector<double> qvalues;
  ivalues.reserve(3142);
  qvalues.reserve(3142);
  iqdata.push_back(ivalues);
  iqdata.push_back(qvalues);

  ifstream infile;
  infile.open(drehzeiger.c_str());
  string str;
  int i = 0; 
  while (getline(infile, str, ' '))
  {
    iqdata[i++ % 2].push_back(stod(str));
  }
  
  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 133.0, 155.0));

  auto file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  int returnCode = file->writeOpen(IqDataFormat::Polar, 2, "Mosaik DataImportExport", "comment", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = file->appendArrays(iqdata);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = file->close();
  ASSERT_EQ(returnCode, ErrorCodes::Success);

  delete file;

  file = FileTypeService::create(filename, TypeParam::Ft);

  vector<string> arrayNames;
  returnCode = file->readOpen(arrayNames);
  ASSERT_EQ(arrayNames.size(), 2);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  returnCode = file->getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size());

  //5 standard entries + 3 user specific
  if (fileType != FileType::Matlab4 && fileType != FileType::Matlab73 && fileType != FileType::Csv)
  {
    ASSERT_EQ(metadataRead.size(), 8);
  }
  else
  {
    // no scaling factor
    ASSERT_EQ(metadataRead.size(), 7);
  }

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }
  
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlFormat].c_str(), IqDataFormatNames[IqDataFormat::Polar]);
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float64]);

  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfosWrite[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfosWrite[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfosWrite[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  file->close();
  delete file;
  remove(filename.c_str());
}

// iqtar without tmp file fails on jenkins with varying error messages...
// currently no bug detected, therefore switched to manually tests...

TYPED_TEST(WriteTest, DISABLED_WriteComplexArrayWithoutTmpFile)
{
  FileType fileType = TypeParam::Ft;
  if (fileType != FileType::Iqtar)
  {
    return;
  }

  const string filename = Common::TestOutputDir + "WriteComplexArrayWithoutTmpFile.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel1", 13, 2));

  size_t nofValuesToWrite = 2222;
  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 2, nofValuesToWrite);

  IqTar file(filename);

  int ret = 0;
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ret = file.disableTempFile(2 * nofValuesToWrite, 1, IqDataFormat::Complex, IqDataType::Float32);
  }
  else
  {
    ret = file.disableTempFile(2 * nofValuesToWrite, 1, IqDataFormat::Complex, IqDataType::Float64);
  }
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Complex, 2, "application", "comment", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // read file

  IqTar readFile(filename);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(arrayNames.size(), 2);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size());

  //5 standard entries + 3 user specific
  ASSERT_EQ(metadataRead.size(), 8);

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }
  
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlFormat].c_str(), IqDataFormatNames[IqDataFormat::Complex]);
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float32]);
  }
  else 
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float64]);
  }
  
  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfosWrite[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfosWrite[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfosWrite[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  auto size = readFile.getArraySize("Channel1_I") / 2;
  vector<typename TypeParam::Dt> readValues(size);

  // comare I values
  ret = readFile.readArray("Channel1_I", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  // comare I values offset
  ret = readFile.readArray("Channel1_I", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  // compare Q values
  ret = readFile.readArray("Channel1_Q", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[1], readValues);

  // comare Q values offset
  ret = readFile.readArray("Channel1_Q", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[1], readValues);

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TYPED_TEST(WriteTest, DISABLED_WriteRealArrayWithoutTmpFile)
{
  FileType fileType = TypeParam::Ft;
  if (fileType != FileType::Iqtar)
  {
    return;
  }

  const string filename = Common::TestOutputDir + "WriteComplexArrayWithoutTmpFile.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel1", 13, 2));

  size_t nofValuesToWrite = 2222;
  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 1, nofValuesToWrite);

  IqTar file(filename);

  int ret = 0;
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ret = file.disableTempFile(nofValuesToWrite, 1, IqDataFormat::Real, IqDataType::Float32);
  }
  else
  {
    ret = file.disableTempFile(nofValuesToWrite, 1, IqDataFormat::Real, IqDataType::Float64);
  }
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Real, 1, "application", "comment", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // read file

  IqTar readFile(filename);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(arrayNames.size(), 1);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size());

  //5 standard entries + 3 user specific
  ASSERT_EQ(metadataRead.size(), 8);

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }
  
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlFormat].c_str(), IqDataFormatNames[IqDataFormat::Real]);
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float32]);
  }
  else 
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float64]);
  }
  
  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfosWrite[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfosWrite[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfosWrite[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  auto size = readFile.getArraySize("Channel1");
  vector<typename TypeParam::Dt> readValues(size);

  // comare values
  ret = readFile.readArray("Channel1", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TYPED_TEST(WriteTest, WriteComplexChannelWithoutTmpFile)
{
  FileType fileType = TypeParam::Ft;
  if (fileType != FileType::Iqtar)
  {
    return;
  }

  const string filename = Common::TestOutputDir + "WriteComplexChannelWithoutTmpFile.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel1", 13, 2));
  channelInfosWrite.push_back(ChannelInfo("Channel2", 13, 2));

  size_t nofValuesToWrite = 2222;
  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 4, nofValuesToWrite);

  IqTar file(filename);

  int ret = 0;
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ret = file.disableTempFile(2 * nofValuesToWrite, 2, IqDataFormat::Complex, IqDataType::Float32);
  }
  else
  {
    ret = file.disableTempFile(2 * nofValuesToWrite, 2, IqDataFormat::Complex, IqDataType::Float64);
  }
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Complex, 4, "application", "comment", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // read file

  IqTar readFile(filename);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(arrayNames.size(), 4);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size());

  //5 standard entries + 3 user specific
  ASSERT_EQ(metadataRead.size(), 8);

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }
  
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlFormat].c_str(), IqDataFormatNames[IqDataFormat::Complex]);
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float32]);
  }
  else 
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float64]);
  }
  
  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfosWrite[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfosWrite[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfosWrite[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  auto size = readFile.getArraySize("Channel1_I") / 2;
  vector<typename TypeParam::Dt> readValues(size);

  // comare I values
  ret = readFile.readArray("Channel1_I", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  // comare I values offset
  ret = readFile.readArray("Channel1_I", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  // compare Q values
  ret = readFile.readArray("Channel1_Q", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[1], readValues);

  // comare Q values offset
  ret = readFile.readArray("Channel1_Q", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[1], readValues);

  size = readFile.getArraySize("Channel2_I") / 2;
  readValues.resize(size);

  // comare I values
  ret = readFile.readArray("Channel2_I", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[2], readValues);

  // comare I values offset
  ret = readFile.readArray("Channel2_I", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[2], readValues);

  // compare Q values
  ret = readFile.readArray("Channel2_Q", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[3], readValues);

  // comare Q values offset
  ret = readFile.readArray("Channel2_Q", readValues, size, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[3], readValues);

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TYPED_TEST(WriteTest, DISABLED_WriteRealChannelWithoutTmpFile)
{
  FileType fileType = TypeParam::Ft;
  if (fileType != FileType::Iqtar)
  {
    return;
  }

  const string filename = Common::TestOutputDir + "WriteRealChannelWithoutTmpFile.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel1", 13, 2));

  size_t nofValuesToWrite = 2222;
  vector<vector<typename TypeParam::Dt>> iqValues;
  Common::initVector(iqValues, 1, nofValuesToWrite);

  IqTar file(filename);

  int ret = 0;
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ret = file.disableTempFile(nofValuesToWrite, 1, IqDataFormat::Real, IqDataType::Float32);
  }
  else
  {
    ret = file.disableTempFile(nofValuesToWrite, 1, IqDataFormat::Real, IqDataType::Float64);
  }
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Real, 1, "application", "comment", channelInfosWrite, &this->metaDataWrite_);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // read file

  IqTar readFile(filename);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(arrayNames.size(), 1);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size());

  //5 standard entries + 3 user specific
  ASSERT_EQ(metadataRead.size(), 8);

  //check metadata if it contains the user specific data
  for (auto pair : this->metaDataWrite_)
  {
    ASSERT_EQ(metadataRead.count(pair.first), 1) << "user specific key not contained in written file";
    ASSERT_EQ(pair.second, metadataRead[pair.first]) << "values of same key are not equal";
  }
  
  ASSERT_STRCASEEQ(metadataRead[Constants::XmlFormat].c_str(), IqDataFormatNames[IqDataFormat::Real]);
  if (typeid(typename TypeParam::Dt) == typeid(float))
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float32]);
  }
  else 
  {
    ASSERT_STRCASEEQ(metadataRead[Constants::XmlDataType].c_str(), IqDataTypeNames[IqDataType::Float64]);
  }
  
  //check channel infos
  ASSERT_EQ(channelInfosRead.size(), channelInfosWrite.size()) << "Invalid number of channels read";
  for (auto idx = 0; idx < channelInfosRead.size(); idx++)
  {
    ASSERT_EQ(channelInfosRead[idx].getChannelName(), channelInfosWrite[idx].getChannelName()) << "Unexpected channel name read from file";
    ASSERT_NEAR(channelInfosRead[idx].getClockRate(), channelInfosWrite[idx].getClockRate(), 0.5) << "Unexpected clock rate read from file";
    ASSERT_NEAR(channelInfosRead[idx].getFrequency(), channelInfosWrite[idx].getFrequency(), 0.5) << "Unexpected center frequency read from file";
  }

  auto size = readFile.getArraySize("Channel1");
  vector<typename TypeParam::Dt> readValues(size);

  // comare values
  ret = readFile.readChannel("Channel1", readValues, size);
  ASSERT_EQ(ret, ErrorCodes::Success);
  Common::almostEqual(iqValues[0], readValues);

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}


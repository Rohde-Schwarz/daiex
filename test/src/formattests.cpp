#include "gtest/gtest.h"

#include <iostream>
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
class FormatTests : public ::testing::Test
{
};

typedef ::testing::Types<
  Container<float, FileType::IQW>, 
  Container<double, FileType::IQW>,
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
TYPED_TEST_CASE(FormatTests, MyTypes);


TYPED_TEST(FormatTests, AccessViaInterface)
{
  FileType fileType = TypeParam::Ft;

  string ext = FileTypeService::getFileExtension(fileType);
  const string filename = Common::TestOutputDir + "AccessViaInterfaceSingle_ViaInterface." + ext;

  auto file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }
  
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("ChannelInfo", 12, 12));
  
  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 6);

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->appendArrays(data);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  const string filename2 = Common::TestOutputDir + "AccessViaInterfaceSingle." + ext;

  switch (fileType)
  {
  case rohdeschwarz::mosaik::dataimportexport::FileType::Iqtar:
    {
      IqTar fileNonI(filename2);
      ret = fileNonI.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.appendArrays(data);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.close();
      ASSERT_EQ(ErrorCodes::Success, ret);
      break;
    }
  case rohdeschwarz::mosaik::dataimportexport::FileType::IQW:
    {
      Iqw fileNonI(filename2);
      ret = fileNonI.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.appendArrays(data);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.close();
      ASSERT_EQ(ErrorCodes::Success, ret);
      break;
    }
  case rohdeschwarz::mosaik::dataimportexport::FileType::Matlab4:
    {
      IqMatlab fileNonI(filename2);
      fileNonI.setMatlabVersion(MatlabVersion::Mat4);
      ret = fileNonI.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.appendArrays(data);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.close();
      ASSERT_EQ(ErrorCodes::Success, ret);
      break;
    }
  case rohdeschwarz::mosaik::dataimportexport::FileType::Matlab73:
    {
      IqMatlab fileNonI(filename2);
      fileNonI.setMatlabVersion(MatlabVersion::Mat73);
      ret = fileNonI.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.appendArrays(data);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.close();
      ASSERT_EQ(ErrorCodes::Success, ret);
      break;
    }
  case rohdeschwarz::mosaik::dataimportexport::Csv:
    {
      IqCsv fileNonI(filename2);
      ret = fileNonI.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.appendArrays(data);
      ASSERT_EQ(ErrorCodes::Success, ret);
      ret = fileNonI.close();
      ASSERT_EQ(ErrorCodes::Success, ret);
      break;
    }
  default:
    break;
  }

  // bin compare
  bool b = Common::binCompareFiles(filename, filename2);
  ASSERT_TRUE(b);

  // Open files and compare content read with vectors written
  file = FileTypeService::create(filename, fileType);
  
  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";

  // read arrays
  for (size_t i = 0; i < arrayNames.size(); ++i)
  {
    long size = file->getArraySize(arrayNames[i]);
    vector<typename TypeParam::Dt> readData(size);

    ret = file->readArray(arrayNames[i], readData, size);
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readData, data[i]);
  }

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  remove(filename.c_str());
  remove(filename2.c_str());
}

TYPED_TEST(FormatTests, ReadWriteArray)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "ReadWriteArray." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 6);
  vector<vector<typename TypeParam::Dt>> data2;
  Common::initVector(data2, 2, 6, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->appendArrays(data);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->appendArrays(data2);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  // read
  IDataImportExport* readFile = FileTypeService::create(filename, TypeParam::Ft);

  vector<string> arrayNames;
  ret = readFile->readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile->getArraySize("Channel1_I") / 2;
  vector<typename TypeParam::Dt> readValues(size);

  // compare I values 
  ret = readFile->readArray("Channel1_I", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[0], readValues);

  // compare I values offset
  ret = readFile->readArray("Channel1_I", readValues, size, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[0], readValues);

  // compare Q values 
  ret = readFile->readArray("Channel1_Q", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[1], readValues);

  // compare Q values offset
  ret = readFile->readArray("Channel1_Q", readValues, size, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[1], readValues);

  ret = readFile->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete readFile;

  remove(filename.c_str());
}

TYPED_TEST(FormatTests, ReadWriteChannel)
{
  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "ReadWriteChannel." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 1, 6);
  vector<vector<typename TypeParam::Dt>> data2;
  Common::initVector(data2, 1, 6, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, 1, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->appendChannels(data);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->appendChannels(data2);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  // read
  IDataImportExport* readFile = FileTypeService::create(filename, TypeParam::Ft);

  vector<string> arrayNames;
  ret = readFile->readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile->getArraySize("Channel1_I");
  vector<typename TypeParam::Dt> readValues(size);

  // compare I/Q values
  ret = readFile->readChannel("Channel1", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteChannel failed";
  Common::almostEqual(data[0], readValues);

  // compare I/Q values offset
  ret = readFile->readChannel("Channel1", readValues, size, 3);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteChannel failed";
  Common::almostEqual(data2[0], readValues);

  ret = readFile->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete readFile;

  remove(filename.c_str());
}

TYPED_TEST(FormatTests, ChangeDataType)
{
  // IQW format -> will always cast to float values
  if (TypeParam::Ft == FileType::IQW ||
    TypeParam::Ft == FileType::Matlab4 ||
    TypeParam::Ft == FileType::Matlab73)
  {
    return;
  }

  FileType fileType = TypeParam::Ft;
  const string filename = Common::TestOutputDir + "ChangeDataType." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "App name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->appendArrays(data);
  ASSERT_EQ(ErrorCodes::Success, ret);

  if (is_same<typename TypeParam::Dt, float>::value)
  {
    vector<vector<double>> ddata;
    Common::initVector(ddata, 2, 6);

    ret = file->appendArrays(ddata);
    ASSERT_EQ(ret, ErrorCodes::WrongDataType);
  }
  else if (is_same<typename TypeParam::Dt, double>::value)
  {
    vector<vector<float>> fdata;
    Common::initVector(fdata, 2, 6);

    ret = file->appendArrays(fdata);
    ASSERT_EQ(ret, ErrorCodes::WrongDataType);
  }
  else
  {
    ASSERT_TRUE(false) << "unexpected data type.";
  }

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  remove(filename.c_str());
}

TYPED_TEST(FormatTests, CloseWithoutWritingData)
{
  FileType fileType = TypeParam::Ft;

  const string filename = Common::TestOutputDir + "CloseWithoutWritingData." + FileTypeService::getFileExtension(fileType);
  vector<ChannelInfo> channelsInput;
  channelsInput.push_back(ChannelInfo("Channel1", 12, 13));
  
  IDataImportExport* file = FileTypeService::create(filename, fileType);
  
  // open for writing
  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "appl", "comment", channelsInput);
  ASSERT_EQ(ret, ErrorCodes::Success);

  // close immediately
  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(FormatTests, WriteAndReadAgainSpecialChars)
{
  FileType fileType = TypeParam::Ft;
  if (fileType == FileType::IQW) // no meta data
  {
    return;
  }

  const string filename = Common::TestOutputDir + "WriteAndReadAgainSpecialChars." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  const char * specialchars = "\xE4\xB8\xAD\xE5\x9B\xBD\xE8\xAA\x9E";
  const char * specialchars2 = "(\xE7\xB9\x81\xE4\xBD\x93)";

  vector<ChannelInfo> channelsInput;
  channelsInput.push_back(ChannelInfo(specialchars, 20000, 3333333.0 ));

  map<string, string> metadata;
  metadata.insert( make_pair(specialchars, specialchars2) );

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  vector<vector<float>> data;
  Common::initVector(data, 2, 6);

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, specialchars, specialchars2, channelsInput, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file->appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);
  delete file;

  vector<string> arrayNames;
  file = FileTypeService::create(filename, TypeParam::Ft);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), 2);

  // check meta data
  vector<ChannelInfo> channels;
  map<string, string> metadataRead;

  ret = file->getMetadata(channels, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  
  map<string, string>::iterator it;
  it = metadataRead.find(specialchars);
  if( it != metadataRead.end() )
  {
    string sValue = specialchars2;
    ASSERT_EQ(specialchars2, it->second) << "Custom metadata could not be retrieved from file.";
  }
  else
  {
    ASSERT_TRUE(false) << "Custom metadata could not be retrieved from file.";
  }

  auto nofIValues = file->getArraySize(arrayNames[0]);
  ASSERT_EQ(6, nofIValues);
  vector<float> iValues(nofIValues);
  ret = file->readArray(arrayNames[0], iValues, nofIValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  auto nofQValues = file->getArraySize(arrayNames[1]);
  ASSERT_EQ(nofIValues, nofQValues);
  vector<float>qValues(nofQValues);
  ret = file->readArray(arrayNames[1], qValues, nofQValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  Common::almostEqual(data[0], iValues);
  Common::almostEqual(data[1], qValues);

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  remove(filename.c_str());
}

TYPED_TEST(FormatTests, CreateSpecialCharFilename)
{
  FileType fileType = TypeParam::Ft;

  const char* specialchars = "\xD0\xB7\xD0\xB0\xD0\xBF\xD0\xB8\xD1\x81\xD0\xB0\xD0\xBD\xD0\xBE";
  string ext = FileTypeService::getFileExtension(fileType);
  const string filename = Common::TestOutputDir + specialchars + "." + ext;

#if defined(_WIN32)
  _wremove(Common::utf8toUtf16(filename).c_str());
#else
  remove(filename.c_str());
#endif

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 5);
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("My Channel", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, fileType);

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "app name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);
  delete file;
  
  bool fileExists = Common::isFileAccessible(filename);
  ASSERT_TRUE(fileExists);

#if defined(_WIN32)
    _wremove(Common::utf8toUtf16(filename).c_str());
#else
    remove(filename.c_str());
#endif
}

TYPED_TEST(FormatTests, SpecialCharFilename)
{
  FileType fileType = TypeParam::Ft;

  const char* specialchars = "\xD0\xB7\xD0\xB0\xD0\xBF\xD0\xB8\xD1\x81\xD0\xB0\xD0\xBD\xD0\xBE";
  string ext = FileTypeService::getFileExtension(fileType);
  const string filename = Common::TestDataDir + specialchars + "." + ext;

  IDataImportExport* file = FileTypeService::create(filename, fileType);
  
  vector<string> names;
  auto ret = file->readOpen(names);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_NE(0, names.size());

  bool fileExists = Common::isFileAccessible(filename);
  ASSERT_EQ(true, fileExists);

  file->close();
  delete file;
}

TYPED_TEST(FormatTests, DateTimeWriteRead)
{
  FileType fileType = TypeParam::Ft;

  // no meta data
  if (fileType == FileType::IQW)
  {
    return;
  }

  const string filename = Common::TestOutputDir + "DateTimeWriteRead." + FileTypeService::getFileExtension(fileType);
  
  vector<ChannelInfo> channelInfo;
  channelInfo.push_back(ChannelInfo("Channel1", 12, 12));

  time_t time_now = time(0);

  vector<vector<typename TypeParam::Dt>> iqdata;
  Common::initVector(iqdata, 2, 5);

  // write
  IDataImportExport* file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);

  file->setTimestamp(time_now);
  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfo);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file->appendArrays(iqdata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  auto time_read = file->getTimestamp();
  ASSERT_EQ(time_now, time_read);
  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  // read again
  file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  ret = file->readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  auto time_saved = file->getTimestamp();

  ASSERT_EQ(time_now, time_saved);
  ret = file->close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete file;

  remove(filename.c_str());
}
#include "gtest/gtest.h"

#include "dataimportexport.h"
#include "common.h"

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>

#ifdef _WIN32
#define isfinite(x) _finite(x)
#endif

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class IqwTest : public ::testing::Test
{
};

template<typename T, IqDataOrder order>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const IqDataOrder Order = order; 
};

template<typename T>
class IqwDataTypeTest : public ::testing::Test
{
};

typedef ::testing::Types<float, double> MyDataTypes;
TYPED_TEST_CASE(IqwDataTypeTest, MyDataTypes);

template<typename T>
class IqwDataOrderTest : public ::testing::Test
{
};

typedef ::testing::Types<
  Container<float, IqDataOrder::IIIQQQ>,
  Container<double, IqDataOrder::IIIQQQ>,
  Container<float, IqDataOrder::IQIQIQ>,
  Container<double, IqDataOrder::IQIQIQ>
> MyTypes;
TYPED_TEST_CASE(IqwDataOrderTest, MyTypes);

TEST_F(IqwTest, EmptyFile)
{
  const string input = Common::TestOutputDir + "empty_file.iqw";

  // create file
  ofstream o(input.c_str());
  o.close();

  Iqw file(input);

  vector<string> arrayNames;
  int ret = file.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::NoDataFoundInFile, ret) << "file open failed";
  EXPECT_EQ(arrayNames.size(), 0) << "As file is empty no arrays should be found";
  
  remove(input.c_str());
}

TEST_F(IqwTest, CheckTempDir)
{
  const string filename = Common::TestOutputDir + "CheckDefaultTempDir.iqw";
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

TEST_F(IqwTest, ChangeTempDirAfterWriterOpen)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.iqw";
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

TEST_F(IqwTest, UnknownTempPath)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.iqw";
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

TEST_F(IqwTest, LockDataOrder)
{
  const string input = Common::TestDataDir + "FromFSW_1ch_asiqw.iqw";

  Iqw readFile(input);

  vector<string> arrayNames;
  auto ret = readFile.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret);
  ret = readFile.setDataOrder(IqDataOrder::IQIQIQ);
  EXPECT_EQ(ErrorCodes::ReaderAlreadyInitialized, ret);
  readFile.close();

  const string outFile = Common::TestOutputDir + "LockDataOrder.iqw";

  Iqw writeFile(outFile);
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("", 12, 12));
  ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  EXPECT_EQ(ErrorCodes::Success, ret);
  ret = writeFile.setDataOrder(IQIQIQ);
  EXPECT_EQ(ErrorCodes::WriterAlreadyInitialized, ret);
  writeFile.close();

  remove(outFile.c_str());
}

TEST_F(IqwTest, ReadLegacyFile)
{
  const string input = Common::TestDataDir + "FromFSW_1ch_asiqw.iqw";
  Iqw file(input);

  vector<string> arrayNames;
  unsigned int ret = file.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret);
  EXPECT_EQ(arrayNames.size(), 2) << "1 channel I and Q expected";
  EXPECT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  EXPECT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  long nofIValues = file.getArraySize(arrayNames[0]);
  vector<float> ivalues; 
  ret = file.readArray(arrayNames[0], ivalues, nofIValues);
  EXPECT_EQ(ErrorCodes::Success, ret);

  long nofQValues = file.getArraySize(arrayNames[1]);
  EXPECT_EQ(nofIValues, nofQValues) << "I And Q array must have equal length";

  vector<double> qvalues;
  ret = file.readArray(arrayNames[1], qvalues, nofQValues);
  EXPECT_EQ(ErrorCodes::Success, ret);

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = file.getMetadata(channels, metadata);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
  EXPECT_TRUE(!isfinite(channels[0].getClockRate())) << "Unexpected clock rate read from file";
  EXPECT_TRUE(!isfinite(channels[0].getFrequency())) << "Unexpected center frequency read from file";
}

TYPED_TEST(IqwDataTypeTest, ReadDataFromLegacyIqw)
{
  // this test verifies reading IQW files - the same data has been saved as iq.tar and iqw file
  // by RSFW implementation - both files are read and the data will be compared.

  const string tarfile = Common::TestDataDir + "FromFSW_1ch_WithPreview.iq.tar";

  vector<string> arrayNames;
  IqTar readFile(tarfile);
  auto returnCode = readFile.readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.OpenFile failed";
  ASSERT_EQ(2, arrayNames.size()) << "1 channel I And Q expected";
  
  long countIValues = readFile.getArraySize(arrayNames[0]);
  vector<TypeParam> IValues(countIValues);
  returnCode = readFile.readArray(arrayNames[0], IValues, countIValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.GetArrayAsSingle failed";

  long countQValues = readFile.getArraySize(arrayNames[1]);
  ASSERT_EQ(countQValues, countIValues) << "Values arrays of one channel must have equal length";
  vector<TypeParam> QValues(countQValues);
  returnCode = readFile.readArray(arrayNames[1], QValues, countQValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.GetArrayAsSingle failed";

  // Read data from iqw file saved by RSFW.FileIO.Generic_IO.Write

  const string iqwfile = Common::TestDataDir +  "FromFSW_1ch_asiqw.iqw";

  Iqw readFile2(iqwfile);
  readFile2.setDataOrder(IqDataOrder::IIIQQQ);
  returnCode = readFile2.readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.OpenFile failed";
  ASSERT_EQ(arrayNames.size(), 2) <<  "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  countIValues = readFile2.getArraySize(arrayNames[0]);
  vector<TypeParam> IValuesIQW(countIValues);
  returnCode = readFile2.readArray(arrayNames[0], IValuesIQW, countIValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.GetArrayAsSingle failed";

  countQValues = readFile2.getArraySize(arrayNames[1]);
  vector<TypeParam> QValuesIQW(countQValues);
  returnCode = readFile2.readArray(arrayNames[1], QValuesIQW, countQValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.GetArrayAsSingle failed";

  Common::almostEqual(IValues, IValuesIQW);
  Common::almostEqual(QValues, QValuesIQW);
}

TEST_F(IqwTest, VerifyMetadataIQW)
{
  // An IQW file does not contain meta data - but data type and format are returned as meta data
  // just as an IQTAR file would do.
  const string input = Common::TestDataDir + "FromFSW_1ch_asiqw.iqw";
  Iqw file(input);

  vector<string> arrayNames;
  unsigned int ret = file.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  EXPECT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  EXPECT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name I";
  EXPECT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name Q";

  vector<ChannelInfo> info;
  map<string, string> meta;
  ret = file.getMetadata(info, meta);
  EXPECT_EQ(ErrorCodes::Success, ret) << "reading meta data failed";
  EXPECT_EQ(info.size(), 1) << "Unexpected number of channels read from file";
  EXPECT_EQ(info[0].getChannelName(), ChannelInfo::getDefaultChannelName(0)) << "Unexpected channel name read from file";
  EXPECT_TRUE(!(info[0].getClockRate() <= numeric_limits<double>::quiet_NaN())) << "Unexpected clock rate read from file";
  EXPECT_TRUE(!(info[0].getFrequency() <= numeric_limits<double>::quiet_NaN())) << "Unexpected center frequency read from file";

  EXPECT_EQ(meta.size(), 2) << "Unexpected number of meta data entries";
  
  // Check for mandatory data
  for (const pair<string, string> &e : meta)
  {
    if (0 == e.first.compare("Format"))
    {
      EXPECT_EQ(0, e.second.compare("complex")) << "Format read from file is incorrect";
    }
    else if (0 == e.first.compare("DataType"))
    {
      EXPECT_EQ(0, e.second.compare("float32")) << "DataType read from file is incorrect";
    }
    else
    {
      EXPECT_TRUE(false) << "Unexpected metadata key: " + e.first;
    }
  }
}

TEST_F(IqwTest, WrongFormat)
{
  const string out = Common::TestOutputDir + "TestWriteSingleData.iqw";

  vector<vector<float>> data;
  vector<ChannelInfo> channelInfos;

  Iqw file(out);
  int ret = file.writeOpen(IqDataFormat::Polar, 2, "", "", channelInfos);
  EXPECT_EQ(ErrorCodes::InvalidDataFormat, ret) << "IQW.WriteIQDataDouble failed";

  remove(out.c_str());
}

TEST_F(IqwTest, GetSetDataOrder)
{
  const string out = Common::TestOutputDir + "GetSetDataOrder.iqw";
  const string out2 = Common::TestOutputDir + "GetSetDataOrder2.iqw";

  vector<vector<float>> floatData;
  Common::initVector(floatData, 2, 5);

  Iqw file(out);
  auto order = file.getDataOrder();
  ASSERT_EQ(IqDataOrder::IIIQQQ, order) << "Default value of data order should be IIIQQQ";
  auto ret = file.setDataOrder(IqDataOrder::IQIQIQ);
  ASSERT_EQ(ErrorCodes::Success, ret);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("", 12, 12));
  ret = file.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file.appendArrays(floatData);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  Iqw file2(out2);
  ret = file2.setDataOrder(IqDataOrder::IQIQIQ);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file2.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file2.appendArrays(floatData);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file2.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  bool e = Common::binCompareFiles(out, out2);
  ASSERT_TRUE(e);

  remove(out.c_str());
  remove(out2.c_str());
}

TYPED_TEST(IqwDataTypeTest, ReadIQWWrongDataOrder)
{
  //This test verifys that the DataOrder specified for IQW is executed correctly
  //The IQW file that is read has been saved as IIIQQQ, but will be read as IQIQIQ
  //therefore I_IQTAR(1) should be Q_IQW(0) and
  //          I_IQTAR(3) should be Q_IQW(1) and
  //          I_IQTAR(5) should be Q_IQW(2) and

  const string tarfile = Common::TestDataDir + "FromFSW_1ch_WithPreview.iq.tar";

  vector<string> arrayNames;
  
  IqTar ReadFile(tarfile);
  auto returnCode = ReadFile.readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.OpenFile failed";
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";

  auto countIValues = ReadFile.getArraySize(arrayNames[0]);
  vector<TypeParam> IValues(countIValues);
  returnCode = ReadFile.readArray(arrayNames[0], IValues, countIValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.GetArrayAsSingle failed";

  auto countQValues = ReadFile.getArraySize(arrayNames[1]);
  vector<TypeParam> QValues(countQValues);
  returnCode = ReadFile.readArray(arrayNames[1], QValues, countQValues);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.GetArrayAsSingle failed";

  // Read data from iqw file saved by RSFW.FileIO.Generic_IO.Write

  const string iqwfile = Common::TestDataDir + "FromFSW_1ch_asiqw.iqw";
  Iqw ReadFile2(iqwfile);
  ReadFile2.setDataOrder(IqDataOrder::IQIQIQ);
  returnCode = ReadFile2.readOpen(arrayNames);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.OpenFile failed";
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto countIValuesIQW = ReadFile2.getArraySize(arrayNames[0]);
  vector<TypeParam> IValuesIQW(countIValuesIQW);
  returnCode = ReadFile2.readArray(arrayNames[0], IValuesIQW, countIValuesIQW);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.GetArrayAsSingle failed";

  auto countQValuesIQW = ReadFile2.getArraySize(arrayNames[1]);
  vector<TypeParam> QValuesIQW(countQValuesIQW);
  returnCode = ReadFile2.readArray(arrayNames[1], QValuesIQW, countQValuesIQW);
  ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQW.GetArrayAsSingle failed";

  ASSERT_EQ(IValues.size(), IValuesIQW.size()) << "unexpected length of IValues read from IQW";
  ASSERT_EQ(QValues.size(), QValuesIQW.size()) << "unexpected length of IValues read from IQW";

  // It is enough to verify the data order pattern on the first 3 Q values, the rest will be identical
  ASSERT_NEAR(QValuesIQW[0], IValues[1], 0.00001) << "DataOrder IQIQIQ has not been executed correctly";
  ASSERT_NEAR(QValuesIQW[1], IValues[3], 0.00001) << "DataOrder IQIQIQ has not been executed correctly";
  ASSERT_NEAR(QValuesIQW[2], IValues[5], 0.00001) << "DataOrder IQIQIQ has not been executed correctly";
}

TYPED_TEST(IqwDataOrderTest, CompareFilesWithSingleAndDoubleInput)
{
  IqDataOrder dataOrder = TypeParam::Order;

  // write IQW file with same data once given as single once given as double
  // and compare resulting files - should be identical, as always saved as float32.

  vector<vector<float>> dataF;
  vector<vector<double>> dataD;
  Common::initVector(dataF, 2, 5);
  Common::initVector(dataD, 2, 5);

  const string filenameF = Common::TestOutputDir + "CompareFilesWithSingleAndDoubleInputF.iqw";
  const string filenameD = Common::TestOutputDir + "CompareFilesWithSingleAndDoubleInputD.iqw";

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 12));

  Iqw fileD(filenameD);
  fileD.setDataOrder(dataOrder);

  auto ret = fileD.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = fileD.appendArrays(dataD);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = fileD.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  Iqw fileF(filenameF);
  fileF.setDataOrder(dataOrder);

  ret = fileF.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = fileF.appendArrays(dataF);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = fileF.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  // binary compare file content
  bool e = Common::binCompareFiles(filenameD, filenameF);
  ASSERT_TRUE(e);

  remove(filenameF.c_str());
  remove(filenameD.c_str());
}

TYPED_TEST(IqwDataOrderTest, ReadWriteArray)
{
  IqDataOrder dataOrder = TypeParam::Order;

  const string filename = Common::TestOutputDir + "ReadWriteArray.iqw";

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 6);
  vector<vector<typename TypeParam::Dt>> data2;
  Common::initVector(data2, 2, 6, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 12));

  Iqw file(filename);
  file.setDataOrder(dataOrder);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.appendArrays(data);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file.appendArrays(data2);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  // read
  Iqw readFile(filename);
  readFile.setDataOrder(dataOrder);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile.getArraySize("Channel1_I") / 2;
  vector<typename TypeParam::Dt> readValues(size);

  // compare I values 
  ret = readFile.readArray("Channel1_I", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[0], readValues);

  // compare I values offset
  ret = readFile.readArray("Channel1_I", readValues, size, data[0].size());
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[0], readValues);

  // compare Q values 
  ret = readFile.readArray("Channel1_Q", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[1], readValues);

  // compare Q values offset
  ret = readFile.readArray("Channel1_Q", readValues, size, data[0].size());
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[1], readValues);
  
  ret = readFile.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  remove(filename.c_str());
}

TYPED_TEST(IqwDataOrderTest, ReadWriteChannel)
{
  IqDataOrder dataOrder = TypeParam::Order;

  const string filename = Common::TestOutputDir + "ReadWriteChannel.iqw";

  vector<vector<typename TypeParam::Dt>> data;
  vector<vector<typename TypeParam::Dt>> data2;
  Common::initVector(data, 1, 200);
  Common::initVector(data2, 1, 54, 200);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 12));

  Iqw file(filename);
  file.setDataOrder(dataOrder);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.appendChannels(data);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file.appendChannels(data2);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  // read
  Iqw readFile(filename);
  readFile.setDataOrder(dataOrder);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile.getArraySize("Channel1_I") * 2;
  ASSERT_EQ(size, data[0].size() + data2[0].size());

  vector<typename TypeParam::Dt> readValues(data[0].size());

  ret = readFile.readChannel("Channel1", readValues, data[0].size());
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[0], readValues);

  readValues.resize(data2[0].size());
  ret = readFile.readChannel("Channel1", readValues, data2[0].size(), data[0].size() / 2);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[0], readValues);

  ret = readFile.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  remove(filename.c_str());
}

TYPED_TEST(IqwDataOrderTest, WriteArrayReadChannel)
{
  IqDataOrder dataOrder = TypeParam::Order;

  const string filename = Common::TestOutputDir + "WriteArrayReadChannel.iqw";

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 12));

  Iqw file(filename);
  file.setDataOrder(dataOrder);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.appendArrays(data);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  // read
  Iqw readFile(filename);
  readFile.setDataOrder(dataOrder);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile.getArraySize("Channel1_I") * 2;
  vector<typename TypeParam::Dt> readChannel(size);

  ret = readFile.readChannel("Channel1", readChannel, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadChannel failed";
  
  for (size_t i = 0; i < data[0].size(); ++i)
  {
    ASSERT_NEAR(readChannel[i * 2], data[0][i], 0.00001);
    ASSERT_NEAR(readChannel[i * 2 + 1], data[1][i], 0.00001);
  }
  
  ret = readFile.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  remove(filename.c_str());
}

TYPED_TEST(IqwDataOrderTest, WriteChannelReadArray)
{
  IqDataOrder dataOrder = TypeParam::Order;

  const string filename = Common::TestOutputDir + "ReadWriteChannel.iqw";

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 1, 6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 12));

  Iqw file(filename);
  file.setDataOrder(dataOrder);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.appendChannels(data);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  // read
  Iqw readFile(filename);
  readFile.setDataOrder(dataOrder);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile.getArraySize("Channel1_I") * 2;

  vector<typename TypeParam::Dt> readArray(data[0].size() / 2);

  ret = readFile.readArray("Channel1_I", readArray, data[0].size() / 2);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";

  for (size_t i = 0; i < data.size() / 2; i++)
  {
    ASSERT_NEAR(data[0][i*2], readArray[i], 0.00001);
  }

  ret = readFile.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  remove(filename.c_str());
}
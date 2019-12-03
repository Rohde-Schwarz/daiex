#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "archive.h"
#include "archive_entry.h"

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class IqTarTests : public ::testing::Test
{
};



TEST_F(IqTarTests, ReadDifferentFileWithSameName)
{
  const string filename = Common::TestOutputDir + "File.iq.tar";

  // write first version of file with same name
  { 
    vector<ChannelInfo> channelInfos;
    channelInfos.push_back(ChannelInfo("Channel1", 1000, 1000));
    IqTar writeFile(filename);
    auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "v1", "comment", channelInfos);
    ASSERT_EQ(ret, ErrorCodes::Success);
    vector<vector<float>> iqValues;
    Common::initVector(iqValues, 2, 100, 100);
    ret = writeFile.appendArrays(iqValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
    ret = writeFile.close();
    ASSERT_EQ(ret, ErrorCodes::Success);
  }
  // read file
  size_t samplesInFirstFile;
  IqTar readFile(filename);
  {
    vector<string> arrayNames;
    int ret = readFile.readOpen(arrayNames);
    ASSERT_EQ(ret, ErrorCodes::Success);
    vector<ChannelInfo> channelInfosRead;
    map<string, string> metadataRead;
    ret = readFile.getMetadata(channelInfosRead, metadataRead);
    ASSERT_EQ(ret, ErrorCodes::Success);
    samplesInFirstFile = channelInfosRead[0].getSamples();
    vector<float> channelValues;
    ret = readFile.readChannel(channelInfosRead[0].getChannelName(), channelValues, channelInfosRead[0].getSamples() * 2);
    ASSERT_EQ(ret, ErrorCodes::Success);
    readFile.close();
  }
  // write second version of file with same name
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 2000, 2000));
  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "v2", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 200, 200);
  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
  // read second version of file
  {
    vector<string> arrayNames;
    int ret = readFile.readOpen(arrayNames);
    ASSERT_EQ(ret, ErrorCodes::Success);
    vector<ChannelInfo> channelInfosRead;
    map<string, string> metadataRead;
    ret = readFile.getMetadata(channelInfosRead, metadataRead);
    ASSERT_EQ(ret, ErrorCodes::Success);
    ASSERT_EQ(channelInfosRead[0].getSamples(),200);
    vector<float> channelValues;
    ret = readFile.readChannel(channelInfosRead[0].getChannelName(), channelValues, channelInfosRead[0].getSamples() * 2);
    ASSERT_EQ(ret, ErrorCodes::Success);
    readFile.close();
  }
}


#if 0
TEST_F(IqTarTests, RemoveStickWhileReading)
{
  const string filename = "G:/ihavecueandtrigger.iqx.iq.tar";
  IqTar readFile(filename);
  vector<string> arrayNames;
  int ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfosRead, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  vector<float> channelValues;
  // set breakpoint here
  ret = readFile.readChannel(channelInfosRead[0].getChannelName(), channelValues, channelInfosRead[0].getSamples() * 2);
  ASSERT_EQ(ret, ErrorCodes::Success);
  size_t read = 0;
  size_t offset = 0;
  size_t toRead = channelInfosRead[0].getSamples();

  while (toRead > 0)
  {
    size_t readNow = toRead;
    if (readNow > (size_t)1E7) readNow = (size_t)1E7;
    vector<float> valuesI;
    vector<float> valuesQ;
    try
    {
      ret = readFile.readArray(channelInfosRead[0].getChannelName() + "_I", valuesI, readNow, read);
      ASSERT_EQ(ret, ErrorCodes::Success);
      ret = readFile.readArray(channelInfosRead[0].getChannelName() + "_Q", valuesQ, readNow, read);
      ASSERT_EQ(ret, ErrorCodes::Success);
    }
    catch (...)
    {
      printf("read exception");
    }

    toRead -= readNow;
    offset += readNow;
    read += readNow;
    std::cout << "\r" << read / 1E6 << " MSamples read";
  }


  readFile.close();

}
#endif



#if 0
TEST_F(IqTarTests, WriteBigFile)
{
  const string filename = Common::TestOutputDir + "WriteBigFile.iq.tar";


  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IqTar writeFile(filename);

  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  for (size_t i = 0; i < 1000000; i++)
  {
    cout << i;
    cout << "\n";
    vector<vector<float>> iqValues;
    Common::initVector(iqValues, 2, 10000000, i*10000000);
    ret = writeFile.appendArrays(iqValues);
    ASSERT_EQ(ret, ErrorCodes::Success);
  }

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}
#endif

#if 0
TEST_F(IqTarTests, ReadAndWriteSimultaneous)
{
	const string filename = "e:/iqtar/Sweep.iq.tar";
	IqTar readFile(filename);
	vector<string> arrayNames;
	int ret = readFile.readOpen(arrayNames);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<ChannelInfo> channelInfosRead;
	map<string, string> metadataRead;
	ret = readFile.getMetadata(channelInfosRead, metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<float> channelValues;
	readFile.readChannel(channelInfosRead[0].getChannelName(), channelValues, channelInfosRead[0].getSamples() * 2);
	readFile.close();

	// 1. write file
	const string inOutFile = Common::TestOutputDir + "inout.iq.tar";
	IqTar writeFile(inOutFile);
	ret = writeFile.writeOpen(IqDataFormat::Complex, 1, "dailib test", "read while write test", channelInfosRead, &metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<vector<float>> channels;
	channels.push_back(channelValues);
	ret = writeFile.appendChannels(channels);
	ASSERT_EQ(ret, ErrorCodes::Success);
	ret = writeFile.close();
	ASSERT_EQ(ret, ErrorCodes::Success);
	// 2. read file
	IqTar readFile2(inOutFile);
	ret = readFile2.readOpen(arrayNames);
	ASSERT_EQ(ret, ErrorCodes::Success);
	ret = readFile2.getMetadata(channelInfosRead, metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<float> channelValuesRead;
	ret = readFile2.readChannel(channelInfosRead[0].getChannelName(), channelValuesRead, channelInfosRead[0].getSamples() * 2);
	ASSERT_EQ(ret, ErrorCodes::Success);
	// 3. remove file
	ret = remove(inOutFile.c_str());
	ASSERT_EQ(ret, ErrorCodes::Success);
	// 4. write file with new sweep

	ret = writeFile.writeOpen(IqDataFormat::Complex, 1, "dailib test", "read while write test", channelInfosRead, &metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	//channels.push_back(channelValues);
	ret = writeFile.appendChannels(channels);
	ASSERT_EQ(ret, ErrorCodes::Success);
	ret = writeFile.close();
	ASSERT_EQ(ret, ErrorCodes::Success);
	// 5. read again
	ret = readFile2.readChannel(channelInfosRead[0].getChannelName(), channelValuesRead, channelInfosRead[0].getSamples() * 2);
	ASSERT_EQ(ret, ErrorCodes::Success);
	ret = readFile2.close();
	ASSERT_EQ(ret, ErrorCodes::Success);
}
#endif

#if 0
TEST_F(IqTarTests, NewXslTest)
{
	const string filename = "e:/iqtar/Sweep.iq.tar";
	IqTar readFile(filename);

	vector<string> arrayNames;
	int ret = readFile.readOpen(arrayNames);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<ChannelInfo> channelInfosRead;
	map<string, string> metadataRead;
	ret = readFile.getMetadata(channelInfosRead, metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<float> channelValues;
	readFile.readChannel(channelInfosRead[0].getChannelName(), channelValues, channelInfosRead[0].getSamples()*2);

	const string filenameout = "e:/iqtar/MySweep.iq.tar";
	IqTar writeFile(filenameout);
	ret = writeFile.writeOpen(IqDataFormat::Complex, 1, "dailib test", "new xsl", channelInfosRead, &metadataRead);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<vector<float>> channels;
	channels.push_back(channelValues);
	ret = writeFile.appendChannels(channels);
	writeFile.close();

}
#endif

TEST_F(IqTarTests, MetadataXml)
{
  const string filename = Common::TestOutputDir + "MetadataXml.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  string segmentedCaptureXml = "<NoOfEvents>1</NoOfEvents>"
    "<SegmentLength unit=\"samples\">100</SegmentLength>"
    "<TriggerDroupoutTime unit=\"s\">0</TriggerDroupoutTime>"
    "<TriggerHoldoff unit=\"s\">0</TriggerHoldoff>"
    "<TriggerHysteresis unit=\"dB\">3</TriggerHysteresis>"
    "<TriggerOffset unit=\"s\">0</TriggerOffset>"
    "<TriggerSlope>Positive</TriggerSlope>"
    "<TriggerSource>Free Run</TriggerSource>"
    "<SegmentIndices>"
    "<ArrayOfInt length=\"5\">"
    "<int>0</int>"
    "<int>1</int>"
    "<int>2</int>"
    "<int>3</int>"
    "<int>4</int>"
    "</ArrayOfInt>"
    "</SegmentIndices>"
    "<SegmentTimestamps unit=\"s\">"
    "<ArrayOfFloat length=\"5\"><float>0.1</float><float>1.2</float><float>2.3</float><float>3.4</float><float>4.5</float>"
    "</ArrayOfFloat>"
    "</SegmentTimestamps>"
    "<TriggerTimestamps unit=\"s\">"
    "<ArrayOfFloat length=\"5\"><float>2</float><float>3</float><float>4</float><float>5</float><float>6</float>"
    "</ArrayOfFloat>"
    "</TriggerTimestamps>";

  string keyName = "Ch1_SegmentedCapture[XML]";
  map<string, string> metadata;
  metadata.insert(make_pair(keyName, segmentedCaptureXml));

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqTar readFile(filename);
  
  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfos, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(metadataRead.size(), 5 + metadata.size());

  auto val = metadataRead.find(keyName);
  if (val == metadataRead.end())
  {
    ASSERT_FALSE(true) << "metadata key not found";
  }

  ASSERT_EQ(val->second, segmentedCaptureXml);

  remove(filename.c_str());
}

TEST_F(IqTarTests, CheckDefaultTempDir)
{
  const string filename = Common::TestOutputDir + "CheckDefaultTempDir.iq.tar";

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  string tmpDir = writeFile.getTempDir();

  string defaultTempDir;
#ifdef _WIN32
  TCHAR pathBuf[MAX_PATH];
  memset(pathBuf, 0, sizeof(pathBuf));
  GetTempPath(MAX_PATH, pathBuf);
  defaultTempDir = string(pathBuf);
#else
  defaultTempDir = string(P_tmpdir);
#endif

  ASSERT_EQ(tmpDir, defaultTempDir);

  writeFile.close();
  remove(filename.c_str());
}

TEST_F(IqTarTests, CheckTempDir)
{
  const string filename = Common::TestOutputDir + "CheckDefaultTempDir.iq.tar";
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

  IqTar writeFile(filename);

  auto ret = writeFile.setTempDir(tempDir);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  string setTempDir = writeFile.getTempDir() + "/";
  ASSERT_EQ(tempDir, setTempDir);

  remove(filename.c_str());
}

TEST_F(IqTarTests, ChangeTempDirAfterWriterOpen)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.iq.tar";
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

TEST_F(IqTarTests, UnknownTempPath)
{
  const string filename = Common::TestOutputDir + "ChangeTempDirAfterWriterOpen.iq.tar";
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

TEST_F(IqTarTests, MetadataNoXml)
{
  const string filename = Common::TestOutputDir + "MetadataInvalidXml.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  string invalidXml = "<NoOfEvent1</NoOfEvents>";

  string keyName = "Ch1_SegmentedCapture[XML]";
  map<string, string> metadata;
  metadata.insert(make_pair(keyName, invalidXml));

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqTar readFile(filename);

  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadataRead;
  ret = readFile.getMetadata(channelInfos, metadataRead);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(metadataRead.size(), 5 + metadata.size());

  auto val = metadataRead.find(keyName);
  if (val == metadataRead.end())
  {
    ASSERT_FALSE(true) << "metadata key not found";
  }

  ASSERT_EQ(val->second, invalidXml);

  remove(filename.c_str());
}

TEST_F(IqTarTests, EnableTarPreview)
{
  const string filename = Common::TestOutputDir + "EnableTarPreview.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // does xslt exists?
  auto archive = archive_read_new();
  archive_read_support_filter_none(archive);
  archive_read_support_format_gnutar(archive);
  archive_read_support_format_tar(archive);
  auto r = archive_read_open_filename(archive, filename.c_str(), 10240);
  if (r != ARCHIVE_OK)
  {
    ASSERT_TRUE(false) << "archive read open error";
  }

  bool foundXslt = false;
  struct archive_entry* entry;
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK)
  {
    string headerName = archive_entry_pathname(entry);
    if (strcmp(headerName.c_str(), "open_IqTar_xml_file_in_web_browser.xslt") == 0)
    {
      foundXslt = true;
      break;
    }
  }

  r = archive_read_free(archive);
  if (r != ARCHIVE_OK)
  {
    ASSERT_TRUE(false) << "archive close error";
  }

  ASSERT_TRUE(foundXslt) << "Xslt not found!";

  remove(filename.c_str());
}

TEST_F(IqTarTests, DisableTarPreview)
{
  const string filename = Common::TestOutputDir + "DisableTarPreview.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IqTar writeFile(filename);
  writeFile.setPreviewEnabled(false);

  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // does xslt exists?
  auto archive = archive_read_new();
  archive_read_support_filter_none(archive);
  archive_read_support_format_gnutar(archive);
  archive_read_support_format_tar(archive);
  auto r = archive_read_open_filename(archive, filename.c_str(), 10240);
  if (r != ARCHIVE_OK)
  {
    ASSERT_TRUE(false) << "archive read open error";
  }

  bool foundXslt = false;
  struct archive_entry* entry;
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK)
  {
    string headerName = archive_entry_pathname(entry);
    if (strcmp(headerName.c_str(), "open_IqTar_xml_file_in_web_browser.xslt") == 0)
    {
      foundXslt = true;
      false;
    }
  }

  r = archive_read_free(archive);
  if (r != ARCHIVE_OK)
  {
    ASSERT_TRUE(false) << "archive close error";
  }

  ASSERT_FALSE(foundXslt) << "Xslt found, but should not exists as preview is disabled!";

  remove(filename.c_str());
}

TEST_F(IqTarTests, WriteDeprecatedInfo)
{
  const string filename = Common::TestOutputDir + "WriteDeprecatedInfo.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  map<string, string> metadata;
  metadata.insert(make_pair("key", "value"));

  string deprecatedXml = "<SpectrumAnalyzer><!--Deprecated Information--><CenterFrequency unit=\"Hz\">13250000000.000000</CenterFrequency><NumberOfPreSamples>0</NumberOfPreSamples><NumberOfPostSamples>0</NumberOfPostSamples><!--Deprecated Information End--></SpectrumAnalyzer>";

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos, metadata, deprecatedXml);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqTar readFile(filename);
  
  vector<string> names;
  ret = readFile.readOpen(names);
  ASSERT_EQ(ret, ErrorCodes::Success);

  string readDepInfo;
  ret = readFile.getDeprecatedInfo(readDepInfo);

  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(deprecatedXml, readDepInfo);

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(IqTarTests, WriteFileWithoutChannelNames)
{
  const string filename = Common::TestOutputDir + "WriteFileWithoutChannelNames.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 4, 6);

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("MyChannel", 12, 45));
  channelInfos.push_back(ChannelInfo("", 12, 45));

  IqTar writeFile(filename);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 4, "App name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqTar readFile(filename);
  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(arrayNames.size(), 2 * channelInfos.size());

  vector<ChannelInfo> channelInfosRead;
  map<string, string> metadata;
  ret = readFile.getMetadata(channelInfosRead, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(channelInfosRead.size(), 2);
  ASSERT_EQ(channelInfosRead[0].getChannelName(), channelInfos[0].getChannelName());
  ASSERT_EQ(channelInfosRead[1].getChannelName(), ChannelInfo::getDefaultChannelName(1));

  remove(filename.c_str());
}

TEST_F(IqTarTests, CheckXmlSpecialCharacters)
{
  const string filename = Common::TestOutputDir + "CheckXmlSpecialCharacters.iq.tar";

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 6);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("RealerKanal1", 13, 2));
  channelInfosWrite.push_back(ChannelInfo("RealerKanal2", 13, 2));

  map<string, string> metadata;
  metadata.insert(make_pair("test&key", "Rohde&Schwarz"));
  metadata.insert(make_pair("\"Quotes\"", "\"Test\""));
  metadata.insert(make_pair("\'Single Quotes\'", "\'Test\'"));
  metadata.insert(make_pair("<>", "<>'"));

  IqTar file(filename);
  auto ret = file.writeOpen(IqDataFormat::Real, 2, "App", "comment", channelInfosWrite, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqTar readFile(filename);
  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  vector<ChannelInfo> infoRead;
  map<string, string> metaRead;
  ret = readFile.getMetadata(infoRead, metaRead);
  ASSERT_EQ(ret, ErrorCodes::Success);

  //check metadata if it contains the user specific data
  for (auto pair : metaRead)
  {
    ASSERT_EQ(metaRead.count(pair.first), 1);
    ASSERT_EQ(metaRead[pair.first], pair.second);
  }

  remove(filename.c_str());
}

TEST_F(IqTarTests, setIqDataSizeWriterAlreadyInitialized)
{
  const string filename = Common::TestOutputDir + "SetIqDataSizeWriterAlreadyInitialized.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("RealerKanal1", 13, 2));
  channelInfosWrite.push_back(ChannelInfo("RealerKanal2", 13, 2));

  IqTar file(filename);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfosWrite);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.disableTempFile(2, 2, IqDataFormat::Complex, IqDataType::Float32);
  ASSERT_EQ(ret, ErrorCodes::WriterAlreadyInitialized);

  remove(filename.c_str());
}

TEST_F(IqTarTests, NoTempFileNoData)
{
  const string filename = Common::TestOutputDir + "NoTempFileNoData.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("RealerKanal1", 13, 2));
  channelInfosWrite.push_back(ChannelInfo("RealerKanal2", 13, 2));

  IqTar file(filename);

  auto ret = file.disableTempFile(2, 2, IqDataFormat::Complex, IqDataType::Float64);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfosWrite);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::InvalidTarArchive);

  remove(filename.c_str());
}

TEST_F(IqTarTests, setIqDataSizeDataOverflowArray)
{
  const string filename = Common::TestOutputDir + "SetIqDataSizeDataOverflow.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel", 13, 2));

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 2, 44);

  IqTar file(filename);

  auto ret = file.disableTempFile(2, 2, IqDataFormat::Complex, IqDataType::Float32);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfosWrite);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendArrays(iqValues);
  ASSERT_EQ(ret, ErrorCodes::DataOverflow);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::InvalidTarArchive);

  remove(filename.c_str());
}

TEST_F(IqTarTests, setIqDataSizeDataOverflowChannel)
{
  const string filename = Common::TestOutputDir + "SetIqDataSizeDataOverflowChannel.iq.tar";

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Channel", 13, 2));

  vector<vector<float>> iqValues;
  Common::initVector(iqValues, 1, 44);

  IqTar file(filename);

  auto ret = file.disableTempFile(2, 2, IqDataFormat::Complex, IqDataType::Float32);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfosWrite);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.appendChannels(iqValues);
  ASSERT_EQ(ret, ErrorCodes::DataOverflow);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::InvalidTarArchive);

  remove(filename.c_str());
}
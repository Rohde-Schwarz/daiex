#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <list>
#include <map>

#include "dataimportexport.h"
#include "common.h"


using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class IqTarErrorHandlingTest : public ::testing::Test
{
public:
  IqTarErrorHandlingTest() : 
    filename_(Common::TestDataDir + "FromFSW_1ch_WithPreview.iq.tar")
  {
  }

public:
  string filename_;
  vector<string> arrayNames_;
};

template <typename T>
class IqTarErrorHandlingSingleAndDouble : public IqTarErrorHandlingTest
{
};

typedef ::testing::Types<float, double> MyTypes;
TYPED_TEST_CASE(IqTarErrorHandlingSingleAndDouble, MyTypes);


TEST_F(IqTarErrorHandlingTest, GetDeprecatedInfoWithoutOpeningFile)
{
  const string readFile = Common::TestDataDir + "MultiChannel_4ch_fromLTE.iq.tar";

  IqTar file(readFile);

  string info;
  auto retCode = file.getDeprecatedInfo(info);
  ASSERT_EQ(retCode, ErrorCodes::OpenFileHasNotBeenCalled);
}

TEST_F(IqTarErrorHandlingTest, WriteInvalidDeprecatedInfo)
{
  const string writeFileName = Common::TestOutputDir + "WriteInvalidDeprecatedInfo.iq.tar";

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  map<string, string> metadata;
  metadata.insert(make_pair("key", "value"));

  string invalidXml = "<Deprecated>invalid";

  IqTar writeFile(writeFileName);
  auto retCode = writeFile.writeOpen(IqDataFormat::Complex, 2, "app name", "comment", channelInfos, metadata, invalidXml);
  ASSERT_EQ(retCode, ErrorCodes::InvalidXmlString);

  remove(writeFileName.c_str());
}

TEST_F(IqTarErrorHandlingTest, NoXmlContainedTest)
{
  IqTar file(Common::TestDataDir + "MissingXML.iq.tar");
  auto ret = file.readOpen(this->arrayNames_);
  ASSERT_EQ(ret, ErrorCodes::InvalidFormatNoXmlFoundInTar) << "Wrong return code of IQTar.OpenFile";
  ASSERT_EQ(this->arrayNames_.size(), 0) << "No array names shall be returned in case of error";
  ASSERT_NE(ErrorCodes::getErrorText(ret), "unknown error") << "Case statement missing for ErrorCodes.InvalidFormatNoXmlFoundInTar";
}

TEST_F(IqTarErrorHandlingTest, InvalidArchiveTest)
{
  IqTar file(Common::TestDataDir + "CorruptTarHeader.iq.tar");
  auto ret = file.readOpen(this->arrayNames_);
  ASSERT_EQ(ret, ErrorCodes::InvalidTarArchive) << "Wrong return code of IQTar.OpenFile";
  ASSERT_EQ(this->arrayNames_.size(), 0) << "No array names shall be returned in case of error";
  ASSERT_NE(ErrorCodes::getErrorText(ret), "unknown error") << "Case statement missing for ErrorCodes.InvalidTarArchive";
}

TEST_F(IqTarErrorHandlingTest, InvalidXmlContentTest)
{
  IqTar file(Common::TestDataDir + "MissingTag.iq.tar");
  auto ret = file.readOpen(this->arrayNames_);
  ASSERT_EQ(ret, ErrorCodes::InvalidFormatOfIQTarXmlContent) << "Wrong return code of IQTar.OpenFile";
  ASSERT_EQ(this->arrayNames_.size(), 0) << "No array names shall be returned in case of error";
  ASSERT_NE(ErrorCodes::getErrorText(ret), "unknown error") << "Case statement missing for ErrorCodes.InvalidTarArchive";

  IqTar file2(Common::TestDataDir + "WrongValueFormat.iq.tar");
  ret = file2.readOpen(this->arrayNames_);
  ASSERT_EQ(ret, ErrorCodes::InvalidFormatOfIQTarXmlContent) << "Wrong return code of IQTar.OpenFile";
  ASSERT_EQ(this->arrayNames_.size(), 0) << "No array names shall be returned in case of error";
  ASSERT_NE(ErrorCodes::getErrorText(ret), "unknown error") << "Case statement missing for ErrorCodes.InvalidTarArchive";
}


TYPED_TEST(IqTarErrorHandlingSingleAndDouble, InconsistentNumberOfDataArrays)
{
  // Testcase: not all data arrays and channels have equal length!

  vector<vector<TypeParam>> iqvalues;
  Common::initVector(iqvalues, 4, 5);
  iqvalues[2].pop_back(); iqvalues[2].pop_back();
  iqvalues[3].pop_back(); iqvalues[3].pop_back();

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 1.0, 1.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 1.0, 1.0));

  const string outfile = Common::TestOutputDir + "InconsistentIQDataTest.iq.tar";

  IqTar writeFile(outfile);
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 4, "Mosaik DataImportExport", "1 channel, complex, float 32", channelInfosWrite);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = writeFile.appendArrays(iqvalues);
  ASSERT_EQ(ret, ErrorCodes::InconsistentInputData) << "write open failed";
  ret = writeFile.close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  remove(outfile.c_str());
}

TYPED_TEST(IqTarErrorHandlingSingleAndDouble, InconsistentClockRate)
{
  // Testcase: not all data arrays and channels have equal length!

  vector<vector<TypeParam>> iqvalues;
  Common::initVector(iqvalues, 4, 5);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 1.0, 1.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 2.0, 1.0));

  const string outfile = Common::TestOutputDir + "InconsistentIQDataTest.iq.tar";
  IqTar file(outfile);
  auto returnCode = file.writeOpen(IqDataFormat::Complex, 4, "name", "comment", channelInfosWrite);
  ASSERT_EQ(returnCode, ErrorCodes::InconsistentInputData) << "Wrong return code returned by IQTar.";
  file.close();

  remove(outfile.c_str());
}

TYPED_TEST(IqTarErrorHandlingSingleAndDouble, InconsistentCenterFreq)
{
  // Testcase: not all data arrays and channels have equal length!

  vector<vector<TypeParam>> iqvalues;
  Common::initVector(iqvalues, 4, 5);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 1.0, 1.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 1.0, 2.0));

  const string outfile = Common::TestOutputDir + "InconsistentIQDataTest.iq.tar";
  IqTar file(outfile);
  auto returnCode = file.writeOpen(IqDataFormat::Complex, 4, "name", "comment", channelInfosWrite);
  ASSERT_EQ(returnCode, ErrorCodes::InconsistentInputData) << "Wrong return code returned by IQTar.";

  remove(outfile.c_str());
}

TYPED_TEST(IqTarErrorHandlingSingleAndDouble, NotEnoughDataForChannels)
{
  // Testcase : less data arrays than channelinfo entries

  vector<vector<TypeParam>> iqvalues;
  Common::initVector(iqvalues, 2, 5);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 1.0, 1.0));
  channelInfosWrite.push_back(ChannelInfo("Kanal2", 1.0, 1.0));

  const string outfile = Common::TestOutputDir + "InconsistentIQDataTest.iq.tar";

  IqTar file(outfile);
  auto returnCode = file.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfosWrite);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = file.appendArrays(iqvalues);
  ASSERT_EQ(returnCode, ErrorCodes::InconsistentInputData) << "Wrong return code returned by IQTar";

  remove(outfile.c_str());
}

TYPED_TEST(IqTarErrorHandlingSingleAndDouble, NotEnoughDataForDataFormat)
{
  // Testcase : less data arrays than channelinfo entries

  vector<vector<TypeParam>> iqvalues;
  Common::initVector(iqvalues, 1, 5);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 1.0, 1.0));

  const string outfile = Common::TestOutputDir + "InconsistentIQDataTest.iq.tar";

  IqTar file(outfile);
  auto returnCode = file.writeOpen(IqDataFormat::Complex, 1, "Mosaik DataImportExport", "1 channel, complex float 32 data", channelInfosWrite);
  ASSERT_EQ(returnCode, ErrorCodes::Success);
  returnCode = file.appendArrays(iqvalues);
  ASSERT_EQ(returnCode, ErrorCodes::InconsistentInputData) << "Wrong return code returned by IQTar.";
}

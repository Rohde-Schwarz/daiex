#include "gtest/gtest.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "constants.h"

#include "dataimportexport.h"
#include "common.h"

#ifdef _WIN32
 #if _MSC_VER <= 1700 // until VS2012
namespace std
{
  template<typename T>
  int isnan(const T &x)
  {
    return _isnan(x);
  }
}
 #endif
#endif


using namespace std;
using namespace chrono;
using namespace rohdeschwarz::mosaik::dataimportexport;

class IqTarLegacyTest : public ::testing::TestWithParam<int>
{
protected:
  void SetUp()
  {
    int testCaseNumber = GetParam();
    this->init(testCaseNumber);
  }

  void TearDown()
  {
    auto ret = this->readFile->close();
    ASSERT_EQ(ErrorCodes::Success, ret);
    delete this->readFile;
  }

public:
  // Members to hold parameters of the test
  string filename;
  string expApplicName;
  string expComment;
  string expFormat;
  string expDataType;
  string expScalingFactor;
  double expClockrate;
  double expCenterFreq;
  int expSamples;
  int expChannelCount;

  // Members that will be filled from parameters
  // as legacy files don't contain channel names
  vector<string> expArrayNames;
  vector<string> expChannelNames;
  int arraysPerChannel; 

  // Members to hold data read from the file
  // will be reinitialized in CleanUp
  IqTar* readFile;
  vector<string> arrayNames;
  vector<ChannelInfo> channelInfos;
  map<string, string> metadata;

  void init(int testCaseNumber);
};



INSTANTIATE_TEST_CASE_P(IqTarLegacyTest, IqTarLegacyTest,
                        ::testing::Values(1,2,3,4,5));



void IqTarLegacyTest::init(int testCaseNumber)
{
  this->arraysPerChannel = 2; //default for complex und polar format

  switch (testCaseNumber)
  {
  case 1:
    this->filename = Common::TestDataDir + "FromFSW_1ch_WithPreview.iq.tar";
    this->expApplicName = "FSW-26";
    this->expComment = "";
    this->expFormat = "complex";
    this->expDataType = "float32";
    this->expScalingFactor = "1";
    this->expChannelCount = 1;
    this->expClockrate = 32000000;
    this->expCenterFreq = 13250000000.000000;
    this->expSamples = 1001;
    break;
  case 2:
    this->filename = Common::TestDataDir + "MultiChannel_4ch_fromLTE.iq.tar";
    this->expApplicName = "Rohde-Schwarz EUTRA/LTE Analysis Software Version 2.8 Beta 3";
    this->expComment = "Rohde-Schwarz EUTRA/LTE Analysis Software Version 2.8 Beta 3 Comment";
    this->expFormat = "complex";
    this->expDataType = "float32";
    this->expScalingFactor = "1";
    this->expChannelCount = 4;
    this->expClockrate = 15360000;
    this->expCenterFreq = numeric_limits<double>::quiet_NaN();
    this->expSamples = 307200;
    break;
  case 3:
    this->filename = Common::TestDataDir + "1MioSamples_1ch.iq.tar";
    this->expApplicName = "---";
    this->expComment = "TEST";
    this->expFormat = "complex";
    this->expDataType = "float32";
    this->expScalingFactor = "1";
    this->expChannelCount = 1;
    this->expClockrate = 16000000;
    this->expCenterFreq = numeric_limits<double>::quiet_NaN();
    this->expSamples = 1000000;
    break;
  case 4:
    this->filename = Common::TestDataDir + "TestScalingFactorAsDouble.iq.tar";
    this->expApplicName = "RSFW.dll";
    this->expComment = "";
    this->expFormat = "complex";
    this->expDataType = "float32";
    this->expScalingFactor = "1.00000";
    this->expChannelCount = 1;
    this->expClockrate = 32000000;
    this->expCenterFreq = numeric_limits<double>::quiet_NaN();
    this->expSamples = 1001;
    break;
  case 5:
  default:
    this->filename = Common::TestDataDir + "TestScalingFactorNot1.iq.tar";
    this->expApplicName = "RSFW.dll";
    this->expComment = "";
    this->expFormat = "complex";
    this->expDataType = "float32";
    this->expScalingFactor = "3.123";
    this->expChannelCount = 1;
    this->expClockrate = 32000000;
    this->expCenterFreq = numeric_limits<double>::quiet_NaN();
    this->expSamples = 1001;
    break;
  }

  this->expChannelNames.reserve(this->expChannelCount);
  for (int idx = 0; idx < this->expChannelCount; idx++)
  {
    this->expChannelNames.push_back("Channel" + to_string(idx + 1));
  }

  if (0 == this->expFormat.compare("REAL"))
  {
    this->arraysPerChannel = 1;
  }

  size_t nofArrays = this->expChannelCount * this->arraysPerChannel;
  this->expArrayNames.reserve(nofArrays);
  if (this->arraysPerChannel == 1)
  {
    for (size_t idx = 0; idx < nofArrays; idx++)
    {
      this->expArrayNames.push_back("Channel" + to_string(idx + 1));
    }
  }
  else
  {
    for (size_t idx = 0; idx < nofArrays; idx++)
    {
      if (idx % 2 == 0)
      {
        this->expArrayNames.push_back("Channel" + to_string((idx / 2) + 1) + "_I");
      }
      else
      {
        this->expArrayNames.push_back("Channel" + to_string((idx / 2) + 1) + "_Q");
      }
    }
  }

  this->readFile = new IqTar(this->filename);

  auto ret = this->readFile->readOpen(this->arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
}

TEST_P(IqTarLegacyTest, CheckMetaDataReturns)
{
  auto ret = this->readFile->getMetadata(this->channelInfos, this->metadata);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQTar.GetMetadata failed";
  ASSERT_NE(channelInfos.size(), 0) << "At least one valid ChannelInfo object expected";

  // Legacy files contains no optional meta data!
  // the metadata dict should only contain 4 entries if comment is empty, 5 otherwise
  if (this->expComment.empty())
  {
    ASSERT_EQ(this->metadata.size(), 4) << "Unexpected number of metadata entries";
  }
  else
  {
    ASSERT_EQ(this->metadata.size(), 5) << "Unexpected number of metadata entries";
  }
}

TEST_P(IqTarLegacyTest, CheckChannelInfos)
{
  auto ret = this->readFile->getMetadata(this->channelInfos, this->metadata);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQTar.GetMetadata failed";
  ASSERT_NE(this->channelInfos.size(), 0) << "At least one valid ChannelInfo object expected";

  ASSERT_EQ(this->channelInfos.size(), this->expChannelCount) << "Unexpected number of channels read from file";

  for (size_t idx = 0; idx < this->channelInfos.size(); idx++)
  {
    ASSERT_EQ(this->channelInfos[idx].getChannelName(), this->expChannelNames[idx]) << "Unexpected channel name read from file";
    ASSERT_EQ(this->channelInfos[idx].getClockRate(), this->expClockrate) << "Unexpected clock rate read from file";

    if ((bool)std::isnan(this->expCenterFreq))
    {
      bool r = (bool)std::isnan(this->channelInfos[idx].getFrequency());
      ASSERT_TRUE(r) <<"Unexpected center frequency read from file";
    }
    else
    {
      ASSERT_DOUBLE_EQ(this->channelInfos[idx].getFrequency(), this->expCenterFreq) << "Unexpected center frequency read from file"; 
    }
  }
}

TEST_P(IqTarLegacyTest, CheckMandatoryMetaDataContent)
{
  auto ret  = this->readFile->getMetadata(this->channelInfos, this->metadata);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQTar.GetMetadata failed";

  // Check for mandatory data
  for (auto pair : metadata)
  {
    if (0 == pair.first.compare(Constants::XmlApplicationName))
    {
      ASSERT_EQ(pair.second, this->expApplicName) << "ApplicationName read from file is incorrect";
    }
    else if (0 == pair.first.compare(Constants::XmlComment))
    {
      if (this->expComment.empty())
      {
        // This key should not exists as the comment in the file is empty!
        ASSERT_TRUE(true) << "This file doesn't contain a comment";
      }
      else
      {
        ASSERT_EQ(pair.second, this->expComment) << "Comment read from file is incorrect";
      }
    }
    else if (0 == pair.first.compare(Constants::XmlFormat))
    {
      ASSERT_EQ(pair.second, this->expFormat) << "Format read from file is incorrect";
    }
    else if (0 == pair.first.compare(Constants::XmlDataType))
    {
      ASSERT_EQ(pair.second, this->expDataType) << "DataType read from file is incorrect";
    }
    else if (0 == pair.first.compare(Constants::XmlScalingFactor))
    {
      ASSERT_EQ(pair.second, this->expScalingFactor) << "ScalingFactor read from file is incorrect";
    }
  }
}

TEST_P(IqTarLegacyTest, CheckArrayNames)
{
  ASSERT_EQ(this->arrayNames.size(), this->expChannelCount * this->arraysPerChannel) << "Unexpected number of arrays read from file";
  for (size_t idx = 0; idx < arrayNames.size(); idx++)
  {
    ASSERT_EQ(this->arrayNames[idx], this->expArrayNames[idx]) << "Unexpected array name read from file";
  }
}

TEST_P(IqTarLegacyTest, CheckArrayLengthAsSingle)
{
  // Check the length of the returned arrays
  for (size_t idx = 0; idx < this->arrayNames.size(); idx++)
  {
    if (Common::strEndsWithIgnoreCase(this->arrayNames[idx], "_I") || Common::strEndsWithIgnoreCase(this->arrayNames[idx], "_Q"))
    {
      long countValues = this->readFile->getArraySize(this->arrayNames[idx]);
      ASSERT_EQ(countValues, this->expSamples) << "IQTar.getArraySize returned wrong array length";

      vector<float> fValues(countValues);
      int returnCode = this->readFile->readArray(this->arrayNames[idx], fValues, countValues);
      ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.readArray failed";

      vector<float> fValues1(countValues + 2);
      returnCode = this->readFile->readArray(this->arrayNames[idx], fValues1, countValues + 2);
      ASSERT_EQ(returnCode, ErrorCodes::InvalidDataInterval) << "IQTar.readArray failed";
    }
    else
    {
      long countValues = this->readFile->getArraySize(this->arrayNames[idx]);
      ASSERT_EQ(countValues, 2 * this->expSamples) << "IQTar.getArraySize returned wrong array length";

      vector<float> fValues(countValues);
      int returnCode = this->readFile->readChannel(this->arrayNames[idx], fValues, countValues);
      ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.readChannel failed";

      vector<float> fValues1(countValues + 2);
      returnCode = this->readFile->readChannel(this->arrayNames[idx], fValues1, countValues + 2);
      ASSERT_EQ(returnCode, ErrorCodes::InvalidDataInterval) << "IQTar.readChannel failed";
    }
  }
}

TEST_P(IqTarLegacyTest, CheckArrayLengthAsDouble)
{
  // Check the length of the returned arrays
  for (size_t idx = 0; idx < this->arrayNames.size(); idx++)
  {
    if (Common::strEndsWithIgnoreCase(this->arrayNames[idx], "_I") || Common::strEndsWithIgnoreCase(this->arrayNames[idx], "_Q"))
    {
      long countValues = this->readFile->getArraySize(this->arrayNames[idx]);
      ASSERT_EQ(countValues, this->expSamples) << "IQTar.getArraySize returned wrong array length";

      vector<double> dValues(countValues);
      int returnCode = this->readFile->readArray(this->arrayNames[idx], dValues, countValues);
      ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.readArray failed";

      vector<double> dValues1(countValues + 2);
      returnCode = this->readFile->readArray(this->arrayNames[idx], dValues1, countValues + 2);
      ASSERT_EQ(returnCode, ErrorCodes::InvalidDataInterval) << "IQTar.readArray failed";
    }
    else
    {
      long countValues = this->readFile->getArraySize(this->arrayNames[idx]);
      ASSERT_EQ(countValues, 2 * this->expSamples) << "IQTar.getArraySize returned wrong array length";

      vector<double> dValues(countValues);
      int returnCode = this->readFile->readChannel(this->arrayNames[idx], dValues, countValues);
      ASSERT_EQ(returnCode, ErrorCodes::Success) << "IQTar.readChannel failed";

      vector<double> dValues1(countValues + 2);
      returnCode = this->readFile->readChannel(this->arrayNames[idx], dValues1, countValues + 2);
      ASSERT_EQ(returnCode, ErrorCodes::InvalidDataInterval) << "IQTar.readChannel failed";
    }
  }
}

TEST_P(IqTarLegacyTest, ReadPerformance)
{
  double totalTime = 0;
  // Display the timer frequency and resolution.
  auto start = high_resolution_clock::now();
  auto ret  = this->readFile->getMetadata(this->channelInfos, this->metadata);
  auto stop = high_resolution_clock::now();

  ASSERT_EQ(ErrorCodes::Success, ret) <<  "IQTar.GetMetadata failed";
  duration<double> span = duration_cast<duration<double>>(stop - start);
  totalTime += span.count();
  cout << "Read meta data - " << this->channelInfos.size() << " channels - in " << span.count() << " seconds\n";

  // Check the length of the returned arrays
  for (size_t idx = 0; idx < this->arrayNames.size(); idx++)
  {
    vector<float> fValues(this->expSamples);

    start = high_resolution_clock::now();
    ret = this->readFile->readArray(this->arrayNames[idx], fValues, this->expSamples);
    stop = high_resolution_clock::now();
    ASSERT_EQ(ErrorCodes::Success, ret) << "IQTar.GetArrayAsSingle failed";
    span = duration_cast<duration<double>>(stop - start);
    cout << "Read " << this->arrayNames[idx] + " as single - " << fValues.size() << " values - in " << span.count() << " seconds\n";
    totalTime += span.count();
  }

  // Check the length of the returned arrays
  for (size_t idx = 0; idx < this->arrayNames.size(); idx++)
  {
    vector<double> dValues(this->expSamples);
    start = high_resolution_clock::now();
    ret = this->readFile->readArray(this->arrayNames[idx], dValues, this->expSamples);
    stop = high_resolution_clock::now();
    span = duration_cast<duration<double>>(stop - start);
    ASSERT_EQ(ErrorCodes::Success, ret) << "IQTar.GetArrayAsdouble failed";
    cout << "Read " << this->arrayNames[idx] << " as double - " << dValues.size() << " values - in " << span.count() << " seconds\n";
    totalTime += span.count();
  }

  cout << "Read all contained data in " << totalTime << " seconds\n";
}




#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>

#include "dataimportexport.h"
#include "common.h"
#include "constants.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class CsvTest : public ::testing::Test
{
};


template<typename T, CsvSeparatorConfig config>
class ContainerSepConfig
{
public:
  ContainerSepConfig();
  typedef T Dt;
  static const CsvSeparatorConfig Config = config;
};

typedef ::testing::Types<
  ContainerSepConfig<float, CsvSeparatorConfig::DecimalColonValueSemicolon>,
  ContainerSepConfig<double, CsvSeparatorConfig::DecimalColonValueSemicolon>,
  ContainerSepConfig<float, CsvSeparatorConfig::DecimalDotValueColon>,
  ContainerSepConfig<double, CsvSeparatorConfig::DecimalDotValueColon>,
  ContainerSepConfig<float, CsvSeparatorConfig::DecimalDotValueSemicolon>,
  ContainerSepConfig<double, CsvSeparatorConfig::DecimalDotValueSemicolon>
> MyTypesSepTypes;
TYPED_TEST_CASE(CsvSeparatorTest, MyTypesSepTypes);

template<typename T>
class CsvSeparatorTest : public ::testing::Test
{
};

static const string RawFiles[3] = {
  "RawArrayDecimalColonValueSemicolon.csv",
  "RawArrayDecimalDotValueColon.csv",
  "RawArrayDecimalDotValueSemicolon.csv"
};

template<typename T, size_t fileIndex>
class ContainerRawFile
{
public:
  ContainerRawFile();
  typedef T Dt;
  static const size_t FileIndex = fileIndex;
};

typedef ::testing::Types<
  ContainerRawFile<float, 0>,
  ContainerRawFile<double, 0>,
  ContainerRawFile<float, 1>,
  ContainerRawFile<double, 1>,
  ContainerRawFile<float, 2>,
  ContainerRawFile<double, 2>
> MyRawFiles;
TYPED_TEST_CASE(RawFileTest, MyRawFiles);

template<typename T>
class RawFileTest : public ::testing::Test
{
};

struct FormatSpec
{
  string spec;
  bool valid;

  FormatSpec(string formatSpec, bool isValid)
  {
    this->spec = formatSpec;
    this->valid = isValid;
  }
};

class CsvFormatSpecifierTest : public ::testing::TestWithParam<FormatSpec>
{
};

INSTANTIATE_TEST_CASE_P(CsvFormatSpecifierTest, CsvFormatSpecifierTest, ::testing::Values(
  FormatSpec("7E", true),
  FormatSpec("7e", true),
  FormatSpec("2E", true),
  FormatSpec("9E", true),
  FormatSpec("5G", true),
  FormatSpec("10E", false),
  FormatSpec("E", false),
  FormatSpec("XY", false),
  FormatSpec("C", false),
  FormatSpec("12", false)));

template<typename T, IqDataFormat format>
class ContainerFormat
{
public:
  ContainerFormat();
  typedef T Dt;
  static const IqDataFormat Format = format;
};

typedef ::testing::Types<
  ContainerFormat<float, IqDataFormat::Complex>,
  ContainerFormat<double, IqDataFormat::Complex>,
  ContainerFormat<float, IqDataFormat::Polar>,
  ContainerFormat<double, IqDataFormat::Polar>,
  ContainerFormat<float, IqDataFormat::Real>,
  ContainerFormat<double, IqDataFormat::Real>
>  MyFormatTypes;
TYPED_TEST_CASE(CsvFormatTest, MyFormatTypes);

template<typename T>
class CsvFormatTest : public ::testing::Test
{
};

template<typename T, IqDataFormat format, CsvSeparatorConfig sepConfig, int channels>
class ContainerChannels
{
public:
  ContainerChannels();
  typedef T Dt;
  static const IqDataFormat Format = format;
  static const CsvSeparatorConfig Config = sepConfig;
  static const int Channels = channels;
};

typedef ::testing::Types<
    // Not necessary to test DecimalDotValueSemiColon, as only the different ValueSeparators matter!
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<float, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 3>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<float, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 3>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<float, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 3>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 1>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 2>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalColonValueSemicolon, 3>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<double, IqDataFormat::Complex, CsvSeparatorConfig::DecimalDotValueColon, 3>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<double, IqDataFormat::Real, CsvSeparatorConfig::DecimalDotValueColon, 3>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 1>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 2>,
    ContainerChannels<double, IqDataFormat::Polar, CsvSeparatorConfig::DecimalDotValueColon, 3>
> MyChannelTypes;
TYPED_TEST_CASE(CsvChannelTest, MyChannelTypes);

template<typename T>
class CsvChannelTest : public ::testing::Test
{
};

TEST_F(CsvFormatSpecifierTest, removeAdditionalSemicolons)
{
#if 0
	const string outputIqx = Common::TestOutputDir + "removeAdditionalSemicolons.csv";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("My_Key1", "My meta data value"));

	size_t nofArraysWrite = 2;
	IqCsv outCsv(outputIqx);
	auto retCode = outCsv.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<vector<float>> writeVector;
	vector<float> iVector(10);
	vector<float> qVector(10);
	const float p180 = 3.14159265 / 180;
	for (size_t i = 0; i < 10; i++)
	{
		iVector[i] = sin((float)i*p180);
		qVector[i] = -iVector[i];
	}
	// ... fill i/q-vectors with data ...
	writeVector.push_back(iVector);
	writeVector.push_back(qVector);

	// add data to Iqx
	retCode = outCsv.appendArrays(writeVector);
	outCsv.close();
#endif
}
TEST_F(CsvFormatSpecifierTest, CRLF)
{
}


#if 0
TEST_F(CsvFormatSpecifierTest, Jungnickel20181022)
{
  {   
    IqCsv readFile("e:/csvtest/delimiterbug.csv");
    vector<string> arrayNames;
    auto ret = readFile.readOpen(arrayNames);
    ASSERT_EQ(ret, ErrorCodes::Success);

    vector<ChannelInfo> channels;
    map<string, string> metadata;
    ret = readFile.getMetadata(channels, metadata);
    ASSERT_EQ(ret, ErrorCodes::Success);
    vector<float> iValues;
    vector<float> qValues;
    ret = readFile.readArray(arrayNames[0], iValues, channels[0].getSamples());
    ASSERT_EQ(ret, ErrorCodes::Success);
    ret = readFile.readArray(arrayNames[1], qValues, channels[0].getSamples());
    ASSERT_EQ(ret, ErrorCodes::Success);

    readFile.close();
  }
}
#endif

TEST_F(CsvFormatSpecifierTest, Lehmann20180115)
{
#if 0
	{   // works
		IqCsv readFile("e:/csvtest/test_comma.csv");
		vector<string> arrayNames;
		auto ret = readFile.readOpen(arrayNames);
		ASSERT_EQ(ret, ErrorCodes::Success);
		vector<float> iValues;
		vector<float> qValues;
		ret = readFile.readArray(arrayNames[0], iValues, arrayNames[0].size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		ret = readFile.readArray(arrayNames[1], qValues, arrayNames[1].size());
		ASSERT_EQ(ret, ErrorCodes::Success);
	}
	{   // does not work
		IqCsv readFile("e:/csvtest/test_point.csv");
		vector<string> arrayNames;
		auto ret = readFile.readOpen(arrayNames);
		ASSERT_EQ(ret, ErrorCodes::Success);
		vector<float> iValues;
		vector<float> qValues;
		ret = readFile.readArray(arrayNames[0], iValues, arrayNames[0].size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		ret = readFile.readArray(arrayNames[1], qValues, arrayNames[1].size());
		ASSERT_EQ(ret, ErrorCodes::Success);
	}
#endif
}

TEST_P(CsvFormatSpecifierTest, TestValidFormatSpecifier)
{
  FormatSpec const& p = GetParam();

  const string filename = Common::TestOutputDir + "TestValidFormatSpecifier.csv";

  vector<vector<float>> data;
  Common::initVector(data, 2, 5);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 13, 55));

  IqCsv writeFile(filename);
  writeFile.setFormatSpecifier(p.spec);
  writeFile.setFormatSpecifierChannelInfo(p.spec);

  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 2, "", "", channelInfosWrite);
  if (p.valid)
  {
    ASSERT_EQ(ret, ErrorCodes::Success);
  }
  else
  {
    ASSERT_EQ(ret, ErrorCodes::InvalidFormatSpecifier);
  }

  writeFile.close();

  remove(filename.c_str());
}

TEST_F(CsvTest, ReadLegacyFile_ANSI)
{
  const string filename = Common::TestDataDir + "MultiChannel_Mosaik_ANSI.csv";

  vector<string> arrayNames;

  IqCsv file(filename);
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);
  
  ASSERT_EQ(arrayNames.size(), 6);
  ASSERT_EQ(arrayNames.at(0), "Kanal0_I");
  ASSERT_EQ(arrayNames.at(1), "Kanal0_Q");
  ASSERT_EQ(arrayNames.at(2), "Kanal1_I");
  ASSERT_EQ(arrayNames.at(3), "Kanal1_Q");
  ASSERT_EQ(arrayNames.at(4), "Kanal2_I");
  ASSERT_EQ(arrayNames.at(5), "Kanal2_Q");

  auto nofIValues = file.getArraySize(arrayNames[0]);
  vector<float> iValues(nofIValues);
  ret = file.readArray(arrayNames[0], iValues, nofIValues);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(nofIValues, 7);

  auto nofQValues = file.getArraySize(arrayNames[1]);
  vector<float> qValues(nofQValues);
  ret = file.readArray(arrayNames[1], qValues, nofQValues);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(nofQValues, 7);

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = file.getMetadata(channels, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channels.size(), 3);

  //Check channel infos
  double expClockRate = 32000000;
  double expCenterFreq = 13250000000.000000;
  ASSERT_EQ(expClockRate, channels[0].getClockRate());
  ASSERT_EQ(expCenterFreq, channels[0].getFrequency());

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TEST_F(CsvTest, ReadLegacyFile)
{
  const string filename = Common::TestDataDir + "MultiChannel_Mosaik.csv";

  vector<string> arrayNames;

  IqCsv file(filename);
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(arrayNames.size(), 6);
  ASSERT_EQ(arrayNames.at(0), "Kanal0_I");
  ASSERT_EQ(arrayNames.at(1), "Kanal0_Q");
  ASSERT_EQ(arrayNames.at(2), "Kanal1_I");
  ASSERT_EQ(arrayNames.at(3), "Kanal1_Q");
  ASSERT_EQ(arrayNames.at(4), "Kanal2_I");
  ASSERT_EQ(arrayNames.at(5), "Kanal2_Q");

  auto nofIValues = file.getArraySize(arrayNames[0]);
  vector<float> iValues(nofIValues);
  ret = file.readArray(arrayNames[0], iValues, nofIValues);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(nofIValues, 7);

  auto nofQValues = file.getArraySize(arrayNames[1]);
  vector<float> qValues(nofQValues);
  ret = file.readArray(arrayNames[1], qValues, nofQValues);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(nofQValues, 7);

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = file.getMetadata(channels, metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_EQ(channels.size(), 3);

  //Check channel infos
  double expClockRate = 32000000;
  double expCenterFreq = 13250000000.000000;
  ASSERT_EQ(expClockRate, channels[0].getClockRate());
  ASSERT_EQ(expCenterFreq, channels[0].getFrequency());

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TEST_F(CsvTest, AccessorFormatSpecifier)
{
  const string filename = Common::TestOutputDir + "AccessorFormatSpecifier.csv";
  
  IqCsv file(filename);
  std::string format = "ANY_FORMAT";
  auto ret = file.setFormatSpecifier(format);
  ASSERT_EQ(ret, ErrorCodes::Success);
  std::string getFormat = file.getFormatSpecifier();
  ASSERT_EQ(format, getFormat);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv file2(filename);
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 13));
  ret = file2.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file2.setFormatSpecifier(format);
  ASSERT_EQ(ret, ErrorCodes::WriterAlreadyInitialized);
  ret = file2.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(CsvTest, AccessorFormatSpecifierChannelInfo)
{
  const string filename = Common::TestOutputDir + "AccessorFormatSpecifierChannelInfo.csv";

  IqCsv file(filename);
  std::string format = "ANY_FORMAT";
  auto ret = file.setFormatSpecifierChannelInfo(format);
  ASSERT_EQ(ret, ErrorCodes::Success);
  std::string getFormat = file.getFormatSpecifierChannelInfo();
  ASSERT_EQ(format, getFormat);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv file2(filename);
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 13));
  ret = file2.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file2.setFormatSpecifierChannelInfo(format);
  ASSERT_EQ(ret, ErrorCodes::WriterAlreadyInitialized);
  ret = file2.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(CsvTest, AccessorSeperatorConfig)
{
  const string filename = Common::TestOutputDir + "AccessorSeperatorConfig.csv";

  IqCsv file(filename);
  CsvSeparatorConfig config = CsvSeparatorConfig::DecimalDotValueColon;
  auto ret = file.setSeparatorConfiguration(config);
  ASSERT_EQ(ret, ErrorCodes::Success);
  CsvSeparatorConfig getConfig = file.getSeparatorConfiguration();
  ASSERT_EQ(config, getConfig);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv file2(filename);
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 13));
  ret = file2.writeOpen(IqDataFormat::Complex, 2, "app", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file2.setSeparatorConfiguration(config);
  ASSERT_EQ(ret, ErrorCodes::WriterAlreadyInitialized);
  ret = file2.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(CsvTest, TestWithoutChannelNames)
{
  const string filename = Common::TestOutputDir + "TestWithoutChannelNames.csv";

  string channelName = "";

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo(channelName, 12, 15));
  channelInfos.push_back(ChannelInfo(channelName, 12, 16));

  IqCsv file(filename);

  auto ret = file.writeOpen(IqDataFormat::Complex, 4, "name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv readFile(filename);
  vector<string> arrayNames;
  ret = readFile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(arrayNames[0], ChannelInfo::getDefaultArrayName(0, true));
  ASSERT_EQ(arrayNames[1], ChannelInfo::getDefaultArrayName(0, false));
  ASSERT_EQ(arrayNames[2], ChannelInfo::getDefaultArrayName(1, true));
  ASSERT_EQ(arrayNames[3], ChannelInfo::getDefaultArrayName(1, false));

  ret = readFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(CsvTest, TestWithDuplicateChannelNames)
{
  const string filename = Common::TestOutputDir + "TestWithDuplicateChannelNames.csv";

  string channelName = "Kanal";

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo(channelName, 12, 15));
  channelInfos.push_back(ChannelInfo(channelName, 12, 16));

  IqCsv file(filename);

  auto ret = file.writeOpen(IqDataFormat::Complex, 4, "name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::InconsistentInputData);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TEST_F(CsvTest, TestUnequalArrayLengthsComplex)
{
  const string filename = Common::TestOutputDir + "TestUnequalArrayLengthsComplex.csv";

  vector<vector<float>> data(4);
  vector<float> sh(2);
  vector<float> lo(3);
  iota(sh.begin(), sh.end(), 1.0f);
  iota(lo.begin(), lo.end(), 5.0f);
  data[0] = sh;
  data[1] = sh;
  data[2] = lo;
  data[3] = lo;

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 15));
  channelInfos.push_back(ChannelInfo("Kanal2", 12, 16));

  IqCsv file(filename);

  auto ret = file.writeOpen(IqDataFormat::Complex, 4, "name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv readfile(filename);
  vector<string> arrayNames;
  ret = readfile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(arrayNames.size(), 4);
  for (size_t i = 0; i < arrayNames.size(); ++i)
  {
    ASSERT_EQ(readfile.getArraySize(arrayNames[i]), 2 * data[i].size());
  }

  remove(filename.c_str());
}

TEST_F(CsvTest, TestUnequalArrayLengthsReal)
{
  const string filename = Common::TestOutputDir + "TestUnequalArrayLengthsReal.csv";

  vector<vector<float>> data(2);
  vector<float> sh(2);
  vector<float> lo(3);
  iota(sh.begin(), sh.end(), 1.0f);
  iota(lo.begin(), lo.end(), 5.0f);
  data[0] = sh;
  data[1] = lo;

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Kanal1", 12, 15));
  channelInfos.push_back(ChannelInfo("Kanal2", 12, 16));

  IqCsv file(filename);

  auto ret = file.writeOpen(IqDataFormat::Real, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  IqCsv readfile(filename);
  vector<string> arrayNames;
  ret = readfile.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ASSERT_EQ(arrayNames.size(), 2);
  for (size_t i = 0; i < arrayNames.size(); ++i)
  {
    ASSERT_EQ(readfile.getArraySize(arrayNames[i]), 2 * data[i].size());
  }

  remove(filename.c_str());
}

TEST_F(CsvTest, TestWriteDataColumnMajor)
{
  const string filename = Common::TestOutputDir + "TestWriteDataColumnMajor.csv";

  vector<vector<float>> data;
  Common::initVector(data, 4, 1);

  vector<ChannelInfo> channelInfosWrite;
  channelInfosWrite.push_back(ChannelInfo("Kanal0", 12, 45));
  channelInfosWrite.push_back(ChannelInfo("Kanal1", 45, 23));

  string applicNameWritten = "CSVWriteTest";
  string commentWritten = "a comment ...";

  IqCsv writeFile(filename);
  writeFile.setSeparatorConfiguration(CsvSeparatorConfig::DecimalColonValueSemicolon);
  
  auto ret = writeFile.writeOpen(IqDataFormat::Complex, 4, "appl name", "comment", channelInfosWrite);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = writeFile.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = writeFile.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  fstream stream(filename.c_str());

  string line;
  //skip header
  for (int i = 0; i < 18; ++i)
  {
    getline(stream, line);
  }

  while (getline(stream, line))
  {
    vector<string> cols;
    Common::strSplit(line, ';', cols);

    for (int i = 0; i < cols.size(); ++i)
    {
      stringstream ss(cols[i], stringstream::in | stringstream::out);
      float value;
      ss >> value;

      auto b = data[i][0];

      ASSERT_NEAR(value, data[i][0], 0.0001) << "I/Q Data Order Wrong: Not Column-Major.";
    }
  }

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(CsvSeparatorTest, TestSeparatorConfig)
{
  const string filename = Common::TestOutputDir + "TestSeparatorConfig.csv";
  auto separator = TypeParam::Config;

  IqCsv file(filename);
  file.setSeparatorConfiguration(separator);

  switch (separator)
  {
  case CsvSeparatorConfig::DecimalColonValueSemicolon:
    ASSERT_EQ(file.getValueSeparator(), ';') << "unexpected value separator";
    ASSERT_EQ(file.getDecimalSeparator(), ',') << "unexpected decimal separator";;
    break;
  case CsvSeparatorConfig::DecimalDotValueColon:
    ASSERT_EQ(file.getValueSeparator(), ',') << "unexpected value separator";
    ASSERT_EQ(file.getDecimalSeparator(), '.') << "unexpected decimal separator";;
    break;
  case CsvSeparatorConfig::DecimalDotValueSemicolon:
    ASSERT_EQ(file.getValueSeparator(), ';') << "unexpected value separator";
    ASSERT_EQ(file.getDecimalSeparator(), '.') << "unexpected decimal separator";;
    break;
  }

  remove(filename.c_str());
}

TYPED_TEST(CsvSeparatorTest, TestCorrectSeparator)
{
  const string filename = Common::TestOutputDir + "TestCorrectSeparator.csv";
  auto separator = TypeParam::Config;

  vector<vector<typename TypeParam::Dt>> values;
  Common::initVector(values, 2, 6);

  vector<ChannelInfo> channelinfos;
  channelinfos.push_back(ChannelInfo("Kanael1", 12, 15));

  IqCsv file(filename);
  file.setSeparatorConfiguration(separator);
  char sepChar = file.getValueSeparator();

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelinfos);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  
  ASSERT_EQ(ret, ErrorCodes::Success);

  fstream stream(filename.c_str());

  string line;
  getline(stream, line);
  auto l = line.size();
  string substr = line.substr(l-1, 1);
  ASSERT_EQ(substr, string(&sepChar, 1));

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(CsvSeparatorTest, TestSeparatorInMetaData)
{
  const string filename = Common::TestOutputDir + "TestSeparatorInMetaData.csv";
  auto separator = TypeParam::Config;

  vector<vector<typename TypeParam::Dt>> values;
  Common::initVector(values, 2, 6);

  vector<ChannelInfo> channelinfos;
  channelinfos.push_back(ChannelInfo("Kanael1", 12, 15));

  IqCsv file(filename);
  file.setSeparatorConfiguration(separator);

  string invalidString = "Invalid;StringValue";
  if (separator == CsvSeparatorConfig::DecimalDotValueColon)
  {
    invalidString = "Invalid,StringValue";
  }

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, invalidString, "comment", channelinfos);
  ASSERT_EQ(ret, ErrorCodes::StringValuesContainValueSeparator);

  ret = file.writeOpen(IqDataFormat::Complex, 2, "name", invalidString, channelinfos);
  ASSERT_EQ(ret, ErrorCodes::StringValuesContainValueSeparator);

  map<string, string> metadata;
  metadata.insert(make_pair(invalidString, "valueForInvalidString"));
  ret = file.writeOpen(IqDataFormat::Complex, 2, "name", invalidString, channelinfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::StringValuesContainValueSeparator);

  metadata.clear();
  metadata.insert(make_pair("keyForInvalidString", invalidString));
  ret = file.writeOpen(IqDataFormat::Complex, 2, "name", invalidString, channelinfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::StringValuesContainValueSeparator);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  remove(filename.c_str());
}

TYPED_TEST(CsvSeparatorTest, TestWrittenMetaData)
{
  const string filename = Common::TestOutputDir + "TestWrittenMetaData.csv";
  auto separator = TypeParam::Config;

  vector<vector<typename TypeParam::Dt>> values;
  Common::initVector(values, 2, 6);

  vector<ChannelInfo> channelinfos;
  string channelName = "Kanal1";
  channelinfos.push_back(ChannelInfo(channelName, 12, 15));

  string applicNameWritten = "CSVWriteTest";
  string commentWritten = "a comment ...";

  map<string, string> metadata;
  string metadata_key1 = "Key1<>";
  string metadata_key2 = "2 Key";
  string metadata_key3 = "Rohde&Schwarz";
  string metadata_val1 = "Wert1";
  string metadata_val2 = "Wert2";
  string metadata_val3 = "Wert3";
  metadata.insert(make_pair(metadata_key1, metadata_val1));
  metadata.insert(make_pair(metadata_key2, metadata_val2));
  metadata.insert(make_pair(metadata_key3, metadata_val3));

  IqCsv file(filename);
  file.setSeparatorConfiguration(separator);

  auto ret = file.writeOpen(IqDataFormat::Complex, 2, applicNameWritten, commentWritten, channelinfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(values);
  ASSERT_EQ(ret, ErrorCodes::Success);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  // Read created file with plain StreamReader
  // Order of meta data lines in meta data block shall not be checked. -

  string lineApplicName = Constants::XmlApplicationName + file.getValueSeparator() + applicNameWritten;
  bool foundApplicName = false;
  string lineComment = Constants::XmlComment + file.getValueSeparator() + commentWritten;
  bool foundComment = false;
  string lineFormat = Constants::XmlFormat + file.getValueSeparator() + IqDataFormatNames[IqDataFormat::Complex];
  bool formatFound = false;
  
  string lineType;
  if (is_same<typename TypeParam::Dt, float>::value)
  {
    lineType = Constants::XmlDataType + file.getValueSeparator() + IqDataTypeNames[IqDataType::Float32];
  }
  else if (is_same<typename TypeParam::Dt, double>::value)
  {
    lineType = Constants::XmlDataType + file.getValueSeparator() + IqDataTypeNames[IqDataType::Float64];
  }
  bool typeFound = false;

  string lineChannelName = "Ch1_" + Constants::XmlChannelName + file.getValueSeparator() + channelName;
  bool foundChannelName = false;
  string lineChannelSamples = "Ch1_" + Constants::XmlSamples + file.getValueSeparator();
  bool foundSamples = false;
  
  bool foundClock = false;
  bool foundCenterFreq = false;
  string lineClock;
  string lineCenterFreq;
#ifdef _WIN32

#if _MSC_VER >= 1900 // from VS2015
  lineClock = "Ch1_" + Constants::XmlClock + "[Hz]" + file.getValueSeparator() + "1.2000000e+01";
  lineCenterFreq = "Ch1_" + Constants::XmlCenterFrequency + "[Hz]" + file.getValueSeparator() + "1.5000000e+01";
#else
  lineClock = "Ch1_" + Constants::XmlClock + "[Hz]" + file.getValueSeparator() + "1.2000000e+001";
  lineCenterFreq = "Ch1_" + Constants::XmlCenterFrequency + "[Hz]" + file.getValueSeparator() + "1.5000000e+001";
#endif
#else
  lineClock = "Ch1_" + Constants::XmlClock + "[Hz]" + file.getValueSeparator()  + "1.2000000e+01";
  lineCenterFreq = "Ch1_" + Constants::XmlCenterFrequency + "[Hz]" + file.getValueSeparator() + "1.5000000e+01";
#endif
  if (file.getDecimalSeparator() == Constants::SeparatorColon)
  {
    Common::strReplace(lineClock, ".", ",");
    Common::strReplace(lineCenterFreq, ".", ",");
  }

  string lineOptMetaData = metadata_key2 + file.getValueSeparator() + metadata_val2;
  bool foundOptMetaData = false;

  fstream stream(filename.c_str());

  string line;
  while (getline(stream, line))
  {
    if (Common::strStartsWithIgnoreCase(line, lineApplicName))
    {
      if (!foundApplicName)
      {
        foundApplicName = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one application name line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineComment))
    {
      if (!foundComment)
      {
        foundComment = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one comment line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineFormat))
    {
      if (!formatFound)
      {
        formatFound = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one format line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineType))
    {
      if (!typeFound)
      {
        typeFound = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one type line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineChannelName))
    {
      if (!foundChannelName)
      {
        foundChannelName = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one channel name for channel 1 line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineChannelSamples))
    {
      if (!foundSamples)
      {
        foundSamples = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one samples for channel 1 line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineClock))
    {
      if (!foundClock)
      {
        foundClock = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one clock for channel 1 line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineCenterFreq))
    {
      if (!foundCenterFreq)
      {
        foundCenterFreq = true;
      }
      else
      {
        ASSERT_FALSE(true) << "did create more than one center frequency for channel 1 line!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, lineOptMetaData))
    {
      if (!foundOptMetaData)
      {
        foundOptMetaData = true;
      }
      else
      {
        ASSERT_FALSE(true) << "create more than one line for one meta data dictionary entry!";
      }
    }

    if (Common::strStartsWithIgnoreCase(line, Constants::EndUserData))
    {
      break;
    }
  }

  ASSERT_TRUE(foundApplicName) << "did not save application name correctly!";
  ASSERT_TRUE(foundComment) << "did not save application name correctly!";
  ASSERT_TRUE(formatFound) << " did not save center frequency per channel correctly!";
  ASSERT_TRUE(typeFound) << " did not save center frequency per channel correctly!";
  ASSERT_TRUE(foundChannelName) << "did not save channel name per channel correctly!";
  ASSERT_TRUE(foundSamples) << " did not save samples per channel correctly!";
  ASSERT_TRUE(foundClock) << "did not save clock per channel correctly!";
  ASSERT_TRUE(foundCenterFreq) << " did not save center frequency per channel correctly!";
  ASSERT_TRUE(foundOptMetaData) << " did not save center frequency per channel correctly!";

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(CsvFormatTest, TestGeneralTagsHeaderSection)
{
  const string filename = Common::TestOutputDir + "TestGeneralTagsHeaderSection.csv";

  vector<vector<typename TypeParam::Dt>> values;
  Common::initVector(values, 2, 6);

  vector<ChannelInfo> channelinfos;
  channelinfos.push_back(ChannelInfo("Kanael1", 12, 15));
  if (TypeParam::Format == IqDataFormat::Real)
  {
    // In this case 2 value arrays are 2 channels ...
    channelinfos.push_back(ChannelInfo("Kanal2", 12, 15));
  }

  map<string, string> metadata;
  metadata.insert(make_pair("key", "value"));

  IqCsv file(filename);
  auto ret = file.writeOpen(TypeParam::Format, 2, "appl name", "comment", channelinfos, &metadata);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(values);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  fstream stream(filename.c_str());

  string line;
  getline(stream, line);

  line = line.substr(3, line.size() - 3);
  
  // First line must be MOSAIK creation identifier
  string creationIdentifier = "saved by Rohde & Schwarz MOSAIK DataImportExport (version";
  ASSERT_TRUE(Common::strStartsWithIgnoreCase(line, creationIdentifier));

  // Second line must be start of mandatory data
  getline(stream, line);
  ASSERT_TRUE(Common::strStartsWithIgnoreCase(line, Constants::XmlMandatoryUserData));

  bool foundFormat = false;
  while(getline(stream, line))
  {
    if (Common::strStartsWithIgnoreCase(line, Constants::XmlFormat))
    {
      foundFormat = true;
      break;
    }
  }

  ASSERT_TRUE(foundFormat) << "Format tag not found";

  bool foundDataType = false;
  while(getline(stream, line))
  {
    if (Common::strStartsWithIgnoreCase(line, Constants::XmlDataType))
    {
      foundDataType = true;
      break;
    }
  }

  ASSERT_TRUE(foundDataType) << "DataType tag not found";

  bool foundOptUserData = false;
  while(getline(stream, line))
  {
    if (Common::strStartsWithIgnoreCase(line, Constants::XmlOptionalUserData))
    {
      foundOptUserData = true;
      break;
    }
  }

  ASSERT_TRUE(foundOptUserData) << "OptUserData tag not found";

  bool foundEndTag = false;
  while(getline(stream, line))
  {
    if (Common::strStartsWithIgnoreCase(line, Constants::EndUserData))
    {
      foundEndTag = true;
      break;
    }
  }

  ASSERT_TRUE(foundEndTag) << "EndUser tag not found";

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(CsvChannelTest, TestEqualNrOfColumns)
{
  const string filename = Common::TestOutputDir + "TestEqualNrOfColumns.csv";

  IqDataFormat format = TypeParam::Format;
  int nofChannels = TypeParam::Channels;
  CsvSeparatorConfig config = TypeParam::Config;

  vector<ChannelInfo> channelInfo;
  for (int i = 0; i < nofChannels; ++i)
  {
    channelInfo.push_back(ChannelInfo("Kanal" + to_string(i), 12, 23));
  }

  int arrayCount = 2 * nofChannels;
  if (format == IqDataFormat::Real)
  {
    arrayCount = nofChannels;
  }

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, arrayCount, 5);

  IqCsv file(filename);
  file.setSeparatorConfiguration(config);
  char sep = file.getValueSeparator();

  auto ret = file.writeOpen(format, arrayCount, "name", "comment", channelInfo);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  fstream stream(filename.c_str());

  int columnCount = max(2, arrayCount);
  
  //Check for number of columns by counting the value separators per line!
  string line;
  while(getline(stream, line))
  {
    vector<string> cols;
    Common::strSplit(line, sep, cols);
    ASSERT_EQ(cols.size(), columnCount) << " line does not contain correct number of separators";
  }

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(CsvChannelTest, TestEqualNrOfColumnsDifferingSamples)
{
  const string filename = Common::TestOutputDir + "TestEqualNrOfColumnsDifferingSamples.csv";

  IqDataFormat format = TypeParam::Format;
  int indexOfShorterArray = TypeParam::Channels;
  CsvSeparatorConfig config = TypeParam::Config;

  const int nofChannels = 3;

  vector<ChannelInfo> channelInfo;
  for (int i = 0; i < nofChannels; ++i)
  {
    channelInfo.push_back(ChannelInfo("Kanal" + to_string(i), 12, 23));
  }

  int arrayCount = 2 * nofChannels;
  if (format == IqDataFormat::Real)
  {
    arrayCount = nofChannels;
  }

  vector<vector<float>> data(arrayCount);
  vector<float> sh(5);
  vector<float> lo(7);
  iota(sh.begin(), sh.end(), 1.0f);
  iota(lo.begin(), lo.end(), 5.0f);
  
  for (int i = 0; i < arrayCount; ++i)
  {
    if (format == IqDataFormat::Real)
    {
      if (indexOfShorterArray == i)
      {
        data[i] = sh;
      }
      else
      {
        data[i] = lo;
      }
    }
    else
    {
      if (indexOfShorterArray = (int)(i / 2))
      {
        data[i] = sh;
      }
      else
      {
        data[i] = lo;
      }
    }
  }

  IqCsv file(filename);
  file.setSeparatorConfiguration(config);
  char sep = file.getValueSeparator();

  auto ret = file.writeOpen(format, arrayCount, "name", "comment", channelInfo);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.appendArrays(data);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  fstream stream(filename.c_str());

  int columnCount = max(2, arrayCount);

  //Check for number of columns by counting the value separators per line!
  string line;
  while(getline(stream, line))
  {
    vector<string> cols;
    Common::strSplit(line, sep, cols);
    ASSERT_EQ(cols.size(), columnCount) << " line does not contain correct number of separators";
  }

  stream.close();
  remove(filename.c_str());
}

TYPED_TEST(RawFileTest, TestRawArrayDimensions)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  auto cols = file.getNofCols();
  ASSERT_EQ(cols, 6);

  auto rows = file.getNofRows(0);
  ASSERT_EQ(rows, 7);
  rows = file.getNofRows(1);
  ASSERT_EQ(rows, 7);
  rows = file.getNofRows(2);
  ASSERT_EQ(rows, 5);
  rows = file.getNofRows(3);
  ASSERT_EQ(rows, 5);
  rows = file.getNofRows(4);
  ASSERT_EQ(rows, 7);
  rows = file.getNofRows(5);
  ASSERT_EQ(rows, 7);

  auto ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, TestReadRawVector)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  vector<typename TypeParam::Dt> values;
  auto ret = file.readRawArray(3, file.getNofRows(3), values);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NEAR(values[0], 1111.1111, 0.001);
  ASSERT_NEAR(values[1], 2222.2222, 0.001);
  ASSERT_NEAR(values[2], 3333.3333, 0.001);
  ASSERT_NEAR(values[3], 4444.4444, 0.001);
  ASSERT_NEAR(values[4], 5555.5555, 0.001);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, TestReadRawVectorOffset)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  vector<typename TypeParam::Dt> values;
  auto ret = file.readRawArray(3, 2, values, 3);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NEAR(values[0], 4444.4444, 0.001);
  ASSERT_NEAR(values[1], 5555.5555, 0.001);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, TestReadRawVectorInvalidOffset)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  vector<typename TypeParam::Dt> values;
  auto ret = file.readRawArray(3, 2, values, 8);
  ASSERT_EQ(ret, ErrorCodes::CsvUnexpectedEndOfFile);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, TestReadRawVectorInvalidRange)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  vector<typename TypeParam::Dt> values;
  auto ret = file.readRawArray(3, 10, values, 2);
  ASSERT_EQ(ret, ErrorCodes::InvalidDataInterval);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, TestReadRawArray)
{
  const string filename = Common::TestDataDir + RawFiles[TypeParam::FileIndex];

  IqCsv file(filename);

  typename TypeParam::Dt* values = new typename TypeParam::Dt[5];

  auto ret = file.readRawArray(3, file.getNofRows(3), values);
  ASSERT_EQ(ret, ErrorCodes::Success);
  ASSERT_NEAR(values[0], 1111.1111, 0.001);
  ASSERT_NEAR(values[1], 2222.2222, 0.001);
  ASSERT_NEAR(values[2], 3333.3333, 0.001);
  ASSERT_NEAR(values[3], 4444.4444, 0.001);
  ASSERT_NEAR(values[4], 5555.5555, 0.001);

  ret = file.close();
  ASSERT_EQ(ret, ErrorCodes::Success);

  delete [] values;
}

TYPED_TEST(RawFileTest, ReadInvalidRawCsv)
{
  const string filename = Common::TestDataDir + "RawArrayInvalid.csv";

  IqCsv file(filename);

  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::UnsupportedCsvFormat);

  auto cols = file.getNofCols();
  ASSERT_EQ(cols, -1);

  auto rows = file.getNofRows(0);
  ASSERT_EQ(rows, -1);

  vector<typename TypeParam::Dt> values;
  ret = file.readRawArray(0, 2, values);
  ASSERT_NE(ret, ErrorCodes::Success);
}

TYPED_TEST(RawFileTest, ReadInvalidRawCsvString)
{
  const string filename = Common::TestDataDir + "RawArrayInvalidWithString.csv";

  IqCsv file(filename);

  vector<string> arrayNames;
  auto ret = file.readOpen(arrayNames);
  ASSERT_EQ(ret, ErrorCodes::UnsupportedCsvFormat);

  auto cols = file.getNofCols();
  ASSERT_EQ(cols, 6);

  auto rows = file.getNofRows(0);
  ASSERT_EQ(rows, 5);

  for (size_t i = 0; i < cols; ++i)
  {
    vector<double> readData(rows);
    ret = file.readRawArray(i, rows, readData);
  }
}

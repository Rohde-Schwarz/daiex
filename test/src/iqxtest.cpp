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

const size_t KB = 1024;
const size_t MB = KB * 1024;
const size_t MB10 = MB * 10;
const size_t GB = MB * 1024;

class IqxTest : public ::testing::Test
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

void i2f(int32_t ctr, float &i, float &q)
{
	int32_t hctr = ctr / INT16_MAX;
	int32_t lctr = ctr % INT16_MAX;
	i = ((float)hctr / INT16_MAX);
	q = ((float)lctr / INT16_MAX);
}
void i2d(int32_t ctr, double &i, double &q)
{
	int32_t hctr = ctr / INT16_MAX;
	int32_t lctr = ctr % INT16_MAX;
	i = ((double)hctr / INT16_MAX);
	q = ((double)lctr / INT16_MAX);
}
void f2i(float i, float q, int32_t &ctr)
{
	i *= INT16_MAX;
	q *= INT16_MAX;
	ctr = i * INT16_MAX + q + .01;
}
void d2i(double i, double q, int32_t &ctr)
{
	i *= INT16_MAX;
	q *= INT16_MAX;
	ctr = i * INT16_MAX + q + .01;
}

TEST_F(IqxTest, CounterTest)
{
	int32_t ctr;
	float i;
	float q;
	i2f(0, i, q);
	f2i(i, q, ctr);
	EXPECT_EQ(0, ctr) << "CounterTest 0 failed";
	i2f(1000, i, q);
	f2i(i, q, ctr);
	EXPECT_EQ(1000, ctr) << "CounterTest 1000 failed";
	i2f(1000000000, i, q);
	f2i(i, q, ctr);
	EXPECT_EQ(1000000000, ctr) << "CounterTest 1000000000 failed";
}

string fileTypeMatlab(string fileName)
{
	IqMatlab Iqm(fileName);
	vector<string> arrayNames;
	int res = Iqm.matchArrayDimensions(1, 1, false, arrayNames);
	if (res != (int)ErrorCodes::Success)
	{
		return "Matlab broken";
	}
	res = Iqm.readOpen(arrayNames);
	Iqm.close();
	if (res == (int)ErrorCodes::Success)
	{
		return "Matlab full";
	}
	return "Matlab raw";
}

string fileTypeCsv(string fileName)
{
	IqCsv Iqc(fileName);
	vector<string> arrayNames;
	if (Iqc.getNofCols() == 0)
	{
		return "CSV broken";
	}
	int res = Iqc.readOpen(arrayNames);
	Iqc.close();
	if (res == (int)ErrorCodes::Success)
	{
		return "CSV full";
	}
	return "CSV raw";
}

#if 0
TEST_F(IqxTest, withGps)
{

  const string inIqxStr = "d:/iqwfiles/6121b040-f8a6-40e6-9c66-e421ac0d6864.iqx";
  Iqx inIqx(inIqxStr);

  vector<string> arrayNames;
  int ret = inIqx.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  int ret2 = inIqx.getMetadata(channels, metadata);
  EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
  vector<float> values1;
  vector<float> values2;
  size_t samples = channels[0].getSamples();
  std::cout << samples << " Samples\n";
  const size_t no = 10000;
  for (size_t i = 0; i < samples - no; i += samples / 100)
  {
    std::cout << "read " << no << " at " << i << "\n";
    ret = inIqx.readArray(channels[0].getChannelName() + "_I", values1, no, i);
    EXPECT_EQ(ErrorCodes::Success, ret) << "read failed";
    ret = inIqx.readArray(channels[0].getChannelName() + "_Q", values2, no, i);
    EXPECT_EQ(ErrorCodes::Success, ret) << "read failed";
  }
}
#endif

#if 0
TEST_F(IqxTest, muchTriggers)
{

  const string inIqxStr = "D:/iqwfiles/336569e6-f8f2-4eab-b9e0-382e0123ae1a.iqx";
  Iqx inIqx(inIqxStr);

  vector<string> arrayNames;
  int ret = inIqx.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  int ret2 = inIqx.getMetadata(channels, metadata);
  EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
}
#endif

#if 0
TEST_F(IqxTest, ihaveCueAndTrigger)
{
  //const string inIqxStr = "F:/iqx files/4b1000cf-96ea-4375-878f-60f9aa4d5b51.iqx";
  const string inIqxStr = "E:/iqx files/ihavecueandtrigger.iqx";
  Iqx inIqx(inIqxStr);

  vector<string> arrayNames;
  int ret = inIqx.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = inIqx.getMetadata(channels, metadata);
  EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
  std::cout << "MarkerInfo: " << metadata["Ch1_MarkerInfo[XML]"];
  EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
  vector<float> values1;
  vector<float> values2;
  size_t samples = channels[0].getSamples();
  std::cout << samples << " Samples\n";
  const size_t no = 10000;
  for (size_t i = 0; i < samples - no; i += samples / 100)
  {
    std::cout << "read " << no << " at " << i << "\n";
    ret = inIqx.readArray(channels[0].getChannelName() + "_I", values1, no, i);
    EXPECT_EQ(ErrorCodes::Success, ret) << "read failed";
    ret = inIqx.readArray(channels[0].getChannelName() + "_Q", values2, no, i);
    EXPECT_EQ(ErrorCodes::Success, ret) << "read failed";
  }
  //std::cout << "press enter\n"; string s; getline(std::cin, s);

}
#endif

#if 0
TEST_F(IqxTest, ronzheimerWlan)
{

	vector<string> fileNames;
	//fileNames.push_back("E:/1e wlan/2762f9eb-f0a7-4a5a-a639-61860bb15c6d.iqx");
	//fileNames.push_back("E:/1e wlan/72f55884-7fa6-4397-901b-cb03ba44152f.iqx");
	///fileNames.push_back("E:/1e wlan/90d710f8-1798-4cfe-96fe-201cfa0868c6.iqx");
	fileNames.push_back("c:/temp/90d710f8-1798-4cfe-96fe-201cfa0868c6.iqx");
	for (auto fileName : fileNames)
	{
		Iqx inIqx(fileName);

		vector<string> arrayNames;
		int ret = inIqx.readOpen(arrayNames);
		EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
		vector<ChannelInfo> channels;
		map<string, string> metadata;
		ret = inIqx.getMetadata(channels, metadata);
		EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
		EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
		vector<float> valuesIQ;
		std::cout << channels[0].getSamples() << " Samples\n";
		ret = inIqx.readChannel(channels[0].getChannelName(), valuesIQ, channels[0].getSamples(), 0);
		inIqx.close();

		string outIqtarStr = fileName + ".iq.tar";
		IqTar iqtar(outIqtarStr);
		ret = iqtar.writeOpen(IqDataFormat::Complex, 1, "iqx to iqtar conversion", fileName.c_str(), channels, &metadata);
		ASSERT_EQ(ret, ErrorCodes::Success);
		vector<vector<float>> writeChan;
		writeChan.push_back(valuesIQ);
		ret = iqtar.appendChannels(writeChan);
		iqtar.close();
#if 0
		string outIqcsvStr = fileName + ".csv";
		IqCsv iqcsv(outIqcsvStr);
		ret = iqcsv.writeOpen(IqDataFormat::Complex, 1, "iqx to csv conversion", fileName.c_str(), channels, &metadata);
		ASSERT_EQ(ret, ErrorCodes::Success);
		//vector<vector<float>> writeChan;
		//writeChan.push_back(valuesIQ);
		ret = iqcsv.appendChannels(writeChan);
		iqcsv.close();
#endif
	}
}
#endif


#if 0

TEST_F(IqxTest, haunolderFormatTest)
{
	const string ft1 = Common::TestOutputDir + "vse_all_cmd_iq_v4_simple.mat";
	const string ft2 = Common::TestOutputDir + "out.m";
	const string ft3 = Common::TestOutputDir + "vse_raw_iq_data.csv";

	std::cout << "File type (" << ft1 << ") = " << fileTypeMatlab(ft1) << "\n";
	std::cout << "File type (" << ft2 << ") = " << fileTypeMatlab(ft2) << "\n";
	std::cout << "File type (" << ft3 << ") = " << fileTypeCsv(ft3) << "\n";
	std::cout << "File type (" << ft3 << ") = " << fileTypeMatlab(ft3) << "\n";
	std::cout << "File type (" << ft1 << ") = " << fileTypeCsv(ft1) << "\n";
	std::cout << "File type (" << ft2 << ") = " << fileTypeCsv(ft2) << "\n";
}


TEST_F(IqxTest, haunholderMatlab)
{
	return;
	const string inStr = Common::TestOutputDir + "vse_all_cmd_iq_v4_simple.mat";
	IqMatlab Iqm(inStr);
	vector<string> arrayNames;
	Iqm.matchArrayDimensions(1, 1, false, arrayNames);
	ASSERT_EQ(4, arrayNames.size());
	vector<double> iValues;
	vector<double> qValues;
	int ret;
	ret = Iqm.readRawArray(arrayNames[0], 0, 25304, iValues);
    ret = Iqm.readRawArray(arrayNames[0], 1, 25304, qValues);
	ret = Iqm.readRawArray(arrayNames[1], 0, 25304, iValues);
	ret = Iqm.readRawArray(arrayNames[1], 1, 25304, qValues);
	ret = Iqm.readRawArray(arrayNames[2], 0, 25304, iValues);
	ret = Iqm.readRawArray(arrayNames[2], 1, 25304, qValues);
	ret = Iqm.readRawArray(arrayNames[3], 0, 25304, iValues);
	ret = Iqm.readRawArray(arrayNames[3], 1, 25304, qValues);
	Iqm.close();
}

TEST_F(IqxTest, haunholderCsv)
{
	return;
	const string inStr = Common::TestOutputDir + "vse_raw_iq_data.csv";
	IqCsv Iqc(inStr);
	size_t cols = Iqc.getNofCols();
	vector<double> values;
	for (size_t i = 0; i < cols; i++)
	{
		Iqc.readRawArray(i, Iqc.getNofRows(i), values);
	}
	Iqc.close();
}

TEST_F(IqxTest, loan)
{
	return;

	const string inIqxStr = "E:/iqx files/v3/4d5887ca-7002-4a24-85fb-099f1b6d4646.iqx";
	const string inIqxStrLarge = "E:/iqx files/v3/745d892d-3ac7-4893-a519-b6ac02581dff.iqx";
	Iqx inIqx(inIqxStrLarge);

	vector<string> arrayNames;
	int ret = inIqx.readOpen(arrayNames);
	EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
	vector<ChannelInfo> channels;
	map<string, string> metadata;
	ret = inIqx.getMetadata(channels, metadata);
	EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
	EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
	vector<float> values1;
	vector<float> values2;
	std::cout << channels[0].getSamples() << " Samples\n";
	ret = inIqx.readArray(channels[0].getChannelName() + "_I", values1, 1e6, 0);
	ret = inIqx.readArray(channels[0].getChannelName() + "_Q", values2, 1e6, 0);
}
#endif

TEST_F(IqxTest, WriteLargeArray)
{
	return;
	try
	{   
		int retCode;
		const string outputIqx = Common::TestOutputDir + "outlargearr.iqx";
		vector<ChannelInfo> channelInfoWrite;
		channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
		map<string, string> metadataWrite;
		metadataWrite.insert(make_pair("My_Key1", "My meta data value"));

		size_t nofArraysWrite = 2;
		Iqx outIqx(outputIqx);
		//outIqx.setDataOrder(IqDataOrder::IQIQIQ);
		std::cout << "write IQX file " + outputIqx + "\n";
		std::cout << "remove\n";
		remove(outputIqx.c_str());
		std::cout << "writeOpen\n";
		retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);
		std::cout << "loop\n";
		ASSERT_EQ(0, retCode) << "write open failed";

		vector<vector<float>> writeVector;
		vector<float> iVector(MB10);
		vector<float> qVector(MB10);
		const float p180 = 3.14159265 / 180;
		for (size_t i = 0; i < 100*MB10; i++)
		{
#if 0
			iVector[i] = sin((float)i*p180);
			qVector[i] = -iVector[i];
#else
			i2f(i, iVector[i], qVector[i]);
#endif
		}
		// ... fill i/q-vectors with data ...
		std::cout << "push_back\n";
		writeVector.push_back(iVector);
		writeVector.push_back(qVector);

		// add data to Iqx
		std::cout << "appendArrays: wait for 10 '.'\n";
		time_t startTime = time(0);
		for (size_t i = 0; i < 2; i++)
		{
			if (i % 10 == 0) std::cout << ".";
			retCode = outIqx.appendArrays(writeVector);
			ASSERT_EQ(0, retCode) << "appendArrays failed";
		}
		std::cout << "\nclose\n";
    // set duration here
		outIqx.close();
		std::cout << "ready\n";
		time_t stopTime = time(0);
		std::cout << difftime(stopTime, startTime) << " seconds\n";

	    // -------------- and now read the file and compare results ----------------------------
		Iqx inIqx(outputIqx);
		//inIqx.setDataOrder(IqDataOrder::IQIQIQ);

		vector<string> arrayNames;
		int ret = inIqx.readOpen(arrayNames);
		EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
		vector<ChannelInfo> channels;
		map<string, string> metadata;
		ret = inIqx.getMetadata(channels, metadata);
		EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
		EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
		vector<float> values1;
		vector<float> values2;
		std::cout << "read and compare\n";
		startTime = time(0);
		for (int32_t r = 0; r < 2; r++)
		{
			if (r % 10 == 0) std::cout << ".";

			ret = inIqx.readArray(channels[0].getChannelName() + "_I", values1, MB10, r*MB10);
			ASSERT_EQ(0, retCode) << "readArray I failed";
			ret = inIqx.readArray(channels[0].getChannelName() + "_Q", values2, MB10, r*MB10);
			ASSERT_EQ(0, retCode) << "readArray Q failed";
			int32_t ctr;
			for (int i = 0; i < MB10; i++)
			{
				f2i(values1[i], values2[i], ctr);
				if (ctr != i /*(i + r*(int)1e7) % (int)1e7*/)
				{
					std::cout << ctr << "!=" << i << " : write/read diff!\n";
				}
			}
		}
		stopTime = time(0);
		std::cout << difftime(stopTime, startTime) << " seconds\n";

	}
	catch (const std::invalid_argument& ia) 
	{
		std::cout << "WriteLargeArray: " << ia.what() << '\n';
	}

}

TEST_F(IqxTest, WriteLargeChannel)
{
	return;
	try
	{
		int retCode;
		const string outputIqx = Common::TestOutputDir + "outlargechan.iqx";
		vector<ChannelInfo> channelInfoWrite;
		channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
		map<string, string> metadataWrite;
		metadataWrite.insert(make_pair("My_Key1", "My meta data value"));

		size_t nofArraysWrite = 2;
		Iqx outIqx(outputIqx);
		//outIqx.setDataOrder(IqDataOrder::IQIQIQ);
		std::cout << "write IQX file " + outputIqx + "\n";
		std::cout << "remove\n";
		remove(outputIqx.c_str());
		std::cout << "writeOpen\n";
		retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);
		std::cout << "loop\n";
		ASSERT_EQ(0, retCode) << "write open failed";

		vector<vector<float>> writeVector;
		vector<float> iqVector(2 * MB10);

		const float p180 = 3.14159265 / 180;
		for (size_t i = 0; i < MB10; i++)
		{
#if 0
			iqVector[2*i] = sin((float)2*i*p180);
			iqVector[2*i+1] = -iqVector[2*i];
#else
			i2f(i, iqVector[2*i], iqVector[2*i+1]);
#endif
		}
		writeVector.push_back(iqVector);

		// add data to Iqx
		std::cout << "appendChannels: wait for 10 '.'\n";
		time_t startTime = time(0);
		for (size_t i = 0; i < 2; i++)
		{
			if (i % 10 == 0) std::cout << ".";
			retCode = outIqx.appendChannels(writeVector);
		}
		std::cout << "close\n";
		outIqx.close();
		std::cout << "ready\n";
		time_t stopTime = time(0);
		std::cout << difftime(stopTime, startTime) << " seconds\n";

		//std::cout << "Press any key to continue . . ." << std::endl;
		//_getch(); // wait for keypress


		// -------------- and now read the file and compare results ----------------------------
		Iqx inIqx(outputIqx);
		//inIqx.setDataOrder(IqDataOrder::IQIQIQ);

		vector<string> arrayNames;
		int ret = inIqx.readOpen(arrayNames);
		EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
		vector<ChannelInfo> channels;
		map<string, string> metadata;
		ret = inIqx.getMetadata(channels, metadata);
		EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
		EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
		vector<float> values1;
		vector<float> values2;

		for (int32_t r = 0; r < 2; r++)
		{
			ret = inIqx.readArray(channels[0].getChannelName() + "_I", values1, MB10, r*MB10);
			ret = inIqx.readArray(channels[0].getChannelName() + "_Q", values2, MB10, r*MB10);
			int32_t ctr;
			for (int i = 0; i < MB10; i++)
			{
				f2i(values1[i], values2[i], ctr);
				if (ctr != i)
				{
					std::cout << ctr << "!=" << i << " : write/read diff!\n";
				}
			}
		}
	}
	catch (const std::invalid_argument& ia)
	{
		std::cout << "WriteLargeChannel: " << ia.what() << '\n';
	}
}


TEST_F(IqxTest, AppendArraysFloatPointer)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendArraysFloatPointer.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendArraysFloatPointer"));

	size_t nofArraysWrite = 2;
	Iqx outIqx(outputIqx);
	//remove(outputIqx.c_str());
	retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "IQX Test", "AppendArraysFloatPointer", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<float*> writeVector;
	vector<size_t> sizeVector;
	float *iArray = new float[MB];
	float *qArray = new float[MB];
	for (size_t i = 0; i < MB; i++)
	{
		i2f(i, iArray[i], qArray[i]);
	}
	// ... fill i/q-vectors with data ...
	writeVector.push_back(iArray);
	writeVector.push_back(qArray);
	sizeVector.push_back(MB);
	sizeVector.push_back(MB);

	// add data to Iqx
	retCode = outIqx.appendArrays(writeVector, sizeVector);

	delete[] iArray;
	delete[] qArray;
	outIqx.close();
}
TEST_F(IqxTest, AppendArraysFloatVector)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendArraysFloatVector.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendArraysFloatPointer"));

	size_t nofArraysWrite = 2;
	Iqx outIqx(outputIqx);
	remove(outputIqx.c_str());
	retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "IQX Test", "AppendArraysFloatVector", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<vector<float>> writeVector;
	vector<float> iVector(MB);
	vector<float> qVector(MB);
	for (size_t i = 0; i < MB; i++)
	{
		i2f(i, iVector[i], qVector[i]);
	}
	// ... fill i/q-vectors with data ...
	writeVector.push_back(iVector);
	writeVector.push_back(qVector);
	// add data to Iqx
	retCode = outIqx.appendArrays(writeVector);
  ASSERT_EQ(0, retCode) << "append failed";
  outIqx.close();
}

TEST_F(IqxTest, AppendArraysDoublePointer)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendArraysDoublePointer.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendArraysDoublePointer"));

	size_t nofArraysWrite = 2;
	Iqx outIqx(outputIqx);
	//remove(outputIqx.c_str());
	retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "IQX Test", "AppendArraysDoublePointer", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<double*> writeVector;
	vector<size_t> sizeVector;
	double *iArray = new double[MB];
	double *qArray = new double[MB];
	for (size_t i = 0; i < MB; i++)
	{
		i2d(i, iArray[i], qArray[i]);
	}
	// ... fill i/q-vectors with data ...
	writeVector.push_back(iArray);
	writeVector.push_back(qArray);
	sizeVector.push_back(MB);
	sizeVector.push_back(MB);

	// add data to Iqx
	retCode = outIqx.appendArrays(writeVector, sizeVector);
	delete[] iArray;
	delete[] qArray;
	outIqx.close();
}
TEST_F(IqxTest, AppendArraysDoubleVector)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendArraysDoubleVector.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendArraysFloatPointer"));

	size_t nofArraysWrite = 2;
	Iqx outIqx(outputIqx);
	remove(outputIqx.c_str());
	retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "IQX Test", "AppendArraysDoubleVector", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<vector<double>> writeVector;
	vector<double> iVector(MB);
	vector<double> qVector(MB);
	for (size_t i = 0; i < MB; i++)
	{
		i2d(i, iVector[i], qVector[i]);
	}
	// ... fill i/q-vectors with data ...
	writeVector.push_back(iVector);
	writeVector.push_back(qVector);
	// add data to Iqx
	retCode = outIqx.appendArrays(writeVector);
	outIqx.close();
}

string openAndGetName(Iqx &inIqx)
{
	vector<string> arrayNamesFV;
	int ret = inIqx.readOpen(arrayNamesFV);
	EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
	vector<ChannelInfo> channels;
	map<string, string> metadata;
	ret = inIqx.getMetadata(channels, metadata);
	EXPECT_EQ(ret, ErrorCodes::Success) << "get metadata failed" ;
	EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
	return channels[0].getChannelName();
}

#if 0 // 12 Bit Test
TEST_F(IqxTest, Test12Test)
{
	const string inputIqx12 = Common::TestOutputDir + "f1259e2a-25d7-4793-a0e9-d13f5f0374ac.iqx"; // tg100
	//const string inputIqx12 = Common::TestOutputDir + "09386c4c-f860-43f9-a518-ed690063cc74.iqx"; // tg80
   Iqx inIqx12(inputIqx12);
   string chan12;
   chan12 = openAndGetName(inIqx12);
   vector<float> valuesIFV12;
   int ret = inIqx12.readArray(chan12 + "_I", valuesIFV12, 20*KB, 0/*2*MB+1*/);

   const string inputIqx16 = Common::TestOutputDir + "c001cb03-4110-4e88-9af6-3775f6820d88.iqx"; // tg100
   //const string inputIqx16 = Common::TestOutputDir + "e9d99a47-7e35-40a0-9bbe-771d1d75731b.iqx"; // tg80
   Iqx inIqx16(inputIqx16);
   string chan16;
   chan16 = openAndGetName(inIqx16);
   vector<float> valuesIFV16;
   ret = inIqx16.readArray(chan16 + "_I", valuesIFV16, 20 * KB, 0/*2 * MB + 1*/);

	
	vector<float> valuesIFV2;
	ret = inIqx12.readChannel(chan12, valuesIFV2, 20 * KB, 2 * MB + 1);
}
#endif

TEST_F(IqxTest, CompareAppendArrayFiles)
{
	const string inputIqxFV = Common::TestOutputDir + "AppendArraysFloatVector.iqx";
	Iqx inIqxFV(inputIqxFV);
	vector<float> valuesIFV;
	string chan;
	chan = openAndGetName(inIqxFV);
	int ret = inIqxFV.readArray(chan + "_I", valuesIFV, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";
	vector<float> valuesQFV;
	ret = inIqxFV.readArray(chan + "_Q", valuesQFV, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";

	const string inputIqxFP = Common::TestOutputDir + "AppendArraysFloatPointer.iqx";
	Iqx inIqxFP(inputIqxFP);
	chan = openAndGetName(inIqxFP);
	float valuesIFP[KB];
	ret = inIqxFP.readArray(chan + "_I", valuesIFP, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";
  float valuesQFP[KB];
	ret = inIqxFP.readArray(chan + "_Q", valuesQFP, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";

	const string inputIqxDV = Common::TestOutputDir + "AppendArraysDoubleVector.iqx";
	Iqx inIqxDV(inputIqxDV);
	chan = openAndGetName(inIqxDV);
	vector<double> valuesIDV;
	ret = inIqxDV.readArray(chan + "_I", valuesIDV, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";
  vector<double> valuesQDV;
	ret = inIqxDV.readArray(chan + "_Q", valuesQDV, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";

	const string inputIqxDP = Common::TestOutputDir + "AppendArraysDoublePointer.iqx";
	Iqx inIqxDP(inputIqxDP);
	chan = openAndGetName(inIqxDP);
	double valuesIDP[KB];
	ret = inIqxDP.readArray(chan + "_I", valuesIDP, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";
  double valuesQDP[KB];
	ret = inIqxDP.readArray(chan + "_Q", valuesQDP, KB, KB);
  ASSERT_EQ(0, ret) << "readArray failed";
  int32_t ctr;
	for (size_t i = 0; i < KB; i++)
	{
		f2i(valuesIFV[i], valuesQFV[i], ctr);
		ASSERT_EQ(i+KB, ctr) << "Invalid data valuesIFV, valuesQFV " << i;
		f2i(valuesIFP[i], valuesQFP[i], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data valuesIFP, valuesQFP " << i;
		d2i(valuesIDV[i], valuesQDV[i], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data valuesIDV, valuesQDV " << i;
		d2i(valuesIDP[i], valuesQDP[i], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data valuesIDP, valuesQDP " << i ;
	}
}


TEST_F(IqxTest, AppendChannelFloatPointer)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendChannelFloatPointer.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendChannelFloatPointer"));
	Iqx outIqx(outputIqx);
	retCode = outIqx.writeOpen(IqDataFormat::Complex, 1, "IQX Test", "AppendChannelFloatPointer", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<float*> writeVector;
	float *iqVector = new float[MB];

	for (size_t i = 0; i < MB/2; i++)
	{
		i2f(i, iqVector[2 * i], iqVector[2 * i + 1]);
	}
	writeVector.push_back(iqVector);
	vector<size_t> sizeVector;
	sizeVector.push_back(MB/2);

	retCode = outIqx.appendChannels(writeVector, sizeVector);
	outIqx.close();
}
TEST_F(IqxTest, AppendChannelFloatVector)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendChannelFloatVector.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendChannelFloatVector"));
	Iqx outIqx(outputIqx);
	retCode = outIqx.writeOpen(IqDataFormat::Complex, 1, "IQX Test", "AppendChannelFloatVector", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<vector<float>> writeVector;
	vector<float> iqVector(MB);

	for (size_t i = 0; i < MB/2; i++)
	{
		i2f(i, iqVector[2 * i], iqVector[2 * i + 1]);
	}
	writeVector.push_back(iqVector);

    retCode = outIqx.appendChannels(writeVector);
	outIqx.close();
}

TEST_F(IqxTest, AppendChannelDoublePointer)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendChannelDoublePointer.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendChannelDoublePointer"));
	Iqx outIqx(outputIqx);
	retCode = outIqx.writeOpen(IqDataFormat::Complex, 1, "IQX Test", "AppendChannelDoublePointer", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<double*> writeVector;
	double *iqVector = new double[MB];

	for (size_t i = 0; i < MB/2; i++)
	{
		i2d(i, iqVector[2 * i], iqVector[2 * i + 1]);
	}
	writeVector.push_back(iqVector);
	vector<size_t> sizeVector;
	sizeVector.push_back(MB/2);

	retCode = outIqx.appendChannels(writeVector, sizeVector);
	delete[] iqVector;
	outIqx.close();
}

TEST_F(IqxTest, AppendChannelDoubleVector)
{
	int retCode;
	const string outputIqx = Common::TestOutputDir + "AppendChannelDoubleVector.iqx";
	vector<ChannelInfo> channelInfoWrite;
	channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));
	map<string, string> metadataWrite;
	metadataWrite.insert(make_pair("Type", "AppendChannelDoubleVector"));
	Iqx outIqx(outputIqx);
	retCode = outIqx.writeOpen(IqDataFormat::Complex, 1, "IQX Test", "AppendChannelDoubleVector", channelInfoWrite, &metadataWrite);
	ASSERT_EQ(0, retCode) << "write open failed";

	vector<vector<double>> writeVector;
	vector<double> iqVector(MB);

	for (size_t i = 0; i < MB/2; i++)
	{
		i2d(i, iqVector[2 * i], iqVector[2 * i + 1]);
	}
	writeVector.push_back(iqVector);

	retCode = outIqx.appendChannels(writeVector);
	outIqx.close();
}

TEST_F(IqxTest, CompareAppendChannelFiles)
{
	const string inputIqxFV = Common::TestOutputDir + "AppendChannelFloatVector.iqx";
	Iqx inIqxFV(inputIqxFV);
	vector<float> valuesFV;
	string chan;
	chan = openAndGetName(inIqxFV);
	int ret = inIqxFV.readChannel(chan, valuesFV, 1E5, KB);

	const string inputIqxDV = Common::TestOutputDir + "AppendChannelDoubleVector.iqx";
	Iqx inIqxDV(inputIqxDV);
	vector<float> valuesDV;
	chan = openAndGetName(inIqxDV);
	ret = inIqxDV.readChannel(chan, valuesDV, 1E5, KB);

	const string inputIqxFP = Common::TestOutputDir + "AppendChannelFloatPointer.iqx";
	Iqx inIqxFP(inputIqxFP);
	chan = openAndGetName(inIqxFP);
	static float valuesFP[(size_t)(1E5*2)];
	ret = inIqxFP.readChannel(chan, valuesFP, 1E5, KB);

	const string inputIqxDP = Common::TestOutputDir + "AppendChannelDoublePointer.iqx";
	Iqx inIqxDP(inputIqxDP);
	chan = openAndGetName(inIqxDP);
  static double valuesDP[(size_t)(1E5 * 2)];
	ret = inIqxDP.readChannel(chan, valuesDP, 1E5, KB);

	int32_t ctr;
	for (size_t i = 0; i < 1E5; i++)
	{
		f2i(valuesFV[2*i], valuesFV[2 * i + 1], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data " << i;
		d2i(valuesDV[2 * i], valuesDV[2 * i + 1], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data " << i;
		f2i(valuesFP[2 * i], valuesFP[2 * i + 1], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data " << i;
		d2i(valuesDP[2 * i], valuesDP[2 * i + 1], ctr);
		ASSERT_EQ(i + KB, ctr) << "Invalid data " << i;
	}
}

TEST_F(IqxTest, WriteIqx)
{
  int retCode;
  const string outputIqx = Common::TestOutputDir +"out.iqx";
  vector<ChannelInfo> channelInfoWrite;
  channelInfoWrite.push_back(ChannelInfo("Channel1", 1e6, 1e6));
  map<string, string> metadataWrite;
  metadataWrite.insert(make_pair("My_Key1", "My meta data value"));

  size_t nofArraysWrite = 2;
  Iqx outIqx(outputIqx);
  std::cout << "write IQX file\n";
  remove(outputIqx.c_str());
  retCode = outIqx.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);
  ASSERT_EQ(0, retCode) << "write open failed";

  vector<vector<float>> writeVector;
  vector<float> iVector(1E5);
  vector<float> qVector(1E5);
  const float p180 = 3.14159265 / 180;
  for (size_t i = 0; i < 1E5; i++)
  {
	iVector[i] = sin((float)i*p180);
	qVector[i] = -iVector[i];
  }
  // ... fill i/q-vectors with data ...
  writeVector.push_back(iVector);
  writeVector.push_back(qVector);

  // add data to Iqx
  retCode = outIqx.appendArrays(writeVector);
  EXPECT_EQ(ErrorCodes::Success, retCode) << "append failed";
  outIqx.close();

  Iqx inIqx(outputIqx);
  vector<string> arrayNames;
  std::cout << "read IQX file\n";
  int ret = inIqx.readOpen(arrayNames);
  EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  std::cout << "getMetadata\n";
  ret = inIqx.getMetadata(channels, metadata);
  EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
  EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
  vector<float> valuesI;
  ret = inIqx.readArray(channels[0].getChannelName() + "_I", valuesI, 1e5);
  EXPECT_EQ(ErrorCodes::Success, ret) << "readArray failed";
  vector<float> valuesQ;
  ret = inIqx.readArray(channels[0].getChannelName() + "_Q", valuesQ, 1e5);
  EXPECT_EQ(ErrorCodes::Success, ret) << "readArray failed";

  for (size_t i = 0; i < 1e5; i++)
  {
    if (abs(iVector[i] - valuesI[i]) > 1E-4)
    {
      std::cout << "write/read diff!\n";
    }
    if (abs(qVector[i] - valuesQ[i]) > 1E-4)
    {
      std::cout << "write/read diff!\n";
    }
  }
  const size_t offs = 6000;
  vector<float> valuesIo;
  vector<float> valuesQo;
  ret = inIqx.readArray(channels[0].getChannelName() + "_I", valuesIo, 1e5-offs, offs);
  EXPECT_EQ(ErrorCodes::Success, ret) << "readArray failed";
  ret = inIqx.readArray(channels[0].getChannelName() + "_Q", valuesQo, 1e5-offs, offs);
  EXPECT_EQ(ErrorCodes::Success, ret) << "readArray failed";

  for (size_t i = offs; i < 1e5-offs; i++)
  {
    if (abs(iVector[i] - valuesIo[i-offs]) > 1E-4)
    {
      std::cout << "write/read diff!\n";
    }
    if (abs(qVector[i] - valuesQo[i-offs]) > 1E-4)
    {
      std::cout << "write/read diff!\n";
    }
  }
  inIqx.close();
}

TEST_F(IqxTest, Iqx2MatLab73)
{
	const string inIqxName = Common::TestOutputDir +"out.iqx";
	const string outMatName = Common::TestOutputDir +"out.m";
	Iqx inIqx(inIqxName);
	vector<string> arrayNames;
	std::cout << "read IQX file\n";
	int ret = inIqx.readOpen(arrayNames);
	EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
	vector<ChannelInfo> channels;
	map<string, string> metadata;
	std::cout << "getMetadata\n";
	ret = inIqx.getMetadata(channels, metadata);
	EXPECT_EQ(ErrorCodes::Success, ret) << "get metadata failed";
	EXPECT_EQ(channels.size(), 1) << "Unexpected number of channels read from file";
	vector<float> valuesIQ;
	ret = inIqx.readChannel(channels[0].getChannelName(), valuesIQ, channels[0].getSamples());
	inIqx.close();

	vector<vector<float>> iqdata;
	iqdata.push_back(valuesIQ);

	const string appName("IQX");
	const string comment("comment");
	IqMatlab outMat(outMatName);
	outMat.setMatlabVersion(MatlabVersion::Mat73);
	outMat.writeOpen(IqDataFormat::Complex, (size_t)1, appName, comment, channels, &metadata);

	outMat.appendChannels(iqdata);
	outMat.close();
}

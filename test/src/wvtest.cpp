#include "gtest/gtest.h"

#include "dataimportexport.h"
#include "common.h"

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <memory>
#ifdef HAS_SCRAMBLER
#include "E:/wvtst/app/WvScrambler.h"
#endif


#ifdef _WIN32
#define isfinite(x) _finite(x)
#endif

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class WvTest : public ::testing::Test
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

TEST_F(WvTest, TestMultipleOpen)
{
  const string inFile = Common::TestDataDir + "FG_Sine_0.35MHz.wv";
  vector<string> channelNames;
  vector<ChannelInfo> channelInfos;
  map<string, string> metadata;
  for (size_t i = 0; i < 10; i++)
  {
    Wv wv(inFile);
    int ret = wv.readOpen(channelNames);
    EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
    EXPECT_EQ(channelNames.size(), 1) << "open bug";
    ret = wv.getMetadata(channelInfos, metadata);
    size_t samples = channelInfos[0].getSamples();
    EXPECT_EQ(samples, 100) << "open bug";
    wv.close();
  }
}


TEST_F(WvTest, TestChannel)
{
	//const string inFile = Common::TestOutputDir + "FG_Sine_1MHz_080deg.wv";
	//const string inFile = Common::TestOutputDir + "QPSK_BurstsDown.wv";
#ifdef HAS_SCRAMBLER
	const string inFile = Common::TestOutputDir + "LTE_TDD_10MHz_MinMaxPowerv05.wv";
	//const string inFile = Common::TestOutputDir + "PowStepsMSeg.wv";
	const string outFile = Common::TestOutputDir + "LTE_TDD_10MHz_MinMaxPowerv05.iq.tar";
#else
  const string inFile = Common::TestDataDir + "FG_Sine_0.35MHz.wv";
	const string outFile = Common::TestOutputDir + "FG_Sine_0.35MHz.iq.tar";
#endif

	Wv wv(inFile);
#ifdef HAS_SCRAMBLER
	WvScrambler wvScrambler;
	wv.setScrambler(&wvScrambler);
#endif
	vector<string> channelNames;
	int ret = wv.readOpen(channelNames);
	EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
	size_t size = wv.getArraySize(channelNames[0]);
	vector<ChannelInfo> channelInfos;
	map<string, string> metadata;
	ret = wv.getMetadata(channelInfos, metadata);
	vector<float> valuesFV;
	ret = wv.readChannel(channelNames[0], valuesFV, size, 0);
	vector<double> valuesDV;
	ret = wv.readChannel(channelNames[0], valuesDV, size, 0);
	unique_ptr<float[]> valuesF(new float[size * 2]);
	ret = wv.readChannel(channelNames[0], valuesF.get(), size, 0);
	unique_ptr<double[]> valuesD(new double[size * 2]);
	ret = wv.readChannel(channelNames[0], valuesD.get(), size, 0);
	vector<float> valuesI;
	ret = wv.readArray(channelNames[0] + "_I", valuesI, size);
	vector<float> valuesQ;
	ret = wv.readArray(channelNames[0] + "_Q", valuesQ, size);
	for (size_t i = 0; i < size; i++)
	{
		if (abs(valuesFV[i] - valuesDV[i]) > 1E-4)
		{
			std::cout << "write/read diff!\n";
		}
		if (abs(valuesFV[i] - valuesF[i]) > 1E-4)
		{
			std::cout << "write/read diff!\n";
		}
		if (abs(valuesFV[i] - valuesD[i]) > 1E-4)
		{
			std::cout << "write/read diff!\n";
		}
		if (i % 2 == 0)
		{
			if (abs(valuesFV[i] - valuesI[i/2]) > 1E-4)
			{
				std::cout << "write/read diff!\n";
			}
		}
		else
		{
			if (abs(valuesFV[i] - valuesQ[i / 2]) > 1E-4)
			{
				std::cout << "write/read diff!\n";
			}
		}
	}
	wv.close();

	// write as iq tar file
	IqTar iqtar(outFile);
	ret = iqtar.writeOpen(IqDataFormat::Complex, 1, "wv to iqtar conversion", inFile.c_str(), channelInfos, &metadata);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<vector<float>> channels;
	channels.push_back(valuesFV);
	ret = iqtar.appendChannels(channels);
	iqtar.close();
}

TEST_F(WvTest, TestArray)
{
}



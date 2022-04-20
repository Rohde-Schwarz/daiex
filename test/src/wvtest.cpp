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
#include "d:/wvscrambler/WvScrambler.h"
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

#if 0
TEST_F(WvTest, TestScaling)
{
   const string inFile = "c:/temp/sim_out.wv";
   Wv wv(inFile);
   vector<string> channelNames;
   vector<ChannelInfo> channelInfos;
   map<string, string> metadata;
   int ret = wv.readOpen(channelNames);
   EXPECT_EQ(ErrorCodes::Success, ret) << "file open failed";
   EXPECT_EQ(channelNames.size(), 1) << "open bug";
   ret = wv.getMetadata(channelInfos, metadata);
   size_t samples = channelInfos[0].getSamples();
   vector<float> valuesFV;
   ret = wv.readChannel(channelNames[0], valuesFV, samples, 0);
   vector<double> valuesDV;
   ret = wv.readChannel(channelNames[0], valuesFV, samples, 0);
   ret = wv.readChannel(channelNames[0], valuesDV, samples, 0);

   wv.close();
}
#endif

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
#ifdef HAS_SCRAMBLER
	const string inFile = Common::TestOutputDir + "MyTestFile.wv";
	const string outFile = Common::TestOutputDir + "MyTestFile.iq.tar";
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
   size_t size = 512 * 1024;//wv.getArraySize(channelNames[0]);
   if (size > wv.getArraySize(channelNames[0])) size = wv.getArraySize(channelNames[0]);
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
	//wv.close();

	// write as iq tar file
	IqTar iqtar(outFile);
	ret = iqtar.writeOpen(IqDataFormat::Complex, 1, "wv to iqtar conversion", inFile.c_str(), channelInfos, &metadata);
	ASSERT_EQ(ret, ErrorCodes::Success);
	vector<vector<float>> channels;
   size = wv.getArraySize(channelNames[0])/2;
   vector<float> allValues;
   allValues.reserve(wv.getArraySize(channelNames[0]));

   for (int i = 0; i < 2; i++)
   {
      //valuesFV.clear();
      ret = wv.readChannel(channelNames[0], valuesFV, size, i*size);
      allValues.insert(allValues.end(), valuesFV.begin(), valuesFV.end());
   }
   channels.push_back(allValues);
   ret = iqtar.appendChannels(channels);
   wv.close();
   iqtar.close();
}

TEST_F(WvTest, TestArray)
{
}



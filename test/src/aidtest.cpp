#include "gtest/gtest.h"

#include "dataimportexport.h"
#include "common.h"

#ifdef _WIN32
#define isfinite(x) _finite(x)
#endif

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

class AidTest : public ::testing::Test
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

const size_t KB = 1024;
const size_t MB = KB * 1024;

TEST_F(AidTest, writeAndReadTest)
{
  float l_pfReal[] = { 0.100f, 0.200f, 0.300f, 0.400f, 0.500f,0.100f, 0.200f, 0.300f, 0.400f, 0.500f,
    0.101f, 0.201f, 0.301f, 0.401f, 0.501f,0.101f, 0.201f, 0.301f, 0.401f, 0.501f,
    0.102f, 0.202f, 0.302f, 0.402f, 0.502f,0.102f, 0.202f, 0.302f, 0.402f, 0.502f,
    0.103f, 0.203f, 0.303f, 0.403f, 0.503f,0.103f, 0.203f, 0.303f, 0.403f, 0.503f,
    0.104f, 0.204f, 0.304f, 0.404f, 0.504f,0.104f, 0.204f, 0.304f, 0.404f, 0.504f,
    0.100f, 0.200f, 0.300f, 0.400f, 0.500f,0.100f, 0.200f, 0.300f, 0.400f, 0.500f,
    0.101f, 0.201f, 0.301f, 0.401f, 0.501f,0.101f, 0.201f, 0.301f, 0.401f, 0.501f,
    0.102f, 0.202f, 0.302f, 0.402f, 0.502f,0.102f, 0.202f, 0.302f, 0.402f, 0.502f,
    0.103f, 0.203f, 0.303f, 0.403f, 0.503f,0.103f, 0.203f, 0.303f, 0.403f, 0.503f,
    0.104f, 0.204f, 0.304f, 0.404f, 0.504f,0.104f, 0.204f, 0.304f, 0.404f, 0.504f,
    0.100f, 0.200f, 0.300f, 0.400f, 0.500f,0.100f, 0.200f, 0.300f, 0.400f, 0.500f,
    0.101f, 0.201f, 0.301f, 0.401f, 0.501f,0.101f, 0.201f, 0.301f, 0.401f, 0.501f,
    0.102f, 0.202f, 0.302f, 0.402f, 0.502f,0.102f, 0.202f, 0.302f, 0.402f, 0.502f,
    0.103f, 0.203f, 0.303f, 0.403f, 0.503f,0.103f, 0.203f, 0.303f, 0.403f, 0.503f,
    0.104f, 0.204f, 0.304f, 0.404f, 0.504f,0.104f, 0.204f, 0.304f, 0.404f, 0.504f,
    0.100f, 0.200f, 0.300f, 0.400f, 0.500f,0.100f, 0.200f, 0.300f, 0.400f, 0.500f,
    0.101f, 0.201f, 0.301f, 0.401f, 0.501f,0.101f, 0.201f, 0.301f, 0.401f, 0.501f,
    0.102f, 0.202f, 0.302f, 0.402f, 0.502f,0.102f, 0.202f, 0.302f, 0.402f, 0.502f,
    0.103f, 0.203f, 0.303f, 0.403f, 0.503f,0.103f, 0.203f, 0.303f, 0.403f, 0.503f,
    0.104f, 0.204f, 0.304f, 0.404f, 0.504f,0.104f, 0.204f, 0.304f, 0.404f, 0.504f };
  float l_pfImag[] = { 0.110f, 0.210f, 0.300f, 0.410f, 0.510f,0.110f, 0.210f, 0.300f, 0.410f, 0.510f,
    0.111f, 0.211f, 0.311f, 0.411f, 0.511f,0.111f, 0.211f, 0.311f, 0.411f, 0.511f,
    0.112f, 0.212f, 0.312f, 0.412f, 0.512f,0.112f, 0.212f, 0.312f, 0.412f, 0.512f,
    0.113f, 0.213f, 0.313f, 0.413f, 0.513f,0.113f, 0.213f, 0.313f, 0.413f, 0.513f,
    0.114f, 0.214f, 0.314f, 0.414f, 0.514f,0.114f, 0.214f, 0.314f, 0.414f, 0.514f,
    0.110f, 0.210f, 0.300f, 0.410f, 0.510f,0.110f, 0.210f, 0.300f, 0.410f, 0.510f,
    0.111f, 0.211f, 0.311f, 0.411f, 0.511f,0.111f, 0.211f, 0.311f, 0.411f, 0.511f,
    0.112f, 0.212f, 0.312f, 0.412f, 0.512f,0.112f, 0.212f, 0.312f, 0.412f, 0.512f,
    0.113f, 0.213f, 0.313f, 0.413f, 0.513f,0.113f, 0.213f, 0.313f, 0.413f, 0.513f,
    0.114f, 0.214f, 0.314f, 0.414f, 0.514f,0.114f, 0.214f, 0.314f, 0.414f, 0.514f,
    0.110f, 0.210f, 0.300f, 0.410f, 0.510f,0.110f, 0.210f, 0.300f, 0.410f, 0.510f,
    0.111f, 0.211f, 0.311f, 0.411f, 0.511f,0.111f, 0.211f, 0.311f, 0.411f, 0.511f,
    0.112f, 0.212f, 0.312f, 0.412f, 0.512f,0.112f, 0.212f, 0.312f, 0.412f, 0.512f,
    0.113f, 0.213f, 0.313f, 0.413f, 0.513f,0.113f, 0.213f, 0.313f, 0.413f, 0.513f,
    0.114f, 0.214f, 0.314f, 0.414f, 0.514f,0.114f, 0.214f, 0.314f, 0.414f, 0.514f,
    0.110f, 0.210f, 0.300f, 0.410f, 0.510f,0.110f, 0.210f, 0.300f, 0.410f, 0.510f,
    0.111f, 0.211f, 0.311f, 0.411f, 0.511f,0.111f, 0.211f, 0.311f, 0.411f, 0.511f,
    0.112f, 0.212f, 0.312f, 0.412f, 0.512f,0.112f, 0.212f, 0.312f, 0.412f, 0.512f,
    0.113f, 0.213f, 0.313f, 0.413f, 0.513f,0.113f, 0.213f, 0.313f, 0.413f, 0.513f,
    0.114f, 0.214f, 0.314f, 0.414f, 0.514f,0.114f, 0.214f, 0.314f, 0.414f, 0.514f };

  Aid aid(Common::TestOutputDir + "test.aid");
  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000,10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";

  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "app name", "comment", channels, &metadata));
  vector<float *> iqdata;
  iqdata.push_back(l_pfReal);
  iqdata.push_back(l_pfImag);
  vector<size_t> sizes;
  sizes.push_back(sizeof(l_pfReal) / sizeof(float));
  sizes.push_back(sizeof(l_pfImag) / sizeof(float));
  ASSERT_EQ(0, aid.appendArrays(iqdata, sizes));
  ASSERT_EQ(0, aid.close());

  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  channels.clear();
  metadata.clear();
  vector<float> iqread;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqread,channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());
  for (size_t i = 0; i < iqread.size() / 2; i++)
  {
    if (iqread[i * 2] != l_pfReal[i])
    {
      cout << "Diff re " << i << " " << iqread[i * 2] << " " << l_pfReal[i] << '\n';
    }
    ASSERT_EQ(iqread[i * 2], l_pfReal[i]);
    if (iqread[i * 2 + 1] != l_pfImag[i])
    {
      cout << "Diff im " << i << " " << iqread[i * 2 + 1] << " " << l_pfImag[i] << '\n';
    }
    ASSERT_EQ(iqread[i * 2 + 1], l_pfImag[i]);
  }

}

TEST_F(AidTest, appendArrayFloatPointer)
{
  Aid aid(Common::TestOutputDir + "appendArrayFloatPointer.aid");
  float * iVector = new float[MB];
  float * qVector = new float[MB];
  for (size_t i = 0; i < MB; i++)
  {
    iVector[i] = i;
    qVector[i] = iVector[i] * - 1;
  }
  vector<float *> iqdata;
  iqdata.push_back(iVector);
  iqdata.push_back(qVector);
  vector<size_t> sizes;
  sizes.push_back(MB);
  sizes.push_back(MB);
  
  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendArrayFloatPointer", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendArrays(iqdata, sizes));
  ASSERT_EQ(0, aid.close());

  delete[] iVector;
  delete[] qVector;
}

TEST_F(AidTest, appendArrayDoublePointer)
{
  Aid aid(Common::TestOutputDir + "appendArrayDoublePointer.aid");
  double * iVector = new double[MB];
  double * qVector = new double[MB];
  for (size_t i = 0; i < MB; i++)
  {
    iVector[i] = i;
    qVector[i] = iVector[i] * -1;
  }
  vector<double *> iqdata;
  iqdata.push_back(iVector);
  iqdata.push_back(qVector);
  vector<size_t> sizes;
  sizes.push_back(MB);
  sizes.push_back(MB);

  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendArrayDoublePointer", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendArrays(iqdata, sizes));
  ASSERT_EQ(0, aid.close());

  delete[] iVector;
  delete[] qVector;
}

TEST_F(AidTest, appendArrayFloatVector)
{
  Aid aid(Common::TestOutputDir + "appendArrayFloatVector.aid");
  vector<vector<float>> writeVector;
  vector<float> iVector(MB);
  vector<float> qVector(MB);
  for (size_t i = 0; i < MB; i++)
  {
    iVector[i] = i;
    qVector[i] = iVector[i] * -1;
  }
  // ... fill i/q-vectors with data ...
  writeVector.push_back(iVector);
  writeVector.push_back(qVector);

  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendArrayFloatVector", "comment", channels, &metadata));
  ASSERT_EQ(0, aid.appendArrays(writeVector));
  ASSERT_EQ(0, aid.close());
}

TEST_F(AidTest, appendArrayDoubleVector)
{
  Aid aid(Common::TestOutputDir + "appendArrayDoubleVector.aid");
  vector<vector<double>> writeVector;
  vector<double> iVector(MB);
  vector<double> qVector(MB);
  for (size_t i = 0; i < MB; i++)
  {
    iVector[i] = i;
    qVector[i] = iVector[i] * -1;
  }
  // ... fill i/q-vectors with data ...
  writeVector.push_back(iVector);
  writeVector.push_back(qVector);

  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendArrayDoubleVector", "comment", channels, &metadata));
  ASSERT_EQ(0, aid.appendArrays(writeVector));
  ASSERT_EQ(0, aid.close());
}

TEST_F(AidTest, readArrayFloatPointer)
{
  Aid aid(Common::TestOutputDir + "appendArrayFloatPointer.aid");
  float * iVector = new float[MB];
  float * qVector = new float[MB];
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<float> iqread;
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  //ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqread, channels[0].getSamples()));

  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_I", iVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_Q", qVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iVector[i], i);
    ASSERT_EQ(qVector[i], iVector[i] * -1);
  }

  delete[] iVector;
  delete[] qVector;
}

TEST_F(AidTest, readArrayDoublePointer)
{
  Aid aid(Common::TestOutputDir + "appendArrayDoublePointer.aid");
  double * iVector = new double[MB];
  double * qVector = new double[MB];
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<double> iqread;
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  if (metadata.count("Ch1_MeasBandwidth[Hz]"))
  {
    ASSERT_TRUE(metadata["Ch1_MeasBandwidth[Hz]"] == "1600000");
  }

  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_I", iVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_Q", qVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iVector[i], i);
    ASSERT_EQ(qVector[i], iVector[i] * -1);
  }

  delete[] iVector;
  delete[] qVector;
}

TEST_F(AidTest, readArrayFloatVector)
{
  Aid aid(Common::TestOutputDir + "appendArrayFloatVector.aid");
  vector<float> iVector;
  vector<float> qVector;
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);

  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_I", iVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_Q", qVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iVector[i], i);
    ASSERT_EQ(qVector[i], iVector[i] * -1);
  }
}

TEST_F(AidTest, readArrayDoubleVector)
{
  Aid aid(Common::TestOutputDir + "appendArrayDoubleVector.aid");
  vector<double> iVector;
  vector<double> qVector;
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);

  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_I", iVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.readArray(channels[0].getChannelName() + "_Q", qVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iVector[i], i);
    ASSERT_EQ(qVector[i], iVector[i] * -1);
  }
}

TEST_F(AidTest, appendChannelFloatPointer)
{
  Aid aid(Common::TestOutputDir + "appendChannelFloatPointer.aid");
  float * iqVector = new float[MB*2];
  for (size_t i = 0; i < MB; i++)
  {
    iqVector[2*i] = i;
    iqVector[2 * i + 1] = iqVector[2 * i] * -1;
  }
  vector<float *> iqdata;
  iqdata.push_back(iqVector);
  vector<size_t> sizes;
  sizes.push_back(MB*2);

  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendChannelFloatPointer", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendChannels(iqdata, sizes));
  ASSERT_EQ(0, aid.close());

  delete[] iqVector;
}

TEST_F(AidTest, appendChannelDoublePointer)
{
  Aid aid(Common::TestOutputDir + "appendChannelDoublePointer.aid");
  double * iqVector = new double[MB * 2];
  for (size_t i = 0; i < MB; i++)
  {
    iqVector[2 * i] = i;
    iqVector[2 * i + 1] = iqVector[2 * i] * -1;
  }
  vector<double *> iqdata;
  iqdata.push_back(iqVector);
  vector<size_t> sizes;
  sizes.push_back(MB*2);

  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendChannelDoublePointer", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendChannels(iqdata, sizes));
  ASSERT_EQ(0, aid.close());

  delete[] iqVector;
}

TEST_F(AidTest, appendChannelFloatVector)
{
  Aid aid(Common::TestOutputDir + "appendChannelFloatVector.aid");
  vector<float> iqVector;
  for (size_t i = 0; i < MB; i++)
  {
    iqVector.push_back(i);
    iqVector.push_back((float)i * -1.0);
  }
  vector<vector <float>> iqdata;
  iqdata.push_back(iqVector);
  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendChannelFloatVector", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendChannels(iqdata));
  ASSERT_EQ(0, aid.close());
}

TEST_F(AidTest, appendChannelDoubleVector)
{
  Aid aid(Common::TestOutputDir + "appendChannelDoubleVector.aid");
  vector<float> iqVector;
  for (size_t i = 0; i < MB; i++)
  {
    iqVector.push_back(i);
    iqVector.push_back((float)i * -1.0);
  }
  vector<vector <float>> iqdata;
  iqdata.push_back(iqVector);
  vector<ChannelInfo> channels;
  channels.emplace_back(ChannelInfo("Channel1", 2000000, 10000000));
  map<string, string> metadata;
  metadata["Test"] = "Test";
  metadata["Ch1_MeasBandwidth[Hz]"] = "1600000";
  ASSERT_EQ(0, aid.writeOpen(IqDataFormat::Complex, 1, "appendChannelDoubleVector", "comment", channels, &metadata));

  ASSERT_EQ(0, aid.appendChannels(iqdata));
  ASSERT_EQ(0, aid.close());
}

TEST_F(AidTest, readChannelFloatPointer)
{
  Aid aid(Common::TestOutputDir + "appendChannelFloatPointer.aid");
  float * iqVector = new float[MB*2];
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());
  for (size_t i = 0; i < MB; i++)
  {
    if (iqVector[i * 2] != i)
    {
      cout << i * 2 << " " << iqVector[i * 2] << " != " << i << '\n';
    }
    ASSERT_EQ(iqVector[i*2], i);
    if (iqVector[i * 2 + 1] != (float)i * -1.0)
    {
      cout << i * 2 + 1 << " " << iqVector[i * 2 + 1] << " != " << (float)i * -1.0 << '\n';
    }
    ASSERT_EQ(iqVector[i * 2 + 1], (float) i * -1.0);
  }
  delete[] iqVector;
}

TEST_F(AidTest, readChannelDoublePointer)
{
  Aid aid(Common::TestOutputDir + "appendChannelDoublePointer.aid");
  double * iqVector = new double[MB * 2];
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());
  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iqVector[i * 2], i);
    ASSERT_EQ(iqVector[i * 2 + 1], (double)i * -1.0);
  }
  delete[] iqVector;
}

TEST_F(AidTest, readChannelFloatVector)
{
  Aid aid(Common::TestOutputDir + "appendChannelFloatVector.aid");
  vector<float> iqVector;
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    if (iqVector[i * 2] != i)
    {
      cout << i * 2 << " " << iqVector[i * 2] << " != " << i << '\n';
    }
    ASSERT_EQ(iqVector[i * 2], i);
    if (iqVector[i * 2 + 1] != (float)i * -1.0)
    {
      cout << i * 2 + 1 << " " << iqVector[i * 2 + 1] << " != " << (float)i * -1.0 << '\n';
    }
    ASSERT_EQ(iqVector[i * 2 + 1], (float)i * -1.0);
  }
}

TEST_F(AidTest, readChannelDoubleVector)
{
  Aid aid(Common::TestOutputDir + "appendChannelDoubleVector.aid");
  vector<double> iqVector;
  vector<string> arrayNames;
  ASSERT_EQ(0, aid.readOpen(arrayNames));
  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ASSERT_EQ(0, aid.getMetadata(channels, metadata));
  ASSERT_NE(channels.size(), 0);
  ASSERT_EQ(0, aid.readChannel(channels[0].getChannelName(), iqVector, channels[0].getSamples()));
  ASSERT_EQ(0, aid.close());

  for (size_t i = 0; i < MB; i++)
  {
    ASSERT_EQ(iqVector[i * 2], i);
    ASSERT_EQ(iqVector[i * 2 + 1], (double)i * -1.0);
  }
}

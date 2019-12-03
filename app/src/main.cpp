#include "dataimportexport.h"

#define _USE_MATH_DEFINES
#include "math.h"
#include <iostream>
#include <time.h>

#include <ctime>

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

int iqx2iqtar(int argc, const char* argv[])
{
  bool readChannels = false;
  if (argc != 2)
  {
    printf("call %s <iqx-file>\n", argv[0]);
    exit(1);
  }
  Iqx inIqx(argv[1]);

  vector<string> arrayNames;
  int ret = inIqx.readOpen(arrayNames);
  if (ret != ErrorCodes::Success)
  {
    printf("file open %s failed\n", argv[1]);
    exit(1);
  }

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = inIqx.getMetadata(channels, metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("get metadata failed\n");
    exit(1);
  }
  if (channels.size() != 1)
  {
    printf("Unexpected number of channels read from file\n");
    exit(1);
  }

  string outIqtarStr = argv[1];
  outIqtarStr = outIqtarStr + ".iq.tar";
  IqTar iqtar(outIqtarStr);
  string applicationName = argv[1];
  if (metadata.find("ApplicationName") != metadata.end())
  {
    applicationName = metadata.at("ApplicationName");
    std::cout << "ApplicationName: " << applicationName << "\n";
  }
  ret = iqtar.writeOpen(IqDataFormat::Complex, 1, "iqx to iqtar conversion", applicationName, channels, &metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("iq.tar file open for write failed\n");
    exit(1);
  }
  size_t read = 0;
  size_t offset = 0;
  size_t toRead = channels[0].getSamples();

  std::cout << toRead / 1E6 << " MSamples\n";
  std::cout << "writing iq.tar file\n";
  while (toRead > 0)
  {
    size_t readNow = toRead;
    if (readNow > (size_t)1E7) readNow = (size_t)1E7;
    if (readChannels)
    {
      vector<float> valuesIQ;
      ret = inIqx.readChannel(channels[0].getChannelName(), valuesIQ, readNow, read);
      if (ret != ErrorCodes::Success)
      {
        printf("iqx read channel failed\n");
        exit(1);
      }
      vector<vector<float>> writeChan;
      writeChan.push_back(valuesIQ);
      ret = iqtar.appendChannels(writeChan);
      if (ret != ErrorCodes::Success)
      {
        printf("iqtar append channel failed\n");
        exit(1);
      }
    }
    else
    {
      vector<float> valuesI;
      vector<float> valuesQ;
      ret = inIqx.readArray(channels[0].getChannelName() + "_I", valuesI, readNow, read);
      if (ret != ErrorCodes::Success)
      {
        printf("iqx read array failed\n");
        exit(1);
      }
      ret = inIqx.readArray(channels[0].getChannelName() + "_Q", valuesQ, readNow, read);
      if (ret != ErrorCodes::Success)
      {
        printf("iqx read array failed\n");
        exit(1);
      }
      vector<vector<float>> writeArrays;
      writeArrays.push_back(valuesI);
      writeArrays.push_back(valuesQ);
      ret = iqtar.appendArrays(writeArrays);
      if (ret != ErrorCodes::Success)
      {
        printf("iqtar append arrays failed\n");
        exit(1);
      }

    }
    toRead -= readNow;
    offset += readNow;
    read += readNow;
    std::cout << "\r" << read / 1E6 << " MSamples converted";
  }

  inIqx.close();
  std::cout << "\nclosing iq.tar file\n";

  iqtar.close();
  std::cout << "done\n";
  return 0;
}

int wv2iqtar(int argc, const char* argv[])
{
  if (argc != 2)
  {
    printf("call %s <wv-file>\n", argv[0]);
    exit(1);
  }
  Wv inWv(argv[1]);

  vector<string> arrayNames;
  int ret = inWv.readOpen(arrayNames);
  if (ret != ErrorCodes::Success)
  {
    printf("file open %s failed\n", argv[1]);
    exit(1);
  }

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = inWv.getMetadata(channels, metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("get metadata failed\n");
    exit(1);
  }
  if (channels.size() != 1)
  {
    printf("Unexpected number of channels read from file\n");
    exit(1);
  }

  std::cout << channels.size() << " Channels\n";
  std::cout << channels[0].getSamples() << " Samples\n";

  string outIqtarStr = argv[1];
  outIqtarStr = outIqtarStr + ".iq.tar";
  IqTar iqtar(outIqtarStr);
  string applicationName = argv[1];
  if (metadata.find("ApplicationName") != metadata.end())
  {
    applicationName = metadata.at("ApplicationName");
    std::cout << "ApplicationName: " << applicationName << "\n";
  }
  ret = iqtar.writeOpen(IqDataFormat::Complex, 1, "wv to iqtar conversion", applicationName, channels, &metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("iq.tar file open for write failed\n");
    exit(1);
  }
  size_t read = 0;
  size_t offset = 0;
  size_t toRead = channels[0].getSamples();

  std::cout << toRead / 1E6 << " MSamples\n";
  std::cout << "writing iq.tar file\n";
  while (toRead > 0)
  {
    size_t readNow = toRead;
    if (readNow > (size_t)1E7) readNow = (size_t)1E7;
    vector<float> valuesIQ;
    ret = inWv.readChannel(channels[0].getChannelName(), valuesIQ, readNow, read);
    if (ret != ErrorCodes::Success)
    {
      printf("wv read channel failed\n");
      exit(1);
    }
    vector<vector<float>> writeChan;
    writeChan.push_back(valuesIQ);
    ret = iqtar.appendChannels(writeChan);
    if (ret != ErrorCodes::Success)
    {
      printf("iqtar append channel failed\n");
      exit(1);
    }
    toRead -= readNow;
    offset += readNow;
    read += readNow;
    std::cout << "\r" << read / 1E6 << " MSamples converted";
  }

  inWv.close();
  std::cout << "\nclosing iq.tar file\n";

  iqtar.close();
  std::cout << "done\n";
  return 0;
}

int wvinfo(int argc, const char* argv[])
{
  if (argc != 2)
  {
    printf("call %s <wv-file>\n", argv[0]);
    exit(1);
  }
  Wv inWv(argv[1]);

  vector<string> arrayNames;
  int ret = inWv.readOpen(arrayNames);
  if (ret != ErrorCodes::Success)
  {
    printf("file open %s failed\n", argv[1]);
    exit(1);
  }
#if 1
  inWv.close();
  ret = inWv.readOpen(arrayNames);
  if (ret != ErrorCodes::Success)
  {
    printf("file open %s failed\n", argv[1]);
    exit(1);
  }
#endif

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = inWv.getMetadata(channels, metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("get metadata failed\n");
    exit(1);
  }
  if (channels.size() != 1)
  {
    printf("Unexpected number of channels read from file\n");
    exit(1);
  }

  std::cout << channels.size() << " Channels\n";
  std::cout << channels[0].getSamples() << " Samples\n";


  inWv.close();
  return 0;
}

int iqtarinfo(int argc, const char* argv[])
{
  if (argc != 2)
  {
    printf("call %s <iqtar-file>\n", argv[0]);
    exit(1);
  }
  IqTar inWv(argv[1]);

  vector<string> arrayNames;
  int ret = inWv.readOpen(arrayNames);
  if (ret != ErrorCodes::Success)
  {
    printf("file open %s failed\n", argv[1]);
    exit(1);
  }

  vector<ChannelInfo> channels;
  map<string, string> metadata;
  ret = inWv.getMetadata(channels, metadata);
  if (ret != ErrorCodes::Success)
  {
    printf("get metadata failed\n");
    exit(1);
  }
  if (channels.size() != 1)
  {
    printf("Unexpected number of channels read from file\n");
    exit(1);
  }

  std::cout << channels.size() << " Channels\n";
  std::cout << channels[0].getSamples() << " Samples\n";


  inWv.close();
  return 0;
}


int main( int argc, const char* argv[] )
{
#if 0
  IqMatlab in(argv[1]);

  vector<string> arrayNames;
  for (int i = 0; i < 3; i++)
  {
    int ret = in.readOpen(arrayNames);
    if (ret != ErrorCodes::Success)
    {
      printf("file open %s failed\n", argv[1]);
      exit(1);
    }
  }
#endif

#if 1
  iqx2iqtar(argc, argv);
  //wv2iqtar(argc, argv);

  //iqtarinfo(argc, argv);
  //wvinfo(argc, argv);

#else

  //////////////////////////////////////////////////////////////////////////
  // Read file example -- start --
  //////////////////////////////////////////////////////////////////////////
  
  // create instance of IqTar format
  IqTar readFile("c:/temp/testfile.iq.tar");

  // open file for reading
  vector<string> readArrayNames;
  int retCode = readFile.readOpen(readArrayNames);
  if (retCode != ErrorCodes::Success)
  {
    // error handling...
  }

  // get channel information and meta data
  vector<ChannelInfo> readChannelInfos;
  map<string, string> readMetadata;
  retCode = readFile.getMetadata(readChannelInfos, readMetadata);

  // get number of stored samples of first array.
  int64_t nofSamples = readFile.getArraySize(readArrayNames[0]);

  // check if the first array does contain at least 100 values
  if (nofSamples <= 100)
  {
    // error handling...
  }

  // read first 100 values into float vector
  vector<float> readFloatVector(100);
  retCode = readFile.readArray(readArrayNames[0], readFloatVector, 100);

  // read second 100 values into a double array
  double readDoubleArray[100];
  retCode = readFile.readArray(readArrayNames[0], readDoubleArray, 100, 100);

  // read data as interleaved (IQIQIQ) channel data
  retCode = readFile.readChannel("Channel1", readFloatVector, 100);

  // the offset for channels is specified as pairs of I/Q samples, thus
  // 100 values equal 50 I/Q pairs
  retCode = readFile.readChannel("Channel1", readFloatVector, 100, 50);

  // close file when done
  retCode = readFile.close();
#endif
  //////////////////////////////////////////////////////////////////////////
  // Read file example -- end --
  //////////////////////////////////////////////////////////////////////////



  //////////////////////////////////////////////////////////////////////////
  // Write array example -- start --
  //////////////////////////////////////////////////////////////////////////

  //// create instance if IqTar
  //IqTar writeFile("c:/temp/mytestfile.iq.tar");

  //// create channel information for 1 channel with channel name, center frequency and clock rate
  //vector<ChannelInfo> channelInfoWrite;
  //channelInfoWrite.push_back(ChannelInfo("Channel1", 1000.0, 1000.0));

  //// meta data is optional and not supported by all file formats (currently all formats but IQW support meta data).
  //map<string, string> metadataWrite;
  //metadataWrite.insert(make_pair("My_Key1", "My meta data value"));

  //// open file for writing
  //// we will write 1 channel complex I/Q data, which will result in 2 data arrays (one I and one Q array)
  //size_t nofArraysWrite = 2;
  //retCode = writeFile.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);

  //// append I/Q data using std::vector
  //vector<vector<float>> writeVector;
  //vector<float> iVector(50);
  //vector<float> qVector(50);
  //// ... fill i/q-vectors with data ...
  //writeVector.push_back(iVector);
  //writeVector.push_back(qVector);

  //// add data to IqTar
  //retCode = writeFile.appendArrays(writeVector);

  //// append I/Q data using arrays -> pass vector with pointers to data arrays...
  //vector<double*> writeArray;
  //double iArray[50];
  //double qArray[50];
  //// ... fill i/q arrays with data ...
  //writeArray.push_back(iArray);
  //writeArray.push_back(qArray);

  //// when passing vector containing pointer, you must specify the length of the data arrays in addition 
  //// to the actual data vector
  //vector<size_t> arraySizes;
  //arraySizes.push_back(50);
  //arraySizes.push_back(50);

  //// append data
  //retCode = writeFile.appendArrays(writeArray, arraySizes);

  //// close file when done
  //retCode = writeFile.close();

  ////////////////////////////////////////////////////////////////////////////
  //// Write array example -- end --
  ////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////
  //// Write channel example -- start --
  ////////////////////////////////////////////////////////////////////////////

  //// create instance of IqTar
  //IqTar writeFileChannel("c:/temp/mychannelfile.iq.tar");

  //// open file for writing
  //// we will write 1 channel complex I/Q data, with data provided in interleaved (IQIQIQ) format, thus nofArraysWrite equals 1.
  //nofArraysWrite = 1;
  //retCode = writeFile.writeOpen(IqDataFormat::Complex, nofArraysWrite, "Application Name", "Application specific comment", channelInfoWrite, &metadataWrite);

  //// append 1 channel interleaved I/Q data.
  //vector<vector<float>> writeChannelVector;
  //vector<float> channelData(100);
  //// ... fill with interleaved I/Q data ...
  //writeChannelVector.push_back(channelData);

  //// append data
  //retCode = writeFile.appendChannels(writeChannelVector);

  //// close file
  //retCode = writeFile.close();

  ////////////////////////////////////////////////////////////////////////////
  //// Write channel example -- end --
  ////////////////////////////////////////////////////////////////////////////



  ////////////////////////////////////////////////////////////////////////////
  //// use FileTypeService to get IDataImportExport interface. -- start --
  ////////////////////////////////////////////////////////////////////////////

  //string filename = "c:/temp/myinterfacetest." + FileTypeService::getFileExtension(Iqtar);
  //IDataImportExport* idai = FileTypeService::create(filename, FileType::Iqtar);

  //retCode = idai->readOpen(readArrayNames);
  //retCode = idai->close();

  //delete idai;

  ////////////////////////////////////////////////////////////////////////
  // use FileTypeService to get IDataImportExport interface. -- end --
  ////////////////////////////////////////////////////////////////////////
}

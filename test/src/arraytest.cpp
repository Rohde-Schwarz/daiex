#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace rohdeschwarz::mosaik::dataimportexport;

template<typename T, FileType ft>
class Container
{
public:
  Container();
  typedef T Dt; 
  static const FileType Ft = ft; 
};

template<typename T>
class ArrayTest : public ::testing::Test
{
};

typedef ::testing::Types<
  Container<float, FileType::IQW>,
  Container<double, FileType::IQW>,
  Container<float, FileType::Iqtar>,
  Container<double, FileType::Iqtar>,
  #ifndef __APPLE__
  Container<float, FileType::Matlab4>,
  Container<double, FileType::Matlab4>,
  Container<float, FileType::Matlab73>,
  Container<double, FileType::Matlab73>,
  #endif
  Container<float, FileType::Csv>,
  Container<double, FileType::Csv>
> MyTypes;
TYPED_TEST_CASE(ArrayTest, MyTypes);

TYPED_TEST(ArrayTest, ReadWriteArrayWithPtr)
{
  const string filename = Common::TestOutputDir + "ReadWriteArray." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  vector<typename TypeParam::Dt*> data(2);
  vector<typename TypeParam::Dt*> data2(2);

  data[0] = new typename TypeParam::Dt[6];
  data[1] = new typename TypeParam::Dt[6];
  data2[0] = new typename TypeParam::Dt[6];
  data2[1] = new typename TypeParam::Dt[6];
  typename TypeParam::Dt s = 0.11111;
  for (size_t i = 0; i < 6; i++)
  {
    data[0][i] = s * i;
    data[1][i] = 1 + s * i;
    data2[0][i] = 2 + s * i;
    data2[1][i] = 3+ s * i;
  }

  vector<size_t> sizes;
  sizes.reserve(2);
  sizes.push_back(6);
  sizes.push_back(6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (TypeParam::Ft == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (TypeParam::Ft == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->appendArrays(data, sizes);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->appendArrays(data2, sizes);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  // read
  IDataImportExport* readFile = FileTypeService::create(filename, TypeParam::Ft);

  vector<string> arrayNames;
  ret = readFile->readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile->getArraySize("Channel1_I") / 2;
  typename TypeParam::Dt* readValues = new typename TypeParam::Dt[size];

  // compare I values 
  ret = readFile->readArray("Channel1_I", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[0], readValues, size);

  // compare I values offset
  ret = readFile->readArray("Channel1_I", readValues, size, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[0], readValues, size);

  // compare Q values 
  ret = readFile->readArray("Channel1_Q", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data[1], readValues, size);

  // compare Q values offset
  ret = readFile->readArray("Channel1_Q", readValues, size, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteArray failed";
  Common::almostEqual(data2[1], readValues, size);

  ret = readFile->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete readFile;

  delete [] data[0];
  delete [] data[1];
  delete [] data2[0];
  delete [] data2[1];

  delete [] readValues;

  remove(filename.c_str());
}

TYPED_TEST(ArrayTest, ReadWriteChannelWithPtr)
{
  const string filename = Common::TestOutputDir + "ReadWriteChannel." + FileTypeService::getFileExtension(TypeParam::Ft) ;

  vector<typename TypeParam::Dt*> data(1);
  vector<typename TypeParam::Dt*> data2(1);

  data[0] = new typename TypeParam::Dt[6];
  data2[0] = new typename TypeParam::Dt[6];
  typename TypeParam::Dt s = 0.11111;
  for (size_t i = 0; i < 6; i++)
  {
    data[0][i] = s * i;
    data2[0][i] = 1 + s * i;
  }

  vector<size_t> sizes;
  sizes.reserve(1);
  sizes.push_back(6);

  // write
  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("Channel1", 12, 12));

  IDataImportExport* file = FileTypeService::create(filename, TypeParam::Ft);
  if (TypeParam::Ft == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (TypeParam::Ft == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  auto ret = file->writeOpen(IqDataFormat::Complex, 1, "name", "comment", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->appendChannels(data, sizes);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ret = file->appendChannels(data2, sizes);
  ASSERT_EQ(ErrorCodes::Success, ret);

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete file;

  // read
  IDataImportExport* readFile = FileTypeService::create(filename, TypeParam::Ft);

  vector<string> arrayNames;

  ret = readFile->readOpen(arrayNames);
  ASSERT_EQ(ErrorCodes::Success, ret);
  ASSERT_EQ(arrayNames.size(), 2) << "1 channel I And Q expected";
  ASSERT_EQ(arrayNames[0], "Channel1_I") << "unexpected array name";
  ASSERT_EQ(arrayNames[1], "Channel1_Q") << "unexpected array name";

  auto size = readFile->getArraySize("Channel1_I");
  typename TypeParam::Dt* readValues = new typename TypeParam::Dt[size];

  // compare I/Q values
  ret = readFile->readChannel("Channel1", readValues, size);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteChannel failed";
  Common::almostEqual(data[0], readValues, size);

  // compare I/Q values offset
  ret = readFile->readChannel("Channel1", readValues, size, size / 2);
  ASSERT_EQ(ErrorCodes::Success, ret) << "IQW.ReadWriteChannel failed";
  Common::almostEqual(data2[0], readValues, size);

  ret = readFile->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  delete readFile;
  delete [] data[0];
  delete [] data2[0];
  
  delete [] readValues;

  remove(filename.c_str());
}
#include "gtest/gtest.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "dataimportexport.h"
#include "common.h"

using namespace std;
using namespace chrono;
using namespace rohdeschwarz::mosaik::dataimportexport;

template<typename T, FileType ft, size_t valuesPerCycle, uint64_t size>
class Container
{
public:
	Container();
	typedef T Dt; 
	static const FileType Ft = ft;
	static const size_t ValuesPerCycle = valuesPerCycle;
	static const uint64_t Size = size;
};

template<typename T>
class BigFileTests : public ::testing::Test
{
};

template<typename T, IqDataOrder order, size_t valuesPerCycle, uint64_t size>
class IqwContainer
{
public:
	IqwContainer();
	typedef T Dt;
	static const IqDataOrder Order = order;
	static const size_t ValuesPerCycle = valuesPerCycle;
	static const uint64_t Size = size;
};

template<typename T>
class BigFileIqwDataOrder : public ::testing::Test
{
};

typedef ::testing::Types<
	Container<float, FileType::IQW, 1000000, 2500000000>, 
	Container<double, FileType::IQW, 1000000, 2500000000>,
  Container<float, FileType::Csv, 1000000, 10000000>,
  Container<float, FileType::Csv, 1000000, 10000000>,
  Container<double, FileType::Csv, 1000000, 10000000>,
	Container<float, FileType::Iqtar, 1000000, 2500000000>,
	Container<double, FileType::Iqtar, 1000000, 2500000000>,
	Container<float, FileType::Matlab4, 1000000, 250000000>,
  Container<double, FileType::Matlab4, 1000000, 250000000>,
  Container<float, FileType::Matlab73, 1000000, 250000000>,
  Container<double, FileType::Matlab73, 1000000, 250000000>
> MyTypes;
TYPED_TEST_CASE(BigFileTests, MyTypes);

typedef ::testing::Types< 
	IqwContainer<float, IqDataOrder::IIIQQQ, 1000000, 2500000000>,
	IqwContainer<double, IqDataOrder::IIIQQQ, 1000000, 2500000000>,
	IqwContainer<float, IqDataOrder::IQIQIQ, 1000000, 2500000000>,
	IqwContainer<double, IqDataOrder::IQIQIQ, 1000000, 2500000000>,
	IqwContainer<float, IqDataOrder::IQIQIQ, 1000000, 12000000000>
> MyIqwTypes;
TYPED_TEST_CASE(BigFileIqwDataOrder, MyIqwTypes);

TYPED_TEST(BigFileIqwDataOrder, DISABLED_BigIqw1ChComplexArray)
{
	FileType fileType = FileType::IQW;

	string ext = FileTypeService::getFileExtension(fileType);
	const string filename = Common::TestOutputDir + "BigIqw1ChComplexArray." + ext;

	auto file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);
	auto ret = ((Iqw*)file)->setDataOrder(TypeParam::Order);
	ASSERT_EQ(ret, ErrorCodes::Success);

	vector<ChannelInfo> channelInfos;
	channelInfos.push_back(ChannelInfo("ChannelInfo", 12, 12));

	vector<vector<typename TypeParam::Dt>> data;
	Common::initVector(data, 2, TypeParam::ValuesPerCycle);

	size_t sizePerCycle = 2 * TypeParam::ValuesPerCycle * sizeof(float);
	size_t cycles = max<size_t>(1, TypeParam::Size / sizePerCycle);

	ret = file->writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
	ASSERT_EQ(ErrorCodes::Success, ret);

	std::cout << "--> adding data to file '" << filename << "', cycles: " << cycles << "\n";
	std::cout << "--> step [x] of " << cycles;

	auto start = high_resolution_clock::now();

	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->appendArrays(data);
		ASSERT_EQ(ErrorCodes::Success, ret);
		std::cout << ".";
	}

	auto stop = high_resolution_clock::now();
	duration<double> span = duration_cast<duration<double>>(stop - start);

	std::cout << "\n--> data added in " << span.count() << " seconds\n";
	std::cout << "--> finalizing file...";

	ret = file->close();
	ASSERT_EQ(ErrorCodes::Success, ret);

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "written in " << span.count() << " seconds\n";

	delete file;

	file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);
	ret = ((Iqw*)file)->setDataOrder(TypeParam::Order);
	ASSERT_EQ(ret, ErrorCodes::Success);

	vector<string> arrayNames;
	file->readOpen(arrayNames);
	ASSERT_EQ(2, arrayNames.size());

	std::cout << "--> reading data again and comparing results ";

	vector<typename TypeParam::Dt> readData(TypeParam::ValuesPerCycle);
	start = high_resolution_clock::now();
	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->readArray(arrayNames[0], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[0].data(), readData.size(), 0.01);
		std::cout << "I";

		ret = file->readArray(arrayNames[1], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[1].data(), readData.size(), 0.01);
		std::cout << "Q";
	}

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "\n--> read and compared values in " << span.count() << " seconds\n";

	file->close();

	remove(filename.c_str());
}

TYPED_TEST(BigFileTests, DISABLED_Big1ChComplexArray)
{
  FileType fileType = TypeParam::Ft;

  string ext = FileTypeService::getFileExtension(fileType);
  const string filename = Common::TestOutputDir + "Big1ChComplexArray." + ext;

  auto file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);
  if (fileType == FileType::Matlab4)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
  }
  else if (fileType == FileType::Matlab73)
  {
    ((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
  }

  vector<ChannelInfo> channelInfos;
  channelInfos.push_back(ChannelInfo("ChannelInfo", 12, 12));

  vector<vector<typename TypeParam::Dt>> data;
  Common::initVector(data, 2, TypeParam::ValuesPerCycle);

  size_t sizePerCycle = 2 * TypeParam::ValuesPerCycle * sizeof(typename TypeParam::Dt);
  size_t cycles = max<size_t>(1, TypeParam::Size / sizePerCycle);

  auto ret = file->writeOpen(IqDataFormat::Complex, 2, "", "", channelInfos);
  ASSERT_EQ(ErrorCodes::Success, ret);

  std::cout << "--> adding data to file '" << filename << "', cycles: " << cycles << "\n";
  std::cout << "--> step [x] of " << cycles;

  auto start = high_resolution_clock::now();

  for (size_t i = 0; i < cycles; ++i)
  {
    ret = file->appendArrays(data);
    ASSERT_EQ(ErrorCodes::Success, ret);
    std::cout << ".";
  }

  auto stop = high_resolution_clock::now();
  duration<double> span = duration_cast<duration<double>>(stop - start);

  std::cout << "\n--> data added in " << span.count() << " seconds\n";
  std::cout << "--> finalizing file...";

  ret = file->close();
  ASSERT_EQ(ErrorCodes::Success, ret);

  stop = high_resolution_clock::now();
  span = duration_cast<duration<double>>(stop - start);
  std::cout << "written in " << span.count() << " seconds\n";

  delete file;

  file = FileTypeService::create(filename, fileType);
  ASSERT_NE(file, nullptr);

  vector<string> arrayNames;
  file->readOpen(arrayNames);
  ASSERT_EQ(2, arrayNames.size());

  std::cout << "--> reading data again and comparing results ";

  vector<typename TypeParam::Dt> readData(TypeParam::ValuesPerCycle);
  start = high_resolution_clock::now();
  for (size_t i = 0; i < cycles; ++i)
  {
    ret = file->readArray(arrayNames[0], readData, readData.size(), i * readData.size());
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readData.data(), data[0].data(), readData.size(), 0.01);
    std::cout << "I";

    ret = file->readArray(arrayNames[1], readData, readData.size(), i * readData.size());
    ASSERT_EQ(ret, ErrorCodes::Success);
    Common::almostEqual(readData.data(), data[1].data(), readData.size(), 0.01);
    std::cout << "Q";
  }

  stop = high_resolution_clock::now();
  span = duration_cast<duration<double>>(stop - start);
  std::cout << "\n--> read and compared values in " << span.count() << " seconds\n";

  file->close();

  remove(filename.c_str());
}

TYPED_TEST(BigFileTests, DISABLED_Big2ChComplexArray)
{
	if (TypeParam::Ft == FileType::IQW)
	{
		return;
	}

	FileType fileType = TypeParam::Ft;

	string ext = FileTypeService::getFileExtension(fileType);
	const string filename = Common::TestOutputDir + "Big2ChComplexArray." + ext;

	auto file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);
	if (fileType == FileType::Matlab4)
	{
		((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
	}
	else if (fileType == FileType::Matlab73)
	{
		((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
	}

	vector<ChannelInfo> channelInfos;
	channelInfos.push_back(ChannelInfo("Channel1", 12, 12));
	channelInfos.push_back(ChannelInfo("Channel2", 12, 12));

	vector<vector<typename TypeParam::Dt>> data;
	Common::initVector(data, 4, TypeParam::ValuesPerCycle);

	size_t sizePerCycle = 4 * TypeParam::ValuesPerCycle * sizeof(typename TypeParam::Dt);
	size_t cycles = max<size_t>(1, TypeParam::Size / sizePerCycle);

	auto ret = file->writeOpen(IqDataFormat::Complex, 4, "", "", channelInfos);
	ASSERT_EQ(ErrorCodes::Success, ret);

	std::cout << "--> adding data to file '" << filename << "', cycles: " << cycles << "\n";
	std::cout << "--> step [x] of " << cycles;

	auto start = high_resolution_clock::now();

	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->appendArrays(data);
		ASSERT_EQ(ErrorCodes::Success, ret);
		std::cout << ".";
	}

	auto stop = high_resolution_clock::now();
	duration<double> span = duration_cast<duration<double>>(stop - start);

	std::cout << "\n--> data added in " << span.count() << " seconds\n";
	std::cout << "--> finalizing file...";

	ret = file->close();
	ASSERT_EQ(ErrorCodes::Success, ret);

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "written in " << span.count() << " seconds\n";

	delete file;

	file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);

	vector<string> arrayNames;
	file->readOpen(arrayNames);
	ASSERT_EQ(4, arrayNames.size());

	std::cout << "--> reading data again and comparing results ";

	vector<typename TypeParam::Dt> readData(TypeParam::ValuesPerCycle);
	start = high_resolution_clock::now();
	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->readArray(arrayNames[0], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[0].data(), readData.size(), 0.01);
		std::cout << "I1";

		ret = file->readArray(arrayNames[1], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[1].data(), readData.size(), 0.01);
		std::cout << "Q1";

		ret = file->readArray(arrayNames[2], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[2].data(), readData.size(), 0.01);
		std::cout << "I2";

		ret = file->readArray(arrayNames[3], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[3].data(), readData.size(), 0.01);
		std::cout << "Q2";
	}

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "\n--> read and compared values in " << span.count() << " seconds\n";

	file->close();

	remove(filename.c_str());
}

TYPED_TEST(BigFileTests, DISABLED_Big3ChComplexArray)
{
	if (TypeParam::Ft == FileType::IQW)
	{
		return;
	}

	FileType fileType = TypeParam::Ft;

	string ext = FileTypeService::getFileExtension(fileType);
	const string filename = Common::TestOutputDir + "Big3ChComplexArray." + ext;

	auto file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);
	if (fileType == FileType::Matlab4)
	{
		((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat4);
	}
	else if (fileType == FileType::Matlab73)
	{
		((IqMatlab*)file)->setMatlabVersion(MatlabVersion::Mat73);
	}

	vector<ChannelInfo> channelInfos;
	channelInfos.push_back(ChannelInfo("Channel1", 12, 12));
	channelInfos.push_back(ChannelInfo("Channel2", 12, 12));
	channelInfos.push_back(ChannelInfo("Channel3", 12, 12));

	vector<vector<typename TypeParam::Dt>> data;
	Common::initVector(data, 6, TypeParam::ValuesPerCycle);

	size_t sizePerCycle = 6 * TypeParam::ValuesPerCycle * sizeof(typename TypeParam::Dt);
	size_t cycles = max<size_t>(1, TypeParam::Size / sizePerCycle);

	auto ret = file->writeOpen(IqDataFormat::Complex, 6, "", "", channelInfos);
	ASSERT_EQ(ErrorCodes::Success, ret);

	std::cout << "--> adding data to file '" << filename << "', cycles: " << cycles << "\n";
	std::cout << "--> step [x] of " << cycles;

	auto start = high_resolution_clock::now();

	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->appendArrays(data);
		ASSERT_EQ(ErrorCodes::Success, ret);
		std::cout << ".";
	}

	auto stop = high_resolution_clock::now();
	duration<double> span = duration_cast<duration<double>>(stop - start);

	std::cout << "\n--> data added in " << span.count() << " seconds\n";
	std::cout << "--> finalizing file...";

	ret = file->close();
	ASSERT_EQ(ErrorCodes::Success, ret);

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "written in " << span.count() << " seconds\n";

	delete file;

	file = FileTypeService::create(filename, fileType);
	ASSERT_NE(file, nullptr);

	vector<string> arrayNames;
	file->readOpen(arrayNames);
	ASSERT_EQ(6, arrayNames.size());

	std::cout << "--> reading data again and comparing results ";

	vector<typename TypeParam::Dt> readData(TypeParam::ValuesPerCycle);
	start = high_resolution_clock::now();
	for (size_t i = 0; i < cycles; ++i)
	{
		ret = file->readArray(arrayNames[0], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[0].data(), readData.size(), 0.01);
		std::cout << "I1";

		ret = file->readArray(arrayNames[1], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[1].data(), readData.size(), 0.01);
		std::cout << "Q1";

		ret = file->readArray(arrayNames[2], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[2].data(), readData.size(), 0.01);
		std::cout << "I2";

		ret = file->readArray(arrayNames[3], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[3].data(), readData.size(), 0.01);
		std::cout << "Q2";

		ret = file->readArray(arrayNames[4], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[4].data(), readData.size(), 0.01);
		std::cout << "I3";

		ret = file->readArray(arrayNames[5], readData, readData.size(), i * readData.size());
		ASSERT_EQ(ret, ErrorCodes::Success);
		Common::almostEqual(readData.data(), data[5].data(), readData.size(), 0.01);
		std::cout << "Q3";
	}

	stop = high_resolution_clock::now();
	span = duration_cast<duration<double>>(stop - start);
	std::cout << "\n--> read and compared values in " << span.count() << " seconds\n";

	file->close();

	remove(filename.c_str());
}
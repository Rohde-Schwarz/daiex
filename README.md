# I/Q Data Import Export library (daiex)

is a cross-platform C++ library that provides functions to import and export numeric I/Q data to or from various file formats.

Two groups of file format are considered:
- special binary formats that contain I/Q data, as used by various instruments, for instance iq-tar.
- generic file formats that can be used to exchange raw data with standard PC tools, such as Microsoft Office or Matlab.

# Features

The library provides standardized read and write functions that encapsulate all file operations. The following file formats are supported:

|File format| file extension | comment|
| --- | --- |:------------------------------|
| iq-tar|	.iq.tar | An iq-tar file contains I/Q data in binary format together with meta information that describes the nature and the source of data, e.g. sample rate. The objective of the iq-tar file format is to separate I/Q data from the meta information while still having both in one file. In addition, the file format allows a preview of the I/Q data in a web browser and inclusion of user-specific data. For details see class IqTar. Note that each channel is written to a temporary file and the final .iq.tar file is only generated when calling close(). Thus, the close-method might be a long-running operation, as all temporary files need to be merged to the final tar file. For best read performance set setBufferSize to the size usually read with one readArray or readChannel call. Note that whenever new I/Q data is added, an I/Q preview is calculated. Dependent on the number of I/Q values added, this might be a time consuming operation. If the number of I/Q values is well-known, temp-files can be disabled using "disableTempFile()"|
|IQW (IIIQQQ) |	.iqw |	A file that contains Float32 data in a binary format ( first all I values are stored, followed by all Q values ). The file does not contain any additional header information. Note that I and Q data are first buffered to temporary files and are merged when calling close(). Prefer IQW-format with data order IQIQIQ. The data order has to be changed before readOpen or writeOpen is called. |
| IQW (IQIQIQ)	| .iqw	| A file that contains Float32 data in a binary format ( values are stored in interleaved format, starting with the first I value ). The file does not contain any additional header information. The data order has to be changed before readOpen or writeOpen is called. |
| WV (IQIQIQ)	| .wv	| A file that contains INT16 data in a binary format ( values are stored in interleaved format, starting with the first I value ). This format is used in signal generators.|
|IQX (IQIQIQ)	| .iqx	| A file that contains INT16 data in a binary format ( values are stored in interleaved format, starting with the first I value ). This format is used in device IQW.|
|AID (IQIQIQ)	|.iqx	|A file that contains I/Q data in a binary format ( values are stored in interleaved format, starting with the first I value ). This format is used in AMMOS project.|
|CSV	|.csv	|A file containing I/Q data in comma-separated values format (CSV). The comma-separator used can either be a semicolon or a comma, depending on the decimal separator used to save floating-point values (either dot or comma). Additional meta data can be saved. For details see class Csv.|
|Matlab v4	|.mat	|A file containing I/Q data in matlab file format v4. Channel related information is stored in matlab variables with names starting with 'ChX_'. 'X' represents the number of the channel with a lower bound of 1, e.g. variable Ch1_ChannelName contains the name of the first channel. The corresponding data is contained in ChX_Data. Optional user data can be saved to variables named UserDataX, where 'X' starts at 0. Variable UserData_Count contains the number of UserData variables. For compatibility reasons user data needs to be saved as a 2xN char array, where the first row contains the key of the user data and the second row the actual value. Both rows must have the same column count and are therefore right-padded with white spaces. Variables can be written in arbitary order to the *.mat files. For details see IqMatlab. Limitations: In general, the file format is limited to a maximum of 2GB. A maximum of 100000000 can be stored in a single variable. Consequently, complex data can contain up to 50000000 samples. Note that all data is loaded into RAM before being written to the .mat file. Make sure to provide sufficient free memory.|
|Matlab v7.3	|.mat	|A file containing I/Q data in matlab file format v7.3. Supportes the same functionality as matlab v4 file format, but requires the Matlab Compiler Runtime (MCR) to be installed on the system. The installation needs to be registered in the global PATH environment variable. The machine type of the installation needs to match the machine type of DataImportExport, e.g. 32bit DataImportExport cannot be linked against 64bit MCR. For details see IQMatlab. Limitations: When calling FinishPartialIQ, all data needs to be loaded into memory. The matlab v7.3. file format requires the Matlab Compiler Runtime (MCR) to be installed on the system and registered in the PATH environment variable. Download an MCR version >= 7.2 from http://www.mathworks.de/products/compiler/mcr/. Note that all data is loaded into RAM before being written to the .mat file. Make sure to provide sufficient free memory.|

Whenever supported by the file format, the following meta data will be stored when writing a file using IDataImportExport.

|Key	|Description|
| ---------- |:-------------|
|Application Name	|Name of the application or instrument that created the file. (optional)|
|Comment	|A text that further describes the contents of the file. (optional)|
|DateTime	|Date and time of the creation of the file. Formated as "yyyy-MM-ddTHH:mm:ss". If not set explicitly, the current date is set before file is saved.|
|Samples	|Number of samples of the stored data. One sample can be a complex number represented as a pair of I and Q values or as a pair of magnitude and phase values or a real number represented as a single real value|
|Clock	|The clock frequency in Hz, i.e. the sample rate of the I/Q data. A signal generator typically outputs the I/Q data at a rate that equals the clock frequency. If the I/Q data was captured with a signal analyzer, the signal analyzer used the clock frequency as the sample rate.|
|CenterFrequency	|Center frequency of the modulated waveform in Hz.|
|Format	|Specifies how the binary data is saved. Every sample must be in the same format. The format can be one of the following: complex, real or polar.|
|DataType	|Specifies the binary format used for saving the samples. 32 or 64 bit floating point data (IEEE 754).|
|ChannelNames	|List of strings containing the names for each data channel stored in the file. Default value is "Channel1" etc.. The array names returned by IDataImportExport.OpenFile are built from those channel names by appending "_I" and "_Q".|



The following operating systems are supported

| OS | comment |
| ---------- |:-------------|
| Linux | tested on Ubuntu 18.04
| Windows | tested on Windows 10 with Visual Studio 2017
| macOS | tested on Mojave 10.14.4. There are still some problems with .mat files.|

## Open source acknowledgement

This library uses the following modules.

| Module | version | license | origin | copyright | comment |
| ------ | ------- | ------- |------- | --------- | ------- |
| zlib   | 1.2.11  | Zlib    | http://zlib.net/zlib-1.2.11.tar.gz | Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler | added two files in devpackage and changed cmakelist.txt for find_package |
| libarchive | 3.1.2 | BSD-2-Clause | http://www.libarchive.org/downloads/libarchive-3.2.1.tar.gz | Copyright (c) 2003-2009 Tim Kientzle | |
| libhdf5 | 1.10.0-patch1 | HDF5 | https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.0-patch1/src/hdf5-1.10.0-patch1.tar | Copyright 2006-2016 by The HDF Group. ||
| matio | 1.5.8 | BSD-2-Clause | https://github.com/tbeu/matio/releases/tag/v1.5.8 | Copyright (c) 2011-2016, Christopher C. Hulbert ||
| Memory-Mapped File C++ | 2017-01-31 | MPL-2.0 | https://github.com/carlomilanesi/cpp-mmf | Copyright (c) 2014, Carlo Milanesi ||
| PugiXml | 1.8  | MIT | https://github.com/zeux/pugixml | Copyright (C) 2006-2017, by Arseny Kapoulkine ||
| Google C++ Testing Framework | 1.7.0 | BSD 3-Clause | http://code.google.com/p/googletest/ | Copyright 2008, Google Inc. ||

This library uses a lot of open source packages. We would like to thank the authors for their valuable work.
A list of all used packages is located in lib/doc/Inventory.xml.


## Directory structure

| dir        | comment       |
| ---------- |:-------------|
|  3rdparty  | contains open source projects wihich are used by daiex lib |
|  app       | an example app |
|  cmake     | cmake modules  |
|  lib       | the source directory of daiex lib. Contains also documentation |
|  test      | test sources |
|  wrapper   | .Net wrapper for daiex lib |
|  wrapper_test | tests for the .Net wrapper |

## Building daiex lib

### Requirements

Install git and cmake.

### Getting sources

Clone this repository to your local hard disc.

### Linux Ubuntu (18.04)

* make sure that zlib and libarchive are available. If not, install via "sudo apt-get install zlib1g-dev libarchive-dev".
* make sure doxygen is installed
* download https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.0-patch1/src/hdf5-1.10.0-patch1.tar and unpack
* Build hdf5:
    - create build folder "build" at the level of the "hdf5-1.10.0-patch1"-folder
    - cd "hdf5-1.10.0-patch1/build"
    - set install path and configure project using cmake. E.g., install to folder "usr/local/hdf5":  "cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/hdf5 ../".
    - make sure to enable ZLIB support (DHDF5_ENABLE_Z_LIB_SUPPORT)
    - build target: "make"
    - install: "make install" -> hdf5 should now be available in "/usr/local/hdf5"
* build libdai:
    - create build folder e.g. "build"
    - cd "build"
    - set cmake prefix path: "export CMAKE_PREFIX_PATH={$HDF5_DIR}/share/cmake", where $HDF5_DIR is the installation folder of HDF5, e.g. "/usr/local/hdf5"
    - configure project: "cmake -DCMAKE_BUILD_TYPE={Debug|Release} {$path_to_src_libdai_root}"
    - build project: make


### Windows (Visual Studio 2017 64)
* make sure that you have Visual Studio 2017 64 installed.
* download and install nunit 2.x and doxygen from the www.
* and also download the html help workshop, which generates a chm file.
* download and build zlib:
   - unpack zlib from 3rdparty directory to your  hard disc and change into the unpacked directory
   - mkdir build
   - cd build
   - cmake -G"Visual Studio 15 2017 Win64" ..
   - cmake --build . --config Release
* build daiex:
   - Change to the source directory of your local copy of the daiex repository.
   - Create a folder build.
   - Change to this folder
   - cmake -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release ..
   - cmake --build . --config Release

Create a package, which includes all files you need to use daiex
* cmake --build . --config Release --target package
If you have not downloaded and installed the HTML help workshop from Microsoft, you will get a strange error (setlocale).


### macOS (Mojave 10.14.4)

* download and install homebrew
* install all needed packages with brew. On my machine there were the following packets (brew list):
cmake, gmp,	libarchive,	ossp-uuid, xz,
gcc, hdf5, libmatio, mpfr, gdbm, isl, libmpc, openssl, readline,	szip.
* build libdai:
   - create build folder e.g. "build"
   - cd "build"
   - configure project: "cmake -DCMAKE_BUILD_TYPE={Debug|Release}"
   - build project: make

The version of libmatio downloaded by brew uses szip instead of zlib. You should have a look at the szip license or try to compile libmatio with zlib yourself.
Using macOS there are some problems with .mat files so some tests are disabled at the moment.

## Test

Tests are executed via the daitest application. Usage: 'daitest {path-to-testfile} {path-to-temp-dir}'.
  {path-to-testfile} has to point to the location of the tests files, which is './test/res' and {path-to-temp-dir} is a location
  to temporarily write the test file.

  By default no "big-file-tests" are executed. To include tests that write a large amount of data run 'daitest {path-to-testfile} {path-to-temp-dir} --gtest_also_run_disabled_tests'.

## Documentation

Further documentation generated with doxygen can be found in build/lib/doc/gen/. Open index.html with your browser.

## Using daiex lib

The main interface to perform read and write operations is IDataImportExport, which is implemented by various classes like IqTar to support the different file formats.

When you work with the daiex library, you need the following include

    #include "dataimportexport.h"

and namespace

    using namespace rohdeschwarz::mosaik::dataimportexport;

To open a .iqx file, declare a instance of Iqx class and provide the path and name of the file.
Use UTF-8 encoding for all strings passed to classes of LibDataImportExport.



    Iqx inIqx("/tmp/example.iqx");

To read data from the file, the file must be opened to scan the contained data. During this process only meta data will be read, but no actual I/Q values. Most methods will return an error code. Evaluate this code to be ErrorCode::Success for successful operation. Otherwise an error message can be obtained using getErrorText for the specified error code.

The readOpen method opens the file in read only mode. The parameter arrayNames returns string identifiers of the data arrays found in this file. Use these identifiers to read the corresponding I/Q data.

    vector<string> arrayNames;
    int ret = inIqx.readOpen(arrayNames);
    if (ret != ErrorCodes::Success)
    {
      printf("file open failed\n");
      exit(1);
    }

getMetadata gives additional information about the data

    vector<ChannelInfo> channels;
    map<string, string> metadata;
    ret = inIqx.getMetadata(channels, metadata);
    if (ret != ErrorCodes::Success)
    {
      printf("get metadata failed\n");
      exit(1);
    }

    cout << "Number of channels: " << channels.size() << endl;
    for (const auto& c : channels)
    {
      cout << "Information for channel named " '" << c.getChannelName()  << "' ": clock rate: " << c.getClockRate() << ", center frequency: " << c.getFrequency() << endl;
    }

    // print all meta data found in file
    for (const auto& kvp : metadata)
    {
      cout << "Key: " << kvp.first << " has value: " << kvp.second << endl;
    }

If you want to change parameters that affect the read or write behaviour of the data format, changes have to be applied before calling readOpen() or writeOpen(). For instance, the data order of an IQW file to be read can be changed using

    Iqw readFile(filename);
    returnCode = readFile.setDataOrder(IqDataOrder::IQIQIQ);
    returnCode = readFile.readOpen(arrayNames);

In a similar fasion, the preview of I/Q data for the IqTar file format can be disabled using

    IqTar writeFile(filename);
    returnCode = writeFile.setPreviewEnabled(false);
    returnCode = writeFile.writeOpen(...);

The actual I/Q data arrays can be accessed using the corresponding array names returned by IDataImportExport.readOpen. Each data array can be read with single or double precision regardless of the precision used to save the data to file. The read methods will perform the conversion internally.

    for (size_t arrayIdx = 0; arrayIdx < arrayNames.size(); ++arrayIdx)
    {
      // get the number of values stored in the array
      int64_t size = readFile.getArraySize(arrayNames[arrayIdx]);

      // Read data into vector.
      // For better read performance, resize the vector to the number of elements you want to read.
      // Otherwise, memory allocation will be performed by the read operation.
      vector<float> floatVector(size);

      // read data
      returnCode = readFile.readArray(arrayNames[arrayIdx], floatVector, size);

      // If you would like to read only a certain selection of the data array, define an additional offset.
      // The offset defines the number of values that are skipped before the read operation starts.
      size_t offset = 10;
      size_t nofValuesToRead = 100;
      returnCode = readFile.readArray(arrayNames[arrayIdx], floatVector, nofValuesToRead, offset);

      // If you prefer to read data to an array instead to a vector, you can do so.
      // Memory has to be preallocated.
      double doubleArray[100];
      returnCode = readFile.readArray(arrayNames[arrayIdx], &doubleArray, 100);
    }

Data can not only be read array-wise using the stored array names (e.g. "Channel1_I", "Channel1_Q"), but also channel-wise (e.g. "Channel1"). Therefor use

    returnCode = readFile.readChannel("Channel1", floatVector, size);

and

    returnCode = readFile.readChannel("Channel1", &doubleArray, size);

respectively.

The read-offset specified in readChannel refers to the number of I/Q pairs, e.g. an offset of 4 skips the first 2 I/Q pairs.   

    returnCode = readFile.readChannel("Channel1", &doubleArray, size, offset);

When you are done with your read operations, close the file by calling

    returnCode = readFile.close();

If a file format does not match the specification, an error code will be returned when readOpen() is called. In case of the CSV and Matlab file format, data fields can still be read using index-based access, cf. ICsvSelector and IArraySelector.

Attention: When reading large amounts of data incrementally reuse the allocated memory in order to avoid heap fragmentation. Array length is returned as int64_t as data arrays in a file can be very large, but the number of values that can be read with one function call is restricted to size_t.

Saving data to file is realized in a similar fashion than reading data. Therefore use methods writeOpen, writeArray or writeChannel and close.

    vector<vector<float>> writeVector;
    // ... fill data ...
    // writeVector contains all data vectors, i.g. vector<vector containing Channel1_I values, vector containing Channel1_Q values>
    // for a 1 Channel complex data array. For two complex channels, writeVector must contain 4 data vectors, etc.

    // create channel information
    double centerFreq = 15.5;
    double clock = 1000.2;
    vector<ChannelInfo> channelInfos;
    channelInfos.push_back(ChannelInfo("Channel1", clock, centerFreq));

    // create object of desired file format
    IqTar writeFile("c:/myfile.iq.tar");

    // open file for write operations: define the I/Q data format used and the number of arrays to be written.
    size_t nofArraysToWrite = writeVector.size();
    returnCode = writeFile.writeOpen(IqDataFormat::Complex, nofArraysToWrite, "My R/S test application", "My comment for test application", channelInfo);

    // you can add optional meta data as key-value pairs to the file, if supported by the file format (currently all formats but IQW)
    map<string, string> metadata;
    metadata.insert(make_pair("Key", "Value"));
    metadata.insert(make_pair("Key2", "Value2"));
    returnCode = writeFile.writeOpen(IqDataFormat::Complex, nofArraysToWrite, "My R/S test application", "My comment for test application", channelInfo, &metadata);


    for (size_t i = 0; i < 4; ++i)
    {
      // fill writeVector with new data...
      // and write data to file
      returnCode = writeFile.appendArrays(writeVector);
    }

    // When all data has been added, call close
    returnCode = writeFile.close();

Analogous to readArray, data can also be added using arrays instead of vectors. Therefore, provide a vector containing pointers to the actual data arrays and an additional vector that contains the lengths of the data arrays.

    vector<float*> writeArray;
    // ... add pointers to data arrays to writeArray ...

    vector<size_t> arrayLengths;
    arrayLength.push_back("length of array1");
    arrayLength.push_back("length of array2");

    // write data
    writeFile.appendArrays(writeArray, arrayLengths);

Channels are written in a similar fashion:

    // note that nofArraysToWrite is 1 instead of 2, as only 1 array containing interleaved I/Q data of the channel
    // is passed instead of two arrays of separate I and Q values.
    size_t nofArraysToWrite = 1;
    returnCode = writeFile.writeOpen(IqDataForm::Complex, nofArraysToWrite, "My R/S test application", "My comment for test application", channelInfo);

    vector<vector<float>> writeChannelVector;
    // ...fill data ...
    // writeChannelVector contains the data data of each channel per vector element, e.g. vector<Channel1 values, Channel2 values, etc.>.
    // Complex-valued channels must contain I/Q data in interleaved format per channel, i.e. IQIQIQ.

    vector<size_t> channelLengths;
    channelLengths.push_back("nof values of channel");

    returnCode = writeFile.appendChannels(writeChannelVector, channelLengths);

**Performance:**
Many file formats make use of temporary files while adding new I/Q data. The final file is written when the close() operation is called. The default location used to write temporary files is the system's temp-directory, which is usually located on the primary hard drive. To obtain the best write performance, make sure to store temporary files on the same hard-drive than your final file is going to be stored. To change the temporary file location, call setTempDir on the respective file object.

The directory app contains a more detailed example.
If you use the library with Windows, you have to include windows.h, otherwise you get compile errors.

## .NET Wrapper
The LibDataImportExport library provides a .NET Wrapper for the native c++ dll, in order to make the functionallity of LibDataImportExport usable in managed projects. An installation of the .NET Framework is required on the target machine. For details on how to use the wrapper classes, please refer to the Doxygen documentation.

## Other I/Q tools

The R&S® IQ File Cutter can load I/Q data from supported I/Q file formats and export part of the signal to another file.
https://www.rohde-schwarz.com/appnote/GFM338

## License

The daiex library is licensed under the Apache 2.0 license. For more information see license.txt

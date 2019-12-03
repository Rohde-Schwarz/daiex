/*
	* Copyright (c) Rohde & Schwarz
	*
	* Licensed under the Apache License, Version 2.0 (the "License");
	* you may not use this file except in compliance with the License.
	* You may obtain a copy of the License at
	*
	*     http://www.apache.org/licenses/LICENSE-2.0
	*
	* Unless required by applicable law or agreed to in writing, software
	* distributed under the License is distributed on an "AS IS" BASIS,
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
*/

#pragma once

#include "channelinfo_wrapper.h"
#include "enums_wrapper.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;
      using namespace System::Collections::Generic;

      /// <summary>
      /// Wraps up the native interface IDataImportExport.
      /// </summary>
      public interface class IDataImportExport
      {
      public:
        /// <summary>
        /// Gets or sets the timestamp stored with the meta data of an I/Q record.
        /// Must be set before writeOpen() is called. If the file format does not 
        /// support meta data or a timestamp, the value will not be saved to the I/Q file.
        /// </summary>
        property Nullable<DateTime>^ Timestamp
        {
          void set(Nullable<DateTime>^ time);
          Nullable<DateTime>^ get();
        }

        /// <summary>
        /// Gets the corresponding channel information of the I/Q record, i.e. name, clock rate and center frequency of each 
        /// channel contained in the file.
        /// Make sure to call readOpen() to initialize an I/Q record before reading its meta data.
        /// </summary>
        /// <exception cref="DaiException">Thrown if an error occurred while reading the channel information, i.e. the
        /// file was not opened. See the error code of the native implementation for more information.</exception>
        property IList<ChannelInfo^>^ ChannelInformation
        {
          IList<ChannelInfo^>^ get();
        }

        /// <summary>
        /// Gets additional meta data, represented as key-value-pairs. The dictionary contains standardized information as well
        /// as optional user-defined information.
        /// Make sure to call readOpen() to initialize an I/Q record before reading its meta data.
        /// </summary>
        /// <exception cref="DaiException">Thrown if an error occurred while reading the channel information, i.e. the
        /// file was not opened. See the error code of the native implementation for more information.</exception>
        property IDictionary<String^, String^>^ Metadata
        {
          IDictionary<String^, String^>^ get();
        }

        /// <summary>
        /// Opens a file in read-only mode and reads all meta data contained as well as the names
        /// of the I/Q value arrays. The meta data can be queried using the properties ChannelInformation
        /// and Metadata. No I/Q data will be read during this operation. Use readArray() or readChannel() to read I/Q values.
        /// Call close() when you're done.
        /// </summary>
        /// <returns>Returns a list containing the names of the found I/Q data arrays.</returns>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        IList<System::String^>^ ReadOpen();

        /// <summary>
        /// Opens a file in write-only mode. If supported by the file format, the specified meta data will also be stored in the file.
        /// If a file with the specified filename does already exists, the file is deleted. In case the file cannot be deleted, a FileOpenError
        /// will be returned.
        /// Call close() to finalize the file.
        /// </summary>
        /// <param name="format">Specifies how I/Q values are saved, i.e. complex, real, or polar.</param>
        /// <param name="nofArrays">The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.</param>
        /// <param name="applicationName">Application or instrument name exporting this I/Q data.</param>
        /// <param name="comment">Text that further describes the file contents.</param>
        /// <param name="channelInfos">Channel information with name, clock rate and center frequency. One object is required per channel.</param>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        void WriteOpen(
          IqDataFormat format,
          int nofArrays,
          String^ applicationName,
          String^ comment,
          IList<ChannelInfo^>^ channelInfos);

        /// <summary>
        /// Opens a file in write-only mode. If supported by the file format, the specified meta data will also be stored in the file.
        /// If a file with the specified filename does already exists, the file is deleted. In case the file cannot be deleted, a FileOpenError
        /// will be returned.
        /// Call close() to finalize the file.
        /// </summary>
        /// <param name="format">Specifies how I/Q values are saved, i.e. complex, real, or polar.</param>
        /// <param name="nofArrays">The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.</param>
        /// <param name="applicationName">Application or instrument name exporting this I/Q data.</param>
        /// <param name="comment">Text that further describes the file contents.</param>
        /// <param name="channelInfos">Channel information with name, clock rate and center frequency. One object is required per channel.</param>
        /// <param name="metadata">Additional, non-standardized meta data; provided as key-value pairs.</param>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        void WriteOpen(
          IqDataFormat format,
          int nofArrays,
          String^ applicationName,
          String^ comment,
          IList<ChannelInfo^>^ channelInfos,
          IDictionary<String^, String^>^ metadata);

        /// <summary>
        /// Closes a file that has previously been opened. 
        /// </summary>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        void Close();

        /// <summary>
        /// Returns the length (i.e. the number of samples) of the specified data array. Use the length of the array
        /// to preallocate the required space of the data vector used for reading I/Q data with the methods readArray()
        /// and readChannel().
        /// </summary>
        /// <param name="arrayname">Name of the data array contained in the file.</param>
        /// <returns>Returns the length of the queried array, or -1 if the specified array name is invalid.</returns>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        Int64 GetArraySize(String^ arrayname);

        /// <summary>
        /// Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="arrayName">The name of the array to read.</param>
        /// <param name="values">The values read. Data read from file will be converted to single precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadArray(String^ arrayName, IList<float>^% values, int nofValues);
        
        /// <summary>
        /// Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="arrayName">The name of the array to read.</param>
        /// <param name="values">The values read. Data read from file will be converted to single precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <param name="offset">Defines the start position in the I/Q data record at which the read operation is started.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadArray(String^ arrayName, IList<float>^% values, int nofValues, int offset);

        /// <summary>
        /// Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as double precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="arrayName">The name of the array to read.</param>
        /// <param name="values">The values read. Data read from file will be converted to double precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadArray(String^ arrayName, IList<double>^% values, int nofValues);

        /// <summary>
        /// Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as double precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="arrayName">The name of the array to read.</param>
        /// <param name="values">The values read. Data read from file will be converted to double precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <param name="offset">Defines the start position in the I/Q data record at which the read operation is started.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadArray(String^ arrayName, IList<double>^% values, int nofValues, int offset);

        /// <summary>
        /// Reads the values of the specified channel (e.g. 'Data') and returns them as single precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="channelName">The name of the channel to read.</param>
        /// <param name="values">The values read. The values read. Data read from file will be converted to single precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        virtual void ReadChannel(String^ channelName, IList<float>^% values, int nofValues);

        /// <summary>
        /// Reads the values of the specified channel (e.g. 'Data') and returns them as single precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="channelName">The name of the channel to read.</param>
        /// <param name="values">The values read. The values read. Data read from file will be converted to single precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <param name="offset">Defines the number of I/Q pairs to be skipped before the read operation is started.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadChannel(String^ channelName, IList<float>^% values, int nofValues, int offset);

        /// <summary>
        /// Reads the values of the specified channel (e.g. 'Data') and returns them as double precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="channelName">The name of the channel to read.</param>
        /// <param name="values">The values read. The values read. Data read from file will be converted to double precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadChannel(String^ channelName, IList<double>^% values, int nofValues);

        /// <summary>
        /// Reads the values of the specified channel (e.g. 'Data') and returns them as double precision vector.
        /// Make sure to call readOpen() to initialize an I/Q record before reading.
        /// </summary>
        /// <param name="channelName">The name of the channel to read.</param>
        /// <param name="values">The values read. The values read. Data read from file will be converted to double precision,
        /// independent of the original data type.</param>
        /// <param name="nofValues">Number of values to read.</param>
        /// <param name="offset">Defines the number of I/Q pairs to be skipped before the read operation is started.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void ReadChannel(String^ channelName, IList<double>^% values, int nofValues, int offset);

        /// <summary>
        /// Adds new I/Q data with single precision to the file.
        /// Make sure to call WriteOpen() to enter write-mode.
        /// </summary>
        /// <param name="iqdata">I and Q data vectors in single precision that have to be stored.
        /// The vectors have to be ordered as iqdata&lt;Channel1_I, Channel1_Q, Channel2_I, Channel2_Q, ...&gt;
        /// for complex data. The number of I/Q data vectors must match the specified value nofArrays in writeOpen().</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void AppendArrays(IList<IList<float>^>^ iqdata);

        /// <summary>
        /// Adds new I/Q data with single precision to the file.
        /// Make sure to call WriteOpen() to enter write-mode.
        /// </summary>
        /// <param name="iqdata">I and Q data vectors in single precision that have to be stored.
        /// The vectors have to be ordered as iqdata&lt;Channel1_I, Channel1_Q, Channel2_I, Channel2_Q, ...&gt;
        /// for complex data. The number of I/Q data vectors must match the specified value nofArrays in writeOpen().</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void AppendArrays(IList<IList<double>^>^ iqdata);

        /// <summary>
        /// Adds new I/Q channel data with single precision to the file.
        /// Make sure to call writeOpen() to enter write-mode.
        /// </summary>
        /// <param name="iqdata">Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
        /// for complex data.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void AppendChannels(IList<IList<float>^>^ iqdata);

        /// <summary>
        /// Adds new I/Q channel data with single precision to the file.
        /// Make sure to call writeOpen() to enter write-mode.
        /// </summary>
        /// <param name="iqdata">Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
        /// for complex data.</param>
        /// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
        void AppendChannels(IList<IList<double>^>^ iqdata);
      };
    }
  }
}
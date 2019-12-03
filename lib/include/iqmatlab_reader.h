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

/*!
* @file      iqmatlab_reader.h
*
* @brief     This is the header file of class IqMatlabReader.
*
* @details   This class contains the implementation of IqMatlabReader.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "common.h"

#include "matio.h"

#include "ianalyzecontent.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "enums.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class contains functionality needed to read the contents of a matlab *.mat file.
      * Matlab file format 4, 6, and 7.3 is supported. 
      */
      class IqMatlabReader
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
          @param [in]  updateContent Provides callback methods to process meta data found in file.
        */IqMatlabReader(const std::string& filename, IAnalyzeContent& updateContent);

        /* @copydoc IqMatlab::~IqMatlab() */
        ~IqMatlabReader();

        /**
          @brief Opens the file for reading. If already opened, the file is closed and re-opened again.
        */void open();

        /**
          @brief Closes the file and releases the file handle.
        */void close();

        /**
          @brief Searches a matlab file for all arrays of type float or double that match the specified 
          conditions w.r.t. the minimum number of columns and rows. The names of all matching arrays are 
          returned with parameter arrayNames.
          @param [in]  minCols The minimum number of columns that the matlab array must contain.
          @param [in]  minRows The minimum number of rows that the matlab array must contain.
          @param [in]  exactColMatch If TRUE, the specified number of columns (minCols) must match exactly.
          @param [out]  arrayNames The resulting array names of the query.
          @returns Returns ErrorCode::Success (=0) if no error occurred. For further error codes, see \ref ErrorCodes.
        */void matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames);

        /**
          @param [in]  arrayName Name of the matlab array.
          @returns Returns the number of rows of the matlab array corresponding to the specified name. In case of any
          error, -1 is returned.
        */int getNofRows(const std::string& arrayName);

        /**
          @param [in]  arrayName Name of the matlab array.
          @returns Returns the number of columns of the matlab array corresponding to the specified name. In case of any
          error, -1 is returned.
        */int getNofCols(const std::string& arrayName);

        /**
          @brief Parses the file content and verifies that the file contains all information required
          to be a valid I/Q data file. I.e. all mandatory data and optional meta data is parsed and
          promoted to the callback methods implemented by "updateContent".
        */void analyzeContent();

        /**
          @brief Returns the I/Q data array names found in this file. 
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [out]  arrayNames Names of I/Q data arrays.
        */void getArrayNames(std::vector<std::string>& arrayNames) const;

        /**
          @brief Checks if the specified I/Q data array exists in the file read.
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [in]  arrayName Name of the data array which is assumed in the file.
          @returns Returns TRUE if the I/Q data array was found in the file, otherwise
          FALSE is returned.
        */bool containsArray(const std::string& arrayName) const;

        /**
          @brief Returns the number of I/Q values contained by the specified array.
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [in]  arrayName Name of the corresponding I/Q data array.
          @returns Returns the number of I/Q samples of the specified array.
          If the array does not exist in the file, -1 is returned.
        */long getNofSamples(const std::string& arrayName) const;

        /**
          @returns Returns TRUE if this class has been initialized, i.e. 
          analyseContent() was called and the file content has been parsed
          successfully. Otherwise FALSE is returned.
        */bool isInitialized() const;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single or 
          double precision vector, as defined by the type parameter.
          @pre Make sure to call analyzeContent() to parse the data file.
          @tparam The target precision of the value array - float or double.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to the specified precision,
          independent of the original data type. Make sure that the values array has sufficient size.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset Defines the start position in the I/Q data record at which the read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */template<typename T>
        void readArray(const std::string& arrayName, T* values, size_t nofValues, size_t offset)
        {
          // check if file contains array name
          if (false == this->containsArray(arrayName))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // check if combination of values to read and offset exceeds total number of I/Q values contained in file
          if (nofValues + offset > this->arrayNameToSamples_.at(arrayName))
          {
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }

          // prepare reading I or Q values.
          bool readIValues = true;
          if (Common::strEndsWithIgnoreCase(arrayName, "_Q"))
          {
            readIValues = false;
          }

          // get matlab variable name to read
          int channelNr = this->arrayNameToChannelNo_.at(arrayName);
          std::string matlabArrayName = "Ch" + std::to_string(channelNr) + "_Data";

          // read actual data
          this->readData(matlabArrayName, nofValues, offset, readIValues, values);
        }

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as single or
          double precision vector, as defined by the type parameter.
          @tparam The target precision of the value array - float or double.
          @pre Make sure to call analyzeContent() to initialize an I/Q record before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset Defines the number of I/Q pairs to be skipped before the read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */template<typename T>
        void readChannel(const std::string& channelName, T* values, size_t nofValues, size_t offset)
        {
          // get actual array name
          std::string arrayName;
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            arrayName = channelName;
          }
          else
          {
            arrayName = channelName + "_I";
          }

          // check if file contains array with this name
          if (false == this->containsArray(arrayName))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // get matlab variable name to read
          int channelNr = this->arrayNameToChannelNo_.at(arrayName);
          std::string matlabArrayName = "Ch" + std::to_string(channelNr) + "_Data";

          // read actual data
          this->readChannel(matlabArrayName, nofValues, offset, values);
        }

        /**
          @brief Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
          @tparam The target precision of the value array - float or double.
          @param [in]  arrayName The name of the array to read.
          @param [in]  column The column to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (aka row) within the specified column at which the 
          read operation is started.
          @throws DaiException(InvalidMatlabArrayName) if the specified matlab array name does not exists in the matlab file.
          @throws DaiException(InvalidMatlabArrayType) if the specified matlab array if of an unexpected data type.
          @throws DaiException(InvalidColumnIndex) if the specified column exceeds the size of the matlab array.
          @throws DaiException(InvalidDataInterval) if the specified value range exceeds the array size.
          @throws DaiException(InternalError) if an internal error occurred.
        */template<typename T>
        void readRawArray(const std::string& arrayName, size_t column, size_t nofValues, T* values, size_t offset)
        {
          // check if matio file is read to read
          if (this->matfp_ == nullptr)
          {
            this->open();
          }

          // get matlab variable
          matvar_t* matvar = Mat_VarReadInfo(this->matfp_, arrayName.c_str());
          if (matvar == nullptr)
          {
            throw DaiException(ErrorCodes::InvalidMatlabArrayName);
          }

          // check variable type to match either double or float
          if (matvar->class_type != MAT_C_DOUBLE && matvar->class_type != MAT_C_SINGLE)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidMatlabArrayType);
          }

          // get dimensions of matlab variable 
          auto rows = matvar->dims[0];
          auto cols = matvar->dims[1];

          // does matlab variable dimension match expected size?
          if (column > cols)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidColumnIndex);
          }

          // check read interval to be within matlab variable boundaries
          if (offset + nofValues > rows)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }

          // matlab data is read linearly -> adapt offset
          offset += column * rows;

          int ret = 0;
          if (std::is_same<T, double>::value) // desired output precision of data is double
          {
            // matlab variable contains single precision data
            if (matvar->class_type == MAT_C_SINGLE) 
            {
              // read data and cast type to desired output type
              std::vector<float> tmp(nofValues);
              ret = Mat_VarReadDataLinear(this->matfp_, matvar, tmp.data(), static_cast<int>(offset), 1, nofValues);
              std::copy(tmp.begin(), tmp.end(), values);
            }
            else // otherwise output type does already match matlab type
            {
              ret = Mat_VarReadDataLinear(this->matfp_, matvar, values, static_cast<int>(offset), 1, nofValues);
            }
          }
          else // T == single output precision
          {
            // matlab data contains double precision data
            if (matvar->class_type == MAT_C_DOUBLE)
            {
              // read data and cast type to desired output type
              std::vector<double> tmp(nofValues);
              ret = Mat_VarReadDataLinear(this->matfp_, matvar, tmp.data(), static_cast<int>(offset), 1, nofValues);
              std::copy(tmp.begin(), tmp.end(), values);
            }
            else // otherwise output type does already match matlab type
            {
              ret = Mat_VarReadDataLinear(this->matfp_, matvar, values, static_cast<int>(offset), 1, nofValues);
            }
          }

          Mat_VarFree(matvar);
          if (ret != 0)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

      private:
        /** @brief Private default constructor. */
        IqMatlabReader();

        /** @brief Private copy constructor. */
        IqMatlabReader(const IqMatlabReader&);

        /** @brief Private assignment operator.*/
        IqMatlabReader& operator=(const IqMatlabReader&);

        /**
          @brief Reads I/Q data from the specified matlab array.
          @tparam Precision of the target array - single or double.
          @param [in]  arrayName The name of the matlab array to read.
          @param [in]  nofValues The number of values to read.
          @param [in]  offset Number of pairs to skip
          @param [in]  readIValues If set TRUE, the first column of the array is read, otherwise the second
          column is read.
          @param [in]  values The values read.
          @throws DaiException(InvalidMatlabArrayName) If matlab array name was not found.
          @throws DaiException(InvalidMatlabArrayType) If the found matlab array is not of type double.
          @throws DaiException(InvalidMatlabArraySize) If the found matlab array is ill-sized.
          @throws DaiException(InvalidDataInterval) If combination of offset and number of values to read exceeds the 
          matlab array size.
          @throws DaiException(InternalError) If an error during the actual matlab read operation occurred.
        */template<typename T>
        void readData(const std::string& arrayName, size_t nofValues, size_t offset, bool readIValues, T* values)
        {
          // get matlab variable
          matvar_t* matvar = Mat_VarReadInfo(this->matfp_, arrayName.c_str());
          if (matvar == nullptr)
          {
            throw DaiException(ErrorCodes::InvalidMatlabArrayName);
          }

          // we expect the matlab variable to contain double data
          if (matvar->class_type != MAT_C_DOUBLE)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidMatlabArrayType);
          }

          // check if I/Q data format matches matlab variable dimensions
          if ((this->dataFormat_ == IqDataFormat::Real && matvar->dims[1] != 1) || (this->dataFormat_ != IqDataFormat::Real && matvar->dims[1] != 2))
          {
            throw DaiException(ErrorCodes::InvalidMatlabArraySize);
          }

          // adapt indices for linear matlab read operation if Q-values are going to be read
          size_t maxVal = matvar->dims[0];
          if (false == readIValues)
          {
            offset += matvar->dims[0];
            maxVal += matvar->dims[0];
          }

          // check boundaries
          if (offset + nofValues > maxVal)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }

          // conversion needed in case of float array
          int ret = 0;
          if (std::is_same<T, float>::value)
          {
            // copy double data and cast tmp array if single-precision output is desired
            std::vector<double> tmp(nofValues);
            ret = Mat_VarReadDataLinear(this->matfp_, matvar, tmp.data(), static_cast<int>(offset), 1, nofValues);
            std::copy(tmp.begin(), tmp.end(), values);
          }
          else
          {
            ret = Mat_VarReadDataLinear(this->matfp_, matvar, values, static_cast<int>(offset), 1, nofValues);
          }

          Mat_VarFree(matvar);
          if (ret != 0)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Reads data as channel from the specified matlab array.
          @tparam Precision of the target array - single or double.
          @param [in]  arrayName Name of the matlab array to read.
          @param [in]  nofValues The number of values to read.
          @param [in]  offset Number of pairs to skip.
          @param [in]  values The values read.
          @throws DaiException(InvalidMatlabArrayName) If matlab array name was not found.
          @throws DaiException(InvalidMatlabArrayType) If the found matlab array is not of type double.
          @throws DaiException(InvalidMatlabArraySize) If the found matlab array is ill-sized.
          @throws DaiException(InvalidDataInterval) If combination of offset and number of values to read exceeds the 
          matlab array size.
        */template<typename T>
        void readChannel(const std::string& arrayName, size_t nofValues, size_t offset, T* values)
        {
          // get matlab variable
          matvar_t* matvar = Mat_VarReadInfo(this->matfp_, arrayName.c_str());
          if (matvar == nullptr)
          {
            throw DaiException(ErrorCodes::InvalidMatlabArrayName);
          }

          // we expect the matlab variable to contain double data
          if (matvar->class_type != MAT_C_DOUBLE)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InvalidMatlabArrayType);
          }

          // check if I/Q data format matches matlab variable dimensions
          if ((this->dataFormat_ == IqDataFormat::Real && matvar->dims[1] != 1) || (this->dataFormat_ != IqDataFormat::Real && matvar->dims[1] != 2))
          {
            throw DaiException(ErrorCodes::InvalidMatlabArraySize);
          }

          if (this->dataFormat_ == IqDataFormat::Real)
          {
            if (offset + nofValues > matvar->dims[0])
            {
              Mat_VarFree(matvar);
              throw DaiException(ErrorCodes::InvalidDataInterval);
            }

            if (std::is_same<T, float>::value)
            {
              // conversion needed in case of float array
              std::vector<double> tmp(nofValues);
              
              int ret = Mat_VarReadDataLinear(this->matfp_, matvar, tmp.data(), static_cast<int>(offset), 1, nofValues);
              Mat_VarFree(matvar);
              if (ret != 0)
              {
                throw DaiException(ErrorCodes::InternalError);
              }

              // copy values from tmp to dest array
              std::copy(tmp.begin(), tmp.end(), values);
            }
            else // read to double array
            {
              int ret = Mat_VarReadDataLinear(this->matfp_, matvar, values, static_cast<int>(offset), 1, nofValues);
              Mat_VarFree(matvar);
              if (ret != 0)
              {
                throw DaiException(ErrorCodes::InternalError);
              }
            }
          }
          else // complex data
          {
            if (offset + nofValues / 2 > matvar->dims[0])
            {
              Mat_VarFree(matvar);
              throw DaiException(ErrorCodes::InvalidDataInterval);
            }

            int ret = 0;
            std::vector<double> tempI(nofValues / 2);
            std::vector<double> tempQ(nofValues / 2);
            
            // read I
            ret = Mat_VarReadDataLinear(this->matfp_, matvar, tempI.data(), static_cast<int>(offset), 1, nofValues / 2);
            
            // read Q
            offset = matvar->dims[0] + offset;
            ret = Mat_VarReadDataLinear(this->matfp_, matvar, tempQ.data(), static_cast<int>(offset), 1, nofValues / 2);					
            Mat_VarFree(matvar);
            if (ret != 0)
            {
              throw DaiException(ErrorCodes::InternalError);
            }

            // merge to output vector: data from I and Q vector will be merged to IQIQIQ in values-vector.
            Common::mergeInterleaved(tempI.begin(), tempI.end(), tempQ.begin(), values);
          }
        }

        /**
        @brief Reads the mandatory meta data, that is: application name, comment, timestamp,
        * dataformat, datatype and creates channel information for each channel found (see createChannelInformation()).
        */void readMandatoryMetadata();

        /**@brief Reads all optional user-defined meta data.
        */void readOptionalMetadata();

        /** @brief Creates channel information for each channel found in meta data.
        */void createChannelInformation();

        /**
          @brief Reads data from a matlab character array of dimension [1 x n].
          @param [in]  matfp The matlab file to read from.
          @param [in]  arrayName The name of the array containing the data.
          @returns Returns the value read as string.
          @throws DaiException(InvalidMatlabArrayName) if matlab array was not found in data.
          @throws DaiException(InvalidMatlabArrayType) if matlab array does not satisfy the expected data type.
          @throws DaiException(InvalidMatlabArraySize) if matlab array size does not match the expected array dimensions.
        */static std::string readCharArray(mat_t* const  matfp, const std::string& arrayName);

        /**
          @brief Reads a single double value from a matlab double-precision array of dimension [1 x 1].
          @param [in]  matfp The matlab file to read from.
          @param [in]  arrayName The name of the array containing the data.
          @returns Returns the double value.
          @throws DaiException(InvalidMatlabArrayName) if matlab array was not found in data.
          @throws DaiException(InvalidMatlabArrayType) if matlab array does not satisfy the expected data type.
          @throws DaiException(InvalidMatlabArraySize) if matlab array size does not match the expected array dimensions.
        */static double readDoubleValue(mat_t* const matfp, const std::string& arrayName);

        /**
          @brief Reads key-value meta data from a matlab character array of dimension [2 x n].
          The first row of the array must contain the key, the second row the value information.
          @param [in]  matfp The matlab file to read from.
          @param [in]  arrayName The name of the array containing the data.
          @returns Returns a pair of strings, representing the key-value information.
          @throws DaiException(InvalidMatlabArrayName) if matlab array was not found in data.
          @throws DaiException(InvalidMatlabArrayType) if matlab array does not satisfy the expected data type.
          @throws DaiException(InvalidMatlabArraySize) if matlab array size does not match the expected array dimensions.
        */static std::pair<std::string, std::string> readKeyValue(mat_t* const matfp, const std::string& arrayName);

        /** @brief Callback functions that process meta data found in file. 
        * Called by analyzeContent(). 
        */IAnalyzeContent* updateContent_;

        /** @brief Handle to matlab file. */
        mat_t* matfp_;

        /** @brief Set true, if analyseContent() was called and the file was parsed successfully. */
        bool initialized_;

        /** @brief Filename of the .mat file. */
        const std::string filename_;

        /** @brief I/Q data type of data to be written. */
        IqDataType dataType_;

        /** @brief Data format of this file. */
        IqDataFormat dataFormat_;

        /** @brief Mapping between an array name and the number of samples contained
        * by the array. 
        */std::map<std::string, size_t> arrayNameToSamples_;

        /** @brief Mapping between an array name and the corresponding channel. */
        std::map<std::string, size_t> arrayNameToChannelNo_;
      };
    }
  }
}
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

#ifndef IFILEREADER_H
#define IFILEREADER_H

#include <string>
//lint -save -e451 -e537
#include "Defines.h"
//lint -restore

namespace AmlabFiles
{
  /* CLASS DECLARATION *********************************************/
  /*!
  * @brief   The interface class IFileReader provide operations to
  *          read files
  *
  ******************************************************************/
  class IFileReader
  {
  public:
    /*! Destructor */
    virtual ~IFileReader() { };

    /*! Set the filename of the file to open for reading
    @param a_strFilename The filename of the file to open */
    virtual void                setFilename(const std::wstring& a_strFilename) = 0;
    /*! Return the filename */
    virtual const std::wstring& getFilename() const = 0;
    /*! Return the open status of the file
    true = opened, false = closed */

    virtual bool                isOpen() const = 0;
    /*! Open amlab file for reading. */
    virtual eStatus             open() = 0;
    /*! Close the file and reset to initial status */
    virtual eStatus             close() = 0;
    /*! Reset to initial status */
    virtual eStatus             reset() = 0;
    /*! Return the size of the file in bytes */
    virtual uint64              getFileSize() const = 0;
    /*! Return the last error code and error description occurred by reading.
    @param a_strErrorMsg The out parameter contains the error description. */
    virtual eStatus             getLastError(std::wstring& a_strErrorMsg) = 0;
  };
}

#endif // not IFILEREADER_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

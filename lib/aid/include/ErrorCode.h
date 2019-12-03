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

#ifndef ERRORCODE_H
#define ERRORCODE_H

/* INCLUDE FILES ******************************************************************/
// obsolete - nicht mehr verwenden, nicht mehr erweitern
struct CErrCode
{
  static const uint32 kERR_FILES_BLOCKSETDATANOMEM;
  static const uint32 kERR_FILES_BLOCKGETDATANOMEM;
  static const uint32 kERR_FILES_BLOCKGETDATAEOF;
  static const uint32 kERR_FILES_BLOCKREADEOF;
  static const uint32 kERR_FILES_BLOCKREADNOSTREAM;

  static const uint32 kERR_FILES_SSETNOMEM;
  static const uint32 kERR_FILES_SREADEOF;
  static const uint32 kERR_FILES_SREADNOMEM;
  static const uint32 kERR_FILES_SGETNOSIZE;
  static const uint32 kERR_FILES_SREADNOSTREAM;

  static const uint32 kERR_FILES_ZFSETNOMEM;
  static const uint32 kERR_FILES_ZFREADEOF;
  static const uint32 kERR_FILES_ZFREADNOMEM;
  static const uint32 kERR_FILES_ZFGETNOSIZE;
  static const uint32 kERR_FILES_ZFREADNOSTREAM;

  static const uint32 kERR_FILES_FRAMEZFNOMEM;
  static const uint32 kERR_FILES_FRAMESNOMEM;
  static const uint32 kERR_FILES_FRAMENOSIZE;
  static const uint32 kERR_FILES_FRAMEREADNOMEM;

  static const uint32 kERR_FILES_NOWRITING;
  static const uint32 kERR_FILES_NOREADING;
  static const uint32 kERR_FILES_NOFRAMES;
  static const uint32 kERR_FILES_NOFRAMEMEM;

  static const uint32 kERR_FILES_OUTOFSYNC;
  static const uint32 kERR_FILES_IPP_ERROR;

  static const uint32 kERR_LAST_ERROR;
};
// obsolete - nicht mehr verwenden, nicht mehr erweitern

#endif // not ERRORCODE_H


/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

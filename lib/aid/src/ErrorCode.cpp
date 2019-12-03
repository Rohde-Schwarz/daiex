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

/* INCLUDE FILES ******************************************************************/
#include "Defines.h"
#include "ErrorCode.h"

//***************************************************************************
//*** File Errors ***//
//***************************************************************************

const uint32 CErrCode::kERR_FILES_BLOCKSETDATANOMEM           =  0;
const uint32 CErrCode::kERR_FILES_BLOCKGETDATANOMEM           =  1;
const uint32 CErrCode::kERR_FILES_BLOCKGETDATAEOF             =  2;
const uint32 CErrCode::kERR_FILES_BLOCKREADEOF                =  3;
const uint32 CErrCode::kERR_FILES_BLOCKREADNOSTREAM           =  4;

const uint32 CErrCode::kERR_FILES_SSETNOMEM                   =  5;
const uint32 CErrCode::kERR_FILES_SREADEOF                    =  6;
const uint32 CErrCode::kERR_FILES_SREADNOMEM                  =  7;
const uint32 CErrCode::kERR_FILES_SGETNOSIZE                  =  8;
const uint32 CErrCode::kERR_FILES_SREADNOSTREAM               =  9;

const uint32 CErrCode::kERR_FILES_ZFSETNOMEM                  = 10;
const uint32 CErrCode::kERR_FILES_ZFREADEOF                   = 11;
const uint32 CErrCode::kERR_FILES_ZFREADNOMEM                 = 12;
const uint32 CErrCode::kERR_FILES_ZFGETNOSIZE                 = 13;
const uint32 CErrCode::kERR_FILES_ZFREADNOSTREAM              = 14;

const uint32 CErrCode::kERR_FILES_FRAMEZFNOMEM                = 15;
const uint32 CErrCode::kERR_FILES_FRAMESNOMEM                 = 16;
const uint32 CErrCode::kERR_FILES_FRAMENOSIZE                 = 17;
const uint32 CErrCode::kERR_FILES_FRAMEREADNOMEM              = 18;

const uint32 CErrCode::kERR_FILES_NOWRITING                   = 19;
const uint32 CErrCode::kERR_FILES_NOREADING                   = 20;
const uint32 CErrCode::kERR_FILES_NOFRAMES                    = 21;
const uint32 CErrCode::kERR_FILES_NOFRAMEMEM                  = 22;

const uint32 CErrCode::kERR_FILES_OUTOFSYNC                   = 23;
const uint32 CErrCode::kERR_FILES_IPP_ERROR                   = 24;

const uint32 CErrCode::kERR_LAST_ERROR                        = 25;

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

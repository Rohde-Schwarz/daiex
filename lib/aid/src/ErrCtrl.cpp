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

#include "ErrCtrl.h"

/* STATIC INITIALIZATION **********************************************************/

/* ------------------------------------------------------------------------------ */

CErrCtrl::CErrCtrl()
: m_pErrorHandler   ( 0 )
, m_u32LineNumer    ( 0 )
, m_strSourceFile   ( )
, m_u32ErrorCode    ( 0 )
, m_strErrorMsg     ( )
, m_strInfoMsg      ( )
, m_cError( CErrCode::kERR_LAST_ERROR )
{
  initErrorMessages();
}

//lint -save -e1540
CErrCtrl::~CErrCtrl()
{

}
//lint -restore

//lint -save -e952
void CErrCtrl::setErrorHandler(AmlabCommon::IActionListener* a_pHandler)
{ 
  m_pErrorHandler = a_pHandler;
}
//lint -restore

uint32 CErrCtrl::getLineNumber() const
{
  return m_u32LineNumer;
}

const std::string& CErrCtrl::getSourceFile() const
{
  return m_strSourceFile;
}

uint32 CErrCtrl::getErrorCode() const
{
  return m_u32ErrorCode;
}

const std::string& CErrCtrl::getErrorMsg() const
{
  return m_strErrorMsg;
}

const std::string& CErrCtrl::getInfoMsg() const
{
  return m_strInfoMsg;
}

void CErrCtrl::initErrorMessages()
{
  //* Block level *//
  m_cError[CErrCode::kERR_FILES_BLOCKSETDATANOMEM]  = std::string( "No memory available while creating data block for writing.");
  m_cError[CErrCode::kERR_FILES_BLOCKGETDATANOMEM]  = std::string( "No memory available while creating data block for reading.");
  m_cError[CErrCode::kERR_FILES_BLOCKGETDATAEOF]    = std::string( "End of file reached while reading data.");
  m_cError[CErrCode::kERR_FILES_BLOCKREADEOF]       = std::string( "End of file reached while scanning file.");
  m_cError[CErrCode::kERR_FILES_BLOCKREADNOSTREAM]  = std::string( "No stream object available.");
  //* Spectra-Dataset level *//
  m_cError[CErrCode::kERR_FILES_SSETNOMEM]          = std::string( "No memory while creating a spectra block buffer.");
  m_cError[CErrCode::kERR_FILES_SREADEOF]           = std::string( "End of file reached while reading spectra header.");
  m_cError[CErrCode::kERR_FILES_SREADNOMEM]         = std::string( "No memory while creating a spectra block object.");
  m_cError[CErrCode::kERR_FILES_SGETNOSIZE]         = std::string( "Size of spectra data block is zero.");
  m_cError[CErrCode::kERR_FILES_SREADNOSTREAM]      = std::string( "No stream object available.");
  //* ZF-Dataset level *//
  m_cError[CErrCode::kERR_FILES_ZFSETNOMEM]         = std::string( "No memory while creating a IF block buffer.");
  m_cError[CErrCode::kERR_FILES_ZFREADEOF]          = std::string( "End of file reached while reading IF header.");
  m_cError[CErrCode::kERR_FILES_ZFREADNOMEM]        = std::string( "No memory while creating a IF block object.");
  m_cError[CErrCode::kERR_FILES_ZFGETNOSIZE]        = std::string( "IF data block size is zero.");
  m_cError[CErrCode::kERR_FILES_ZFREADNOSTREAM]     = std::string( "No stream object available.");
  //* ZF-Frame level *//
  m_cError[CErrCode::kERR_FILES_FRAMEZFNOMEM]       = std::string( "No memory while creating a IF set object.");
  m_cError[CErrCode::kERR_FILES_FRAMESNOMEM]        = std::string( "No memory while creating an spectrum set object.");
  m_cError[CErrCode::kERR_FILES_FRAMENOSIZE]        = std::string( "Data block size is zero.");
  m_cError[CErrCode::kERR_FILES_FRAMEREADNOMEM]     = std::string( "No memory while creating spectrum/IF set object for reading.");
  //* ZFS-Data level *//
  m_cError[CErrCode::kERR_FILES_NOWRITING]          = std::string( "File could not be opened for writing.");
  m_cError[CErrCode::kERR_FILES_NOREADING]          = std::string( "File could not be opened for reading.");
  m_cError[CErrCode::kERR_FILES_NOFRAMES]           = std::string( "No frames available.");
  m_cError[CErrCode::kERR_FILES_NOFRAMEMEM]         = std::string( "No memory for Frames.");

  m_cError[CErrCode::kERR_FILES_OUTOFSYNC]          = std::string( "File out of sync while reading.");
  m_cError[CErrCode::kERR_FILES_IPP_ERROR]          = std::string( "IPP error while reading.");
}

//lint -save -e952
void CErrCtrl::error(uint32 a_u32ErrCode, uint32 a_u32Line, const std::string& a_strFile, const std::string& a_strInfo)
{
  m_u32ErrorCode  = a_u32ErrCode;
  m_u32LineNumer  = a_u32Line;
  m_strSourceFile = a_strFile;
  m_strInfoMsg    = a_strInfo;
  m_strErrorMsg   = m_cError[a_u32ErrCode];

  if ( m_pErrorHandler )
  {
    AmlabCommon::CActionEvent l_evt(this, 0, L"CErrCtrl::error");
    m_pErrorHandler->actionPerformed( l_evt );
  }
}
//lint -restore

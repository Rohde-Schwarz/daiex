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

#ifndef ERR_CTRL_H
#define ERR_CTRL_H

//***************************************************************************

#include <string>
#include <vector>

//lint -save -e451 -e537 -e1790
#include "Types.h"
#include "Singleton.h"
#include "ActionListener.h"
//lint -restore

#include "ErrorCode.h"

/***************************************************************************/
/** @class    CErrorControl
*   @brief    Declartion of CErrorControl - the global error object
****************************************************************************/
  //lint -save -e1509 -e1790
class CErrCtrl
  : public AmlabCommon::CSingleton<CErrCtrl>
  , public AmlabCommon::CObject
{
public:
  friend class AmlabCommon::CSingleton<CErrCtrl>;

  void error(uint32 a_u32ErrCode, uint32 a_u32Line, const std::string& a_strFile, const std::string& a_strInfo = "");
  void setErrorHandler(AmlabCommon::IActionListener* a_pHandler);

  uint32 getLineNumber() const;
  const std::string& getSourceFile() const;
  uint32 getErrorCode() const;
  const std::string& getErrorMsg() const;
  const std::string& getInfoMsg() const;

private:
  //lint -save -e1704
  CErrCtrl();  
  ~CErrCtrl();  
  //lint -restore

  void initErrorMessages();

  AmlabCommon::IActionListener* m_pErrorHandler;
  uint32        m_u32LineNumer;
  std::string   m_strSourceFile;
  uint32        m_u32ErrorCode;
  std::string   m_strErrorMsg;
  std::string   m_strInfoMsg;
  std::vector<std::string>  m_cError;      /*! vector containing the code->text assignments */
};
//lint -restore

//***************************************************************************
#endif //ERR_CTRL_H
//***************************************************************************

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
#include "ActionEvent.h"


namespace AmlabCommon
{
  /* METHOD *************************************************************************/
  /**
  * @brief    Default Constructor.
  * 
  *           Initializes all members with zero.
  *
  * @warning  JUST IMPLEMENTED FOR UNIT TESTING PURPOSES.
  ***********************************************************************************/
  CActionEvent::CActionEvent()
    : m_pSource     ( 0 )
    , m_i32EventId  ( 0 )
    , m_strCommand  (  )
  {

  }


  /* METHOD *************************************************************************/
  /**
  * @brief    Copy Constructor.
  * 
  *           Creates a copy of the given ActionEvent object.
  *
  * @warning  JUST IMPLEMENTED FOR UNIT TESTING PURPOSES.
  ***********************************************************************************/
  CActionEvent::CActionEvent(const CActionEvent& a_rhs)
    : m_pSource     ( a_rhs.m_pSource )
    , m_i32EventId  ( a_rhs.m_i32EventId )
    , m_strCommand  ( a_rhs.m_strCommand )
  {

  }


  /* METHOD *************************************************************************/
  /**
  * @brief    Copy Assignment.
  *
  * @warning  JUST IMPLEMENTED FOR UNIT TESTING PURPOSES.
  ***********************************************************************************/
  CActionEvent& CActionEvent::operator=(const CActionEvent& a_rhs)
  {
    m_pSource   = a_rhs.m_pSource;
    m_i32EventId = a_rhs.m_i32EventId;
    m_strCommand = a_rhs.m_strCommand;

    return *this;
  }


  /* METHOD *************************************************************************/
  /**
  * @brief  Constructs an ActionEvent object.
  * 
  * @param  a_pSource     The object that originated the event.
  * @param  a_i32EventId  An integer that identifies the event.
  * @param  a_strCommand  A string that may specify a command associated with the event.
  ***********************************************************************************/
  CActionEvent::CActionEvent(CObject* a_pSource, int32 a_i32EventId, std::wstring  a_strCommand)
    : m_pSource     ( a_pSource )
    , m_i32EventId  ( a_i32EventId )
    , m_strCommand  ( a_strCommand )
  {

  }

  /* METHOD *************************************************************************/
  /**
  * @brief  Destructor 
  ***********************************************************************************/
  CActionEvent::~CActionEvent()
  {

  }

  /* METHOD *************************************************************************/
  /**
  * @brief   Returns the object that originated the event.
  * 
  * @return  The object that originated the event.
  ***********************************************************************************/
  CObject* CActionEvent::getSource() const
  {
    return m_pSource;
  }

  /* METHOD *************************************************************************/
  /**
  * @brief  Returns the integer that identifies the event.
  * 
  * @return  The integer that identifies the event.
  ***********************************************************************************/
  int CActionEvent::getId() const
  {
    return m_i32EventId;
  }

  /* METHOD *************************************************************************/
  /**
  * @brief  Returns the string that specifies a command associated with the event.
  * 
  * @return The string that specifies a command associated with the event.
  ***********************************************************************************/
  const std::wstring& CActionEvent::getCommand() const
  {
    return m_strCommand;
  }

  /* METHOD **************************************************************************
  @brief    Set the command string of the action event
  @date     19.10.2015
  ***********************************************************************************/
  void CActionEvent::setCommand( const std::wstring& a_command )
  {
    m_strCommand = a_command;
  }
}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

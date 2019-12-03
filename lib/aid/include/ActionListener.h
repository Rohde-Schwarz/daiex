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

#ifndef ACTION_LISTENER_H
#define ACTION_LISTENER_H

/* INCLUDE FILES ******************************************************************/
#include "ActionEvent.h"

namespace AmlabCommon
{
  /* CLASS DECLARATION **************************************************************/
  /*!
  * @brief  The listener interface for receiving action events.
  *
  *         The class that is interested in processing an action event implements
  *         this interface, and the object created with that class is registered with
  *         a component, using the component's addActionListener method. When the
  *         action occurs, that object's actionPerformed method is invoked.
  *
  * @see CActionEvent
  ***********************************************************************************/
  class IActionListener
  {
  protected:
    /* METHOD *************************************************************************/
    /**
    * @brief  Constructor.
    ***********************************************************************************/
    IActionListener() { };

  public:
    /* METHOD *************************************************************************/
    /**
    * @brief  Destructor.
    ***********************************************************************************/
    virtual ~IActionListener() { };

    /* METHOD *************************************************************************/
    /**
    * @brief  Invoked when an action occurs.
    ***********************************************************************************/
    virtual void actionPerformed( const CActionEvent& a_Event ) = 0;
  };
}

#endif  // not ACTION_LISTENER_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

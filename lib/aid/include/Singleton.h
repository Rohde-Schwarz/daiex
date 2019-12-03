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

#ifndef SINGLETON_H
#define SINGLETON_H

/* INCLUDE FILES ******************************************************************/

namespace AmlabCommon
{
  class CSingletonTest;

  template<typename T>
  class CSingleton
  {
  public:
    friend class CSingletonTest;

    static T& Instance();
    static void Destroy();

  protected:
    CSingleton();
    ~CSingleton();

  private:
    CSingleton(const CSingleton&);
    CSingleton& operator=(const CSingleton&);

    static T* CreateInstance();
    static void DestroyInstance(T*);

    static T* m_pInstance;
  };

  #include "SingletonImpl.h"
}

#endif // not SINGLETON_H


/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
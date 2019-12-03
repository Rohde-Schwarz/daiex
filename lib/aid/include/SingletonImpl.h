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

#ifndef SINGLETONIMPL_H
#define SINGLETONIMPL_H

template<typename T>
T* CSingleton<T>::m_pInstance = 0;

template<typename T>
CSingleton<T>::CSingleton()
{

}

template<typename T>
CSingleton<T>::~CSingleton()
{

}

template<typename T>
T& CSingleton<T>::Instance()
{
  if ( !CSingleton::m_pInstance )
    CSingleton::m_pInstance = CreateInstance();

  return *CSingleton::m_pInstance;
}

template<typename T>
void CSingleton<T>::Destroy()
{
  if ( CSingleton::m_pInstance )
  {
    DestroyInstance( m_pInstance );
    m_pInstance = 0;
  }
}


template<typename T>
T* CSingleton<T>::CreateInstance()
{
  return new T();
}

template<typename T>
void CSingleton<T>::DestroyInstance(T* a_pInstance)
{
  delete a_pInstance;
}




#endif // not SINGLETONIMPL_H


/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
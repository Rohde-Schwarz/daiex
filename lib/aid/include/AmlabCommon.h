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

#ifndef AMLABCOMMON_H
#define AMLABCOMMON_H

#include <cassert>

/*!
  The (result of the) expression or identifier passed to this macro is not used.
  Mostly used to suppress compiler warnings about unused formal parameters to methods or
  initialized but not referenced local variables.
*/
#define AC_UNUSED(expr)     ((void) expr);

/*! \def AC_ASSERT(expr)
The AC_ASSERT macro can be used to check the boolean result of an expression in debug builds.
In release builds this macro evaluates to no code.
*/

/*! \def AC_VERIFY(expr)
The AC_VERIFY macro can be used to check the boolean result of an expression in debug builds.
In release builds the expression is still executed but the result is ignored.
This is useful to find conceptual errors in debug builds and fix them.
The error should not show up in release builds anymore (thus the result is not needed then)
*/

#ifdef _DEBUG

#define AC_ASSERT(expr)     assert(expr)
#define AC_VERIFY(expr)     AC_ASSERT(expr)

#else


#define AC_ASSERT(expr)     ((void) 0)
#define AC_VERIFY(expr)     ((void)(expr))

#endif

#endif // not AMLABCOMMON_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/

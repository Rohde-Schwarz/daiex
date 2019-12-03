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

/* @cond HIDDEN_SYMBOLS */
#pragma once
#include "idataimportexport.h"

class MOSAIK_MODULE WvScramblerBase
{
public:
	WvScramblerBase();
	virtual ~WvScramblerBase();
	virtual char* optBytes();
	virtual int GetOptWords(unsigned int *uOptWords);
	virtual void descramble(char *readBuffer, char **bufferStart, int &noSamplesInBuffer, long &headerBytes);
	virtual void descrambleNext(unsigned int buff[], int val_count);
	virtual void reload();
};



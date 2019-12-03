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

#include <memory.h>
#include <stdio.h>
#include "WvScramblerBase.h"

WvScramblerBase::WvScramblerBase() {}
WvScramblerBase::~WvScramblerBase() {}
char* WvScramblerBase::optBytes() { return (char *)0; }
int WvScramblerBase::GetOptWords(unsigned int * /*uOptWords*/) {	return(0);}
void WvScramblerBase::descramble(char * /*readBuffer*/, char ** /*bufferStart*/, int & /*noSamplesInBuffer*/, long & /*headerBytes*/) {}
void WvScramblerBase::descrambleNext(unsigned int /*buff*/[], int /*val_count*/) {}
void WvScramblerBase::reload() {}
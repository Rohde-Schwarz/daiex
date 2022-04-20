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


#ifndef _I_WVIN_H_
#define _I_WVIN_H_

#include <vector>
#include <string>

#ifdef __GNUC__
#define WVIN_TEXT_UNUSED __attribute__ ((unused))
#else
#define WVIN_TEXT_UNUSED
#endif

static const char WVIN_TEXT_UNUSED *sWvInErrorText[12] =
{
    "all ok",
    "general error",
    "file already open",
    "file open failed",
    "memory allocation error",
    "premature end of file, possibly wrong file format",
    "unsupported file type",
    "CLOCK tag not found",
    "SAMPLES tag not found",
    "multi segment error",
    "file not yet opened",
    "premature end of file"
};

/* CLASS ******************************************************************/
/**
Waveform input
*
******************************************************************************/
class IWvIn
{
public:

    enum eParamID
    {
        eParamSamples,
        eParamClock,
        eParamLevelOffset,
        eParamComment,
        eParamRfRmsLevel,
        eParamDate,
        eParamCopyRight,
        eParamMSegSettingsFile,
        eParamMSegCount,
        eParamMSegStart,
        eParamMSegLength,
        eParamMSegClock,
        eParamMSegLevelOffsetRms,
        eParamMSegLevelOffsetPeak,
        eParamMSegDuration,
        eParamMSegClockMode,
        eParamMSegLevelMode,
        eParamMSegFiles,
        eParamMSegComment, 
        eParamMSegNext,      // parameter usually in sequencer file
        eParamMSegCycles,    // parameter usually in sequencer file
        eParamCrc,
        eParamIsScrambled,
        eParamType
    } ;

    virtual ~IWvIn() { /* must have for interfaces !!! */ }

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      iParam:   int Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, int *iParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      uParam:   unsigned int Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, unsigned int *uParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID:  Parameter ID
    @param      ullParam: unsigned long long Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, unsigned long long *ullParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      uNumOfValues: number of array elements returned by uParam
    @param      uParam:   unsigned int Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned int *uParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      uNumOfValues: number of array elements returned by ullParam
    @param      ullParam:   unsigned long long Parameter (Array)
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned long long *ullParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get integer parameter
    *
    @param      paramID: Parameter ID
    @param      dParam:   double Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, double *dParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      dParam1:  double Parameter 1
    @param      dParam2:  double Parameter 2
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, double *dParam1, double *dParam2) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      uNumOfValues: number of array elements returned by ullParam
    @param      dParam:   double Parameter (Array)
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, unsigned int uNumOfValues, double *dParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      sParam:  vector of string parameters
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, std::vector<std::string> *sParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID:   Parameter ID
    @param      sParam:    one string parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetParam(eParamID paramID, std::string *sParam) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      uOptWords: input 4 option words
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int GetOptWords(unsigned int *uOptWords) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Open File
    *
    @param      sFileName: Filename including path
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int Open(char *sFileName) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Is file open?
    *
    @return     true:  File is open
                false: File is closed
    ******************************************************************************/
    virtual bool IsOpen(void) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Close File
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int Close() = 0;

    /* FUNCTION ******************************************************************/
    /**
    Read Samples
    *
    @param      uNoSamples: Number of Samples
    @param      pBuffer: Pointer to data
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    virtual int ReadSamples(unsigned int uNoSamples, unsigned int *pBuffer) = 0;

    /* FUNCTION ******************************************************************/
    /**
    Returns original (scrambled) option words as bytes for CRC
    *
    *
    @return     value
    ******************************************************************************/
    virtual char* optBytes() = 0;

    /* FUNCTION ******************************************************************/
    /**
    Returns the offset of the opt words and IQ samples inside the waveform file as well as if waveform is scrambled or not.
    If waveform is not scrambled offset for opWords will be set to -1
    *
    ******************************************************************************/
    virtual void GetSamplesOffset(unsigned int& ui_optWordsOffset, unsigned int& ui_IqSamplesOffset, bool& b_isScrambled) = 0;

};

#endif

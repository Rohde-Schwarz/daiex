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

#pragma once

#include "IWvIn.h"
#include "WvScramblerBase.h"

//class CLBCrcCalculator;

/* CLASS ******************************************************************/
/**
Waveform input from file
*
******************************************************************************/
class CLBWvInFile : public IWvIn
{
public:
    /* FUNCTION ******************************************************************/
    /**
    Constructor
    *
    @return     none
    ******************************************************************************/
    CLBWvInFile();

    /* FUNCTION ******************************************************************/
    /**
    Destructor
    *
    @return     none
    ******************************************************************************/
    ~CLBWvInFile();

	/* FUNCTION ******************************************************************/
	/**
	setScrambler
	*
	@return     none
	******************************************************************************/
	void setScrambler(WvScramblerBase* scrambler);

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      iParam:   int Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, int *iParam);

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID: Parameter ID
    @param      uParam:   unsigned int Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, unsigned int *uParam);

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID:  Parameter ID
    @param      ullParam: unsigned long long Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, unsigned long long *ullParam);

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
    int GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned int *uParam);

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
    int GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned long long *ullParam);

    /* FUNCTION ******************************************************************/
    /**
    Set integer parameter
    *
    @param      paramID: Parameter ID
    @param      dParam:   double Parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, double *dParam);

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
    int GetParam(eParamID paramID, double *dParam1, double *dParam2);

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
    int GetParam(eParamID paramID, unsigned int uNumOfValues, double *dParam);

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID:   Parameter ID
    @param      sParam:    vector of string parameters
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, std::vector<std::string> *sParam);

    /* FUNCTION ******************************************************************/
    /**
    Get parameter
    *
    @param      paramID:   Parameter ID
    @param      sParam:    one string parameter
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int GetParam(eParamID paramID, std::string *sParam);

    /* FUNCTION ******************************************************************/
    /**
    Open File
    *
    @param      sFileName: Filename including path
    *
    @return     0: all ok
                1: general error
                2: file already open
                3: file open failed
                4: memory allocation error
                5: premature end of file, possibly wrong file format
                6: unsupported file type
                7: CLOCK tag not found
                8: SAMPLES tag not found
                9: multi segment error
    ******************************************************************************/
    int Open(char *sFileName);

    /* FUNCTION ******************************************************************/
    /**
    Is file open?
    *
    @return     true:  File is open
                false: File is closed
    ******************************************************************************/
    bool IsOpen(void)  { return m_bFileIsOpen; }

    /* FUNCTION ******************************************************************/
    /**
    Close File
    *
    @return     0: all ok, >0: error
    ******************************************************************************/
    int Close();

    /* FUNCTION ******************************************************************/
    /**
    Read Samples
    *
    @param      uNoSamples: Number of Samples
    @param      pBuffer: Pointer to data
    *
    @return     0: all ok
                1: general error
                10: file not yet opened
                11: premature end of file
    ******************************************************************************/
    int ReadSamples(unsigned int uNoSamples, unsigned int *pBuffer);

    // new version with random access
    int ReadSamples(unsigned int uOffset, unsigned int uNoSamples, unsigned int *pBuffer);

    /* FUNCTION ******************************************************************/
    /**
    Returns the offset of the opt words and IQ samples inside the waveform file as well as if waveform is scrambled or not.
    If waveform is not scrambled offset for opWords will be set to -1
    *
    ******************************************************************************/
    void GetSamplesOffset(unsigned int& ui_optWordsOffset, unsigned int& ui_IqSamplesOffset, bool& b_isScrambled);

	char* optBytes() { return 0; }
	int GetOptWords(unsigned int *) { return 0; }

private:
    bool m_bFileIsOpen;
    FILE *m_pFile;
    bool m_bScramble;
    int m_iBufferSize;
    char *m_pReadBuffer;
    char *m_pReadBuffer2;
    int m_iNoSamplesInBuffer;
    char *m_pBufferStart;
    unsigned long long m_ullCurSampleInFile;
    long m_iHeaderBytes;
    long m_iBytesAfterFirstRead;
    unsigned int m_uCrcTag;
    bool m_isFirstRead;
    
    //Signal parameters
    unsigned long long m_ullSamples;
    double m_dClock;
    double m_dLevelRms;
    double m_dLevelPeak;
    char m_sType[16];
    char m_sDateTime[32];
    char m_sCopyRight[64];
    char m_sComment[512];
    unsigned int m_uMSegCount;
    int m_iMSegClockMode;
    int m_iMSegLevelMode;
    std::vector<unsigned long long> m_ullMSegStart;
    std::vector<unsigned long long> m_ullMSegLength;
    std::vector<double> m_dMSegClock;
    std::vector<double> m_dMSegLevelRms;
    std::vector<double> m_dMSegLevelPeak;
    std::vector<double> m_dMSegDuration;
    std::vector<std::string> m_sMSegComment;
    std::vector<std::string> m_sMSegFileName;
    std::string m_sMSegSettingsFileName;
	WvScramblerBase *m_scrambler;
	bool m_scramblerSet;
};
/* @\endcond HIDDEN_SYMBOLS */

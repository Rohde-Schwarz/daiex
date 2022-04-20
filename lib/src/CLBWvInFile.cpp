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
#define _CRT_SECURE_NO_DEPRECATE
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <new>
#ifdef __BIG_ENDIAN__
#include <byteswap.h>
#endif
#include "CLBWvInFile.h"
//#include "CLBCrcCalculator.h"

#define ROTATE_LEFT(x) ( (x << 1) | (x >> 31) )
#define ROTATE_RIGHT(x) ( (x >> 1) | (x << 31) )


CLBWvInFile::CLBWvInFile()
   : m_bFileIsOpen(false)
   , m_pFile(NULL)
   , m_bScramble(false)
   , m_iBufferSize(1000000)
   , m_pReadBuffer(NULL)
   , m_pReadBuffer2(NULL)
   , m_iNoSamplesInBuffer(0)
   , m_pBufferStart(NULL)
   , m_ullCurSampleInFile(0)
   , m_iHeaderBytes(0)
   , m_iBytesAfterFirstRead(0)
   , m_uCrcTag(0)
   , m_ullSamples(0)
   , m_dClock(0.0)
   , m_dLevelRms(0.0)
   , m_dRfRmsLevel(0.0)
, m_dLevelPeak(0.0)
, m_uMSegCount(0)
, m_iMSegClockMode(0)
, m_iMSegLevelMode(0)
, m_scramblerSet(false)
{
    memset(m_sType, 0, sizeof(m_sType));
    memset(m_sDateTime, 0, sizeof(m_sDateTime));
    memset(m_sCopyRight, 0, sizeof(m_sCopyRight));
    memset(m_sComment, 0, sizeof(m_sComment));
}

CLBWvInFile::~CLBWvInFile()
{
    if(m_bFileIsOpen)
        Close();

    if(m_pReadBuffer != NULL)
        delete [] m_pReadBuffer;
    if(m_pReadBuffer2 != NULL)
        delete [] m_pReadBuffer2;

    m_pReadBuffer=NULL;
    m_pReadBuffer2=NULL;
}

void CLBWvInFile::setScrambler(WvScramblerBase* scrambler)
{
	m_scrambler = scrambler;
	m_scramblerSet = true;
}


int CLBWvInFile::GetParam(eParamID paramID, int *iParam)
{
    if(iParam == NULL)
        return 1;

    switch (paramID)
    {
    case eParamMSegClockMode:
        *iParam = m_iMSegClockMode;
        return 0;
    case eParamMSegLevelMode:
        *iParam = m_iMSegLevelMode;
        return 0;
    case eParamIsScrambled:
        *iParam = static_cast<int>(m_bScramble);
        return 0;
    default:
        break;
    }

    return 1;
}

int CLBWvInFile::GetParam(eParamID paramID, unsigned int *uParam)
{
    if(uParam == NULL)
        return(1);

    switch (paramID)
    {
    case eParamSamples:
        *uParam = static_cast<unsigned int>(m_ullSamples);
        return(0);
    case eParamMSegCount:
        *uParam = m_uMSegCount;
        return(0);
    case eParamMSegClockMode:
        *uParam = static_cast<unsigned int>(m_iMSegClockMode);
        return(0);
    case eParamMSegLevelMode:
        *uParam = static_cast<unsigned int>(m_iMSegLevelMode);
        return(0);
    case eParamCrc:
        *uParam = static_cast<unsigned int>(m_uCrcTag);
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, unsigned long long *ullParam)
{
    if(ullParam == NULL)
        return(1);

    switch (paramID)
    {
    case eParamSamples:
        *ullParam = m_ullSamples;
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned int *uParam)
{
    if(uParam == NULL)
        return(1);

    switch (paramID)
    {
    case eParamMSegNext:    // parameter usually in sequencer file
        for (unsigned int i = 0; i < uNumOfValues; i++)
            uParam[i] = (i + 1) % uNumOfValues;
        return(0);
    case eParamMSegCycles:  // parameter usually in sequencer file
        for (unsigned int i = 0; i < uNumOfValues; i++)
            uParam[i] = 1;
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, unsigned int uNumOfValues, unsigned long long *ullParam)
{
    if(ullParam == NULL)
        return(1);

    switch (paramID)
    {
    case eParamMSegStart:
        if (m_ullMSegStart.empty()) return 1;
        memcpy(ullParam, &m_ullMSegStart.front(), uNumOfValues * sizeof(unsigned long long));
        return(0);
    case eParamMSegLength:
        if (m_ullMSegLength.empty()) return 1;
        memcpy(ullParam, &m_ullMSegLength.front(), uNumOfValues * sizeof(unsigned long long));
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, double *dParam)
{
    if(dParam == NULL)
        return(1);

    switch (paramID)
    {
    case eParamClock:
        *dParam = m_dClock;
        return(0);
    case eParamRfRmsLevel:
        *dParam = m_dRfRmsLevel;
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, double *dParam1, double *dParam2)
{
    if(dParam1 == NULL || dParam2 == NULL)
        return(1);

    switch (paramID)
    {
    case eParamLevelOffset:
        *dParam1 = m_dLevelRms;
        *dParam2 = m_dLevelPeak;
        return(0);
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, unsigned int uNumOfValues, double *dParam)
{
    if(dParam == NULL)
        return 1;

    switch (paramID)
    {
    case eParamMSegClock:
        if (m_dMSegClock.empty()) return 1;
        memcpy(dParam, &m_dMSegClock.front(), uNumOfValues * sizeof(double));
        return 0;
    case eParamMSegLevelOffsetRms:
        if (m_dMSegLevelRms.empty()) return 1;
        memcpy(dParam, &m_dMSegLevelRms.front(), uNumOfValues * sizeof(double));
        return 0;
    case eParamMSegLevelOffsetPeak:
        if (m_dMSegLevelPeak.empty()) return 1;  
        memcpy(dParam, &m_dMSegLevelPeak.front(), uNumOfValues * sizeof(double));
        return 0;
    case eParamMSegDuration:
        if (m_dMSegDuration.empty()) return 1;  
        memcpy(dParam, &m_dMSegDuration.front(), uNumOfValues * sizeof(double));
        return 0;
    default:
        break;
    }

    return(1);
}

int CLBWvInFile::GetParam(eParamID paramID, std::vector<std::string> *sParam)
{
    if (sParam == NULL)
        return 1;
    
    sParam->clear();
    switch (paramID)
    {
    case eParamMSegFiles:
        *sParam=m_sMSegFileName;
        return 0;
    case eParamMSegComment:
        *sParam=m_sMSegComment;
        return 0;
    default:
        break;
    }

    return 1;
}

int CLBWvInFile::GetParam(eParamID paramID, std::string *sParam)
{
    if (sParam == NULL)
        return 1;

    sParam->clear();
    switch (paramID)
    {
    case eParamType:
        sParam->assign(m_sType);
        return 0;
    case eParamDate:
        sParam->assign(m_sDateTime);
        return 0;
    case eParamCopyRight:
        sParam->assign(m_sCopyRight);
        return 0;
    case eParamComment:
        sParam->assign(m_sComment);
        return 0;
    case eParamMSegSettingsFile:
        sParam->assign(m_sMSegSettingsFileName);
        return 0;
    case eParamMSegClockMode:
        switch (m_iMSegClockMode)
        {
        case 0:
            sParam->assign("UNCHANGED");
            break;
        case 1:
            sParam->assign("HIGHEST");
            break;
        case 2: 
            sParam->assign("USER"); 
            break;
        }
        return 0;
    case eParamMSegLevelMode:
        switch (m_iMSegLevelMode)
        {
        case 0:
            sParam->assign("UNCHANGED");
            break;
        case 1:
            sParam->assign("EQUALRMS");
            break;
        }
        return 0;
    default:
        break;
    }

    return 1;
}


int CLBWvInFile::Open(char *sFileName)
{
    if((sFileName == 0) || (*sFileName == 0))
        return(3);
    if(m_bFileIsOpen)
        return(2);

    m_bFileIsOpen=false;
#if _MSC_VER < 1400
    m_pFile = fopen(sFileName, "rb");
#else
    fopen_s(&m_pFile, sFileName, "rb");
#endif
    if(m_pFile == NULL)
        return(3);

    if(m_pReadBuffer != NULL)
    {
        delete [] m_pReadBuffer;
        m_pReadBuffer=NULL;
    }
    if(m_pReadBuffer2 != NULL)
    {
        delete [] m_pReadBuffer2;
        m_pReadBuffer2=NULL;
    }
    char* pHelpBuffer=NULL;
    try
    {
        m_pReadBuffer = new char [m_iBufferSize+1];
        m_pReadBuffer2 = new char [m_iBufferSize+1];
        pHelpBuffer = new char [m_iBufferSize+1];
    }
    catch(std::bad_alloc&)
    {
        fclose(m_pFile);
        m_pFile=NULL;
        return(4);
    }
    if((m_pReadBuffer == NULL) || (m_pReadBuffer2 == NULL) || (pHelpBuffer == NULL))
    {
        fclose(m_pFile);
        m_pFile=NULL;
        return(4);
    }

    //init members
    m_bScramble=false;
    m_iNoSamplesInBuffer=0;
    m_pBufferStart=NULL;
    m_ullCurSampleInFile=0;
    m_iHeaderBytes=0;
    m_iBytesAfterFirstRead=0;
    m_ullSamples=0;
    m_dClock=0.0;
    m_dLevelRms=0.0;
    m_dRfRmsLevel = 0.0;
    m_dLevelPeak=0.0;
    memset(m_sType, 0, sizeof(m_sType));
    memset(m_sDateTime, 0, sizeof(m_sDateTime));
    memset(m_sCopyRight, 0, sizeof(m_sCopyRight));
    memset(m_sComment, 0, sizeof(m_sComment));
    m_sMSegSettingsFileName.clear();
    m_uMSegCount=0;
    m_iMSegClockMode=0;
    m_iMSegLevelMode=0;
    m_ullMSegStart.clear();
    m_ullMSegLength.clear();
    m_dMSegClock.clear();
    m_dMSegLevelRms.clear();
    m_dMSegLevelPeak.clear();
    m_dMSegDuration.clear();
    m_sMSegComment.clear();
    m_sMSegFileName.clear();

    //read start
    int readSize=m_iBufferSize/2;
    int bytesRead=0;
    bool fileStart=true;
    bool secPass=false;
    bool wvTagFound=false;      // Binary tag (W/U)WAVEFORM
    bool wvTagCompleted=false;
    bool clw4TagFound=false;    // Binary tag CONTROL LIST WIDTH4
    bool clockTagFound=false;
    bool samplesTagFound=false;
    bool levelOffsTagFound=false; 
    bool dateTagFound=false;
    bool copyRightTagFound=false;
    bool commentTagFound=false;
    bool rfRmsLevelTagFound = false;
    bool msegSettingsFileTagFound=false;
    bool msegCountTagFound=false;
    bool msegClockModeTagFound=false;
    bool msegLevelModeTagFound=false;
    char *pCurTag=NULL;
    char *pCurBuf=m_pReadBuffer;
    char *pWvStart=NULL;
    int iAddHeadOffset = 0;
    bool fileDone=false;
    bool restoreFilepointer=false;

    while(fileDone == false)
    {
        if(secPass && pCurBuf != m_pReadBuffer)
            memcpy(m_pReadBuffer, pCurBuf, readSize);

        if(!fileStart && !secPass)
        {
            pCurBuf+=bytesRead;
            secPass=true;
        }

        bytesRead=(int)fread(pCurBuf, 1, readSize, m_pFile);
        if(bytesRead == 0)
        {
            if(!wvTagFound)
            {
                //this is an error
                fclose(m_pFile);
                m_pFile=NULL;
                if (pHelpBuffer != NULL)
                {
                    delete[] pHelpBuffer;
                    pHelpBuffer = NULL;
                }
                return(5);
            }
            else
            {
                fileDone=true;
                continue;
            }
        }
        pCurBuf[bytesRead]=0;
        /*int iScanStop=0;
        const char *sScanStopTxt="WAVEFORM";
        for(int i=0; i<bytesRead; i++)
        {
            if(iScanStop == 8)
                break;

            if(pCurBuf[i] == sScanStopTxt[iScanStop++])
                continue;

            iScanStop=0;

            if(pCurBuf[i] == 0)
                pCurBuf[i]=1;
        }*/
        
        //scan ascii tags
        if(fileStart)
        {
            if(strstr(m_pReadBuffer, "{TYPE:SMU") != m_pReadBuffer)
            {
                if(strstr(m_pReadBuffer, "{TYPE: SMU") != m_pReadBuffer)
                {
                    if(strstr(m_pReadBuffer, "{TYPE:SFU") != m_pReadBuffer)
                    {
                        if(strstr(m_pReadBuffer, "{TYPE: SFU") != m_pReadBuffer)
                        {
                            fclose(m_pFile);
                            m_pFile=NULL;
                            if (pHelpBuffer != NULL)
                            {
                                delete[] pHelpBuffer;
                                pHelpBuffer = NULL;
                            }
                            return(6); //unsupported file type
                        }
                    }
                }
            }
            fileStart=false;
            // read waveform type from 'Type' tag (WV or MWV)
            pCurTag = strstr(m_pReadBuffer, "{TYPE:");
            char * pSep1 = strstr(pCurTag+1, "-");
            char * pSep2 = strstr(pCurTag+1, ",");
            char * pEnd = strstr(pCurTag+1, "}");
            if ((pSep2 != NULL) && (pSep2 < pEnd))
                pEnd = pSep2;
            if ((pCurTag != NULL) && (pSep1 != NULL) && (pEnd != NULL))
            {
                int len = (int)(pEnd - (++pSep1));
                if (len > 0)
                    strncpy(m_sType, pSep1, len);
            }
            // read CRC from 'Type' tag
            pCurTag=strstr(m_pReadBuffer, "{TYPE:");
            char* separator=strstr(pCurTag+1, ",");
            char* endTag=strstr(pCurTag+1, "}");
            if (pCurTag != NULL && separator!=NULL && endTag!=NULL && separator<endTag)
            {
#if _MSC_VER < 1400
              sscanf(separator+1, "%u", &m_uCrcTag);
#else
              sscanf_s(separator+1, "%u", &m_uCrcTag);
#endif
            }
        }        
        if(!samplesTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{SAMPLES:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
#if _MSC_VER < 1400
                sscanf(pCurTag+9, "%llu", &m_ullSamples);
#else
                sscanf_s(pCurTag+9, "%llu", &m_ullSamples);
#endif
                samplesTagFound=true;
            }
        }

        if(!clockTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{CLOCK:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
#if _MSC_VER < 1400
                sscanf(pCurTag+7, "%lf", &m_dClock);
#else
                sscanf_s(pCurTag+7, "%lf", &m_dClock);
#endif
                clockTagFound=true;
            }
        }

        if(!levelOffsTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{LEVEL OFFS:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
#if _MSC_VER < 1400
                sscanf(pCurTag+12, "%lf,%lf", &m_dLevelRms, &m_dLevelPeak);
#else
                sscanf_s(pCurTag+12, "%lf,%lf", &m_dLevelRms, &m_dLevelPeak);
#endif
                levelOffsTagFound=true;
            }
        }

        if (!dateTagFound)
        {
            pCurTag = strstr(m_pReadBuffer, "{DATE:");
            if (pCurTag != NULL)
            {
                char * pCurTagEnd = strstr(pCurTag+1, "}");
                int len = (int)(pCurTagEnd - (pCurTag+6));
                if (pCurTagEnd != NULL && len > 0)
                {
                    if (len > (int)sizeof(m_sDateTime)-1)
                        len = (int)sizeof(m_sDateTime)-1;

                    memcpy(m_sDateTime, pCurTag+6, len);

                    dateTagFound=true;
                }
            }
        }

        if (!copyRightTagFound)
        {
            pCurTag = strstr(m_pReadBuffer, "{COPYRIGHT:");
            if (pCurTag != NULL)
            {
                char * pCurTagEnd = strstr(pCurTag+1, "}");
                int len = (int)(pCurTagEnd - (pCurTag+11));
                if (pCurTagEnd != NULL && len > 0)
                {
                    if (len > (int)sizeof(m_sCopyRight)-1)
                        len = (int)sizeof(m_sCopyRight)-1;

                    memcpy(m_sCopyRight, pCurTag+11, len);

                    copyRightTagFound=true;
                }
            }
        }
        
        if (!commentTagFound)
        {
            pCurTag = strstr(m_pReadBuffer, "{COMMENT:");
            if (pCurTag != NULL)
            {
                char * pCurTagEnd = strstr(pCurTag+1, "}");
                int len = (int)(pCurTagEnd - (pCurTag+9));
                if (pCurTagEnd != NULL && len > 0)
                {
                    if (len > (int)sizeof(m_sComment)-1)
                        len = (int)sizeof(m_sComment)-1;

                    memcpy(m_sComment, pCurTag+9, len);

                    commentTagFound=true;
                }
            }
        }

        if (!rfRmsLevelTagFound)
        {
           const char *searchStr = "{COMMENT:Signal generated for SMx RMS level:";
           pCurTag = strstr(m_pReadBuffer, searchStr);
           if (pCurTag != NULL)
           {
              if (pCurTag != NULL && strstr(pCurTag + 1, "}") != NULL)
              {
                 sscanf(pCurTag + strlen(searchStr), "%lf", &m_dRfRmsLevel);
                 rfRmsLevelTagFound = true;
              }
           }
        }

        if (!msegSettingsFileTagFound)
        {
            pCurTag = strstr(m_pReadBuffer, "{MWV_SEGMENT_SETTINGS_FILE:");
            if (pCurTag != NULL)
            {
                char * pCurTagEnd = strstr(pCurTag+1, "}");
                int len = (int)(pCurTagEnd - (pCurTag+27));
                if (pCurTagEnd != NULL && len > 0)
                {
                    if (len > (int)sizeof(m_sMSegSettingsFileName)-1)
                        len = (int)sizeof(m_sMSegSettingsFileName)-1;

                    m_sMSegSettingsFileName.assign(pCurTag+27, len);
                    msegSettingsFileTagFound=true;
                }
            }
        }

        if (!msegCountTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{MWV_SEGMENT_COUNT:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
#if _MSC_VER < 1400
                sscanf(pCurTag+19, "%u", &m_uMSegCount);
#else
                sscanf_s(pCurTag+19, "%u", &m_uMSegCount);
#endif
                msegCountTagFound=true;
            }
        }
      
        if (m_ullMSegStart.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_START:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+19, ",");
                unsigned long long val(0);
                while (sEntry != 0)
                {                    
#if _MSC_VER < 1400
                    sscanf(sEntry, "%llu", &val);
#else
                    sscanf_s(sEntry, "%llu", &val);
#endif
                    m_ullMSegStart.push_back(val);
                    if (strstr(sEntry, "}") != NULL)
                        break;
                    sEntry = strtok(0, ",");
                }
            }
        }

        if (m_ullMSegLength.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_LENGTH:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+20, ",");
                unsigned long long val(0);
                while (sEntry != 0)
                {
#if _MSC_VER < 1400
                    sscanf(sEntry, "%llu", &val);
#else
                    sscanf_s(sEntry, "%llu", &val);
#endif
                    m_ullMSegLength.push_back(val);
                    if (strstr(sEntry, "}") != NULL)
                        break;
                    sEntry = strtok(0, ",");            
                }
            }
        }

        if (m_dMSegClock.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_CLOCK:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+19, ",");
                double val(0.0);
                while (sEntry != 0)
                {                    
#if _MSC_VER < 1400
                    sscanf(sEntry, "%lf", &val);
#else
                    sscanf_s(sEntry, "%lf", &val);
#endif
                    m_dMSegClock.push_back(val); 
                    if (strstr(sEntry, "}") != NULL)
                        break;
                    sEntry = strtok(0, ",");           
                }
            }
        }

        if (m_dMSegLevelRms.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_LEVEL_OFFS:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+24, ",");
                double val(0.0);
                while (sEntry != 0)
                {                    
#if _MSC_VER < 1400
                    sscanf(sEntry, "%lf", &val);
#else
                    sscanf_s(sEntry, "%lf", &val);
#endif
                    m_dMSegLevelRms.push_back(val);
                    if ((sEntry = strtok(0, ",")) == 0)
                        break;
#if _MSC_VER < 1400
                    sscanf(sEntry, "%lf", &val);
#else
                    sscanf_s(sEntry, "%lf", &val);
#endif
                    m_dMSegLevelPeak.push_back(val);
                    if (strstr(sEntry, "}") != NULL)
                        break;
                    sEntry = strtok(0, ",");           
                }
            }
        }

        if (m_dMSegDuration.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_DURATION:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+22, ",");
                while (sEntry != 0)
                {
                    double val(0.0);
#if _MSC_VER < 1400
                    sscanf(sEntry, "%lf", &val);
#else
                    sscanf_s(sEntry, "%lf", &val);
#endif
                    m_dMSegDuration.push_back(val);
                    if (strstr(sEntry, "}") != NULL)
                        break;
                    sEntry = strtok(0, ",");           
                }
            }
        }

        if (m_sMSegComment.empty())
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            for (unsigned int i=0; i<m_uMSegCount; i++)
            {
                char tagName[256];  
                memset(tagName, 0, sizeof(tagName));
                sprintf(tagName, "{MWV_SEGMENT%d_COMMENT:", i);
                pCurTag=strstr(pHelpBuffer, tagName);
                if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
                {
                    char *sEntry = strtok(pCurTag+strlen(tagName), "}");
                    m_sMSegComment.push_back(sEntry);
                } else {
                    m_sMSegComment.push_back("");
                }
            }
        }

        if (m_sMSegFileName.empty())
        {
            char* pCurTagEnd = 0;
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag = strstr(pHelpBuffer, "{MWV_SEGMENT_FILES:");
            if (pCurTag != NULL)
                pCurTagEnd = strstr(pCurTag, "}");
            if ((pCurTag != NULL) && (pCurTagEnd != NULL))
            {
                pCurTag += strlen("{MWV_SEGMENT_FILES:");
                *pCurTagEnd = 0;
                char *sEntry = strtok(pCurTag, ",");
                while (sEntry != 0)
                {                    
                    std::string val=sEntry;
                    size_t pos=val.find('"');
                    if (pos != std::string::npos)
                    {
                        size_t pos1=val.find('"', pos+1);
                        if (pos1 != std::string::npos)
                        {
                            val=val.substr(pos+1, pos1-pos-1);
                        }
                    }
                    m_sMSegFileName.push_back(val);  
                    sEntry = strtok (0, ",");
                }
            }
        }

        if (!msegClockModeTagFound)
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_CLOCK_MODE:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+24, "}");
                if (strcmp(sEntry, "HIGHEST") == 0)
                    m_iMSegClockMode = 1;
                else if (strcmp(sEntry, "USER") == 0)
                    m_iMSegClockMode = 2;
                msegClockModeTagFound=true;
            }
        }

        if (!msegLevelModeTagFound)
        {
            strncpy(pHelpBuffer, m_pReadBuffer, m_iBufferSize+1);
            pCurTag=strstr(pHelpBuffer, "{MWV_SEGMENT_LEVEL_MODE:");
            if(pCurTag != NULL && strstr(pCurTag+1, "}") != NULL)
            {
                char *sEntry = strtok(pCurTag+24, "}");
                if (strcmp(sEntry, "EQUALRMS") == 0)
                    m_iMSegLevelMode = 1;
                msegLevelModeTagFound=true;
            }
        }

        // Detect and ignore optional binary control list tag (before waveform tag).
        if (!clw4TagFound)
        {
            pCurTag = strstr(m_pReadBuffer, "{CONTROL LIST WIDTH");
            if (pCurTag != NULL)
            {
                // Get size in bytes
                unsigned int clw4Bytes = 0;
                char * pBytes = strstr(pCurTag, "-");
                char * pClw4Start = strstr(pCurTag, "#");
                if (pBytes && pClw4Start)
                {
                    sscanf(++pBytes, "%u", &clw4Bytes);
                    if (--clw4Bytes > 0)  // without '#'
                    {
                        pClw4Start++;
                        // Replace all binary bytes by spaces in read buffer only to avoid string scan interrupt.
                        unsigned int i = 0;
                        char *pBufferEnd = m_pReadBuffer+bytesRead;
                        if (secPass)
                            pBufferEnd+=readSize;
                        for (; i < clw4Bytes && pClw4Start+i != pBufferEnd; i++)
                            pClw4Start[i] = ' ';
                        if (i < clw4Bytes)
                        {
                          fseek(m_pFile, clw4Bytes - i, SEEK_CUR);
                          iAddHeadOffset = clw4Bytes - i;
                        }
                        clw4TagFound = true;
                    }
                }
            }
        }

        if (!wvTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{WWAVEFORM-");
            if(pCurTag != NULL)
                pWvStart=strstr(pCurTag+1, "#");
            if(pCurTag != NULL && pWvStart != NULL)
            {
                wvTagFound=true;
                m_bScramble=true;
                *pWvStart=0;
                pWvStart++;
            }
        }
        if (!wvTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{UWAVEFORM-");
            if(pCurTag != NULL)
                pWvStart=strstr(pCurTag+1, "#");
            if(pCurTag != NULL && pWvStart != NULL)
            {
                wvTagFound=true;
                m_bScramble=true;
                *pWvStart=0;
                pWvStart++;
            }
        }
        if (!wvTagFound)
        {
            pCurTag=strstr(m_pReadBuffer, "{WAVEFORM-");
            if(pCurTag != NULL)
                pWvStart=strstr(pCurTag+1, "#");
            if(pCurTag != NULL && pWvStart != NULL)
            {
                wvTagFound=true;
                m_bScramble=false;
                *pWvStart=0;
                pWvStart++;
            }
        }
        if (wvTagFound && !wvTagCompleted)
        {			
            // get waveform size in bytes
            unsigned long long ullWaveformBytes=0;
            char* l1=strstr(pCurTag+1, "-");
            char* l2=strstr(pCurTag+1, ":");
            if (l1 && l2)
            {
                sscanf(l1+1, "%llu", &ullWaveformBytes);
                if(ullWaveformBytes > 0)
                    ullWaveformBytes--;  // '#'
                if (ullWaveformBytes%4 != 0) 
                    ullWaveformBytes=ullWaveformBytes/4*4+4;
            }
            int length=(int)strlen(m_pReadBuffer);
            int length2;
            m_iHeaderBytes+=length+1;
            if (iAddHeadOffset > 0)
              m_iHeaderBytes += iAddHeadOffset;
            if(!secPass)
                length2=bytesRead-length-1;
            else
                length2=readSize+bytesRead-length-1;
            memmove(m_pReadBuffer, pWvStart, length2);                        
            readSize=m_iBufferSize-length2;
            bytesRead=(int)fread(m_pReadBuffer+length2, 1, readSize, m_pFile);            
            unsigned int bytesInBuf=bytesRead+length2;
            if ((unsigned long long)bytesInBuf>ullWaveformBytes) 
                bytesInBuf=(unsigned int)ullWaveformBytes;
            m_iNoSamplesInBuffer=bytesInBuf/4;
            #if defined __BIG_ENDIAN__
            {
              unsigned int* pData32 = 0;
              pData32 = (unsigned int *)m_pReadBuffer;
              for (int i = 0; i < m_iNoSamplesInBuffer; i++)
                pData32[i] = bswap_32(pData32[i]);
            }
            #endif

            if(m_bScramble && m_scramblerSet)
            {
				m_scrambler->descramble(m_pReadBuffer, &m_pBufferStart, m_iNoSamplesInBuffer, m_iHeaderBytes);
            }
            else
            {
                m_pBufferStart=m_pReadBuffer;
            }
            m_ullCurSampleInFile=m_iNoSamplesInBuffer;
            memcpy(m_pReadBuffer2, m_pReadBuffer, m_iBufferSize+1);
            m_iBytesAfterFirstRead=m_iHeaderBytes+(long)bytesInBuf-(m_bScramble?16:0);
            readSize=m_iBufferSize/2;
            pCurBuf=m_pReadBuffer;
            bytesRead=0;
            wvTagCompleted=true;
            unsigned long long fileDest=(unsigned long long)m_iHeaderBytes+ullWaveformBytes;
            if(fileDest >= 16)
                fileDest-=(m_bScramble?16:0);
            if(fileDest < 0x7FFFFFFF)
            {
                fseek(m_pFile, (long)fileDest, SEEK_SET);
                restoreFilepointer=true;
            }
            else
                fileDone=true;
        }
        
        if(secPass && !wvTagFound)
            m_iHeaderBytes+=bytesRead;
    }

    if(restoreFilepointer)
    {
        fseek(m_pFile, m_iBytesAfterFirstRead, SEEK_SET);
        memcpy(m_pReadBuffer, m_pReadBuffer2, m_iBufferSize+1);
    }

    if (pHelpBuffer != NULL)
    {
        delete[] pHelpBuffer;
        pHelpBuffer = NULL;
    }

    if (!clockTagFound)
    { 
        fclose(m_pFile);
        m_pFile=NULL;
        return(7);
    }

    if (!samplesTagFound)
    {
        fclose(m_pFile);
        m_pFile=NULL;
        return(8);
    }

    if (msegCountTagFound) // = if multi segment
    {
        if (   (m_uMSegCount == 0) 
            || (m_ullMSegStart.size() != m_uMSegCount) 
            || (m_ullMSegLength.size() != m_uMSegCount)
            || (m_dMSegClock.size() != m_uMSegCount)
            || (m_dMSegLevelRms.size() != m_uMSegCount))
        {
            fclose(m_pFile);
            m_pFile=NULL;
            return(9);
        }
    }

    m_bFileIsOpen=true;

    return(0);
}

int CLBWvInFile::Close()
{
    if(!m_bFileIsOpen || m_pFile==NULL)
        return(1);

    fclose(m_pFile);
    m_pFile=NULL;
    m_bFileIsOpen=false;

    return(0);
}

int CLBWvInFile::ReadSamples(unsigned int uOffset, unsigned int uNoSamples, unsigned int *pBuffer)
{
   if (!m_bFileIsOpen || m_pFile == NULL || m_pBufferStart == NULL)
   {
      return(10);
   }
   if (m_ullSamples < uOffset + uNoSamples)
   {
      return(11);
   }

   if (!m_bScramble) 
   {  
      // seek to begin of symbol data plus offset
      fseek(m_pFile, m_iHeaderBytes + uOffset * 4, SEEK_SET);
      // read directly into buffer from caller
      if (fread(pBuffer, 4, uNoSamples, m_pFile) != uNoSamples)
      {
         Close();
         return(12);
      }
      return 0;
   }
   // scamble
   if (!m_scramblerSet)
   {
      Close();
      return(13);
   }
   // seek to begin of symbol data
   fseek(m_pFile, m_iHeaderBytes, SEEK_SET);
   // reset scrambler to begin of file
   m_scrambler->reload();
   // "skip" offset
   unsigned int remainOffset = uOffset;
   while (remainOffset > 0)
   {
      unsigned int samples2read = remainOffset > (unsigned int)(m_iBufferSize/4) ? (m_iBufferSize/4) : remainOffset;
      if (fread(m_pReadBuffer, 4, samples2read, m_pFile) != samples2read)
      {
         Close();
         return(12);
      }
      // and descramble them
      m_scrambler->descrambleNext((unsigned int *)m_pReadBuffer, samples2read);
      remainOffset -= samples2read;
   }
   // read desired samples directly into callers buffer
   if (fread(pBuffer, 4, uNoSamples, m_pFile) != uNoSamples)
   {
      Close();
      return(12);
   }
   // and descramble them
   m_scrambler->descrambleNext(pBuffer, uNoSamples);
   return 0;
}

int CLBWvInFile::ReadSamples(unsigned int uNoSamples, unsigned int *pBuffer)
{
    if (!m_bFileIsOpen || m_pFile == NULL || m_pBufferStart == NULL)
        return(10);

    if(uNoSamples <= (unsigned int)m_iNoSamplesInBuffer)
    {
        memcpy(pBuffer, m_pBufferStart, uNoSamples*sizeof(unsigned int));        
        m_iNoSamplesInBuffer-=uNoSamples;
        m_pBufferStart+=uNoSamples*4;
    }
    else
    {
        unsigned int remainSamples=uNoSamples;
        int outIndex=0;
        memcpy(pBuffer, m_pBufferStart, m_iNoSamplesInBuffer*sizeof(unsigned int));
        outIndex+=m_iNoSamplesInBuffer;
        remainSamples-=m_iNoSamplesInBuffer;
        size_t bytes2read(0);
        m_iNoSamplesInBuffer=0;
        while(remainSamples > 0)
        {
            m_pBufferStart=m_pReadBuffer;
            while(m_iNoSamplesInBuffer < m_iBufferSize/4)
            {
                int readSamples=(m_iBufferSize/4)-m_iNoSamplesInBuffer;
                if(m_ullCurSampleInFile+readSamples > m_ullSamples)
                {
                    bytes2read=static_cast<size_t>((m_ullSamples-m_ullCurSampleInFile)*4);
                    if(fread(m_pBufferStart, 1, bytes2read, m_pFile) != bytes2read)
                    {
                      Close();
                      return(11);
                    }

                    #if defined __BIG_ENDIAN__
                    {
                      if((m_ullSamples-m_ullCurSampleInFile) > 0)
                      {
                        unsigned int* pData32 = 0;
                        pData32 = (unsigned int *)m_pBufferStart;
                        for (int i = 0; i < (int)(m_ullSamples-m_ullCurSampleInFile); i++)
                          pData32[i] = bswap_32(pData32[i]);
                      }
                    }
                    #endif

                    fseek(m_pFile, m_iHeaderBytes, SEEK_SET);
                    if(m_bScramble && m_scramblerSet)
                    {
                        if((m_ullSamples-m_ullCurSampleInFile) > 0 && m_scramblerSet)
                            m_scrambler->descrambleNext((unsigned int *)m_pBufferStart, (int)(m_ullSamples-m_ullCurSampleInFile));
						if (m_scramblerSet) m_scrambler->reload();
                    }
                    m_iNoSamplesInBuffer+=(int)(m_ullSamples-m_ullCurSampleInFile);
                    m_pBufferStart+=bytes2read;
                    m_ullCurSampleInFile=0;
                }
                else
                {                    
                    bytes2read=readSamples*4;
                    if(fread(m_pBufferStart, 1, bytes2read, m_pFile) != bytes2read)
                    {
                      Close();
                      return(11);
                    }

                    #if defined __BIG_ENDIAN__
                    {
                      if(readSamples > 0)
                      {
                        unsigned int* pData32 = 0;
                        pData32 = (unsigned int *)m_pBufferStart;
                        for (int i = 0; i < readSamples; i++)
                          pData32[i] = bswap_32(pData32[i]);
                      }
                    }
                    #endif

                    if(m_bScramble)
                    {
                        if(readSamples > 0 && m_scramblerSet)
							m_scrambler->descrambleNext((unsigned int *)m_pBufferStart, readSamples);
                    }
                    m_iNoSamplesInBuffer+=readSamples;
                    m_pBufferStart+=readSamples;
                    m_ullCurSampleInFile+=readSamples;
                }
            }
            m_pBufferStart=m_pReadBuffer;
            int copySamples=remainSamples;
            if(copySamples > m_iNoSamplesInBuffer)
                copySamples=m_iNoSamplesInBuffer;
            memcpy(pBuffer+outIndex, m_pBufferStart, copySamples*sizeof(unsigned int));
            outIndex+=copySamples;
            m_pBufferStart+=copySamples*4;
            remainSamples-=copySamples;
            m_iNoSamplesInBuffer-=copySamples;
        }
    }

    return(0);
}


void CLBWvInFile::GetSamplesOffset(unsigned int& ui_optWordsOffset, unsigned int& ui_IqSamplesOffset, bool& b_isScrambled)
{
  if(m_bScramble)
  {
    ui_IqSamplesOffset=m_iHeaderBytes;
    ui_optWordsOffset=m_iHeaderBytes-16;
  }
  else
  {
    ui_IqSamplesOffset=m_iHeaderBytes;
    ui_optWordsOffset=0;
  }

  b_isScrambled = m_bScramble;

}


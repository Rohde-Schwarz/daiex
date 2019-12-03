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

#ifndef _RS_GX40X_GLOBAL_FRAME_TYPES_IF_DEFS_H
#define _RS_GX40X_GLOBAL_FRAME_TYPES_IF_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/*++++ GLOBAL TYPES DECLARATION ++++++++++++++++++++++++++++++++++++++++++++*/

/** Enumeration of the possible data stream types */
enum eFRH_DATASTREAM__TYPES
{
  /* Type for test data */
  ekFRH_DATASTREAM__TEST_DATA                         = 0x00,   /**< CURRENTLY NOT USED */

  /* Type for IF data 32Bit Real- and 32Bit Imaginary-part, fixed point, not rescaled */
  ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX              = 0x01,   /**< IF data : struct <typIFD_IFDATA_FRAME>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words if result from IQDW data */

  /* Type for IF data 16Bit Real- and 16Bit Imaginary-part, fixed point, not rescaled */
  ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX              = 0x02,   /**< IF data : struct <typIFD_IFDATA_FRAME>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words if result from IQDW data */

  /* Type for IF data 16Bit Real-part, two samples for each word, fixed point, not rescaled */
  ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX              = 0x03,   /**< IF data : struct <typIFD_IFDATA_FRAME>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words if result from IQDW data */

  /* Type for IF data 32Bit Real- and 32Bit Imaginary-part, fixed point, rescaled */
  ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED     = 0x04,   /**< IF data : struct <typIFD_IFDATA_FRAME>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words if result from IQDW data */

  /* Type for IF data 32Bit Real- und 32Bit Imaginary-part, floating point, rescaled */
  ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED   = 0x05,   /**< IF data : struct <typIFD_IFDATA_FRAME>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words if result from IQDW data */
  
  /* Type for Spectrum-data 8Bit */
  ekFRH_DATASTREAM__SPECDATA_8BIT                     = 0x10,   /**< CURRENTLY NOT USED */

  /* Type for Spectrum-data 16Bit */
  ekFRH_DATASTREAM__SPECDATA_16BIT                    = 0x11,   /**< CURRENTLY NOT USED */

  /* Type for Spectrum-data 32Bit */
  ekFRH_DATASTREAM__SPECDATA_32BIT                    = 0x12,   /**< CURRENTLY NOT USED */

  /* Type for Spectrum-data 32Bit float */
  ekFRH_DATASTREAM__SPECDATA_FLOAT                    = 0x13,   /**< Spectrum data : struct <typSPECDATA_FLOAT>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words */

  /* Type for Segmentation Spectrum-data 32Bit float */
  ekFRH_DATASTREAM__SEGMENTATION_SPECDATA_FLOAT       = 0x14,   /**< Segmentation spectrum data : struct <typSEGMENTATION_SPECDATA_FLOAT> */

  /* Data stream types for Tuner HF (EM010) data */
  ekFRH_DATASTREAM__HF_TUNING_INDICATOR_DATA          = 0x20,   /**< Tuning Indicator data : struct <typFRAME_EM010_HF_TUNING_INDICATOR_AND_STATUS_DATA> */

  ekFRH_DATASTREAM__HF_SCF_DATA                       = 0x22,   /**< Scan Channel Found    : struct <typFRAME_EM010_HF_SCAN_CHANNEL_FOUND> */   

  ekFRH_DATASTREAM__HF_SFF_DATA                       = 0x23,   /**< Scan Frequency Found  : struct <typFRAME_EM010_HF_SCAN_FREQUENCY_FOUND> */

  ekFRH_DATASTREAM__HF_SSR_DATA                       = 0x24,   /**< Scan Sweep Restarted  : struct <typFRAME_EM010_HF_SCAN_RESTARTED> */

  /* Type for HF Single Channel Demodulation*/
  ekFRH_DATASTREAM__DEMOD_HF_SYMBOLSTREAM_DATA        = 0x30,   /**< HF Demod data (symbol stream) : struct <struDEMOD_HF_SYMBOL_DATA> */

  /* Type for(ACID) HF Single Channel Decoding*/
  ekFRH_DATASTREAM__DECODER_TEXT_DATA                 = 0x40,   /**< Decoder (ACID) decoded text data : struct <typDECODER__DECODED_TXT_FRAME> */

  /* Types for HF SingleChannel Realtime Visualisation*/
  ekFRH_DATASTREAM__VISUALISATION_HF_SPECTRUM_DATA    = 0x50,   /**< RTVIS spectrum data : struct <struVISUALISATION_HF_SPECTRUM_DATA> */

  ekFRH_DATASTREAM__VISUALISATION_HF_TIMEDOMAIN_DATA  = 0x51,   /**< RTVIS timedomain data : struct <struVISUALISATION_HF_TIMESIGNAL_DATA> */

  /* Type for multi-channel IF data streams as used in EMSD-DDCE, 32Bit Real- and 32Bit Imaginary-part, fixed point, not rescaled */
  ekFRH_DATASTREAM__DDCE_IFDATA_32RE_32IM_FIX         = 0x60,  /**<  DDCE if data : struct <typIFD_IFDATA_DDCE_FRAME> */
  
  /* Type for multi-channel IF data streams as used in EMSD-DDCE, 16Bit Real- and 16Bit Imaginary-part, fixed point, not rescaled */
  ekFRH_DATASTREAM__DDCE_IFDATA_16RE_16IM_FIX         = 0x61,  /**<  DDCE if data : struct <typIFD_IFDATA_DDCE_FRAME> */
  
  /* Type for audio data */
  ekFRH_DATASTREAM__AUDIODATA                         = 0x100,  /**< Audio data : struct <typAUDIODATAFRAME> */

  /* Type for signal emissions list data */
  ekFRH_DATASTREAM__EMISSION_LIST_DATA                = 0x110,  /**< Signal emission list data : struct <typEMISSION_LIST_FRAME> */

  /* Type for spectral detector list data */
  ekFRH_DATASTREAM__SPECTRALDETECTOR_DATA             = 0x111,  /**< Spectral detector list data : no frame type specified! */

  /* Type for level indicator data */
  ekFRH_DATASTREAM__LEVELDATA                         = 0x120,  /**< Level indicator data : struct <typLEVELDATA_FRAME> */
  ekFRH_DATASTREAM__LEVELDATA_DEM                     = 0x121,  /**< Level information from demodulator: struct <typLEVELDATADET_FRAME> */

  /* Type for demodulation result data (symbol stream) */
  ekFRH_DATASTREAM__SYMBOLDATA                        = 0x130,  /**< Demodulation result data (symbol stream) : struct <typSYMBOLDATAFRAME> */

  /* Type for demodulation instantaneous data */
  ekFRH_DATASTREAM__INSTANTANEOUSDATA                 = 0x140,  /**< Demodulation instantaneous data : struct <typINSTANTANEOUSDATAFRAME> */

  /* Type for burst emission list data */
  ekFRH_DATASTREAM__BURST_EMISSIONS_LIST              = 0x150,  /**< Burst emission list data : struct <typBURST_EMISSION_LIST_DATAFRAME> */
  ekFRH_DATASTREAM__EXT_BURST_EMISSIONS_LIST          = 0x151,  /**< Extended Burst emission list data : struct <typEXT_BURST_EMISSION_LIST_DATAFRAME> */

  /* Type for demodulation result data (image stream) */
  ekFRH_DATASTREAM__IMAGEDATA                         = 0x160,  /**< Demodulation result data (image stream) : struct <typIMAGEDATAFRAME> */

  /* Type for transmission system result data */
  ekFRH_DATASTREAM__TRANSMISSION_SYSTEM_RESULT_DATA   = 0x170,  /**< Transmission system result data : struct <typTRANSMISSION_SYSTEM_RESULT_FRAME> */

  /* Type for timedomain data */
  ekFRH_DATASTREAM__TIMEDOMAIN_DATA                   = 0x180,  /**< Timedomain data : struct <typTIMEDOMAINDATAFRAME> */

  /* Type for timedomain data */
  ekFRH_DATASTREAM__HISTOGRAM_DATA                    = 0x190,  /**< Histogram data : struct <typHISTOGRAM_HEADER> */

  /* Type for pulse description words */
  ekFRH_DATASTREAM__PULSE_DESCRIPTION_WORD_DATA       = 0x200,  /**< Pulse description word data : struct <TODO> */

  /* Type for pulse description words with IQ, type for IF data 16Bit Real- and 16Bit Imaginary-part, fixed point */
  ekFRH_DATASTREAM__IQDW_16RE_16IM_FIX                = 0x201,  /**< Pulse description word data with IQ: struct <typIQDW_HEADER>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words */
  ekFRH_DATASTREAM__IQDW_DATA                         = ekFRH_DATASTREAM__IQDW_16RE_16IM_FIX,   /* For backward compatiblity */

  /* Type for pulse description words with IQ, type for IF data 32Bit Real- and 32Bit Imaginary-part, fixed point  */
  ekFRH_DATASTREAM__IQDW_32RE_32IM_FIX                = 0x202,  /**< Pulse description word data with IQ: struct <typIQDW_HEADER>, the size of frames of this type may exceed kFRH_FRAME_LENGTH_MAX 32bit words */

  /* Type for pulse repetition words */
  ekFRH_DATASTREAM__PULSE_REPETITION_WORD_DATA        = 0x210,  /**< Pulse repetition word data : struct <TODO> */

  /* Type for timedomain data */
  ekFRH_DATASTREAM__HOP_DENSITY_WATERFALL_DATA        = 0x220,  /**< Histogram data : struct <typHDWFRAME> */

  /* protobuf datastream; the specific type of protobuf-data is in the dataheader */
  ekFRH_DATASTREAM__PROTOBUF_FRAME                    = 0x230,  /**< protobuf datastream; the specific type of protobuf-data is set at the dataheader <typPROTOBUF_DATAHEADER> */

  /* Type for scan datastream - no content */
  ekFRH_DATASTREAM__SCAN                              = 0x4000, /**< Scan data for EM050 - reserved area from 0x4000 to 0x43ff */

  /* Type for scan datastream -- tuning level indicator data */
  ekFRH_DATASTREAM__SCAN__LEVEL                       = 0x4001, /**< Scan datastream -- tuning level indicator data : struct <typSCANDATA_FRAME__LEVEL> */

  /* Type for scan for datastream -- frequency tuning offset */
  ekFRH_DATASTREAM__SCAN__TUNING                      = 0x4002, /**< Scan datastream -- frequency tuning offset : struct <typSCANDATA_FRAME__TUNING> */

  /* Type for scan datastream -- tuning level indicator and frequency tuning offset */
  ekFRH_DATASTREAM__SCAN__LEVEL_TUNING                = 0x4003, /**< Scan datastream -- tuning level indicator data and frequency tuning offset : struct <typSCANDATA_FRAME__LEVEL_TUNING> */

  /* Type for tuner defined datastream of polychrom IF panorama */
  ekFRH_DATASTREAM__TUNER_PIFPAN_DATA                 = 0x4400, /**< Tuner PIFPanorama - see tuner documentation for content (supplemented with padding bytes for frame size of multiple of words) */

  /* DDF datastreams - no content */
  ekFRH_DATASTREAM__DDF_RESERVED_RANGE_BEGIN          = 0x5000, /**< DDF data streams  - reserved area from 0x5000 to 0x50ff */

  /* DDF datastreams - no content */
  ekFRH_DATASTREAM__DDF_RESERVED_RANGE_END            = 0x50FF,  /**< DDF data streams  - reserved area from 0x5000 to 0x50ff */


  /* Enumeration to be extended as new data stream types are identified... */
};


/*---- End of global types declaration -------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ifndef for file (multi)inclusion lock */
/***** End of File ***********************************************************/

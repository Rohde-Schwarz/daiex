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
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <numeric>

#include "common.h"
#include "wvpimpl.h"

namespace rohdeschwarz
{
	namespace mosaik
	{
		namespace dataimportexport
		{

			using namespace std;

			Wv::Impl::Impl(const std::string& filename)
			  : m_filename(filename),
				m_write(false),
				m_samples(0),
				m_scrambled(false),
				m_scramblerSet(false),
				m_timeStamp(time(nullptr))
			{
			}

			Wv::Impl::~Impl()
			{
			}

			int Wv::Impl::readOpen(std::vector<std::string>& arrayNames)
			{
				try
				{
          resetData();
          arrayNames.clear();
					if (m_wv.Open((char *) m_filename.c_str()))
					{
						return 1;
					}
				}
				catch (...)
				{
					return 1;
				}
				// and now ... build meta data and channel infos:

				std::string s, Date, Type, Comment;
				m_wv.GetParam(IWvIn::eParamType, &Type);
				//printf("Type: %s\n", Type.c_str());
				m_wv.GetParam(IWvIn::eParamDate, &Date);
				//printf("Date: %s\n", Date.c_str());
				m_wv.GetParam(IWvIn::eParamComment, &Comment);
				//printf("Comment: %s\n", Comment.c_str());
				//wv.GetParam(IWvIn::eParamCopyRight, &s);
				//printf("CopyRight: %s\n", s.c_str());
				m_wv.GetParam(IWvIn::eParamSamples, &m_samples);
				//printf("Samples: %d\n", m_samples);
				double clock;
				m_wv.GetParam(IWvIn::eParamClock, &clock);
				//printf("Clock: %f\n", clock);
				double rmsOffs, peakOffs, mSegPeakOffs;
				m_wv.GetParam(IWvIn::eParamLevelOffset, &rmsOffs, &peakOffs);
				//printf("Level RMS: %f\n", rms);
				m_wv.GetParam(IWvIn::eParamMSegLevelOffsetPeak, &mSegPeakOffs);
				//printf("MSegLevelOffsetPeak: %f\n", mSegPeak);
				double rfRmsLevel;
            m_wv.GetParam(IWvIn::eParamRfRmsLevel, &rfRmsLevel);
				if (rfRmsLevel != 0.0)
            {
               //printf("RfRmsLevel: %f\n", rfRmsLevel);
               float fullScale = (10.0 * log10(1.0 / 50.0) + 30.0);
               float fileRms = fullScale - rmsOffs;
               m_scaleFactor = pow(10, (rfRmsLevel - fileRms) / 20.0);
               //printf("m_scaleFactor: %f\n", m_scaleFactor);
               m_multiplicator = m_scaleFactor / INT16_MAX;
				}
				int scrambled;
				m_wv.GetParam(IWvIn::eParamIsScrambled, &scrambled);
				m_scrambled = (scrambled == 1);
				//printf("Scrambled: %s\n", scrambled == 1 ? "yes" : "no");
				unsigned long long mSegLength = 0;
				m_wv.GetParam(IWvIn::eParamMSegLength, &mSegLength);
				if (mSegLength > 0)
				{
					// read mSeg data

				}
				m_metaData.insert(make_pair("Scrambled", scrambled == 1 ? "yes" : "no"));
				m_metaData.insert(make_pair("Date", Date));
				if (Date != "")
				{
					// we have got a time stamp
					// "2014-06-10;14:39:54"
					int year, month, day, hour, min, sec;

					if (sscanf(Date.c_str(), "%d-%d-%d;%d:%d:%d", &year, &month, &day, &hour, &min, &sec) == 6)
					{
						struct tm mytm = { 0 };
						mytm.tm_year = year - 1900; /* years since 1900 */
						mytm.tm_mon = month - 1;
						mytm.tm_mday = day;
						mytm.tm_hour = hour;
						mytm.tm_min = min;
						m_timeStamp = mktime(&mytm);
					}
				}
				m_metaData.insert(make_pair("Type", Type));
				m_metaData.insert(make_pair("Comment", Comment));
				m_metaData.insert(make_pair("RMSOffset_dB", std::to_string(rmsOffs)));
				m_metaData.insert(make_pair("PeakOffset_dB", std::to_string(peakOffs)));
				m_metaData.insert(make_pair("mSegLength", std::to_string(mSegLength)));

				m_channelInfo.push_back(ChannelInfo("Channel1", clock, 0, m_samples));
				arrayNames.push_back("Channel1");

				return 0;
			}

			int Wv::Impl::getMetadata(std::vector<ChannelInfo>& channelInfos, map<string, string>& metadata) const
			{
				if ((m_channelInfo.empty() || (m_metaData.empty())))
				{
					return 1;
				}
				metadata = m_metaData;
				channelInfos = m_channelInfo;
				return 0;
			}

			void Wv::Impl::resetData()
			{
        m_nofArrays = 0;
        m_applicationName = "";
        m_comment = "";
        m_channelInfo.clear();
        m_metaData.clear();
        m_samples = 0;
      }

			int Wv::Impl::writeOpen(IqDataFormat , size_t , const string& , const string& ,
				const vector<ChannelInfo>& , const map<string, std::string>* )
			{
            if (!Platform::isFileAccessible(m_filename))
            {
               return ErrorCodes::FileOpenError;
            }

				return 1;
			}

			int Wv::Impl::close()
			{
				m_wv.Close();
				return 0;
			}

			time_t Wv::Impl::getTimestamp() const
			{
				return m_timeStamp;
			}

			int64_t  Wv::Impl::getArraySize(const std::string& /* arrayName */) const
			{
				return m_samples;
			}

			void Wv::Impl::setTimestamp(const time_t /*timestamp*/)
			{}

			int Wv::Impl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
			{
				std::vector<double> dv(0);
				return readArrayAll(arrayName, values, dv, (float*)nullptr, (double*)nullptr, nofValues, offset, rFloatVector);
			}

			int Wv::Impl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				std::vector<double> dv;
				return readArrayAll(arrayName, fv, dv, values, (double*)nullptr, nofValues, offset, rFloatPointer);
			}

			int Wv::Impl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				return readArrayAll(arrayName, fv, values, (float*)nullptr, (double*)nullptr, nofValues, offset, rDoubleVector);
			}

			int Wv::Impl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				std::vector<double> dv;
				return readArrayAll(arrayName, fv, dv, (float*)nullptr, values, nofValues, offset, rDoublePointer);
			}

			int Wv::Impl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
			{
				std::vector<double> dv;
				return readChannelAll(channelName, values, dv, (float*)nullptr, (double*)nullptr, nofValues, offset, rFloatVector);
			}

			int Wv::Impl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				std::vector<double> dv;
				return readChannelAll(channelName, fv, dv, values, (double*)nullptr, nofValues, offset, rFloatPointer);
			}

			int Wv::Impl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				return readChannelAll(channelName, fv, values, (float*)nullptr, (double*)nullptr, nofValues, offset, rDoubleVector);
			}

			int Wv::Impl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
			{
				std::vector<float> fv;
				std::vector<double> dv;
				return readChannelAll(channelName, fv, dv, (float*)nullptr, values, nofValues, offset, rDoublePointer);
			}

			int Wv::Impl::appendArrays(const std::vector<std::vector<float> >& /* iqdata */)
			{
				return 1;
			}

			int Wv::Impl::appendArrays(const std::vector<float*>& /* iqdata */, const std::vector<size_t>& /* sizes */)
			{
				return 1;
			}

			int Wv::Impl::appendArrays(const std::vector<std::vector<double> >& /* iqdata */)
			{
				return 1;
			}

			int Wv::Impl::appendArrays(const std::vector<double*>& /* iqdata */, const std::vector<size_t>& /* sizes */)
			{
				return 1;
			}

			int Wv::Impl::appendChannels(const std::vector<std::vector<float> >& /* iqdata */)
			{
				return 1;
			}

			int Wv::Impl::appendChannels(const std::vector<float*>& /* iqdata */, const std::vector<size_t>& /* sizes */)
			{
				return 1;
			}

			int Wv::Impl::appendChannels(const std::vector<std::vector<double> >& /* iqdata */)
			{
				return 1;
			}

			int Wv::Impl::appendChannels(const std::vector<double*>& /* iqdata */, const std::vector<size_t>& /* sizes */)
			{
				return 1;
			}

			void Wv::Impl::setScrambler(WvScramblerBase *scrambler)
			{
				m_wv.setScrambler(scrambler);
				m_scramblerSet = true;
			}


			/*
			* IQX:       IQIQIQIQ    IQIQIQIQ    IQIQIQIQ
			*                           \ \ \    / /
			* Target:                     IIIIIIII     or    QQQQQQQ
			*/
			int Wv::Impl::readArrayAll(const std::string& arrayName, std::vector<float>& vfValues, std::vector<double>& vdValues, float* fValues, double* dValues, size_t nofValues, size_t offset, rType rw)
			{
				if (m_scrambled && !m_scramblerSet) return 1;
				float *fPtr = fValues;
				double *dPtr = dValues;
				switch (rw)
				{
				case rFloatVector:
					vfValues.clear();
					break;
				case rDoubleVector:
					vdValues.clear();
					break;
				default:
					break;
				}

            unsigned int samples = (unsigned int) (nofValues);
				if (samples > m_samples)
				{
					return 1;
				}
				unique_ptr<unsigned int[]> databuffer(new unsigned int[samples]);
				m_wv.ReadSamples(offset, samples, databuffer.get());
				int16_t *data = (int16_t *)databuffer.get();

				size_t readBegin8 = 0;
				bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;
				if (!isI)
				{
					readBegin8 += 2;
				}
				//                                            IQ = 4 Byte
				size_t readEnd8 = readBegin8 + (nofValues - 1) * 4;

				switch (rw)
				{
					case rFloatVector:
					{
						// now copy every 2nd int16 and convert it to float32
						while (readBegin8 <= readEnd8)
						{
							float f = data[readBegin8 / 2];
							f = f * m_multiplicator;
							vfValues.push_back(f);
							readBegin8 += 4;
						}
						break;
					}
					case rDoubleVector:
					{
						// now copy every 2nd int16 and convert it to float32
						while (readBegin8 <= readEnd8)
						{
							double d = data[readBegin8 / 2];
							d = d * m_multiplicator;
							vdValues.push_back(d);
							readBegin8 += 4;
						}
						break;
					}
					case rFloatPointer:
					{
						while (readBegin8 <= readEnd8)
						{
							float f = data[readBegin8 / 2];
							f = f * m_multiplicator;
							*fPtr = f;
							fPtr++;
							readBegin8 += 4;
						}
						break;
					}
					case rDoublePointer:
					{
						while (readBegin8 <= readEnd8)
						{
							double d = data[readBegin8 / 2];
							d = d * m_multiplicator;
							*dPtr = d;
							dPtr++;
							readBegin8 += 4;
						}
						break;
					}
				}
				return 0;
			}

			int Wv::Impl::readChannelAll(const std::string& /* channelName */, std::vector<float>& vfValues, std::vector<double>& vdValues, float* fValues, double* dValues, size_t nofValues, size_t offset, rType rw)
			{
				if (m_scrambled && !m_scramblerSet) return 1;
				float *fPtr = fValues;
				double *dPtr = dValues;

				if (rw == rFloatVector)
				{
					vfValues.clear();
					if (vfValues.max_size() < nofValues * 2)
					{
						return 1;
					}
					vfValues.reserve(nofValues * 2);
				}
				if (rw == rDoubleVector)
				{
					vdValues.clear();
					if (vdValues.max_size() < nofValues * 2)
					{
						return 1;
					}
					vdValues.reserve(nofValues * 2);
				}

				size_t readBegin8 = 0;

				//                             Q              IQ = 4 Byte
				size_t readEnd8 = readBegin8 + 2 + (nofValues - 1) * 4;

				size_t symbols = nofValues;
				unique_ptr<unsigned int[]> databuffer(new unsigned int[symbols]);

				m_wv.ReadSamples((unsigned int)offset, (unsigned int)symbols, databuffer.get());
				int16_t *data = (int16_t *) databuffer.get();

				switch (rw)
				{
					case rFloatVector:
					{
						// now copy every int16 and convert it to float32
						while (readBegin8 <= readEnd8)
						{
							float f = data[readBegin8 / 2];
							f = f * m_multiplicator;
							vfValues.push_back(f);
							readBegin8 += 2;
						}
						break;
					}
					case rDoubleVector:
					{
						// now copy every int16 and convert it to double
						while (readBegin8 <= readEnd8)
						{
							double d = data[readBegin8 / 2];
							d = d * m_multiplicator;
							vdValues.push_back(d);
							readBegin8 += 2;
						}
						break;
					}
					case rFloatPointer:
					{
						while (readBegin8 <= readEnd8)
						{
							float f = data[readBegin8 / 2];
							f = f * m_multiplicator;
							*fPtr = f;
							fPtr++;
							readBegin8 += 2;
						}
						break;
					}
					case rDoublePointer:
					{
						// now copy every int16 and convert it to double
						while (readBegin8 <= readEnd8)
						{
							double d = data[readBegin8 / 2];
							d = d * m_multiplicator;
							*dPtr = d;
							dPtr++;
							readBegin8 += 2;
						}
						break;
					}
				}
				return 0;
			}

			int Wv::Impl::writeIqFramesFromArrays(int64_t /* streamno */, float * /* fArrayI */, float * /* fArrayQ */, double * /* dArrayI */, double * /* dArrayQ */, int64_t /* samples */, wType /* w */)
			{
				return 0;
			}

			int Wv::Impl::writeIqFramesFromChannel(int64_t /* streamno */, float * /* fChannel */, double * /* dChannel */, int64_t /* samples */, wType /* w */)
			{
				return 0;
			}
		} // namespace
	} // namespace
} // namespace

/*!
* @file      filetypeservice.cpp
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#include "filetypeservice.h"

#include "iqw.h"
#include "iqtar.h"
#include "iqmatlab.h"
#include "iqcsv.h"
#include "iqx.h"
#include "wv.h"
#include "aid.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IDataImportExport* FileTypeService::create(const std::string& filename, FileType fileType)
      {
        switch (fileType)
        {
        case FileType::Iqtar:
          return new IqTar(filename);

        case FileType::IQW:
          return new Iqw(filename);

        case FileType::IQX:
          return new Iqx(filename);

        case FileType::WV:
          return new Wv(filename);

        case FileType::AID:
          return new Aid(filename);

        case FileType::Matlab4:
          {
            IqMatlab* file = new IqMatlab(filename);
            file->setMatlabVersion(MatlabVersion::Mat4);
            return file;
          }

        case FileType::Matlab73:
          {
            IqMatlab* file = new IqMatlab(filename);
            file->setMatlabVersion(MatlabVersion::Mat73);
            return file;
          }

        case FileType::Csv:
          return new IqCsv(filename);

        // error: unknown file format
        default:
          break;
        }

        // error
        return nullptr;
      }

      std::vector<FileType> FileTypeService::getPossibleFileFormats()
      {
        vector<FileType> types;

        types.push_back(FileType::IQW);
        types.push_back(FileType::Iqtar);
        types.push_back(FileType::Csv);
        types.push_back(FileType::Matlab4);
        types.push_back(FileType::Matlab73);
	      types.push_back(FileType::IQX);
	      types.push_back(FileType::WV);
        types.push_back(FileType::AID);

        return types;
      }

      std::string FileTypeService::getFileExtension(FileType fileType)
      {
        switch (fileType)
        {
        case rohdeschwarz::mosaik::dataimportexport::Iqtar:
          return "iq.tar";
        case rohdeschwarz::mosaik::dataimportexport::IQW:
          return "iqw";
        case rohdeschwarz::mosaik::dataimportexport::Matlab4:
        case rohdeschwarz::mosaik::dataimportexport::Matlab73:
          return "mat";
        case rohdeschwarz::mosaik::dataimportexport::Csv:
          return "csv";
        case rohdeschwarz::mosaik::dataimportexport::IQX:
	        return "iqx";
        case rohdeschwarz::mosaik::dataimportexport::WV:
	        return "wv";
        case rohdeschwarz::mosaik::dataimportexport::AID:
          return "aid";
        default:
          break;
        }

        // error
        return "bin";
      }

      std::string FileTypeService::getFileFilter(FileType fileType)
      {
        switch (fileType)
        {
        case rohdeschwarz::mosaik::dataimportexport::Iqtar:
          return "IQTar files (*.iq.tar)";
        case rohdeschwarz::mosaik::dataimportexport::IQW:
          return "IQW files (*.iqw)";
        case rohdeschwarz::mosaik::dataimportexport::Matlab4:
        case rohdeschwarz::mosaik::dataimportexport::Matlab73:
          return "Matlab files (*.mat)";
        case rohdeschwarz::mosaik::dataimportexport::Csv:
          return "CSV files (*.csv)";
	      case rohdeschwarz::mosaik::dataimportexport::IQX:
          return "IQX files (*.iqx)";
	      case rohdeschwarz::mosaik::dataimportexport::WV:
          return "IQX files (*.wv)";
        case rohdeschwarz::mosaik::dataimportexport::AID:
          return "IQX files (*.aid)";
        default:
          return "";
        }
      }
    }
  }
}
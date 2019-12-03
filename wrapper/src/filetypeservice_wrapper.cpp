#include "filetypeservice_wrapper.h"

#include "helpers.h"
#include "iqw_wrapper.h"
#include "iqtar_wrapper.h"
#include "iqmatlab_wrapper.h"
#include "iqcsv_wrapper.h"

#include <vector>
#include "filetypeservice.h"

using namespace std;

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      IList<FileType>^ FileTypeService::PossibleFileFormats::get()
      {
        vector<rohdeschwarz::mosaik::dataimportexport::FileType> nativeFormats = rohdeschwarz::mosaik::dataimportexport::FileTypeService::getPossibleFileFormats();
        
        List<FileType>^ formats = gcnew List<FileType>(static_cast<int>(nativeFormats.size()));
        for (rohdeschwarz::mosaik::dataimportexport::FileType nativeFormat : nativeFormats)
        {
          formats->Add(FileTypeService::GetFileType(nativeFormat));
        }
      
        return formats;
      }

      IDataImportExport^ FileTypeService::Create(String^ filename, FileType fileType)
      {
        switch (fileType)
        {
          case FileType::Iqtar:
            return gcnew IqTar(filename);
          case FileType::IQW:
            return gcnew Iqw(filename);
          case FileType::Matlab4:
            {
              IqMatlab^ file = gcnew IqMatlab(filename);
              file->MatlabFileVersion = MatlabVersion::Mat4;
              return file;
            }
          case FileType::Matlab73:
            {
              IqMatlab^ file = gcnew IqMatlab(filename);
              file->MatlabFileVersion = MatlabVersion::Mat73;
              return file;
            }
          case FileType::Csv:
            return gcnew IqCsv(filename);
          default: 
            return nullptr;
        }
      }

      String^ FileTypeService::GetFileExtension(FileType fileType)
      {
        rohdeschwarz::mosaik::dataimportexport::FileType nativeFileType = FileTypeService::GetFileType(fileType);
        string ext = rohdeschwarz::mosaik::dataimportexport::FileTypeService::getFileExtension(nativeFileType);
        return Helpers::marshalUTF8String(ext);
      }

      String^ FileTypeService::GetFileFilter(FileType fileType)
      {
        rohdeschwarz::mosaik::dataimportexport::FileType nativeFileType = FileTypeService::GetFileType(fileType);
        string filter = rohdeschwarz::mosaik::dataimportexport::FileTypeService::getFileFilter(nativeFileType);
        return Helpers::marshalUTF8String(filter);
      }

      rohdeschwarz::mosaik::dataimportexport::FileType FileTypeService::GetFileType(FileType fileType)
      {
        switch (fileType)
        {
          default:
          case FileType::Iqtar:
            return rohdeschwarz::mosaik::dataimportexport::FileType::Iqtar;
          case FileType::IQW:
            return rohdeschwarz::mosaik::dataimportexport::FileType::IQW;
          case FileType::Matlab4:
            return rohdeschwarz::mosaik::dataimportexport::FileType::Matlab4;
          case FileType::Matlab73:
            return rohdeschwarz::mosaik::dataimportexport::FileType::Matlab73;
          case FileType::Csv:
            return rohdeschwarz::mosaik::dataimportexport::FileType::Csv;
        }
      }

      FileType FileTypeService::GetFileType(rohdeschwarz::mosaik::dataimportexport::FileType fileType)
      {
        switch (fileType)
        {
          default:
          case rohdeschwarz::mosaik::dataimportexport::FileType::Iqtar:
            return FileType::Iqtar;
          case rohdeschwarz::mosaik::dataimportexport::FileType::IQW:
            return FileType::IQW;
          case rohdeschwarz::mosaik::dataimportexport::FileType::Matlab4:
            return FileType::Matlab4;
          case rohdeschwarz::mosaik::dataimportexport::FileType::Matlab73:
            return FileType::Matlab73;
          case rohdeschwarz::mosaik::dataimportexport::FileType::Csv:
            return FileType::Csv;
        }
      }
    }
  }
}
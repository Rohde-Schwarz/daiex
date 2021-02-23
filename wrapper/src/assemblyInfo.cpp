#include "assemblyInfo.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("LibDataImportExport Wrapper")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Rohde & Schwarz")];
[assembly:AssemblyProductAttribute("LibDataImportExport Wrapper")];
[assembly:AssemblyCopyrightAttribute("Copyright (C) Rohde & Schwarz 2020")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute(FILEVER)];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
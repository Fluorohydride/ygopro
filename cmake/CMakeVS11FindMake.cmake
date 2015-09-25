
#=============================================================================
# Copyright 2007-2011 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Look for MSBuild first
if(NOT CMAKE_CROSSCOMPILING)
  set(_FDIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7;FrameworkDir32]")
  set(_FVER "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7;FrameworkVer32]")
  find_program(CMAKE_MAKE_PROGRAM
    NAMES MSBuild
    HINTS
    ${_FDIR}/${_FVER}
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup\\VS;ProductDir]
    "$ENV{SYSTEMROOT}/Microsoft.NET/Framework/[HKEY_CURRENT_USER\\Software\\Microsoft\\VisualStudio\\11.0_Config;CLR Version]/"
    "c:/WINDOWS/Microsoft.NET/Framework/[HKEY_CURRENT_USER\\Software\\Microsoft\\VisualStudio\\11.0_Config;CLR Version]/"
    "$ENV{SYSTEMROOT}/Microsoft.NET/Framework/[HKEY_CURRENT_USER\\Software\\Microsoft\\VCExpress\\11.0_Config;CLR Version]/")
endif()

# if MSBuild is not found, then use devenv.
find_program(CMAKE_MAKE_PROGRAM
  NAMES devenv
  HINTS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup\\VS;EnvironmentDirectory]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup;Dbghelp_path]
  "$ENV{ProgramFiles}/Microsoft Visual Studio 11.0/Common7/IDE"
  "$ENV{ProgramFiles}/Microsoft Visual Studio11.0/Common7/IDE"
  "$ENV{ProgramFiles}/Microsoft Visual Studio 11/Common7/IDE"
  "$ENV{ProgramFiles}/Microsoft Visual Studio11/Common7/IDE"
  "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio 11.0/Common7/IDE"
  "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio11.0/Common7/IDE"
  "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio 11/Common7/IDE"
  "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio11/Common7/IDE"
  "/Program Files/Microsoft Visual Studio 11.0/Common7/IDE/"
  "/Program Files/Microsoft Visual Studio 11/Common7/IDE/"
  )

mark_as_advanced(CMAKE_MAKE_PROGRAM)
set(MSVC11 1)
set(MSVC_VERSION 1700)

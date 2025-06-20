/******************************************
Copyright (c) 2017, Mate Soos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***********************************************/

#include "GitSHA1.h"

const char* CMSat::get_version_sha1()
{
    static const char myversion_sha1[] = "@GIT_SHA1@";
    return myversion_sha1;
}

const char* CMSat::get_version_tag()
{
    static const char myversion_tag[] = "@PROJECT_VERSION@";
    return myversion_tag;
}

const char* CMSat::get_compilation_env()
{
    static const char compilation_env[] =
    "CMAKE_CXX_COMPILER = @CMAKE_CXX_COMPILER@ | "
    "CMAKE_CXX_FLAGS = @CMAKE_CXX_FLAGS@ | "
    "COMPILE_DEFINES = @COMPILE_DEFINES@ | "
    "STATICCOMPILE = @STATICCOMPILE@ | "
    "ONLY_SIMPLE = @ONLY_SIMPLE@ | "
    "Boost_FOUND = @Boost_FOUND@ | "
    "STATS = @STATS@ | "
    "SQLITE3_FOUND = @SQLITE3_FOUND@ | "
    "ZLIB_FOUND = @ZLIB_FOUND@ | "
    "VALGRIND_FOUND = @VALGRIND_FOUND@ | "
    "ENABLE_TESTING = @ENABLE_TESTING@ | "
    "M4RI_FOUND = @M4RI_FOUND@ | "
    "NOM4RI = @NOM4RI@ | "
    "SLOW_DEBUG = @SLOW_DEBUG@ | "
    "ENABLE_ASSERTIONS = @ENABLE_ASSERTIONS@ | "
    "PYTHON_EXECUTABLE = @PYTHON_EXECUTABLE@ | "
    "PYTHON_LIBRARY = @PYTHON_LIBRARY@ | "
    "PYTHON_INCLUDE_DIRS = @PYTHON_INCLUDE_DIRS@ | "
    "MY_TARGETS = @MY_TARGETS@ | "
    "LARGEMEM = @LARGEMEM@ | "
    "LIMITMEM = @LIMITMEM@ | "
    "BREAKID_LIBRARIES = @BREAKID_LIBRARIES@ | "
    "BREAKID-VER = @BREAKID_VERSION_MAJOR@.@BREAKID_VERSION_MINOR@ | "
    "BOSPHORUS_LIBRARIES = @BOSPHORUS_LIBRARIES@ | "
    "BOSPH-VER = @BOSPHORUS_VERSION_MAJOR@.@BOSPHORUS_VERSION_MINOR@ | "
    "compilation date time = " __DATE__ " " __TIME__
    ""
    ;
    return compilation_env;
}

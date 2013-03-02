/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file languagespecs.cpp
/// \brief Implements LanguageSpecs class

#include "languagespecs.h"


using namespace PythonAST;

LanguageSpecs LanguageSpecs::python2Specs()
{
    LanguageSpecs specs;
    specs.majorVersion = 2;
    specs.minorVersion = 7;

    return specs;
}

LanguageSpecs LanguageSpecs::python3Specs()
{
    LanguageSpecs specs;
    specs.majorVersion = 3;
    specs.minorVersion = 3;

    return specs;
}

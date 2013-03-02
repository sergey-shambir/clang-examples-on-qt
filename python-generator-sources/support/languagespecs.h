/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file language.h
/// \brief Declares LanguageSpecs - class that contains python language version specifications

#ifndef PYTHONAST_LANGUAGE_H
#define PYTHONAST_LANGUAGE_H

#include "../pythonast_global.h"

namespace PythonAST {

class PYTHONAST_EXPORT LanguageSpecs
{
    LanguageSpecs() {}
public:
    /// @return Specs for latest supported python 2 release
    static LanguageSpecs python2Specs();

    /// @return Specs for latest supported python 3 release
    static LanguageSpecs python3Specs();

    int majorVersion;
    int minorVersion;
};

} // namespace PythonAST

#endif // PYTHONAST_LANGUAGE_H

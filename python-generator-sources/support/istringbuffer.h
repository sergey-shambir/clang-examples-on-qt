/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file istringbuffer.h
/// \brief

#ifndef PYTHONAST_ISTRINGBUFFER_H
#define PYTHONAST_ISTRINGBUFFER_H

#include "cstring_p.h"

namespace PythonAST {

class IStringBuffer
{
public:
    IStringBuffer();
    virtual ~IStringBuffer();

    const CString &string();
};

} // namespace PythonAST

#endif // PYTHONAST_ISTRINGBUFFER_H

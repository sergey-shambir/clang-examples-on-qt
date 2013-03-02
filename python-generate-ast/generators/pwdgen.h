/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file pwdgen.h
/// \brief

#ifndef PWDGEN_H
#define PWDGEN_H

#include <generator.h>

class PwdGen : public Generator
{
public:
    PwdGen();
    void run();

private:
    void printClass(const CXCursor &classCursor);
    void printClassDecl(const CXCursor &classCursor);
    void printBaseClass(const CXCursor &baseCursor);
    void printMethod(const CXCursor &methodCursor);
};

#endif // PWDGEN_H

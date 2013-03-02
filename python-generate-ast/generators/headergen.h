/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file headergen.h
/// \brief

#ifndef HEADERGEN_H
#define HEADERGEN_H

#include <generator.h>

class HeaderGen : public Generator
{
public:
    HeaderGen();
    void run();

private:
    void printClass();
    void printNestedEnum();
    void printClassDecl(const CXCursor &classCursor);
    void printBaseClass(const CXCursor &baseCursor);
    void printMethod(const CXCursor &methodCursor);
};

#endif // HEADERGEN_H

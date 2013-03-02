/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file implementationgen.h
/// \brief

#ifndef IMPLEMENTATIONGEN_H
#define IMPLEMENTATIONGEN_H

#include "../generator.h"

class ImplementationGen : public Generator
{
public:
    ImplementationGen();

    void run();

private:
    void implementClass();
};

#endif // IMPLEMENTATIONGEN_H

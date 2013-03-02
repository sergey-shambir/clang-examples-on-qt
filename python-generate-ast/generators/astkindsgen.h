/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file astkindsgen.h
/// \brief

#ifndef ASTKINDSGEN_H
#define ASTKINDSGEN_H

#include "../generator.h"

class ASTKindsGen : public Generator
{
public:
    ASTKindsGen();

    void run();
};

#endif // ASTKINDSGEN_H

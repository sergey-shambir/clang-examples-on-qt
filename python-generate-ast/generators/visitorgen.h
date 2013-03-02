/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file visitorgen.h
/// \brief

#ifndef VISITORGEN_H
#define VISITORGEN_H

#include "../generator.h"

class VisitorGen : public Generator
{
public:
    VisitorGen();
    void run();
};

#endif // VISITORGEN_H

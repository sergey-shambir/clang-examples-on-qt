/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file pwdgen.cpp
/// \brief

#include "pwdgen.h"
#include <set>

PwdGen::PwdGen()
    : Generator("PwdGen")
{
}

void PwdGen::run()
{
    std::set<std::string> names;
    visitCursorChildren(clang_getTranslationUnitCursor(unit()), [this, &names]() {
        if (isLocalNamespace())
            return CXChildVisit_Recurse;
        if (isVisitableClass())
            names.insert(context().selfName);
        return CXChildVisit_Continue;
    });

    beginDefineGuards();
    beginNamespace();
    for (const auto &name : names)
        out() << "class " << name << ";\n";

    endNamespace();
    endDefineGuards();
}

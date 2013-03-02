/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file visitorgen.cpp
/// \brief

#include "visitorgen.h"
#include <set>
#include <string>

VisitorGen::VisitorGen()
    : Generator("VisitorGen")
{
}

void VisitorGen::run()
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
    out() << "#include \"ast_pwd.h\"\n"
          << "\n";
    beginNamespace();
    out() << "class ITreeVisitor\n"
             "{\n"
             "public:\n";

    for (const auto &name : names)
        out() << "    virtual bool visit(" << name << " &ast) = 0;\n";

    out() << "};\n";
    endNamespace();
    endDefineGuards();
}

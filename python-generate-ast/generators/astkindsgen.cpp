/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file astkindsgen.cpp
/// \brief

#include "astkindsgen.h"
#include <set>

ASTKindsGen::ASTKindsGen()
    : Generator("ASTKindsGen")
{
}

static std::string fixedClassName(const std::string &className)
{
    size_t index = className.find("AST");
    if (index == std::string::npos)
        return className;
    return className.substr(0, index);
}

void ASTKindsGen::run()
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

    out() << "enum CursorKind {\n";
    for (const auto &name : names)
        out() << "    Cursor_" << fixedClassName(name) << ",\n";
    out() << "    CursorsNum\n";
    out() << "};\n";

    endNamespace();
    endDefineGuards();
}

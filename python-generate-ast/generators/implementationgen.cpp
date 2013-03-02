/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file implementationgen.cpp
/// \brief

#include "implementationgen.h"
#include "constants.h"
#include <cstring>

ImplementationGen::ImplementationGen()
    : Generator("ImplementationGen")
{
}

void ImplementationGen::run()
{
    beginDefineGuards();
    out() << "#include \"astnodes.h\"\n";
    out() << "#include <assert.h>\n";
    out() << "\n";
    beginNamespace();

    visitCursorChildren(clang_getTranslationUnitCursor(unit()), [this]() {
        if (isVisitableClass() || isBaseClass())
            implementClass();

        if (isLocalNamespace())
            return CXChildVisit_Recurse;

        return CXChildVisit_Continue;
    });

    endNamespace();
    endDefineGuards();
}

void ImplementationGen::implementClass()
{
    std::vector<CXCursor> fields;
    visitCursorChildren(context().self, [this, &fields]() {
        if (context().self.kind == CXCursor_FieldDecl)
            fields.push_back(context().self);
        return CXChildVisit_Continue;
    });

    // make constructor
    out() << context().selfName << "::" << context().selfName << "()\n";
    bool isFirst = true;
    auto printFunc = [&isFirst, this](const CXCursor &cursor, const std::string &initializer) {
        out() << "    " << (isFirst ? ":" : ",") << " m_";
        out() << stringize(cursor) << "(" << initializer << ")\n";
        isFirst = false;
    };

    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        if (type.kind == CXType_Pointer)
            printFunc(cursor, "0");
        else if (type.kind == CXType_Bool)
            printFunc(cursor, "false");
    }
    out() << "{\n"
          << "}\n"
          << "\n";

    // make visitor stuff
    if (isVisitableClass()) {
        out() << "void " << context().selfName << "::accept(ITreeVisitor &visitor)\n"
              << "{\n";

        if (fields.empty()) {
            out() << "    visitor.visit(*this);\n";
        } else {
            out() << "    if (visitor.visit(*this)) {\n";
            for (auto cursor : fields) {
                auto type = clang_getCursorType(cursor);
                std::string fieldName = "m_" + stringize(cursor);
                if (type.kind == CXType_Pointer) {
                    if (getAnnotation(cursor) == Ann_RequiredChild)
                        out() << "        " << fieldName << "->accept(visitor);\n";
                    else
                        out() << "        if (" << fieldName << ")\n"
                              << "            " << fieldName << "->accept(visitor);\n";
                } else if (type.kind == CXType_Unexposed) {
                    out() << "        " << fieldName << ".accept(visitor);\n";
                }
            }
            out() << "    }\n";
        }
        out() << "}\n"
              << "\n";
    }

    // make bool hasNode() methods
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        if (type.kind == CXType_Pointer) {
            std::string printedName = stringize(cursor);
            printedName[0] = toupper(printedName[0]);
            out () << "bool " << context().selfName << "::has" << printedName << "() const\n"
                   << "{\n";
            out() << "    return m_" << stringize(cursor) << " != 0;\n";
            out() << "}\n"
                  << "\n";
        }
    }

    // make getters
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        std::string printed = stringize(type, context().selfName + "::" + stringize(cursor), true /*make const&*/);
        if (type.kind == CXType_Enum) {
            printed = context().selfName + "::" + printed;
        } else {
            size_t i = printed.find(C_LIB_NAMESPACE + std::string("::"));
            if (i != std::string::npos)
                printed.erase(i, std::strlen(C_LIB_NAMESPACE) + std::strlen("::"));
        }
        out() << printed << "() const\n"
              << "{\n";
        if (getAnnotation(cursor) == Ann_RequiredChild)
            out() << "    assert(0 != m_" << stringize(cursor)
                  << " && \"Required child cannot be nullptr\");\n";
        out() << "    return m_" << stringize(cursor) << ";\n";
        out() << "}\n"
              << "\n";
    }

    // make setters
    for (auto cursor : fields) {
        auto type = clang_getCursorType(cursor);
        std::string upperName = stringize(cursor);
        upperName[0] = toupper(upperName[0]);

        std::string printedType = stringize(type, stringize(cursor), true /*make const&*/);
        size_t i = printedType.find(C_LIB_NAMESPACE);
        if (i != std::string::npos)
            printedType.erase(i, strlen(C_LIB_NAMESPACE) + strlen("::"));

        if (type.kind == CXType_Unexposed) {
            out() << "void " << context().selfName << "::set" << upperName
                  << "(" << printedType << ")\n"
                  << "{\n";
            out() << "    m_" << stringize(cursor) << " = " << stringize(cursor) << ";\n";
            out() << "}\n"
                     "\n";

            size_t start = printedType.find("<");
            size_t end = printedType.rfind(">");
            if (start == std::string::npos || end == std::string::npos || start > end)
                continue;

            printedType = printedType.substr(start + 1, end - start - 1);
            out() << "void " << context().selfName << "::appendTo" << upperName
                  << "(" << printedType << " *value, MemoryRegistar &registar)\n"
                  << "{\n";
            out() << "    m_" << stringize(cursor) << ".append(value, registar);\n";
            out() << "}\n"
                  << "\n";
        } else {
            out() << "void " << context().selfName <<  "::set" << upperName
                  << "(" << printedType << ")\n"
                  << "{\n";
            if (getAnnotation(cursor) == Ann_RequiredChild)
                out() << "    assert(0 != " << stringize(cursor)
                      << " && \"Cannot set required child to nullptr\");\n";
            out() << "    m_" << stringize(cursor) << " = " << stringize(cursor) << ";\n";
            out() << "}\n"
                  << "\n";
        }
    }
}

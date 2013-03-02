/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file headergen.cpp
/// \brief Generates PythonAST header

#include "headergen.h"
#include "string.h"
#include "constants.h"

HeaderGen::HeaderGen()
    : Generator("HeaderGen")
{
}

void HeaderGen::run()
{
    beginDefineGuards();
    out() << "#include \"../pythonast_global.h\"\n"
          << "#include \"ast_pwd.h\"\n"
          << "#include \"astbase.h\"\n"
          << "#include \"itreevisitor.h\"\n"
          << "#include \"../support/support.h\"\n"
          << "\n";
    beginNamespace();

    visitCursorChildren(clang_getTranslationUnitCursor(unit()), [this]() {
        if (isVisitableClass() || isBaseClass())
            printClass();

        if (isLocalNamespace())
            return CXChildVisit_Recurse;

        return CXChildVisit_Continue;
    });

    endNamespace();
    endDefineGuards();
}

void HeaderGen::printClass()
{
    printClassDecl(context().self);
    out() << "{\n"
          << "public:\n";

    std::vector<CXCursor> fields;
    visitCursorChildren(context().self, [this, &fields]() {
        if (context().self.kind == CXCursor_FieldDecl)
            fields.push_back(context().self);
        if (context().self.kind == CXCursor_EnumDecl) {
            printNestedEnum();
        }
        return CXChildVisit_Continue;
    });

    // make constructor
    out() << "    " << context().selfName << "();\n";

    // make visitor stuff
    if (isVisitableClass()) {
        out() << "    void accept(ITreeVisitor &visitor);\n";
        if (!fields.empty())
            out() << "\n";
    }

    if (!fields.empty()) {
        // make bool hasNode() methods
        for (auto cursor : fields) {
            auto type = clang_getCursorType(cursor);
            if (type.kind == CXType_Pointer) {
                std::string printedName = stringize(cursor);
                printedName[0] = toupper(printedName[0]);
                out () << "    bool has" << printedName << "() const;\n";
            }
        }

        // make getters
        for (auto cursor : fields) {
            auto type = clang_getCursorType(cursor);
            std::string printed = stringize(type, stringize(cursor), true /*make const&*/);

            size_t i = printed.find(C_LIB_NAMESPACE + std::string("::"));
            if (i != std::string::npos)
                printed.erase(i, strlen(C_LIB_NAMESPACE) + strlen("::"));
            out() << "    "
                  << printed
                  << "() const;\n";
        }

        // make setters
        for (auto cursor : fields) {
            auto type = clang_getCursorType(cursor);
            std::string printedName = stringize(cursor);
            printedName[0] = toupper(printedName[0]);

            std::string printedType = stringize(type, stringize(cursor), true /*make const&*/);
            size_t i = printedType.find(C_LIB_NAMESPACE);
            if (i != std::string::npos)
                printedType.erase(i, strlen(C_LIB_NAMESPACE) + strlen("::"));

            if (type.kind == CXType_Unexposed) {
                out() << "    void set" << printedName
                      << "(" << printedType << ");\n";

                size_t start = printedType.find("<");
                size_t end = printedType.rfind(">");
                if (start == std::string::npos || end == std::string::npos || start > end)
                    continue;

                printedType = printedType.substr(start + 1, end - start - 1);
                out() << "    void appendTo" << printedName
                      << "(" << printedType << " *value, MemoryRegistar &registar);\n";
            } else {
                out() << "    void set" << printedName
                      << "(" << printedType << ");\n";
            }
        }

        out() << "\n"
                 "private:\n";
        // make members
        for (auto cursor : fields) {
            auto type = clang_getCursorType(cursor);
            std::string printed = stringize(type, "m_" + stringize(cursor));
            size_t i = printed.find(C_LIB_NAMESPACE);
            if (i != std::string::npos)
                printed.erase(i, strlen(C_LIB_NAMESPACE) + strlen("::"));
            printed += ";";
            switch (getAnnotation(cursor)) {
            case Ann_OptionalChild:
                printed += " ///< @brief Optional, can be nullptr";
                break;
            case Ann_RequiredChild:
                printed += " ///< @brief Always exists, never nullptr";
                break;
            default:
                break;
            }
            out() << "    " << printed << "\n";
        }
    }

    out() << "};\n";
}

void HeaderGen::printNestedEnum()
{
    out() << "    enum " << context().selfName << " {\n";
    bool isFirst = true;
    visitCursorChildren(context().self, [this, &isFirst]() {
        if (context().self.kind == CXCursor_EnumConstantDecl) {
            if (!isFirst)
                out() << ",\n";
            isFirst = false;
            out() << "        " << context().selfName;
        }
        return CXChildVisit_Continue;
    });
    if (!isFirst)
        out() << "\n";
    out() << "    };\n"
          << "\n";
}

void HeaderGen::printClassDecl(const CXCursor &classCursor)
{
    out() << "\n"
          << "class PYTHONAST_EXPORT " << stringize(classCursor);
    visitCursorChildren(classCursor, [this]() {
        if (context().self.kind == CXCursor_CXXBaseSpecifier)
            printBaseClass(context().self);
        return CXChildVisit_Continue;
    });
    out() << "\n";
}

void HeaderGen::printBaseClass(const CXCursor &baseCursor)
{
    std::string name;
    visitCursorChildren(baseCursor, [this, &name]() {
        if (context().self.kind == CXCursor_TypeRef) {
            CXCursor ref = clang_getCursorReferenced(context().self);
            name = stringize(ref);
            return CXChildVisit_Break;
        }
        return CXChildVisit_Continue;
    });

    out() << ": public " << name;
}

void HeaderGen::printMethod(const CXCursor &methodCursor)
{
    (void)methodCursor;
}


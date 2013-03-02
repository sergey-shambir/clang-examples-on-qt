/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file unitconsumer.cpp
/// \brief Implements Generator - base class for code generation for signle file

#include <iostream>
#include "constants.h"
#include "generator.h"

Generator::Generator(const std::string &subclass)
    : m_subclassName(subclass)
{
}

Generator::~Generator()
{
}

bool Generator::isBaseClass() const
{
    if (context().self.kind != CXCursor_ClassDecl)
        return false;
    if (getFile() != m_unitFile)
        return false;
    if (getAnnotation() == Ann_BaseClass)
        return true;
    return false;
}

bool Generator::isVisitableClass() const
{
    if (context().self.kind != CXCursor_ClassDecl)
        return false;
    if (getFile() != m_unitFile)
        return false;
    if (getAnnotation() == Ann_BaseClass)
        return false;
    return true;
}

bool Generator::isLocalNamespace() const
{
    if (context().self.kind != CXCursor_Namespace)
        return false;
    if (getFile() != m_unitFile)
        return false;
    return true;
}

Annotation Generator::getAnnotation(const CXCursor &cursor) const
{
    std::string result;
    visitCursorChildren(cursor, [this, &result] {
        if (context().self.kind == CXCursor_AnnotateAttr) {
            result = context().selfName;
            return CXChildVisit_Break;
        }
        return CXChildVisit_Continue;
    });

    if (result == "OptionalChild")
        return Ann_OptionalChild;
    if (result == "RequiredChild")
        return Ann_RequiredChild;
    if (result == "AbstractClass")
        return Ann_BaseClass;

    return Ann_Unknown;
}

Annotation Generator::getAnnotation() const
{
    return getAnnotation(context().self);
}

std::string Generator::getFile() const
{
    return getLocationFile(clang_getCursorLocation(context().self));
}

const VisitContext &Generator::context() const
{
    return m_context;
}

CXTranslationUnit Generator::unit() const
{
    return m_unit;
}

std::stringstream &Generator::out()
{
    return m_output;
}

void Generator::init(CXTranslationUnit unit, const std::string &fileBasename, const std::string &targetFileName)
{
    m_unit = unit;
    m_unitFile = fileBasename;

    std::string namespacePart(C_LIB_NAMESPACE);
    for (char &ch : namespacePart)
        ch = toupper(ch);
    std::string fileNamePart(targetFileName);
    for (char &ch : fileNamePart)
        ch = (ch == '.') ? '_' : toupper(ch);
    m_defineGuard = namespacePart + "_" + fileNamePart;
}

std::string Generator::runResult() const
{
    return m_output.str();
}

void Generator::beginDefineGuards()
{
    out() << "/****************************************************************************\n"
             "**\n"
             "** This file is distributed under the University of Illinois Open Source\n"
             "** License. See PYTHONAST_LICENSE.TXT for details.\n"
             "**\n"
             "****************************************************************************/\n"
             "\n"
             "// Do not edit!\n"
             "// File was generated with python-generate-ast utility.\n"
             "\n";

    out() << "#ifndef " << m_defineGuard << "\n";
    out() << "#define " << m_defineGuard << "\n";
    out() << "\n";
}

void Generator::beginNamespace()
{
    out() << "namespace " << C_LIB_NAMESPACE << " {\n";
    out() << "\n";
}

void Generator::endNamespace()
{
    out() << "\n";
    out() << "} // namespace " << C_LIB_NAMESPACE << "\n";
}

void Generator::endDefineGuards()
{
    out() << "\n";
    out() << "#endif //" << m_defineGuard << "\n";
}

std::string Generator::stringize(const CXString &string) const
{
    const char *chars = clang_getCString(string);
    if (!chars)
        return std::string();

    std::string ret(chars);
    clang_disposeString(string);
    return ret;
}

std::string Generator::stringize(const CXCursor &cursor) const
{
    return stringize(clang_getCursorSpelling(cursor));
}

std::string Generator::stringize(const CXType &type, const std::string &varName, bool makeConstReference) const
{
    std::string result;

    if (makeConstReference) {
        switch (type.kind) {
        case CXType_LValueReference:
        case CXType_RValueReference:
        case CXType_Pointer:
        case CXType_Enum:
        case CXType_Bool:
        case CXType_Char_S:
        case CXType_Char_U:
        case CXType_Char16:
        case CXType_Int:
        case CXType_Long:
        case CXType_LongLong:
        case CXType_Float:
        case CXType_Double:
        case CXType_LongDouble:
            makeConstReference = false;
            break;
        default:
            break;
        }
    }

    if (makeConstReference || clang_isConstQualifiedType(type))
        result += "const ";

    switch (type.kind) {
    case CXType_Void:
        result += "void";
        if (!varName.empty())
            result += ' ';
        break;
    case CXType_Bool:
        result += "bool";
        if (!varName.empty())
            result += ' ';
        break;

    case CXType_Pointer: {
        CXType pointee = clang_getPointeeType(type);
        result += stringize(pointee);
        result += " *";
    }
        break;

    case CXType_LValueReference: {
        CXType pointee = clang_getPointeeType(type);
        result += stringize(pointee);
    }
        break;
    case CXType_Record:
    case CXType_Enum: {
        result += stringize(clang_getTypeDeclaration(type));
        if (!varName.empty())
            result += ' ';
    }
        break;

    case CXType_Unexposed:
        result += stringize(clang_getCursorDisplayName(clang_getTypeDeclaration(type)));
        if (!varName.empty())
            result += ' ';
        break;
    case CXType_Overload:
        result += "<CXType_Overload>";
        reportWarning("stringize: CXType_Overload not handled");
        break;
    case CXType_Dependent:
        result += "<CXType_Dependent>";
        reportWarning("stringize: CXType_Dependent not handled");
        break;
    case CXType_Invalid:
        result += "<CXType_Invalid>";
        reportWarning("stringize: CXType_Invalid not handled");
        break;
    default:
        result += "<other CXTypeKind>";
        reportWarning("stringize: such CXTypeKind not handled");
        break;
    }

    if (makeConstReference) {
        result += "&";
    }

    result += varName;
    return result;
}

std::string Generator::getLocationFile(const CXSourceLocation &location) const
{
    std::string fileName;
    unsigned line = 0, column = 0;
    getLocationParts(location, fileName, line, column);

    return fileName;
}

void Generator::reportWarning(const CXCursor &cursor, const std::string &message) const
{
    std::string fileName;
    unsigned line = 0, column = 0;
    getLocationParts(cursor, fileName, line, column);

    std::cerr << m_subclassName << ": "
              << "warning[" << line  << ":" << column << "]: "
              << message << std::endl;
}

void Generator::reportWarning(const std::string &message) const
{
    reportWarning(context().self, message);
}

typedef std::function<CXChildVisitResult
(const CXCursor &self, const CXCursor &parent)> VisitorWrapper;

static CXChildVisitResult childVisitor(CXCursor cursor,
                                       CXCursor parent,
                                       CXClientData client_data)
{
    VisitorWrapper &callback = *reinterpret_cast<VisitorWrapper *>(client_data);
    return callback(cursor, parent);
}

void Generator::visitCursorChildren(CXCursor cursor, Generator::Visitor callback) const
{
    VisitorWrapper wrapper = [&callback, this](const CXCursor &self, const CXCursor &parent) {
        pushContext(self, parent);
        auto ret = callback();
        popContext();
        return ret;
    };

    CXClientData data = reinterpret_cast<CXClientData>(&wrapper);
    clang_visitChildren(cursor,
                        childVisitor,
                        data);
}

/**
 * CXSourceLocation - 4-байтное число, для извлечения имени файла и позиции в
 * файле потребуется один из вызовов clang_getSpellingLocation или
 * clang_getExpansionLocation
 */
void Generator::getLocationParts(const CXSourceLocation &location, std::string &fileName, unsigned &line, unsigned &column) const
{
    CXFile file;
    unsigned offset = 0;
    clang_getSpellingLocation(location, &file, &line, &column, &offset);
    (void)offset;

    fileName = stringize(clang_getFileName(file));
}

void Generator::getLocationParts(const CXCursor &cursor, std::string &fileName, unsigned &line, unsigned &column) const
{
    getLocationParts(clang_getCursorLocation(cursor), fileName, line, column);
}

void Generator::pushContext(const CXCursor &self, const CXCursor &parent) const
{
    m_stack.push_back(m_context);

    m_context.self = self;
    m_context.selfName = stringize(self);
    m_context.parent = parent;
    m_context.parentName = stringize(parent);
}

void Generator::popContext() const
{
    m_context = m_stack.back();
    m_stack.pop_back();
}

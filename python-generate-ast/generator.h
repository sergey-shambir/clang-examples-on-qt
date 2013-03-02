/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file unitconsumer.h
/// \brief

#ifndef UNITCONSUMER_H
#define UNITCONSUMER_H

#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <clang-c/Index.h>

struct VisitContext
{
    CXCursor self;
    CXCursor parent;
    std::string selfName;
    std::string parentName;
};

enum Annotation
{
    Ann_Unknown = 0,
    Ann_OptionalChild,
    Ann_RequiredChild,
    Ann_BaseClass
};

class Generator
{
public:
    Generator(const std::string &subclass);
    virtual ~Generator();

    virtual void init(CXTranslationUnit unit, const std::string &fileBasename, const std::string &targetFileName);

    virtual void run() = 0;
    std::string runResult() const;

protected:
    typedef std::function<CXChildVisitResult()> Visitor;

    void beginDefineGuards();
    void beginNamespace();
    void endNamespace();
    void endDefineGuards();

    bool isBaseClass() const;
    bool isVisitableClass() const;
    bool isLocalNamespace() const;
    Annotation getAnnotation(const CXCursor &cursor) const;
    Annotation getAnnotation() const;
    std::string getFile() const; // self cursor file

    const VisitContext &context() const;
    CXTranslationUnit unit() const;
    std::stringstream &out();

    std::string stringize(const CXString &string) const;
    std::string stringize(const CXCursor &cursor) const;
    std::string stringize(const CXType &type, const std::string &varName = "", bool makeConstReference = false) const;
    std::string getLocationFile(const CXSourceLocation &location) const;

    void reportWarning(const CXCursor &cursor, const std::string &message) const;
    void reportWarning(const std::string &message) const;
    void visitCursorChildren(CXCursor cursor, Visitor callback) const;
    void getLocationParts(const CXSourceLocation &location,
                          std::string &fileName,
                          unsigned &line,
                          unsigned &column) const;
    void getLocationParts(const CXCursor &cursor,
                          std::string &fileName,
                          unsigned &line,
                          unsigned &column) const;

private:
    void pushContext(const CXCursor &self, const CXCursor &parent) const;
    void popContext() const;

    CXTranslationUnit m_unit;
    std::string m_unitFile;
    std::string m_defineGuard;
    std::string m_subclassName;
    std::stringstream m_output;

    mutable VisitContext m_context;
    mutable std::vector<VisitContext> m_stack;
};

#endif // UNITCONSUMER_H

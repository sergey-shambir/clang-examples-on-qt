/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file actions.cpp
/// \brief реализует fileAction - функцию, собирающую список системных
///        заголовков, используемых в указанном файле

#include "actions.h"
#include <iostream>
#include <vector>

CXIndex g_index;
std::set<std::string> g_includes;
unsigned g_parsingFlags = CXTranslationUnit_None;

#define CHECK_FLAG(enumConstantName) \
    if (flags & CXTranslationUnit_##enumConstantName) \
        names.push_back( #enumConstantName );

//! Печатает флаги, которые будут использованы для вызова
//! \sa clang_parseTranslationUnit
void printParsingFlags()
{
    unsigned flags = g_parsingFlags;
    std::vector<std::string> names;
    CHECK_FLAG(DetailedPreprocessingRecord);
    CHECK_FLAG(Incomplete);
    CHECK_FLAG(PrecompiledPreamble);
    CHECK_FLAG(CacheCompletionResults);
    CHECK_FLAG(ForSerialization);
    CHECK_FLAG(CXXChainedPCH);
    CHECK_FLAG(SkipFunctionBodies);
    CHECK_FLAG(IncludeBriefCommentsInCodeCompletion);

    bool isFirst = true;
    if (names.empty())
    {
        std::cout << "None";
    }
    else for (const auto &name : names)
    {
        if (isFirst)
            isFirst = false;
        else
            std::cout << " | ";
        std::cout << name;
    }
    std::cout << std::endl;
}

#undef CHECK_FLAG

//! Конвертация CXString в std::string и освобождение CXString
std::string toStdString(CXString cxString)
{
    std::string stdString(clang_getCString(cxString));
    clang_disposeString(cxString);
    return stdString;
}

//! Если путь относительный, то заголовок является частью проекта
bool isProjectPart(const std::string &filePath)
{
    if (filePath.empty() || filePath[0] == '.')
        return true;
    return false;
}

//! Посетитель получает стек директив #include и может определить,
//! Был файл включён в проект напрямую или косвенно.
//! Косвенное включение зависит от версий библиотек и для нас не интересно.
bool isExplicitInclusion(unsigned stackLen, const CXSourceLocation* inclusionStack)
{
    if (stackLen == 0)
        return true;
    CXSourceLocation const& loc = inclusionStack[0];

    CXFile file;
    unsigned line = 0;
    unsigned column = 0;
    unsigned offset = 0;
    clang_getSpellingLocation(loc, &file, &line, &column, &offset);

    std::string fname = toStdString(clang_getFileName(file));
    return isProjectPart(fname);
}

//! Посетитель получает информацию о включённом заголовке,
//! если заголовок включён явно и при этом не является частью проекта,
//! то он регистрируется в переменной g_include
void inclusionsVisitor(CXFile included_file,
                       CXSourceLocation* inclusion_stack,
                       unsigned include_len,
                       CXClientData /*client_data*/)
{
    std::string name(toStdString(clang_getFileName(included_file)));
    if (!isProjectPart(name) && isExplicitInclusion(include_len, inclusion_stack))
    {
        g_includes.insert(name);
    }
}

//! Для каждого файла посетитель обходит все включённые из файла заголовки
/**
 * Параметр CXTranslationUnit_SkipFunctionBodies позволяет ускорить разбор
 * за счёт игнорирования парсером тел функций
 */
void fileAction(const std::string &path)
{
    CXTranslationUnit unit = clang_parseTranslationUnit(
                g_index,
                path.c_str(),
                nullptr, /* argv */
                0, /* argc */
                nullptr, /* unsaved files */
                0, /* num unsaved files */
                g_parsingFlags
                );

    if (unit == nullptr)
    {
        std::cout << "Cannot parse: " << path << std::endl;
        return;
    }
    clang_getInclusions(unit, inclusionsVisitor, nullptr);
    clang_disposeTranslationUnit(unit);
}

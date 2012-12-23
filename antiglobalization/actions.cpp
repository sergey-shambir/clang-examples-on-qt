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

CXIndex g_index;
std::set<GlobalVariable> g_globals;

/**
 * CXSourceLocation - 4-байтное число, для извлечения имени файла и позиции в
 * файле потребуется один из вызовов clang_getSpellingLocation или
 * clang_getExpansionLocation
 */
void decodeLocation(const CXSourceLocation &location,
                    std::string &fileName,
                    unsigned &line,
                    unsigned &column)
{
    CXFile file;
    unsigned offset;
    clang_getSpellingLocation(location, &file, &line, &column, &offset);
    (void)offset;

    CXString cxName = clang_getFileName(file);
    fileName = clang_getCString(cxName);
    clang_disposeString(cxName);
}

/**
 * @brief globalsFinder
 * @param cursor - исследуемый курсор
 * @param parent - неиспользуемая, курсор всегда показывает на весь unit
 * @param client_data - неиспользуемая
 * @return Всегда CXChildVisit_Continue, потому что нам не нужны вложенные объекты
 */
CXChildVisitResult globalsFinder(CXCursor cursor,
                                 CXCursor parent,
                                 CXClientData client_data)
{
    (void)client_data;
    (void)parent;

    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind == CXCursor_VarDecl) {
        GlobalVariable var;
        CXString spelling = clang_getCursorSpelling(cursor);
        var.identifier = clang_getCString(spelling);
        clang_disposeString(spelling);

        CXSourceRange range = clang_getCursorExtent(cursor);
        decodeLocation(clang_getRangeStart(range), var.file, var.line, var.column);
        g_globals.insert(var);
    }

    return CXChildVisit_Continue;
}

/**
 * Параметр CXTranslationUnit_SkipFunctionBodies позволяет ускорить разбор
 * за счёт игнорирования парсером тел функций
 * Внутри функций обычно нет глобальных переменных
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
                CXTranslationUnit_SkipFunctionBodies
                | CXTranslationUnit_DetailedPreprocessingRecord
                );

    if (unit == nullptr)
    {
        std::cout << "Cannot parse: " << path << std::endl;
        return;
    }

    // Получаем курсор для всей единицы трансляции
    CXCursor cursorTU = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursorTU, globalsFinder, NULL);

    clang_disposeTranslationUnit(unit);
}

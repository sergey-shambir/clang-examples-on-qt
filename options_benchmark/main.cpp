/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file main.cpp
/// \brief Точка входа
///
/// Программа запускает задачу из примера include_resolver с различными опциями,
/// влияющими на время разбора файла с помощью clang_parseTranslationUnit()

#include "directorywalker.h"
#include "actions.h"

#include <iostream>
#include <set>
#include <clang-c/Index.h>
#include <boost/timer/timer.hpp>

using boost::timer::cpu_timer;

DirectoryWalker g_walker;

float measureTime()
{
    std::set<std::string> empty;
    g_includes.swap(empty);
    g_index = clang_createIndex(false /*bool exclude PCH decls*/,
                                false /*bool display diagnostic*/);

    cpu_timer timer;
    timer.start();

    g_walker(".");

    timer.stop();
    clang_disposeIndex(g_index);

    return timer.elapsed().wall * 1e-9;
}

void runMeasureForFlags(unsigned flags)
{
    g_parsingFlags = flags;
    const int MEASUREMENTS_NUM = 10;

    float time = 0.0f;
    for (int i = 0; i < MEASUREMENTS_NUM; ++i)
        time += measureTime();
    time = time / MEASUREMENTS_NUM;

    std::cout << "Average time: " << time << std::endl;
    std::cout << "    flags = ";
    printParsingFlags();
}

int main()
{
    g_walker.setFileAction(fileAction);

    // Настройки по умолчанию
    runMeasureForFlags(CXTranslationUnit_None);

    runMeasureForFlags(CXTranslationUnit_None);
    runMeasureForFlags(CXTranslationUnit_IncludeBriefCommentsInCodeCompletion);
    runMeasureForFlags(CXTranslationUnit_IncludeBriefCommentsInCodeCompletion
                        | CXTranslationUnit_DetailedPreprocessingRecord
                        | CXTranslationUnit_CacheCompletionResults);

    runMeasureForFlags(CXTranslationUnit_SkipFunctionBodies
                        | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion
                        | CXTranslationUnit_DetailedPreprocessingRecord
                        | CXTranslationUnit_CacheCompletionResults);

    runMeasureForFlags(CXTranslationUnit_SkipFunctionBodies);

    runMeasureForFlags(CXTranslationUnit_SkipFunctionBodies
                       | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion);

    return 0;
}

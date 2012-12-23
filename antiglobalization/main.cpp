/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file main.cpp
/// \brief Точка входа
///
/// Программа обходит директорию с исходниками C/C++ и ищёт глобальные переменные

#include "directorywalker.h"
#include "actions.h"
#include <iostream>

using namespace std;

int main()
{
    DirectoryWalker walker;
    walker.setFileAction(fileAction);
    g_index = clang_createIndex(false /*bool exclude PCH decls*/,
                                false /*bool display diagnostic*/);

    walker(".");

    for (const auto &var : g_globals)
        std::cout << var << std::endl;

    clang_disposeIndex(g_index);
    return 0;
}


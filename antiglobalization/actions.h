/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file actions.h
/// \brief предоставляет функцию fileAction и глобальные переменные

#pragma once

#include <clang-c/Index.h>
#include <string>
#include <set>
#include "globalvariable.h"

extern CXIndex g_index;
extern std::set<GlobalVariable> g_globals;

void fileAction(const std::string &path);

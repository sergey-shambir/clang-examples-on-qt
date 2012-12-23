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

extern CXIndex g_index;
extern std::set<std::string> g_includes;

std::string toStdString(CXString cxString);
bool isProjectPart(const std::string &filePath);
bool isExplicitInclusion(unsigned stackLen, const CXSourceLocation* inclusionStack);
void inclusionsVisitor(CXFile included_file,
                       CXSourceLocation* inclusion_stack,
                       unsigned include_len,
                       CXClientData /*unused*/ client_data);
void fileAction(const std::string &path);

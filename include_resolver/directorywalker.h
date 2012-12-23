/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file directorywalker.h
/// \brief класс, предназначенный для обхода директории с исходниками

#pragma once
#include <string>
#include <functional>

typedef std::function<void(const std::string &path)> WalkerAction;

class DirectoryWalker
{
public:
    DirectoryWalker(const std::string &filter = std::string("c;cxx;cpp"),
                    bool acceptWithoutExtension = false);

    //! Переданное действие вызывается, если имя файла подходит под фильтр
    void setFileAction(const WalkerAction &action);

    void operator ()(const std::string &path);

private:
    void walkDir(const std::string &path);
    void walkFile(const std::string &path);
    bool matchesFilter(const std::string &path);

    std::string m_filter;
    bool m_acceptWithoutExtension;
    WalkerAction m_actionForFile;
    WalkerAction m_actionIfPathNotFound;
};

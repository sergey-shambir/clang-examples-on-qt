/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file directorywalker.cpp
/// \brief реализация класса, предназначенного для обхода директории с исходниками

#include "directorywalker.h"
#include <iostream>

#ifdef _WIN32
#include <io.h>
static const char SEPARATOR = '\\';
#else
#include <dirent.h>
static const char SEPARATOR = '/';
#endif

DirectoryWalker::DirectoryWalker(const std::string &filter,
                                 bool acceptWithoutExtension)
    : m_filter(filter)
    , m_acceptWithoutExtension(acceptWithoutExtension)
{
    m_actionForFile = [](const std::string &){
        };
    m_acceptWithoutExtension = [](const std::string &path){
            std::cout << "Path not found:" << path << std::endl;
        };
}

void DirectoryWalker::setFileAction(const WalkerAction &action)
{
    m_actionForFile = action;
}

void DirectoryWalker::operator ()(const std::string &path)
{
    walkDir(path);
}

void DirectoryWalker::walkDir(const std::string &path)
{
    DIR *directory = opendir(path.c_str());
    if (!directory)
    {
        m_actionIfPathNotFound(path);
        return;
    }

    std::string prefix = path;
    if (prefix[prefix.size() - 1] != SEPARATOR)
        prefix += SEPARATOR;

    struct dirent *entry = nullptr;
    while ((entry = readdir(directory)) != nullptr)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        std::string subpath = prefix + name;
        if (entry->d_type == DT_DIR)
            walkDir(subpath);
        else if (entry->d_type == DT_REG)
            walkFile(subpath);
    }

    closedir(directory);
}

void DirectoryWalker::walkFile(const std::string &path)
{
    if (0 != access(path.c_str(), R_OK)) {
        m_actionIfPathNotFound(path);
        return;
    }

    if (matchesFilter(path))
        m_actionForFile(path);
}

bool DirectoryWalker::matchesFilter(const std::string &path)
{
    size_t di = path.rfind('.');
    if ((di == std::string::npos) || (di == path.size() - 1))
        return m_acceptWithoutExtension;

    std::string extension = path.substr(di + 1);
    size_t ei = m_filter.find(extension);
    return (ei != std::string::npos);
}

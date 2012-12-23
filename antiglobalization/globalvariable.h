/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file globalvariable.cpp
/// \brief Класса GlobalVariable хранит имя и местоположение найденной переменной

#pragma once

#include <iostream>

class GlobalVariable
{
public:
    std::string identifier;
    std::string file;
    unsigned line;
    unsigned column;

    GlobalVariable();
    ~GlobalVariable();

    bool operator <(const GlobalVariable &aVariable) const;
    bool operator ==(const GlobalVariable &aVariable) const;
    bool operator !=(const GlobalVariable &aVariable) const;
};

std::ostream &operator <<(std::ostream& stream, const GlobalVariable &variable);

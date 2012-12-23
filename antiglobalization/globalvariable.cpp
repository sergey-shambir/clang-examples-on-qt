/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See LICENSE.TXT for details.
**
****************************************************************************/

/// \file globalvariable.cpp
/// \brief Реализация класса GlobalVariable
///

#include "globalvariable.h"

GlobalVariable::GlobalVariable()
    : line(1)
    , column(1)
{
}

GlobalVariable::~GlobalVariable()
{
}

bool GlobalVariable::operator <(const GlobalVariable &aVariable) const
{
    if (identifier < aVariable.identifier)
        return true;
    if (identifier == aVariable.identifier) {
        if (file < aVariable.file)
            return true;
        if (file == aVariable.file) {
            if (line < aVariable.line)
                return true;
            if (line == aVariable.line)
                return (column < aVariable.column);
        }
    }

    return false;
}

bool GlobalVariable::operator ==(const GlobalVariable &aVariable) const
{
    return (identifier == aVariable.identifier)
            && (file == aVariable.file)
            && (line == aVariable.line)
            && (column == aVariable.column);
}

bool GlobalVariable::operator !=(const GlobalVariable &aVariable) const
{
    return (identifier != aVariable.identifier)
            || (file != aVariable.file)
            || (line != aVariable.line)
            || (column != aVariable.column);
}

std::ostream &operator <<(std::ostream& stream, const GlobalVariable &variable)
{
    stream << "Global variable \'" << variable.identifier << "\'\n";
    stream << "at [" << variable.line << "; " << variable.column << "]"
              " in file \'" << variable.file << "\'";
    return stream;
}

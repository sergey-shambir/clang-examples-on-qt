/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file cstring_p.cpp
/// \brief Implements constant string

#include "cstring_p.h"
#include <cstring>

namespace PythonAST {

CString::CString()
    : m_data(0)
    , m_size(0)
{
}

CString::CString(const char *data, const int size)
    : m_data(data)
    , m_size(size)
{
}

bool CString::operator ==(const CString &other) const
{
    if (m_size != other.m_size)
        return false;
    for (int i = 0; i < m_size; ++i)
        if (m_data[i] != other[i])
            return false;
    return true;
}

bool CString::operator ==(const std::string &other) const
{
    if (m_size != static_cast<int>(other.size()))
        return false;
    for (int i = 0; i < m_size; ++i)
        if (m_data[i] != other[i])
            return false;
    return true;
}

bool CString::operator !=(const CString &other) const
{
    if (m_size != other.m_size)
        return true;
    for (int i = 0; i < m_size; ++i)
        if (m_data[i] != other[i])
            return true;
    return false;
}

bool CString::operator !=(const std::string &other) const
{
    if (m_size != static_cast<int>(other.size()))
        return true;
    for (int i = 0; i < m_size; ++i)
        if (m_data[i] != other[i])
            return true;
    return false;
}

}

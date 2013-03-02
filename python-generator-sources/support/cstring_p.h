/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file cstring_p.h
/// \brief Declares constant string class

#ifndef CSTRING_P_H
#define CSTRING_P_H

#include "../pythonast_global.h"
#include <string>

namespace PythonAST {

/**
 * @class CString
 * @brief Constant string allocated on extenral buffer
 */
class PYTHONAST_EXPORT CString
{
public:
    explicit CString();
    explicit CString(const char *data, const int size);

    inline char operator [](int index) const { return m_data[index]; };
    inline char at(int index) const          { return m_data[index]; };

    inline int size() const                  { return m_size; }
    inline const char *start() const         { return m_data; }

    bool operator ==(const CString &other) const;
    bool operator ==(const std::string &other) const;
    bool operator !=(const CString &other) const;
    bool operator !=(const std::string &other) const;

private:
    const char *m_data;
    const int m_size;
};

} // namespace PythonAST

#endif // CSTRING_P_H

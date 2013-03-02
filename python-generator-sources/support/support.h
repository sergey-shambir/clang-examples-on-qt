#ifndef PYTHONAST_SUPPORT_H
#define PYTHONAST_SUPPORT_H

#include <new>
#include <stddef.h>
#include <memory>
#include "../pythonast_global.h"

#include "cstring_p.h"
#include "list_p.h"
#include "memorypool_p.h"
#include "languagespecs.h"

namespace PythonAST {

class SourceLocation
{
public:
    SourceLocation() throw()
        : m_line(0), m_column(0) {}

    SourceLocation(quint16 line, quint16 column) throw()
        : m_line(line), m_column(column) {}

    inline bool isNull() const { return m_line == 0; }
    inline quint16 line() const { return m_line; }
    inline quint16 column() const { return m_column; }

    /// Setters
    inline void setLine(quint16 line) { m_line = line; }
    inline void setColumn(quint16 col) { m_column = col; }

    /// Bool operators
    inline bool operator ==(const SourceLocation &other)
    {
        return m_line == other.m_line && m_column == other.m_column;
    }

    inline bool operator !=(const SourceLocation &other)
    {
        return !(*this == other);
    }

private:
    quint16 m_line;
    quint16 m_column;
};

class SourceRange
{
public:
    SourceRange() throw() {}

    SourceRange(SourceLocation begin) throw()
        : m_begin(begin), m_end(begin) {}

    SourceRange(SourceLocation begin, SourceLocation end) throw()
        : m_begin(begin), m_end(end) {}

    inline bool isNull() const { return m_begin.isNull(); }

    inline const SourceLocation& begin() const { return m_begin; }
    inline const SourceLocation& end() const { return m_end; }
    inline quint16 firstLine() const { return m_begin.line(); }
    inline quint16 lastLine() const { return m_end.line(); }
    inline quint16 firstColumn() const { return m_begin.column(); }
    inline quint16 lastColumn() const { return m_end.column(); }
    inline quint16 linesCount() const { return m_end.line() - m_begin.line(); }

    /// Setters
    inline void setBegin(const SourceLocation &begin) { m_begin = begin; }
    inline void setEnd(const SourceLocation &end) { m_end = end; }
    inline void setFirstColumn(quint16 value) { m_begin.setColumn(value); }
    inline void setLastColumn(quint16 value) { m_end.setColumn(value); }

    /// Bool operators
    inline bool operator ==(const SourceRange &other)
    {
        return m_begin == other.m_begin && m_end == other.m_end;
    }

    inline bool operator !=(const SourceRange &other)
    {
        return !(*this == other);
    }

private:
    SourceLocation m_begin;
    SourceLocation m_end;
};

}

#endif // PYTHONAST_SUPPORT_H

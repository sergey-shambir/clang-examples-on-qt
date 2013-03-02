/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file inode.cpp
/// \brief

#include "astbase.h"

namespace PythonAST {

AST::AST(AST *parent)
    : m_parent(parent)
{
}

CursorKind AST::cursorKind() const
{
    return m_cursorKind;
}

const SourceRange &AST::extent() const
{
    return m_extent;
}

AST *AST::parent() const
{
    return m_parent;
}

void AST::setCursorKind(CursorKind kind)
{
    m_cursorKind = kind;
}

void AST::setExtent(const SourceRange &extent)
{
    m_extent = extent;
}

void AST::setParent(AST *parent)
{
    m_parent = parent;
}

} // namespace PythonAST

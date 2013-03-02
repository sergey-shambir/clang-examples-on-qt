/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file inode.h
/// \brief

#ifndef PYTHONAST_INODE_H
#define PYTHONAST_INODE_H

#include "../support/support.h"
#include "astkind.h"

namespace PythonAST {
class ITreeVisitor;

/**
 * \class pylang::Node - abstract class, interface to AST nodes
 *
 * Keeps pointer to parent that will be null only for translation unit node
 * Keeps source range of node
 */
class PYTHONAST_EXPORT AST : public PoolManaged
{
public:
    AST(AST *parent = 0);

    virtual void accept(ITreeVisitor &visitor) = 0;

    CursorKind cursorKind() const;
    const SourceRange &extent() const;
    AST *parent() const;

    void setCursorKind(CursorKind kind);
    void setExtent(const SourceRange &extent);
    void setParent(AST *parent);

private:
    CursorKind m_cursorKind;
    SourceRange m_extent;
    AST *m_parent;
};

} // namespace PythonAST

#endif // PYTHONAST_INODE_H

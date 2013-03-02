/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file memorypool_p.cpp
/// \brief Implements memory management clases

#include "memorypool_p.h"
#include <QtGlobal>
#include <assert.h>

namespace PythonAST {

////////////////////////////////////////////////////////////////////////////////
/**
 * @class pylang::MemoryRegistar
 * @brief memory registar for managed classes
 *
 * AST concept cannot garantue that subclass of Managed class will be deleted
 * correctly with calling destructor. If such class want allocate and use
 * dynamic memory, it should get it from MemoryRegistar.
 * MemoryRegistar destructor will free registered memory in own destructor.
 * Managed class can delete it manually by calling MemoryRegistar::free() method.
 */

MemoryRegistar::MemoryRegistar()
{
}

MemoryRegistar::~MemoryRegistar()
{
    foreach (void *chunk, m_chunks)
        ::free(chunk);
}

void *MemoryRegistar::allocate(size_t size)
{
    void* chunk = ::malloc(size);
    m_chunks.insert(chunk);
    return chunk;
}

void *MemoryRegistar::reallocate(void *chunk, size_t size)
{
    SetIterator it = m_chunks.find(chunk);
    assert(it != m_chunks.end() && "Calling reallocate() for unregistered addresss");
    m_chunks.erase(it);

    chunk = ::realloc(chunk, size);
    m_chunks.insert(chunk);
    return chunk;
}

void MemoryRegistar::free(void *chunk)
{
    SetIterator it = m_chunks.find(chunk);
    assert(it != m_chunks.end() && "Calling free() for unregistered addresss");
    ::free(*it);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * \class pylang::MemoryPool - provides memory for managed classes
 *
 * Pylang intensively allocates big sets of managed classes and deletes all
 * objects in one set simultaniously. MemoryPool handles memory for such
 * classes: it allocates memory on managed instance creation and than deletes
 * all instances at one time, without calling their destructors.
 */

MemoryPool::MemoryPool() :
    m_ptr(0),
    m_end(0)
{
}

MemoryPool::~MemoryPool()
{
    for (unsigned i = 0; i < m_blocks.size(); ++i)
        ::free(m_blocks[i]);
}

void *MemoryPool::allocate_helper(size_t size)
{
    if (size < BLOCK_SIZE)
    {
        char *block = reinterpret_cast<char *>(::malloc(BLOCK_SIZE));
        m_blocks.push_back(block);

        m_ptr = block;
        m_end = m_ptr + BLOCK_SIZE;

        void *addr = m_ptr;
        m_ptr += size;
        return addr;
    }
    else
    {
        void *block = reinterpret_cast<char *>(::malloc(size));
        void *const addr = block;
        if (!m_blocks.empty()) {
            std::swap(m_blocks.back(), block);
        }
        m_blocks.push_back(block);

        return addr;
    }
}

} // namespace PythonAST

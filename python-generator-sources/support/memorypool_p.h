/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file memorypool_p.h
/// \brief Declares memory management classes: MemoryPool and MemoryRegistar

#ifndef PYTHONAST_MEMORYPOOL_P_H
#define PYTHONAST_MEMORYPOOL_P_H

#include <stdlib.h>
#include <vector>
#include <set>

namespace PythonAST {

class MemoryRegistar
{
    MemoryRegistar(const MemoryRegistar &);
    MemoryRegistar& operator =(const MemoryRegistar &);
public:
    MemoryRegistar();
    ~MemoryRegistar();
    void *allocate(size_t size);
    void *reallocate(void *chunk, size_t size);
    void free(void* chunk);

private:
    typedef std::set<void *>::iterator SetIterator;
    std::set<void *> m_chunks;
};

class MemoryPool
{
    MemoryPool(const MemoryPool &);
    MemoryPool& operator =(const MemoryPool &);
public:
    MemoryPool();
    ~MemoryPool();

    inline void *allocate(size_t size);

private:
    void *allocate_helper(size_t size);

    std::vector<void *> m_blocks;
    char *m_ptr;
    char *m_end;

    static const unsigned BLOCK_SIZE = 8 * 1024;
    static const unsigned DEFAULT_BLOCK_COUNT = 8;
};

/**
 * @brief Non-copy
 */
class PoolManaged
{
    PoolManaged(const PoolManaged &);
    void operator = (const PoolManaged &);

public:
    PoolManaged() {}
    virtual ~PoolManaged() {}

    void *operator new(size_t size, MemoryPool &pool)
    {
        return pool.allocate(size);
    }

    void operator delete(void *) { }
    void operator delete(void *, MemoryPool &) { }
};

void *MemoryPool::allocate(size_t size)
{
    size = (size + 7) & ~7; // round up to the next multiple of 8
    if (m_ptr && (m_ptr + size < m_end)) {
        void *addr = m_ptr;
        m_ptr += size;
        return addr;
    }
    return allocate_helper(size);
}

} // namespace PythonAST

#endif // PYTHONAST_MEMORYPOOL_P_H

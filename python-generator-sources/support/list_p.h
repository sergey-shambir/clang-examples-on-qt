/****************************************************************************
**
** This file is distributed under the University of Illinois Open Source
** License. See PYTHONAST_LICENSE.TXT for details.
**
****************************************************************************/

/// \file list_p.h
/// \brief Declares template class List

#ifndef PYTHONAST_LIST_P_H
#define PYTHONAST_LIST_P_H

#include "memorypool_p.h"

namespace PythonAST {
class ITreeVisitor;
class AST;

struct ListPOD {
    void** data;
    unsigned size;
    unsigned capacity;
};

/**
 * @brief ASTs list allocated on external MemoryRegistar, which ensures deletion
 * T::accept(ITreeVisitor &) should exist to use List<T>::accept(ITreeVisitor &)
 */
template<class T>
class List
{
    static const int INITIAL_CAPACITY = 16;
public:
    // Types
    typedef T* value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    List();
    List(const ListPOD &pod);
    int size() const;
    T* at(int index) const;
    T* operator [](int index) const;
    void accept(ITreeVisitor &visitor) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    void append(T *value, MemoryRegistar &registar);
    void append(const List<T> &list, MemoryRegistar &registar);
    ListPOD toPOD() const;

private:
    T** m_data;
    int m_size;
    int m_capacity;
};

template<class T>
List<T>::List()
    : m_data(0)
    , m_size(0)
    , m_capacity(0)
{
}

template<class T>
List<T>::List(const ListPOD &pod)
    : m_data(reinterpret_cast<T**>(pod.data))
    , m_size(pod.size)
    , m_capacity(pod.capacity)
{
}

template<class T>
int List<T>::size() const
{
    return m_size;
}

template<class T>
T *List<T>::at(int index) const
{
    return m_data[index];
}

template<class T>
T *List<T>::operator [](int index) const
{
    return m_data[index];
}

template<class T>
void List<T>::accept(ITreeVisitor &visitor) const
{
    for (int i = 0; i < m_size; ++i)
        m_data[i]->accept(visitor);
}

template<class T>
void List<T>::append(T *value, MemoryRegistar &registar)
{
    ++m_size;
    if (m_data == 0) {
        m_capacity = INITIAL_CAPACITY;
        m_data = reinterpret_cast<T**>(registar.allocate(m_capacity * sizeof(T*)));
    } else if (m_capacity <= m_size) {
        m_capacity *= 2;
        m_data = reinterpret_cast<T**>(registar.reallocate(m_data, m_capacity * sizeof(T*)));
    }
    m_data[m_size - 1] = value;
}

template<class T>
void List<T>::append(const List<T> &list, MemoryRegistar &registar)
{
    if (list.size() == 0)
        return;

    int start = m_size;
    m_size += list.size();
    if (m_data == 0)
        m_data = reinterpret_cast<T**>(registar.allocate(m_size * sizeof(T*)));
    else if (m_capacity <= m_size) {
        m_capacity = m_size * 2;
        m_data = reinterpret_cast<T**>(registar.reallocate(m_data, m_capacity * sizeof(T*)));
    }
    for (int i = start; i < m_size; ++i)
        m_data[i] = list[i - start];
}

template<class T>
typename List<T>::iterator List<T>::begin()
{
    return m_data;
}

template<class T>
typename List<T>::iterator List<T>::end()
{
    return m_data + m_size;
}

template<class T>
typename List<T>::const_iterator List<T>::begin() const
{
    return m_data;
}

template<class T>
typename List<T>::const_iterator List<T>::end() const
{
    return m_data + m_size;
}

template<class T>
ListPOD List<T>::toPOD() const
{
    ListPOD pod;
    pod.data = reinterpret_cast<void**>(m_data);
    pod.capacity = m_capacity;
    pod.size = m_size;
    return pod;
}

} // namespace PythonAST

#endif // PYTHONAST_LIST_P_H

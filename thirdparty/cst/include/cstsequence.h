//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_CSTSEQUENCE_H
#define ADVANCEDGL_CSTSEQUENCE_H

#include <vector>
#include <atomic>

template<typename T>
class ShareSequence
{
public:
    std::vector<T> m_data;
    unsigned m_refCount = 0;



    void incRef()
    {
        m_refCount += 1;
    }
    void decRef()
    {
        m_refCount -= 1;
        if (m_refCount == 0)
        {
            delete this;
        }
    }

    bool isShared()
    {
        return m_refCount == 1;
    }
};



template<typename T>
class CstSequence
{
public:
    using impl_type = std::vector<T>;
    using value_type = typename impl_type::value_type;
    using pointer = typename impl_type::pointer;
    using const_pointer = typename impl_type::const_pointer;
    using reference = typename  impl_type::reference;
    using const_reference = typename  impl_type::const_reference;
    using size_type = typename  impl_type::size_type;
    using difference_type = typename  impl_type::difference_type;
    using iterator = typename  impl_type::iterator;
    using const_iterator = typename  impl_type::const_iterator;
    using reverse_iterator = typename  impl_type::reverse_iterator;
    using const_reverse_iterator = typename  impl_type::const_reverse_iterator;


    struct ValueRef
    {
        ValueRef( CstSequence<T>& seq, size_type pos)
                :mSeq(seq)
                ,mPos(pos)
        {
        }

        operator  const value_type&() const
        {
            return mSeq.r_at(mPos);
        }

        void operator=(const value_type& v)
        {
            mSeq.w_at(mPos) = v;
        }

        void operator+=(const value_type& v)
        {
            mSeq.w_at(mPos) += v;
        }

        ValueRef& operator=(const ValueRef&) = delete;

    private:
        CstSequence<T>& mSeq;
        size_type mPos;
    };

public:
    CstSequence()
    {
        m_pData = new ShareSequence<T>();
        m_pData->incRef();
    }

    CstSequence(const CstSequence& other)
    {
        if (this == &other)
            return;

        m_pData = other.m_pData;
        m_pData->incRef();
    }

    CstSequence(CstSequence&& other)
    {
        m_pData = other.m_pData;
        m_pData->incRef();
    }

    ~CstSequence()
    {
        release();
    }

private:
    void clone()
    {
        if (!m_pData->isShared())
        {

            ShareSequence<T>* newData = new ShareSequence<T>();
            newData->m_data = m_pData->m_data;
            newData->incRef();

            m_pData->decRef();
            m_pData = newData;
        }

    }

    void release()
    {
        m_pData->decRef();
    }

public:
    void resize(size_t new_size)
    {
        clone();
        m_pData->m_data.resize(new_size);
    }

    void resize(size_t new_size, const T& value)
    {
        clone();
        m_pData->m_data.resize(new_size, value);
    }

    void operator=(const CstSequence& other)
    {
        if (m_pData == other.m_pData)
            return;

        release();

        m_pData = other.m_pData;
        m_pData->incRef();
    }

    void push_back(const T& value)
    {
        clone();
        m_pData->m_data.push_back(value);
    }

    size_t size() const
    {
        return m_pData->m_data.size();
    }

    bool empty() const
    {
        return m_pData->m_data.empty();
    }

    const_pointer r_address() const
    {
        return m_pData->m_data.data();
    }

    pointer w_address()
    {
        clone();
        return m_pData->m_data.data();
    }

    void reserve(size_t want_size)
    {
        clone();
        m_pData->m_data.reserve(want_size);
    }

    const_iterator begin() const
    {
        return m_pData->m_data.begin();
    }

    const_iterator end() const
    {
        return m_pData->m_data.end();
    }

    const_iterator r_begin() const
    {
        return m_pData->m_data.begin();
    }

    const_iterator r_end() const
    {
        return m_pData->m_data.end();
    }

    iterator begin()
    {
        clone();
        return m_pData->m_data.end();
    }

    iterator end()
    {
        clone();
        return m_pData->m_data.end();
    }

    iterator w_begin()
    {
        clone();
        return m_pData->m_data.end();
    }

    iterator w_end()
    {
        clone();
        return m_pData->m_data.end();
    }

    void clear()
    {
        clone();
        m_pData->m_data.clear();
    }

    const_reference r_at(size_t pos) const
    {
        return m_pData->m_data.at(pos);
    }

    reference w_at(size_t pos)
    {
        clone();
        return m_pData->m_data.at(pos);
    }

    ValueRef operator[](size_type pos)
    {
        return ValueRef(*this, pos);
    }

    iterator insert(const_iterator where, const_iterator begin, const_iterator end)
    {
        clone();
        return m_pData->m_data.insert(where, begin, end);
    }

private:
    ShareSequence<T>* m_pData;

};

#endif //ADVANCEDGL_CSTSEQUENCE_H

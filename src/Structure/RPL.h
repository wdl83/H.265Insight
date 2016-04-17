#ifndef HEVC_Structure_RPL_h
#define HEVC_Structure_RPL_h

/* STDC++ */
#include <algorithm>
/* HEVC */
#include <HEVC.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class RPL
{
public:
    /* 04/2013, 8.3.4 "Decoding process for reference picture list construction" */

    class List
    {
    public:
        typedef PicOrderCntVal ValueType;
        typedef std::array<ValueType, Limits::NumPocTotalCurr::num> ArrayType;
        typedef ArrayType::iterator Iterator;
        typedef ArrayType::const_iterator ConstIterator;
    private:
        ArrayType m_list;
        int m_size;
    public:
        List():
            m_size(0)
        {}

        ValueType operator[] (int i) const
        {
            return m_list[i];
        }

        ValueType &operator[] (int i)
        {
            return m_list[i];
        }

        ValueType operator[] (PicOrderCntVal i) const
        {
            return m_list[i.value];
        }

        ValueType &operator[] (PicOrderCntVal i)
        {
            return m_list[i.value];
        }

        void insert(int i, ValueType value)
        {
            bdryCheck(size() == i);
            bdryCheck(capacity() > i);
            m_list[i] = value;
            ++m_size;
        }

        int capacity() const
        {
            return m_list.size();
        }

        int size() const
        {
            return m_size;
        }

        Iterator begin()
        {
            return std::begin(m_list);
        }

        ConstIterator begin() const
        {
            return std::begin(m_list);
        }

        Iterator end()
        {
            auto x = std::begin(m_list);

            std::advance(x, size());
            return x;
        }

        ConstIterator end() const
        {
            auto x = std::begin(m_list);

            std::advance(x, size());
            return x;
        }
    };
private:
    Pair<List, RefList> m_list;
public:
    const List &operator[] (RefList l) const
    {
        return m_list[l];
    }

    int size(RefList l) const
    {
        return m_list[l].size();
    }

    void insert(RefList l, int i, List::ValueType v)
    {
        m_list[l].insert(i, v);
    }
};
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Structure

#endif // HEVC_Structure_RPL_h

#ifndef VLM_h
#define VLM_h

#include <VLA.h>
#include <Coord.h>
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
/* Variable Length Matrix */
template <typename T>
class VLM
{
public:
    typedef VLA<T> Container;

    typedef typename Container::ValueType ValueType;
    typedef typename Container::Reference Reference;
    typedef typename Container::ConstReference ConstReference;
    typedef typename Container::Pointer Pointer;
    typedef typename Container::ConstPointer ConstPointer;

    typedef typename Container::SizeType SizeType;
    typedef Coord<SizeType> Pos;
    typedef typename Container::Iterator Iterator;
    typedef typename Container::ReverseIterator ReverseIterator;
    typedef typename Container::ConstIterator ConstIterator;
    typedef typename Container::ConstReverseIterator ConstReverseIterator;
private:
    SizeType m_width;
    SizeType m_height;
    Container m_container;
public:
    VLM(): m_width(0), m_height(0)
    {}

    VLM(SizeType w, SizeType h, ConstReference value = ValueType()):
        m_width(w), m_height(h),
        m_container(w * h, value)
    {
        bdryCheck(SizeType(0) < m_width);
        bdryCheck(SizeType(0) < m_height);
    }

    ConstIterator cbegin() const
    {
        return m_container.cbegin();
    }

    ConstIterator begin() const
    {
        return cbegin();
    }

    ConstIterator cend() const
    {
        return m_container.cend();
    }

    ConstIterator end() const
    {
        return cend();
    }

    SizeType width() const
    {
        return m_width;
    }

    SizeType height() const
    {
        return m_height;
    }

    bool isSquare() const
    {
        return width() == height();
    }

    SizeType size() const
    {
        return m_container.size();
    }

    decltype(m_container.capacity()) capacity() const
    {
        return m_container.capacity();
    }

    void resize(SizeType w, SizeType h, ConstReference value = ValueType())
    {
        bdryCheck(SizeType(0) < w);
        bdryCheck(SizeType(0) < h);

        m_width = w;
        m_height = h;
        m_container.resize(w * h, value);
    }

    SizeType calcOffset(Pos at) const
    {
        const auto offset = at.y() * m_width + at.x();
        return offset;
    }

    ConstReference operator[] (Pos at) const
    {
        return m_container[calcOffset(at)];
    }

    Reference operator[] (Pos at)
    {
        return m_container[calcOffset(at)];
    }
};
/*----------------------------------------------------------------------------*/

#endif /* VLM_h */

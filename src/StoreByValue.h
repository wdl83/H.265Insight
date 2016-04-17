#ifndef HEVC_StoreByValue_h
#define HEVC_StoreByValue_h

/*----------------------------------------------------------------------------*/
template <typename Type>
class StoreByValue
{
public:
    typedef Type ValueType;
private:
    ValueType m_value;
public:
    constexpr StoreByValue(Type value):
        m_value{value}
    {}

    constexpr StoreByValue(const StoreByValue &) = default;
    StoreByValue &operator= (const StoreByValue &) = default;

    inline const ValueType &getValue() const
    {
        return m_value;
    }

    inline void setValue(ValueType value)
    {
        m_value = value;
    }
};
/*----------------------------------------------------------------------------*/

#endif /* HEVC_StoreByValue_h */

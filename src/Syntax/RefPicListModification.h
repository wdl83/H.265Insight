#ifndef SyntaxRefPicListModification_h
#define SyntaxRefPicListModification_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace RefPicListModificationContent {
/*----------------------------------------------------------------------------*/
class RefPicListModificationFlagL0 :
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::ref_pic_list_modification_flag_l0;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ListEntryL0 :
    public Embedded,
    public VLD::UInt
{
private:
    std::map<int, ValueType> m_values;
public:
    static const auto Id = ElementId::list_entry_l0;

    ValueType operator[] (int i) const
    {
        if (m_values.find(i) != m_values.end())
        {
            return m_values.at(i);
        }
        else
        {
            return 0; // inferred
        }
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int numPocTotalCurr, int i)
    {
        setLengthInBits(log2(numPocTotalCurr));
        getFrom(streamAccessLayer, decoder, *this);
        syntaxCheck(m_values.find(i) == m_values.end());
        m_values[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class RefPicListModificationFlagL1 :
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::ref_pic_list_modification_flag_l1;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ListEntryL1:
    public Embedded,
    public VLD::UInt
{
private:
    std::map<int, ValueType> m_values;
public:
    static const auto Id = ElementId::list_entry_l1;

    ValueType operator[] (int i) const
    {
        if (m_values.find(i) != m_values.end())
        {
            return m_values.at(i);
        }
        else
        {
            return 0; // inferred
        }
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int numPocTotalCurr, int i)
    {
        setLengthInBits(log2(numPocTotalCurr));
        getFrom(streamAccessLayer, decoder, *this);
        syntaxCheck(m_values.find(i) == m_values.end());
        m_values[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace RefPicListModificationContent */

class RefPicListModification:
    public EmbeddedAggregator<
        RefPicListModificationContent::RefPicListModificationFlagL0,
        RefPicListModificationContent::ListEntryL0,
        RefPicListModificationContent::RefPicListModificationFlagL1,
        RefPicListModificationContent::ListEntryL1>
{
public:
    static const auto Id = ElementId::ref_pic_list_modification;

    typedef RefPicListModificationContent::RefPicListModificationFlagL0 RefPicListModificationFlagL0;
    typedef RefPicListModificationContent::ListEntryL0 ListEntryL0;
    typedef RefPicListModificationContent::RefPicListModificationFlagL1 RefPicListModificationFlagL1;
    typedef RefPicListModificationContent::ListEntryL1 ListEntryL1;

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const SliceSegmentHeader &,
            const Structure::RPSP &);
};
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Syntax

#endif // SyntaxRefPicListModification_h

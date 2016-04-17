#ifndef HEVC_Syntax_SliceSegmentData_h
#define HEVC_Syntax_SliceSegmentData_h
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/ByteAlignment.h>
#include <Syntax/CodingTreeUnit.h>

namespace HEVC { namespace Syntax { namespace SliceSegmentDataContent {
/*----------------------------------------------------------------------------*/
class EndOfSliceSegmentFlag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    CABAD::NonAdaptiveVariable m_nonAdaptive;
public:
    static const auto Id = ElementId::end_of_slice_segment_flag;

    EndOfSliceSegmentFlag():
        /* Draft 10v23, Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [this](CABAD::State &, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return m_nonAdaptive;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class EndOfSubStreamOneBit:
    public Embedded,
    public CABAD::FixedLength
{
private:
    CABAD::NonAdaptiveVariable m_nonAdaptive;
public:
    static const auto Id = ElementId::end_of_sub_stream_one_bit;

    EndOfSubStreamOneBit():
        /* Draft 10v23, Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [this](CABAD::State &, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return m_nonAdaptive;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
} /* SliceSegmentDataContent */

class SliceSegmentData:
    public EmbeddedAggregator<
        SliceSegmentDataContent::EndOfSliceSegmentFlag,
        SliceSegmentDataContent::EndOfSubStreamOneBit>,
    public SubtreeListAggregator<
        CodingTreeUnit,
        ByteAlignment>
{
public:
    static const auto Id = ElementId::slice_segment_data;

    typedef SliceSegmentDataContent::EndOfSliceSegmentFlag EndOfSliceSegmentFlag;
    typedef SliceSegmentDataContent::EndOfSubStreamOneBit EndOfSubStreamOneBit;

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const SliceSegmentHeader &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SliceSegmentData_h */

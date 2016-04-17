#ifndef HEVC_Decoder_State_h
#define HEVC_Decoder_State_h

/* STDC++ */
#include <functional>
/* HEVC */
#include <HEVC.h>
#include <Fwd.h>
#include <Decoder/Context.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>
#include <Structure/CPB.h>
#include <Structure/DPB.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.1 "General decoding process" */
class State
{
    /* TargetDecLayerIdList[0..n] - specifies the list of nuh_layer_id values
     * in increasing order of nuh_layer_id values.
     * 1. TargetDecLayerIdList - set by external means not covered by Specification;
     * 2. TargetDecLayerIdList - set as specified in clause C.1;
     * 3. TargetDecLayerIdList - set equal to 0 (contains only one nuh_layer_id); */

    /* HighestTid - identifies the highest temporal sub-layer to be decoded
     * 1. HighestTid - set by external means not covered by Specification;
     * 2. HighestTid - set as specified in clause C.1;
     * 3. HighestTid - set equal to sps_max_sub_layers_minus1; */

    /* State:
     *
     * NoRaslOutputFlag:
     *
     * a) if current picture is IDR picture: is set equal to 1;
     *
     * b) if current picture is BLA picture: is set equal to 1;
     *
     * c) if current picture is first picture in bitstream in decoding order:
     * is set equal to 1;
     *
     * d) if current picture is first picture that follows and end of sequence
     * NAL unit in decoding order: is set equal to 1;
     *
     * e) otherwise if variable HandleCraAsBlaFlag is set (for current picture)
     * by external means not covered by Specification: NoRaslOutputFlag
     * is set equal to HandleCraAsBlaFlag;
     *
     * f) otherwise, the variable HandleCraAsBlaFlag is set equal to 0 and
     * variable NoRaslOutputFlag is set equal to 0;
     *
     * UseAltCpbParamsFlag:
     *
     * defined if current picture is BLA picture that has nal_unit_type equal to
     * BLA_W_LP or is a CRA picture:
     *
     * a) set by external means not coveres by Specification;
     * b) set equal to 0 otherwise;
     * */


    /* VPS/SPS/PPS/AUD/EOS/EOB/FD/SEI - none-VCL */

    /* 1. set TargetDecLayerIdList
     * 2. set HighestTid
     * 3. sub-bitstream extraction
     *
     */
private:
    Handle<Context> m_context;
    Processes::ProcessIdMask m_processIdMask;
    bool m_SoCVS;

    void onCVS()
    {
        m_SoCVS = true;
        ++cntr.cvs;

        const auto toStr =
            [this](std::ostream &oss) {cntr.toStr(oss);};

        log(LogId::Sequence, toStr);
    }

    void onAU(NalUnitType nut, int tId = -1)
    {
        if(!cpb.empty())
        {
            if(cpb.au()->picture() && !isRASL(picture()->nalUnitType))
            {
                cpb.moveTo(dpb);
            }
            else if(cpb.au()->picture() && isRASL(picture()->nalUnitType))
            {
                cpb.clear();
            }
            else if(!cpb.au()->picture() && -1 == tId)
            {
                cpb.clear();
            }
        }

        if(isIDR(nut) || isBLA(nut) || isCRA(nut))
        {
            onCVS();
        }

        if(cpb.empty())
        {
            cpb.add(makeHandle<Structure::AccessUnit>(*this, tId));
            ++cntr.au;
            log(LogId::All, "# ACCESS UNIT ", cntr.au - 1, '\n');
            log(LogId::Picture, [this](std::ostream &os){cntr.toStr(os), os << '\n';});
        }
        else if(!cpb.au()->picture())
        {
            cpb.au()->temporalId = tId;
            log(LogId::All, "# PICTURE ", cntr.au - 1, '\n');
        }
    }
public:
    typedef std::function<bool (const State &)> Predicate;
    Handle<Syntax::StreamNalUnit> snu;
    Structure::CPB cpb;
    Structure::DPB dpb;

    Ptr<Structure::AccessUnit> currAU() const
    {
        return cpb.au();
    }

    struct Cntr
    {
        /* Stream NAL Unit */
        int64_t snu;
        /* Access Unit */
        int64_t au;
        /* Coded Video Sequence */
        int64_t cvs;

        Cntr(): snu{0}, au{0}, cvs{0}
        {}

        void toStr(std::ostream &os) const
        {
            os
                << "SNU " << snu
                << " CVS " << cvs
                << " AU " << au;
        }
    };

    Cntr cntr;

    /* current min. quantization group status & parameters */
    struct QpStatus
    {
        int qpYpred;
        bool isCuQpDeltaCoded;
        int cuQpDeltaVal;
        bool isCuQpOffsetCoded;
        int cuQpOffsetCb;
        int cuQpOffsetCr;

        QpStatus():
            qpYpred{0},
            isCuQpDeltaCoded{false},
            cuQpDeltaVal{0},
            isCuQpOffsetCoded{false},
            cuQpOffsetCb{0},
            cuQpOffsetCr{0}

        {}
    };

    QpStatus qpStatus;

    State(PictureOutput);
    State(const State &) = delete;
    State(State &&) = delete;
    State &operator= (const State &) = delete;

    /* StreamNAL/NAL/VCL/NoneVCL/AU/CVS/BS */
    void onStart(Syntax::SliceSegmentData &);
    void onStart(Syntax::CodingTreeUnit &);
    void onStart(Syntax::CodingUnit &);
    void onStart(Syntax::ResidualCoding &);

    void onFinish(Syntax::StreamNalUnit &);
    void onFinish(Syntax::NalUnit &);
    void onFinish(Syntax::ShortTermRefPicSet &);
    void onFinish(Syntax::SliceSegmentHeader &);
    void onFinish(Syntax::SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag &);
    void onFinish(Syntax::SliceSegmentHeaderContent::SlicePicParameterSetId &);
    void onFinish(Syntax::SliceSegmentHeaderContent::PicOutputFlag &);
    void onFinish(Syntax::CodingTreeUnit &);
    void onFinish(Syntax::CodingUnit &);
    void onFinish(Syntax::PredictionUnit &);
    void onFinish(Syntax::TransformUnit &);
    void onFinish(Syntax::ResidualCoding &);
    void exec(std::istream &, Predicate);

    Ptr<Context> context() const
    {
        return Ptr<Context>{m_context};
    }

    Ptr<Structure::Picture> picture() const
    {
        return currAU()->picture();
    }

    /* 04/2013, C.5.2.2 "Output and removal of pictures from the DPB" */
    void updateDPB(const Syntax::SliceSegmentHeader &);

    bool isMasked(Processes::ProcessId id) const
    {
        return Processes::isMasked(m_processIdMask ,id);
    }
};
/*----------------------------------------------------------------------------*/
/* Functions from 8.3.1 */
inline
PicOrderCntVal diff(PicOrderCntVal x, PicOrderCntVal y)
{
    const auto d = x - y;

    /* TODO: add semantic check */
    bdryCheck(
            d >= PicOrderCntVal{PicOrderCntVal::ValueType(-(1 << 15))}
            && d <= PicOrderCntVal{PicOrderCntVal::ValueType((1 << 15) - 1)});
    return d;
}
/*----------------------------------------------------------------------------*/
inline
PicOrderCntVal diffPicOrderCnt(const Structure::Picture &x, const Structure::Picture &y)
{
    return diff(x.order.get<PicOrderCntVal>(), y.order.get<PicOrderCntVal>());
}
/*----------------------------------------------------------------------------*/
/* from 8.5.3.2 */
inline
bool longTermRefPic(
        const State &,
        const Structure::Picture &aPic,
        PelCoord aPbCoord,
        RefList l, PicOrderCntVal refIdx)
{
    const auto cnt = aPic.slice(aPbCoord)->rpl[l][refIdx];

    for(auto i = 0; i < aPic.rps()->numLtCurr; ++i)
    {
        if(*aPic.rps()->ltCurr[i] == cnt)
        {
            return true;
        }
    }

    for(auto i = 0; i < aPic.rps()->numLtFoll; ++i)
    {
        if(*aPic.rps()->ltFoll[i] == cnt)
        {
            return true;
        }
    }

    return false;
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_State_h */

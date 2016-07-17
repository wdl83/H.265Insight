#ifndef HEVC_Syntax_CrossCompPred_h
#define HEVC_Syntax_CrossCompPred_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace CrossCompPredContent {
/*----------------------------------------------------------------------------*/
struct ResScaleAbsPlus1:
    public Embedded,
    public CABAD::TruncatedRice
{
    static const auto Id = ElementId::log2_res_scale_abs_plus1;

    ResScaleAbsPlus1():
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
        CABAD::TruncatedRice{0, 4}
    {
        setValue(0);
    }

    Log2 inUnits() const
    {
        return Log2(getValue());
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, Chroma chroma)
    {
        const auto contextModel =
            [chroma](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx || 1 == binIdx || 2 == binIdx || 3 == binIdx);
                return
                    state.getVariable(
                            CABAD::CtxId::log2_res_scale_abs_plus1,
                            4 * int(chroma) + binIdx);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};

struct ResScaleSignFlag:
    public Embedded,
    public CABAD::FixedLength
{
    static const auto Id = ElementId::res_scale_sign_flag;

    ResScaleSignFlag():
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
        CABAD::FixedLength{1}
    {}

    explicit
    operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, Chroma chroma)
    {
        const auto contextModel =
            [chroma](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::res_scale_sign_flag, int(chroma));
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
} /* CrossCompPredContent */

/*----------------------------------------------------------------------------*/
struct CrossCompPred:
    public EmbeddedAggregator<
    CrossCompPredContent::ResScaleAbsPlus1,
    CrossCompPredContent::ResScaleSignFlag>
{
    static const auto Id = ElementId::cross_comp_pred;

    typedef CrossCompPredContent::ResScaleAbsPlus1 ResScaleAbsPlus1;
    typedef CrossCompPredContent::ResScaleSignFlag ResScaleSignFlag;

    int resScaleVal() const
    {
        const auto scaleAbsPlus1 = get<ResScaleAbsPlus1>()->inUnits();

        if(0_log2 == scaleAbsPlus1)
        {
            return 0;
        }
        else
        {
            const auto absVal = toInt(scaleAbsPlus1 - 1_log2);
            return *get<ResScaleSignFlag>() ? -absVal : absVal;
        }
    }

    void onParse(StreamAccessLayer &, Decoder::State &, Chroma);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_CrossCompPred_h */

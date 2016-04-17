#ifndef HEVC_Syntax_ScalingListData_h
#define HEVC_Syntax_ScalingListData_h

/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace ScalingListDataContent {
/*----------------------------------------------------------------------------*/
template <typename T>
using List =
    std::array<
        std::array<T, EnumRange<MatrixId>::length()>,
        EnumRange<SizeId>::length()>;

typedef Tuple<SizeId, MatrixId> ListIndex;

template <typename T>
inline
T get(const List<T> &list, ListIndex i)
{
    return list[int(i.get<SizeId>())][int(i.get<MatrixId>())];
}

template <typename T>
inline
void set(List<T> &list, ListIndex i, T value)
{
    list[int(i.get<SizeId>())][int(i.get<MatrixId>())] = value;
}
/*----------------------------------------------------------------------------*/
class ScalingListPredModeFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
private:
    List<bool> m_predModeFlag;
public:
    static const auto Id = ElementId::scaling_list_pred_mode_flag;

    bool operator[] (ListIndex i) const
    {
        return get(m_predModeFlag, i);
    }

    explicit operator bool () const
    {
        return 0 != getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ListIndex i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        set(m_predModeFlag, i, bool(getValue()));
    }
};
/*----------------------------------------------------------------------------*/
class ScalingListPredMatrixIdDelta:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
private:
    List<int> m_predMatrixIdDelta;
public:
    static const auto Id = ElementId::scaling_list_pred_matrix_id_delta;

    operator int () const
    {
        return getValue();
    }

    int operator[] (const ListIndex i) const
    {
        return get(m_predMatrixIdDelta, i);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ListIndex i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        set(m_predMatrixIdDelta, i, int(getValue()));
    }
};
/*----------------------------------------------------------------------------*/
class ScalingListDcCoefMinus8:
    public Embedded,
    public VLD::IntExpGolombCoded
{
private:
    List<int> m_dcCoefMinus8;
public:
    static const auto Id = ElementId::scaling_list_dc_coef_minus8;
    static const auto InferredValue = 8;

    void infer(ListIndex i)
    {
        set(m_dcCoefMinus8, i, ValueType(InferredValue));
    }

    void infer(ListIndex dst, ListIndex src)
    {
        set(m_dcCoefMinus8, dst, get(m_dcCoefMinus8, src));
    }

    operator int () const
    {
        return getValue();
    }

    int operator[] (ListIndex i) const
    {
        return get(m_dcCoefMinus8, i);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ListIndex i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        set(m_dcCoefMinus8, i, getValue());
    }
};
/*----------------------------------------------------------------------------*/
class ScalingListDeltaCoef:
    public Embedded,
    public VLD::IntExpGolombCoded
{
private:
    List<std::array<int, 64>> m_deltaCoef;
public:
    static const auto Id = ElementId::scaling_list_delta_coef;

    ScalingListDeltaCoef()
    {
        fill(m_deltaCoef, 0);
    }

    operator int () const
    {
        return getValue();
    }

    int operator[] (Tuple<SizeId, MatrixId, int> i) const
    {
        return m_deltaCoef[int(i.get<SizeId>())][int(i.get<MatrixId>())][i.get<int>()];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Tuple<SizeId, MatrixId, int> i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_deltaCoef[int(i.get<SizeId>())][int(i.get<MatrixId>())][i.get<int>()] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace ScalingListDataContent */

class ScalingListData:
    public EmbeddedAggregator<
        ScalingListDataContent::ScalingListPredModeFlag,
        ScalingListDataContent::ScalingListPredMatrixIdDelta,
        ScalingListDataContent::ScalingListDcCoefMinus8,
        ScalingListDataContent::ScalingListDeltaCoef>
{
public:
    static const auto Id = ElementId::scaling_list_data;

    typedef ScalingListDataContent::ScalingListPredModeFlag ScalingListPredModeFlag;
    typedef ScalingListDataContent::ScalingListPredMatrixIdDelta ScalingListPredMatrixIdDelta;
    typedef ScalingListDataContent::ScalingListDcCoefMinus8 ScalingListDcCoefMinus8;
    typedef ScalingListDataContent::ScalingListDeltaCoef ScalingListDeltaCoef;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_ScalingListData_h */

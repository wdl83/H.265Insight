#ifndef HEVC_Structure_ScalingList_h
#define HEVC_Structure_ScalingList_h

/* STDC++ */
#include <array>
#include <string>
/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class ScalingList
{
public:
    typedef VLA<int> Coeff;
    typedef
        std::array<
            std::array<Coeff, EnumRange<MatrixId>::length()>,
            EnumRange<SizeId>::length()> CoeffList;
private:
    CoeffList m_coeffList;
public:
    ScalingList()
    {
        derive(nullptr);
    }

    constexpr
    static
    int getMaxSideLength()
    {
        /* 04/2013, 7.4.5, "Scaling list data semantics",
         * i = 0..Min(63, (1 << (4 + (sizeId << 1))) - 1) */
        return 8;
    }

    constexpr
    static
    bool isSupported(SizeId sizeId, MatrixId matrixId)
    {
        return !(MatrixId::Id1 < matrixId && SizeId::Id3 == sizeId);
    }

    static
    const Coeff &defaultCoeff(SizeId, MatrixId);

    void derive(Ptr<const Syntax::ScalingListData> sld)
    {
        if(!sld)
        {
            for(const auto sizeId : EnumRange<SizeId>())
            {
                for(const auto matrixId : EnumRange<MatrixId>())
                {
                    (*this)[makeTuple(sizeId, matrixId)] =
                        defaultCoeff(sizeId, matrixId);
                }
            }
        }
        else
        {
            derive(*sld);
        }
    }

    void derive(const Syntax::ScalingListData &);

    const Coeff &operator[] (Tuple<SizeId, MatrixId> i) const
    {
        return m_coeffList[int(i.get<SizeId>())][int(i.get<MatrixId>())];
    }

    Coeff &operator[] (Tuple<SizeId, MatrixId> i)
    {
        return m_coeffList[int(i.get<SizeId>())][int(i.get<MatrixId>())];
    }

    Coeff::ValueType operator[] (Tuple<SizeId, MatrixId, int> i) const
    {
        return m_coeffList[int(i.get<SizeId>())][int(i.get<MatrixId>())][i.get<int>()];
    }

    Coeff::ValueType &operator[] (Tuple<SizeId, MatrixId, int> i)
    {
        return m_coeffList[int(i.get<SizeId>())][int(i.get<MatrixId>())][i.get<int>()];
    }

    std::string toStr() const;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_ScalingList_h */

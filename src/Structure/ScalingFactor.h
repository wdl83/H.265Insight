#ifndef HEVC_Structure_ScalingFactor_h
#define HEVC_Structure_ScalingFactor_h

/* STDC++ */
#include <array>
#include <string>
/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class ScalingFactor
{
public:
    typedef VLM<int> Factor;

    typedef std::array<
        std::array<Factor, EnumRange<MatrixId>::length()>,
        EnumRange<SizeId>::length()> FactorList;
private:
    FactorList m_factorList;

    Factor &ref(Tuple<SizeId, MatrixId> i)
    {
        return m_factorList[int(i.get<SizeId>())][int(i.get<MatrixId>())];
    }
public:
    ScalingFactor(const ScalingList &sl, Ptr<const Syntax::ScalingListData> sld)
    {
        derive(sl, sld);
    }

    void derive(const ScalingList &, Ptr<const Syntax::ScalingListData>);

    const Factor &operator[] (Tuple<SizeId, MatrixId> i) const
    {
        return m_factorList[int(i.get<SizeId>())][int(i.get<MatrixId>())];
    }

    std::string toStr() const;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_ScalingFactor_h */

#ifndef HEVC_Syntax_Descriptor_h
#define HEVC_Syntax_Descriptor_h
/* STDC++ */
#include <string>
#include <utility>
/* HEVC */
#include <Fwd.h>
#include <Syntax/CABAD/Fwd.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
class VariableBitLength
{
private:
    int m_lengthInBits;
public:
    VariableBitLength(): m_lengthInBits(0)
    {}

    VariableBitLength(int lengthInBits): m_lengthInBits(lengthInBits)
    {}

    void setLengthInBits(int lengthInBits)
    {
        m_lengthInBits = lengthInBits;
    }

    int getLengthInBits() const
    {
        return m_lengthInBits;
    }
};

enum class DescriptorId
{
    ContextAdaptive = 0,
    SignedExpGolombCoded,
    UnsignedExpGolombCoded,
    FixedPattern,
    FixedSigned,
    FixedUnsigned,
    Byte,
    Signed,
    Unsigned,
    Num
};

inline
const std::string &getName(DescriptorId id)
{
    static const std::string name[] =
    {
        {"ae"}, {"se"}, {"ue"}, {"f"}, {"i"}, {"u"}, {"b"}, {"i"}, {"u"}
    };

    return name[int(id)];
}
/*----------------------------------------------------------------------------*/
class Descriptor
{};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_Descriptor_h */

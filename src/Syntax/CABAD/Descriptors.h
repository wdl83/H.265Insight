#ifndef HEVC_Syntax_CABAD_Descriptors_h
#define HEVC_Syntax_CABAD_Descriptors_h
/* HEVC */
#include <Syntax/Descriptor.h>
#include <Syntax/CABAD/State.h>
#include <Syntax/CABAD/Debinarizer.h>
#include <Decoder/Fwd.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
struct TagCABAD
{};

/* ae(v) */
template <typename T>
class ContextAdaptive:
    public TagCABAD,
    public Descriptor,
    public StoreByValue<BinValueType>,
    public T
{
public:
    static const auto descriptorId = DescriptorId::ContextAdaptive;

    typedef T DebinarizerType;

    using DebinarizerType::getBinarization;
    using DebinarizerType::getBinarizationValue;

    template <typename ...T_s>
    ContextAdaptive(const T_s &... t_s):
        StoreByValue(BinValueType(0)),
        DebinarizerType(t_s...)
    {}

    ContextAdaptive(const ContextAdaptive &) = delete;

    template <typename ...Args>
    void onGetFrom(StreamAccessLayer &streamAccessLayer, State &state, Args &... args)
    {
        setValue(getBinarizationValue(streamAccessLayer, state, args...));
    }
};

typedef ContextAdaptive<FixedLengthDebinarizer> FixedLength;
typedef ContextAdaptive<TruncatedRiceDebinarizer> TruncatedRice;
typedef ContextAdaptive<ExpGolombDebinarizer> ExpGolomb;


/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_Descriptors_h */

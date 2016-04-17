#ifndef HEVC_Decoder_Context_h
#define HEVC_Decoder_Context_h

/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>
#include <Syntax/StreamNalUnit.h>
#include <Structure/ScalingList.h>
#include <Structure/ScalingFactor.h>
#include <Structure/RPSP.h>
/* STDC++ */
#include <array>

namespace HEVC { namespace Decoder {

namespace ContextImpl {

template <typename T>
struct MaxNum;

template <>
struct MaxNum<Syntax::VideoParameterSet>
{
    static const int value = 16;
};

template <>
struct MaxNum<Syntax::SequenceParameterSet>
{
    static const int value = 16;
};

template <>
struct MaxNum<Syntax::PictureParameterSet>
{
    static const int value = 64;
};

} /* ContextImpl */

/*----------------------------------------------------------------------------*/
class Context
{
private:
    /* 04/2013, 7.4.3.3 "Picture parameter set RBSP semantics"
     * vps_video_parameter_set_id [0..15]
     * sps_seq_parameter_set_id [0..15]
     * pps_pic_parameter_set_id [0..63] */

    typedef Syntax::StreamNalUnit SNU;
    typedef Syntax::NalUnit NU;
    typedef Syntax::VideoParameterSet VPS;
    typedef Syntax::SequenceParameterSet SPS;
    typedef Syntax::PictureParameterSet PPS;

    template <typename T, size_t n>
    using List = std::array<Tuple<Handle<SNU>, Ptr<T>>, n>;

    template <typename T>
    struct ListType
    {
        typedef List<T, ContextImpl::MaxNum<T>::value> Type;
    };

    template <typename T>
    using ToList = typename ListType<T>::Type;

    Tuple<
        ToList<VPS>,
        ToList<SPS>,
        ToList<PPS>,
        Handle<Structure::ScalingList>,
        Handle<Structure::ScalingFactor>,
        Handle<Structure::RPSP>> m_set;

    template <typename T>
    Handle<T> &ref()
    {
        return m_set.template get<Handle<T>>();
    }
public:
    Context()
    {
        using namespace Structure;

        // construct default ScalingList & ScalingFactor
        ref<ScalingList>().construct();
        ref<ScalingFactor>().construct(*get<ScalingList>(), nullptr);
        ref<RPSP>().construct();
    }

    template <typename T>
    Ptr<T> get() const
    {
        return Ptr<T>{m_set.template get<Handle<T>>()};
    }

    template <typename T>
    Ptr<const T> get(int id) const
    {
        return m_set.template get<ToList<T>>()[id].template get<Ptr<T>>();
    }

    template <typename T>
    bool valid(int id) const
    {
        return nullptr != get<T>(id);
    }

    template <typename T>
    void add(Handle<Syntax::StreamNalUnit> &&h, Ptr<T> ptr, int id)
    {
        if(valid<T>(id))
        {
            const auto &snu = m_set.template get<ToList<T>>()[id].template get<Handle<SNU>>();

            if(
                    *snu->template getSubtree<NU>()->template get<NU::RbspByte>()
                    != *h->template getSubtree<NU>()->template get<NU::RbspByte>())
            {
                destruct(m_set.template get<ToList<T>>()[id].template get<Handle<SNU>>());
                m_set.template get<ToList<T>>()[id] = makeTuple(std::move(h), ptr);
            }
            else
            {
                destruct(h);
            }
        }
        else
        {
            m_set.template get<ToList<T>>()[id] = makeTuple(std::move(h), ptr);
        }
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Context_h */

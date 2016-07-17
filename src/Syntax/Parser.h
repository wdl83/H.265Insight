#ifndef HEVC_Syntax_Parser_h
#define HEVC_Syntax_Parser_h

/* STDC++ */
#include <cstddef>
#include <chrono>
#include <type_traits>
#include <utility>
/* HEVC */
#include <Fwd.h>
#include <Syntax/Fwd.h>
#include <Syntax/CABAD/Debinarizer.h>
#include <Decoder/Fwd.h>
#include <Decoder/ParserHooks.h>
#include <log.h>
#include <Tuple.h>

namespace HEVC { namespace Syntax {

CABAD::State &cabadState(Decoder::State &);

class ParserStats
{
    Decoder::State &m_decoder;
    ElementId m_id;
    std::chrono::high_resolution_clock::time_point m_begin;
public:
    ParserStats(Decoder::State &decoder,  ElementId id):
        m_decoder{decoder},
        m_id{id},
        m_begin{std::chrono::high_resolution_clock::now()}
    {}

    ~ParserStats();
};
/*----------------------------------------------------------------------------*/
template <typename S, typename ...A_n>
inline
void getFrom(
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &,
        S &s,
        A_n &&... a_n)
{
    s.onGetFrom(streamAccessLayer, std::forward<A_n>(a_n)...);


    log(LogId::Syntax, align(getName(S::descriptorId)), '\t', s.getValue());

    if(isInHeaderData(S::Id))
    {
        log(LogId::Headers, s.getValue());
    }
}

template <typename S, typename ...A_n>
inline
void getFrom(
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &,
        CABAD::State &state,
        S &s,
        A_n &&... a_n)
{
    log(LogId::ArithmeticDecoderState, "# ", getName(S::Id), '\n');
    s.onGetFrom(streamAccessLayer, state, std::forward<A_n>(a_n)...);
    log(LogId::Syntax, " b", s.getBinString(), '\t', s.getValue());
}

namespace ParserImpl {
/*----------------------------------------------------------------------------*/
template <typename S>
std::true_type isDefined_onParse(decltype(&S::onParse));
template <typename S>
std::false_type isDefined_onParse(...);

template <typename S, typename ...A_n>
void callIf_onParse(
        std::true_type,
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        S &s,
        S &,
        A_n &&... a_n)
{
    s.onParse(streamAccessLayer, decoder, std::forward<A_n>(a_n)...);
}

template <typename S, typename ...A_n>
void callIf_onParse(
        std::false_type,
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        S &s,
        CABAD::TagCABAD &,
        A_n &&... a_n)
{
    getFrom(streamAccessLayer, decoder, cabadState(decoder), s, std::forward<A_n>(a_n)...);
}

template <typename S, typename ...A_n>
void callIf_onParse(
        std::false_type,
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        S &s,
        VLD::TagVLD &,
        A_n &&... a_n)
{
    getFrom(streamAccessLayer, decoder, s, std::forward<A_n>(a_n)...);
}

template <typename S>
std::true_type isDefined_Coord(typename S::Coord *);
template <typename S>
std::false_type isDefined_Coord(...);

template <typename S>
inline
std::string toStrCoord(std::false_type, const S &)
{
    return std::string{};
}

template <typename S>
inline
std::string toStrCoord(std::true_type, const S &s)
{
    std::ostringstream oss;

    oss << s.template get<typename S::Coord>()->inUnits();
    return oss.str();
}

template <typename S>
std::true_type isDefined_Size(typename S::Size *);
template <typename S>
std::false_type isDefined_Size(...);

template <typename S>
inline
std::string toStrSize(std::false_type, const S &)
{
    return std::string{};
}

template <typename S>
inline
std::string toStrSize(std::true_type, const S &s)
{
    std::ostringstream oss;

    oss << " s" << s.template get<typename S::Size>()->inUnits();
    return oss.str();
}

template <typename S>
std::true_type isDefined_CIdx(typename S::CIdx *);
template <typename S>
std::false_type isDefined_CIdx(...);

template <typename S>
inline
std::string toStrCIdx(std::false_type, const S &)
{
    return std::string{};
}

template <typename S>
inline
std::string toStrCIdx(std::true_type, const S &s)
{
    std::ostringstream oss;

    oss << ' ' << getName(Plane(*s.template get<typename S::CIdx>()));
    return oss.str();
}

template <typename S>
std::true_type isDefined_Depth(typename S::Depth *);
template <typename S>
std::false_type isDefined_Depth(...);

template <typename S>
inline
std::string toStrDepth(std::false_type, const S &)
{
    return std::string{};
}

template <typename S>
inline
std::string toStrDepth(std::true_type, const S &s)
{
    std::ostringstream oss;

    oss << " d" << int(s.template get<typename S::Depth>()->inUnits());
    return oss.str();
}

template <typename S>
std::true_type isDefined_BlkIdx(typename S::BlkIdx *);
template <typename S>
std::false_type isDefined_BlkIdx(...);

template <typename S>
inline
std::string toStrBlkIdx(std::false_type, const S &)
{
    return std::string{};
}

template <typename S>
inline
std::string toStrBlkIdx(std::true_type, const S &s)
{
    std::ostringstream oss;

    oss << " b" << int(s.template get<typename S::BlkIdx>()->inUnits());
    return oss.str();
}

template <typename S>
void onParseStart(Decoder::State &, S &, const Embedded &)
{
    log(LogId::Syntax, align(getName(S::Id)));

    if(isInHeaderData(S::Id))
    {
        log(LogId::Headers, getName(S::Id), ' ');
    }
}

template <typename S>
void onParseFinish(Decoder::State &, S &, const Embedded &)
{
    log(LogId::Syntax, "\n");

    if (isInHeaderData(S::Id))
    {
        log(LogId::Headers, "\n");
    }
}

template <typename S>
void onParseStart(Decoder::State &, S &s, const EmbeddedAggregatorTag &)
{
    log(
            LogId::Syntax,
            align<'_'>(
                    getName(S::Id),
                    toStrCoord(decltype(isDefined_Coord<S>(nullptr))(), s),
                    toStrSize(decltype(isDefined_Size<S>(nullptr))(), s),
                    toStrDepth(decltype(isDefined_Depth<S>(nullptr))(), s),
                    toStrBlkIdx(decltype(isDefined_BlkIdx<S>(nullptr))(), s),
                    toStrCIdx(decltype(isDefined_CIdx<S>(nullptr))(), s)),
            "{\n");
}

template <typename S>
void onParseFinish(Decoder::State &, S &, const EmbeddedAggregatorTag &)
{
    log(LogId::Syntax, align<'_'>(getName(S::Id)), "}\n");
}
/*----------------------------------------------------------------------------*/
} /* ParseImpl */

/* external interface */
template <typename S, typename ...A_n>
inline
void parse(
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        S &s,
        A_n &&... a_n)
{

    Decoder::ParserHooks::onStart(s, decoder);

    {
        ParserStats collector{decoder, S::Id};

        ParserImpl::onParseStart(decoder, s, s);

        ParserImpl::callIf_onParse(
                decltype(ParserImpl::isDefined_onParse<S>(nullptr))(),
                streamAccessLayer,
                decoder,
                s,
                s,
                std::forward<A_n>(a_n)...);

        ParserImpl::onParseFinish(decoder, s, s);
    }

    Decoder::ParserHooks::onFinish(s, decoder);
}

/* only for recursive parsing */
template <typename S, typename ...A_n>
inline
void subparse(
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        S &s,
        A_n &&... a_n)
{
    Decoder::ParserHooks::onStart(s, decoder);

    ParserImpl::onParseStart(decoder, s, s);

    ParserImpl::callIf_onParse(
            decltype(ParserImpl::isDefined_onParse<S>(nullptr))(),
            streamAccessLayer,
            decoder,
            s,
            s,
            std::forward<A_n>(a_n)...);

    ParserImpl::onParseFinish(decoder, s, s);

    Decoder::ParserHooks::onFinish(s, decoder);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_Parser_h */

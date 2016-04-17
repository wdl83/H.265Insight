#ifndef HEVC_Decoder_Processes_QuantizationParameters_h
#define HEVC_Decoder_Processes_QuantizationParameters_h

/* STDC++ */
/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.6.1, "Derivation process for quantization parameters" */

struct QuantizationParameters
{
    static const auto id = ProcessId::QuantizationParameters;

    int exec(
            State &,
            Ptr<const Structure::Picture>,
            PelCoord);
};

struct QuantizationParametersLuma
{
    static const auto id = ProcessId::QuantizationParametersLuma;

    std::tuple<int, int> exec(
            State &,
            Ptr<const Structure::Picture>);
};

struct QuantizationParametersChroma
{
    static const auto id = ProcessId::QuantizationParametersChroma;

    std::tuple<int, int> exec(
            State &,
            Ptr<const Structure::Picture>,
            PelCoord,
            int, int, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_QuantizationParameters_h */

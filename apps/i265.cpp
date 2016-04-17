/* STDC++ */
#include <iostream>
#include <fstream>
/* POSIX C */
#include <unistd.h>
/* HEVC */
#include <Decoder/State.h>
#include <TerminationTools.h>
#include <log.h>
/*----------------------------------------------------------------------------*/
void enableLogs(const std::string &logs)
{
    auto begin = logs.begin();
    const auto end = logs.end();

    while(begin != end)
    {
        const auto i = std::find(begin, end, ',');
        const std::string name(begin, i);

        if (name == HEVC::allLogsIndicator)
        {
            std::cout << "Enabling all logs..." << std::endl << std::endl;
            HEVC::enableLog(name);
            break;
        }

        if(!HEVC::enableLog(name))
        {
            std::cerr << "log:" << name << " not supported" << std::endl << std::endl;
        }

        begin = i;

        if(i != end)
        {
            runtime_assert(',' == *begin);
            /* skip comma */
            ++begin;
        }
    }
}
/*----------------------------------------------------------------------------*/
void help()
{
    std::cout
        << "i256 -i bitstream [-o mode] [-O mode] [-n num] [-l l0,l1,l3,...]\n"
        << "\t-i bitstream, ITU-T H.265 v2 (10/2014) Annex B compatible bitstream file\n"
        << "\t-o mode, decoding order YUV storage method, 0 - disabled (default), 1 - continuous, 2 - discrete\n"
        << "\t-O mode, display order YUV storage method, 0 - disabled (default), 1 - continuous, 2 - discrete\n"
        << "\t-n num, number of Access Units to process\n"
        << "\t-l l0,l1,l2,... , comma separated list of analysis to output\n"
        << "\nSupported analysis outputs:\n\tall (includes all listed below)\n";

    for(const auto l : EnumRange<HEVC::LogId>())
    {
        std::cout << '\t' << HEVC::toStr(l) << '\n';
    }
}
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    using namespace HEVC;

    registerTerminationTools();

    std::string inputName;
    int outputMode = 0;
    std::string logs;
    int n = 0;

    char c;

    while(-1 != (c = ::getopt(argc, argv, "hi:O:o:n:l:")))
    {
        switch(c)
        {
            case 'h':
                help();
                return EXIT_SUCCESS;
                break;
            case 'i':
                 inputName = optarg;
                break;
            case 'o':
                outputMode = std::atoi(optarg);
                break;
            case 'n':
                n = std::atoi(optarg);
                break;
            case 'l':
                logs = optarg;
                break;
            case ':':
            case '?':
            default:
                return EXIT_FAILURE;
                break;
        }
    }

    if(
            inputName.empty()
            || !(int(PictureOutput::Begin) <= outputMode && int(PictureOutput::End) > outputMode)
            || 0 > n)
    {
        help();
        return EXIT_FAILURE;
    }

    enableLogs(logs);

    std::ifstream bitstream(inputName.c_str(), std::ios_base::binary | std::ios_base::in);

    Decoder::State decoder{PictureOutput(outputMode)};

    decoder.exec(bitstream, [n](const Decoder::State &d) {return !n || int64_t{n} >= d.cntr.au;});
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
/*----------------------------------------------------------------------------*/

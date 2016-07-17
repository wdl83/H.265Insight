#include <Structure/PictureProperties.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/PpsRangeExtension.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
/* PictureProperties */
/*----------------------------------------------------------------------------*/
PictureProperties::PictureProperties(
        Ptr<const VPS> vps_,
        Ptr<const SPS> sps_,
        Ptr<const PPS> pps_,
        NalUnitType nalUnitType_,
        bool noRaslOutputFlag_):
    picOutputFlag(true),
    nalUnitType(nalUnitType_),
    noRaslOutputFlag(noRaslOutputFlag_),
    vps(vps_),
    sps(sps_),
    spsre
    {
        *sps->get<SPS::SpsRangeExtensionFlag>()
        ? sps->getSubtree<SPSRE>()
        : nullptr
    },
    pps(pps_),
    ppsre
    {
        *pps->get<PPS::PpsRangeExtensionFlag>()
         ? pps->getSubtree<PPSRE>()
         : nullptr
    },
    chromaFormatIdc{*sps->get<SPS::ChromaFormatIdc>()},
    separateColourPlaneFlag{bool(*sps->get<SPS::SeparateColourPlaneFlag>())},
    bitDepthY{sps->get<SPS::BitDepthLumaMinus8>()->inUnits() + 8},
    bitDepthC{sps->get<SPS::BitDepthChromaMinus8>()->inUnits() + 8},
    pcmBitDepthY
    {
        *sps->get<SPS::PcmEnabledFlag>()
        ? sps->get<SPS::PcmSampleBitDepthLumaMinus1>()->inUnits() + 1
        : 0
    },
    pcmBitDepthC
    {
        *sps->get<SPS::PcmEnabledFlag>()
        ? sps->get<SPS::PcmSampleBitDepthChromaMinus1>()->inUnits() + 1
        : 0
    },
    minCbSizeY{sps->get<SPS::MinLumaCodingBlockSizeMinus3>()->inUnits() + 3_log2},
    ctbSizeY{minCbSizeY + sps->get<SPS::DiffMaxMinLumaCodingBlockSize>()->inUnits()},
    minTrafoSize{sps->get<SPS::MinTransformBlockSizeMinus2>()->inUnits() + 2_log2},
    maxTrafoSize{minTrafoSize + sps->get<SPS::DiffMaxMinTransformBlockSize>()->inUnits()},
    maxTransformSkipSize
    {
        2_log2
        + (
                ppsre
                && *pps->get<PPS::TransformSkipEnabledFlag>()
                ? ppsre->get<PPSRE::MaxTransformSkipBlockSizeMinus2>()->inUnits()
                : 0_log2)
    },
    widthInLumaSamples{sps->get<SPS::PicWidthInLumaSamples>()->inUnits()},
    heightInLumaSamples{sps->get<SPS::PicHeightInLumaSamples>()->inUnits()},
    widthInMinCbsY{MinCb(roundUp(widthInLumaSamples, HEVC::toPel(minCbSizeY)))},
    heightInMinCbsY{MinCb(roundUp(heightInLumaSamples, HEVC::toPel(minCbSizeY)))},
    sizeInMinCbsY{widthInMinCbsY * heightInMinCbsY},
    widthInCtbsY{Ctb(roundUp(widthInLumaSamples, HEVC::toPel(ctbSizeY)))},
    heightInCtbsY{Ctb(roundUp(heightInLumaSamples, HEVC::toPel(ctbSizeY)))},
    sizeInCtbsY{widthInCtbsY * heightInCtbsY},
    minIpcmCbSizeY
    {
        *sps->get<SPS::PcmEnabledFlag>()
         ? sps->get<SPS::MinPcmLumaCodingBlockSizeMinus3>()->inUnits() + 3_log2
         : 0_log2
    },
    maxIpcmCbSizeY
    {
        *sps->get<SPS::PcmEnabledFlag>()
        ? minIpcmCbSizeY + sps->get<SPS::DiffMaxMinPcmLumaCodingBlockSize>()->inUnits()
        : 0_log2
    }
{
    m_qpBdOffsetY = 6 * (bitDepthY - 8);
    m_qpBdOffsetC = 6 * (bitDepthC - 8);
    maxPicOrderCntLsb.value = toInt(sps->get<SPS::MaxPicOrderCntLsbMinus4>()->inUnits() + 4_log2);

    /* PPS */
    m_minCuQpDeltaSize = ctbSizeY - pps->get<PPS::DiffCuQpDeltaDepth>()->inUnits();
    m_minCuChromaQpOffsetSize =
        ctbSizeY -
        (
         ppsre && *ppsre->get<PPSRE::ChromaQpOffsetListEnabledFlag>()
         ? ppsre->get<PPSRE::DiffCuChromaQpOffsetDepth>()->inUnits()
         : ctbSizeY);

    m_uniformSpacingFlag = bool(*pps->get<PPS::UniformSpacingFlag>());
    m_numTileColumns = pps->get<PPS::NumTileColumnsMinus1>()->inUnits() + 1;
    m_numTileRows = pps->get<PPS::NumTileRowsMinus1>()->inUnits() + 1;
    m_cbQpOffset = pps->get<PPS::PpsCbQpOffset>()->inUnits();
    m_crQpOffset = pps->get<PPS::PpsCrQpOffset>()->inUnits();
    /*------------------------------------------------------------------------*/
    /* colWidth & rowHeight */

    m_colWidth.resize(m_numTileColumns);

    if(m_uniformSpacingFlag || 1 == m_numTileColumns)
    {
        for(auto i = 0; i < m_numTileColumns; ++i)
        {
            m_colWidth[i] =
                (widthInCtbsY * (i + 1)) / m_numTileColumns
                - (widthInCtbsY * i) / m_numTileColumns;
        }
    }
    else
    {
        const auto columnWidthMinus1 = pps->get<PPS::ColumnWidthMinus1>();

        m_colWidth[m_numTileColumns - 1] = widthInCtbsY;

        for(auto i = 0; i < m_numTileColumns - 1; ++i)
        {
            m_colWidth[i] = (*columnWidthMinus1)[i] + 1_ctb;
            m_colWidth[m_numTileColumns - 1] -= m_colWidth[i];
        }
    }

    m_rowHeight.resize(m_numTileRows);

    if(m_uniformSpacingFlag || 1 == m_numTileRows)
    {
        for(auto j = 0; j < m_numTileRows; ++j)
        {
            m_rowHeight[j] =
                (heightInCtbsY * (j + 1)) / m_numTileRows
                - (heightInCtbsY * j) / m_numTileRows;
        }
    }
    else
    {
        const auto &rowHeightMinus1 = pps->get<PPS::RowHeightMinus1>();

        m_rowHeight[m_numTileRows - 1] = heightInCtbsY;

        for(auto j = 0; j < m_numTileRows - 1; ++j)
        {
            m_rowHeight[j] = (*rowHeightMinus1)[j] + 1_ctb;
            m_rowHeight[m_numTileRows - 1] -= m_rowHeight[j];
        }
    }
    /*------------------------------------------------------------------------*/
    /* colBd & rowBd */

    /* Draft 10v19, 6.5.1
     * "Coding tree block raster and tile scanning coversion process" (6-5) */
    m_colBd.resize(m_numTileColumns + 1);

    for(auto i = 1; i < m_numTileColumns + 1; ++i)
    {
        m_colBd[i] = m_colBd[i - 1] + m_colWidth[i - 1];
    }

    m_rowBd.resize(m_numTileRows + 1);

    for(auto j = 1; j < m_numTileRows + 1; ++j)
    {
        m_rowBd[j] = m_rowBd[j - 1] + m_rowHeight[j - 1];
    }

    /*------------------------------------------------------------------------*/
    /* ctbAddrRsToTs & ctbAddrTsToRs */

    const auto picSizeInCtbsY = sizeInCtbsY;

    m_ctbAddrRsToTs.resize(toUnderlying(picSizeInCtbsY));
    m_ctbAddrTsToRs.resize(toUnderlying(picSizeInCtbsY));

    for(auto ctbAddrRs = 0_ctb; ctbAddrRs < picSizeInCtbsY; ++ctbAddrRs)
    {
        const auto xTb = ctbAddrRs % widthInCtbsY;
        const auto yTb = ctbAddrRs / widthInCtbsY;

        /* tile column and row indices */
        auto xTile = 0;
        auto yTile = 0;

        for(auto i = 0; i < m_numTileColumns; ++i)
        {
            if(xTb >= m_colBd[i])
            {
                xTile = i;
            }
        }

        for(auto j = 0; j < m_numTileRows; ++j)
        {
            if(yTb >= m_rowBd[j])
            {
                yTile = j;
            }
        }

        auto ctbAddrTs = 0_ctb;

        for(auto i = 0; i < xTile; ++i)
        {
            ctbAddrTs += m_rowHeight[yTile] * m_colWidth[i];
        }

        for(auto j = 0; j < yTile; ++j)
        {
            ctbAddrTs += widthInCtbsY * m_rowHeight[j];
        }

        ctbAddrTs += (yTb - m_rowBd[yTile]) * m_colWidth[xTile] + xTb - m_colBd[xTile];
        m_ctbAddrRsToTs[toUnderlying(ctbAddrRs)] = ctbAddrTs;
        m_ctbAddrTsToRs[toUnderlying(ctbAddrTs)] = ctbAddrRs;
    }

    log(LogId::Debug, [this](std::ostream &oss){oss << toStr();});
}
/*----------------------------------------------------------------------------*/
std::string PictureProperties::toStr() const
{
    std::ostringstream oss;

    oss
        << "Picture "
        << heightInLumaSamples << 'x' << widthInLumaSamples
        << " ctbSizeY " << ctbSizeY
        << " minCbSizeY " << minCbSizeY
        << " minCuQpDeltaSize " << getMinCuQpDeltaSize()
        << " uniformSpacingFlag " << m_uniformSpacingFlag
        << " numTileRows " << getNumTileRows()
        << " numTileCols " << getNumTileColumns()
        << " chromaFormatIdc " << getName(chromaFormatIdc);

    oss << " rowHeight [ ";

    for(const auto h : m_rowHeight)
    {
        oss  << h << ' ';
    }

    oss << "] colWidth [ ";

    for(const auto w : m_colWidth)
    {
        oss  << w << ' ';
    }

    oss << "] rowBd [ ";

    for(const auto bdry : m_rowBd)
    {
        oss  << bdry << ' ';
    }

    oss << "] colBd [ ";

    for(const auto bdry : m_colBd)
    {
        oss << bdry << ' ';
    }

    oss << "]\n";

    const auto step = HEVC::toPel(ctbSizeY - 1_log2);
    auto tileRowNo = 0;

    for(auto y = 0_pel; y < heightInLumaSamples; y += step)
    {
        auto tileColNo = 0;
        const auto bTileRowBdry = toPel(tileRowBdry(tileRowNo).end()) == y + step;

        for(auto x = 0_pel; x < widthInLumaSamples; x += step)
        {
            const auto rTileColBdry = toPel(tileColumnBdry(tileColNo).end()) == x + step;

            if(
                    0_pel == x && 0_pel == y
                    || 0_pel == x && bTileRowBdry
                    || 0_pel == y && rTileColBdry
                    || rTileColBdry && bTileRowBdry)
            {
                oss << '+';
            }
            else if((0_pel == x || rTileColBdry) && 0_pel != y && !bTileRowBdry)
            {
                oss << '|';
            }
            else if(0_pel != x && !rTileColBdry && (0_pel == y || bTileRowBdry))
            {
                oss << '-';
            }
            else if(
                    tileRowBdry(tileRowNo).end() == toCtb(y + step * 2)
                    && tileColumnBdry(tileColNo).end() == toCtb(x + step * 2))
            {
                const auto id = tileId({x, y});
                oss << char(9 > id ? id + '0' : id - 10 + 'A');
            }
            else if(0_pel == y % HEVC::toPel(ctbSizeY) && 0_pel == x % HEVC::toPel(ctbSizeY))
            {
                oss << 'T';
            }
            else
            {
                oss << ' ';
            }

            if(toCtb(x) != toCtb(x + step) && rTileColBdry && getNumTileColumns() > tileColNo + 1)
            {
                ++tileColNo;
            }
        }

        oss << '\n';

        if(toCtb(y) != toCtb(y + step) && bTileRowBdry && getNumTileRows() > tileRowNo + 1)
        {
            ++tileRowNo;
        }
    }

    oss << "Ctb\n";

    for(auto y = 0_ctb; y < heightInCtbsY; ++y)
    {
        for(auto x = 0_ctb; x < widthInCtbsY; ++x)
        {
            if(0_ctb == x)
            {
                oss << '|';
            }

            oss << std::setw(4) << toAddrInTs({x, y}) << '.' << tileId({x, y}) << '|';
        }

        oss << '\n';
    }

    oss << "MinTb\n";

    for(auto y = 0_min_tb; y < toMinTb(heightInLumaSamples); ++y)
    {
        for(auto x = 0_min_tb; x < toMinTb(widthInLumaSamples); ++x)
        {
            if(0_min_tb == x)
            {
                oss << '|';
            }

            oss << std::setw(6) << toAddrInScanZ({x, y}) << '.' << tileId(toPel({x, y})) << '|';
        }

        oss << '\n';
    }

    return oss.str();
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

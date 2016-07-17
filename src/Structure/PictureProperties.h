#ifndef HEVC_Structure_PictureProperties_h
#define HEVC_Structure_PictureProperties_h

/* STDC++ */
#include <algorithm>
#include <array>
#include <string>
/* HEVC */
#include <HEVC.h>
#include <Structure/PelBuffer.h>
#include <Syntax/Fwd.h>
#include <MD5Hasher.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class DecodedPictureHash
{
    std::array<MD5Hasher::ValueType, EnumRange<Plane>::length()> m_md5Hash;
    std::array<uint16_t, EnumRange<Plane>::length()> m_crcHash;
    std::array<uint32_t, EnumRange<Plane>::length()> m_checksumHash;
public:
    DecodedPictureHash()
    {
        std::fill(std::begin(m_crcHash), std::end(m_crcHash), 0);
        std::fill(std::begin(m_checksumHash), std::end(m_checksumHash), 0);
    }

    MD5Hasher::ValueType md5Hash(Plane plane) const
    {
        return m_md5Hash[int(plane)];
    }

    void md5Hash(Plane plane, MD5Hasher::ValueType value)
    {
        m_md5Hash[int(plane)] = value;
    }

    uint16_t crcHash(Plane plane) const
    {
        return m_crcHash[int(plane)];
    }

    void crcHash(Plane plane, uint16_t value)
    {
        m_crcHash[int(plane)] = value;
    }

    uint32_t checksumHash(Plane plane) const
    {
        return m_checksumHash[int(plane)];
    }

    void checksumHash(Plane plane, uint32_t value)
    {
        m_checksumHash[int(plane)] = value;
    }
};
/*----------------------------------------------------------------------------*/
class PictureProperties:
    public DecodedPictureHash
{
public:
    typedef std::array<PelBuffer, EnumRange<Plane>::length()> PelLayer;
    typedef std::array<PelLayer, EnumRange<PelLayerId>::length()> PelLayers;
    typedef Syntax::VideoParameterSet VPS;
    typedef Syntax::SequenceParameterSet SPS;
    typedef Syntax::SpsRangeExtension SPSRE;
    typedef Syntax::PictureParameterSet PPS;
    typedef Syntax::PpsRangeExtension PPSRE;
    typedef Syntax::SliceSegmentHeader SSH;
    typedef Syntax::SliceSegmentData SSD;
    /* 04/2013, 7.4.2.2 "NAL unit header semantics",
     * "Any picture that has PicOutputFlag equal to 1 that precedes
     * an IRAP picture in decoding order shall precede the IRAP picture
     * in output order and shall precede any RADL picture associated with
     * the IRAP picture in output order" */
    bool picOutputFlag;
    /* 04/2013, 7.4.2.2 ""
     * All coded slice segment NAL units of an access unit shall have
     * the same value of nal_unit_type. A picture or an access unit is also
     * referred to as having a nal_unit_type equal to the nal_unit_type of
     * the coded slice segment NAL units of the picture or access unit. */
    const NalUnitType nalUnitType;
    const bool noRaslOutputFlag;

    /* active VPS/SPS/PPS/SF */
    const Ptr<const VPS> vps;
    const Ptr<const SPS> sps;
    const Ptr<const SPSRE> spsre;
    const Ptr<const PPS> pps;
    const Ptr<const PPSRE> ppsre;

    /* SPS */
    const ChromaFormatIdc chromaFormatIdc;
    const bool separateColourPlaneFlag;
    const int bitDepthY;
    const int bitDepthC;
    const int pcmBitDepthY;
    const int pcmBitDepthC;

    const Log2 minCbSizeY;
    const Log2 ctbSizeY;

    const Log2 minTrafoSize;
    const Log2 maxTrafoSize;
    const Log2 maxTransformSkipSize;

    const Pel widthInLumaSamples;
    const Pel heightInLumaSamples;
    const Pel sizeInLumaSamples;

    const MinCb widthInMinCbsY;
    const MinCb heightInMinCbsY;
    const MinCb sizeInMinCbsY;

    const Ctb widthInCtbsY;
    const Ctb heightInCtbsY;
    const Ctb sizeInCtbsY;

    const Log2 minIpcmCbSizeY;
    const Log2 maxIpcmCbSizeY;

    PicOrderCntLsb maxPicOrderCntLsb;
protected:
    /* SPS */
    int m_qpBdOffsetY;
    int m_qpBdOffsetC;

    /* PPS */
    int m_cbQpOffset;
    int m_crQpOffset;
    Log2 m_minCuQpDeltaSize;
    Log2 m_minCuChromaQpOffsetSize;

    bool m_uniformSpacingFlag;
    int m_numTileRows;
    int m_numTileColumns;
    VLA<Ctb> m_colWidth;
    VLA<Ctb> m_rowHeight;
    VLA<Ctb> m_colBd;
    VLA<Ctb> m_rowBd;
    VLA<Ctb> m_ctbAddrRsToTs;
    VLA<Ctb> m_ctbAddrTsToRs;
    //VLA<Ctb> m_tileId;
public:
    PictureProperties(
            Ptr<const VPS>,
            Ptr<const SPS>,
            Ptr<const PPS>,
            NalUnitType,
            bool);

    std::string toStr() const;

    /*------------------------------------------------------------------------*/
    int chromaArrayType() const
    {
        return separateColourPlaneFlag ? 0 : int(chromaFormatIdc);
    }

    int bitDepth(Component component) const
    {
        return Component::Luma == component ? bitDepthY : bitDepthC;
    }

    int bitDepth(Plane plane) const
    {
        return bitDepth(toComponent(plane));
    }

    int bitDepthPCM(Component component) const
    {
        return Component::Luma == component ? pcmBitDepthY : pcmBitDepthC;
    }

    int bitDepthPCM(Plane plane) const
    {
        return bitDepthPCM(toComponent(plane));
    }

    int qpBdOffset(Component component) const
    {
        return Component::Luma == component ? m_qpBdOffsetY : m_qpBdOffsetC;
    }

    int qpBdOffset(Plane plane) const
    {
        return qpBdOffset(toComponent(plane));
    }

    Pel widthInPels(Component component = Component::Luma) const
    {
        return hScale(widthInLumaSamples, component, chromaFormatIdc);
    }

    Pel heightInPels(Component component = Component::Luma) const
    {
        return vScale(heightInLumaSamples, component, chromaFormatIdc);
    }

    Pel widthInPels(Plane plane = Plane::Y) const
    {
        return widthInPels(toComponent(plane));
    }

    Pel heightInPels(Plane plane = Plane::Y) const
    {
        return heightInPels(toComponent(plane));
    }
    /*------------------------------------------------------------------------*/
    /* MinTb */
    MinTb toMinTb(Pel pel) const
    {
        return HEVC::toMinTb(pel, minTrafoSize);
    }

    Pel toPel(MinTb minCb) const
    {
        return HEVC::toPel(minCb, minTrafoSize);
    }

    MinTbCoord toMinTb(PelCoord coord) const
    {
        return HEVC::toMinTb(coord, minTrafoSize);
    }

    PelCoord toPel(MinTbCoord coord) const
    {
        return HEVC::toPel(coord, minTrafoSize);
    }
    /*------------------------------------------------------------------------*/
    /* MinCb */
    MinCb toMinCb(Pel pel) const
    {
        return HEVC::toMinCb(pel, minCbSizeY);
    }

    Pel toPel(MinCb minCb) const
    {
        return HEVC::toPel(minCb, minCbSizeY);
    }

    MinCbCoord toMinCb(PelCoord coord) const
    {
        return HEVC::toMinCb(coord, minCbSizeY);
    }

    PelCoord toPel(MinCbCoord coord) const
    {
        return HEVC::toPel(coord, minCbSizeY);
    }
    /*------------------------------------------------------------------------*/
    /* Ctb */
    Ctb toCtb(Pel pel) const
    {
        return HEVC::toCtb(pel, ctbSizeY);
    }

    Pel toPel(Ctb ctb) const
    {
        return HEVC::toPel(ctb, ctbSizeY);
    }

    Ctb toCtb(MinCb minCb) const
    {
        return HEVC::toCtb(minCb, ctbSizeY - minCbSizeY);
    }

    CtbCoord toCtb(PelCoord coord) const
    {
        return HEVC::toCtb(coord, ctbSizeY);
    }

    PelCoord toPel(CtbCoord coord) const
    {
        return HEVC::toPel(coord, ctbSizeY);
    }
    /*------------------------------------------------------------------------*/
    int getQpOffset(Chroma chroma) const
    {
        return Chroma::Cb == chroma ? m_cbQpOffset : m_crQpOffset;
    }

    Log2 getMinCuQpDeltaSize() const
    {
        return m_minCuQpDeltaSize;
    }

    MinQpGrp toMinQpGrp(Pel pel) const
    {
        return HEVC::toMinQpGrp(pel, getMinCuQpDeltaSize());
    }

    Pel toPel(MinQpGrp minQpGrp) const
    {
        return HEVC::toPel(minQpGrp, getMinCuQpDeltaSize());
    }

    MinQpGrpCoord toMinQpGrp(PelCoord coord) const
    {
        return HEVC::toMinQpGrp(coord, getMinCuQpDeltaSize());
    }

    PelCoord toPel(MinQpGrpCoord coord) const
    {
        return HEVC::toPel(coord, getMinCuQpDeltaSize());
    }

    Log2 minCuChromaQpOffsetSize() const
    {
        return m_minCuChromaQpOffsetSize;
    }

    PelRect boundaries(Plane plane = Plane::Y) const
    {
        return
            PelRect
            {
                {0_pel, 0_pel},
                hScale(widthInLumaSamples, plane, chromaFormatIdc),
                vScale(heightInLumaSamples, plane, chromaFormatIdc)
            };
    }

    PelRect boundaries(Component component) const
    {
        return
            PelRect
            {
                {0_pel, 0_pel},
                hScale(widthInLumaSamples, component, chromaFormatIdc),
                vScale(heightInLumaSamples, component, chromaFormatIdc)
            };
    }

    bool encloses(PelCoord at, Plane plane = Plane::Y) const
    {
        return boundaries(plane).encloses(at);
    }

    Ctb toAddrInRs(CtbCoord coord) const
    {
        return coord.y() * widthInCtbsY + coord.x();
    }

    CtbCoord toCoord(Ctb addrRs) const
    {
        return {addrRs % widthInCtbsY, addrRs / widthInCtbsY};
    }

    int getNumTileRows() const
    {
        return m_numTileRows;
    }

    int getNumTileColumns() const
    {
        return m_numTileColumns;
    }

    int getNumTiles() const
    {
        return getNumTileRows() * getNumTileColumns();
    }

    Range<Ctb> tileRowBdry(int row) const
    {
        return {m_rowBd[row], m_rowBd[row + 1]};
    }

    Range<Ctb> tileColumnBdry(int column) const
    {
        return {m_colBd[column], m_colBd[column + 1]};
    }

    PelRect tileBdry(int id) const
    {
        runtime_assert(getNumTiles() > id);

        const auto row = id / getNumTileColumns();
        const auto column = id % getNumTileColumns();
        const auto rowBdry = tileRowBdry(row);
        const auto columnBdry = tileColumnBdry(column);
        return
        {
            toPel(CtbCoord{columnBdry.begin(), rowBdry.begin()}),
            toPel(columnBdry.length()),
            toPel(rowBdry.length())
        };
    }

    Range<Ctb> tileAddrBdryInTs(int id) const
    {
        const auto bdry = tileBdry(id);
        return
        {
            toAddrInTs(toAddrInRs(toCtb(bdry.topLeft()))),
            toAddrInTs(toAddrInRs(toCtb(bdry.bottomRight()))) + 1_ctb
        };
    }

    int tileId(CtbCoord at) const
    {
        const auto xDiff =
            std::upper_bound(std::begin(m_colBd), std::end(m_colBd), at.x())
            - std::begin(m_colBd);
        const auto yDiff =
            std::upper_bound(std::begin(m_rowBd), std::end(m_rowBd), at.y())
            - std::begin(m_rowBd);
        const auto id = (yDiff - 1) * getNumTileColumns() + xDiff - 1;
        return id;
    }

    int tileId(PelCoord at) const
    {
        return tileId(toCtb(at));
    }

    Ctb toAddrInTs(Ctb addr) const
    {
        return m_ctbAddrRsToTs[toUnderlying(addr)];
    }

    Ctb toAddrInTs(CtbCoord coord) const
    {
        return toAddrInTs(toAddrInRs(coord));
    }

    Ctb toAddrInRs(Ctb addrInTs) const
    {
        return m_ctbAddrTsToRs[toUnderlying(addrInTs)];
    }

    MinTb toAddrInScanZ(MinTbCoord coord) const
    {
        const auto x = coord.x();
        const auto y = coord.y();
        const auto sizeDiff = ctbSizeY - minTrafoSize;
        const auto ratio = toInt(sizeDiff);
        const auto ctbCoord = toCtb(toPel(coord));
        const auto ctbAddrRs = toAddrInRs(ctbCoord);
        const auto ctbAddrTs = toAddrInTs(ctbAddrRs);
        /* number of MinTb per one Ctb */
        const auto num = MinTb(toInt(sizeDiff + sizeDiff));
        /* number of MinCtb up to position ctbCoord */
        const auto base = num * toUnderlying(ctbAddrTs);
        /* number of MinTb in current Ctb up to position (x, y) */
        const auto offset = toAddrScanZ({x % ratio, y % ratio}, 1_min_tb * ratio, 1_min_tb);
        return base + offset;
    }

    MinTb toAddrInScanZ(PelCoord coord) const
    {
        return toAddrInScanZ(toMinTb(coord));
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_PictureProperties_h */

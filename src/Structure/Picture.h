#ifndef HEVC_Structure_Picture_h
#define HEVC_Structure_Picture_h

/* STDC++ */
/* HEVC */
#include <HEVC.h>
#include <Structure/Fwd.h>
#include <Structure/PictureProperties.h>
#include <Structure/Slice.h>
#include <Structure/Tile.h>
#include <Structure/RPS.h>
#include <Syntax/Fwd.h>
#include <Syntax/CABAD/State.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class Picture:
        public PictureProperties
{
public:
    // in Ts order
    typedef VLA<Handle<Slice>> SliceList;
    // tileId -> Tile
    typedef VLA<Tile> TileList;
    // addrInTs -> CTU
    typedef VLA<Ptr<Syntax::CodingTreeUnit>> CtuList;
private:
    SliceList m_slice;
    TileList m_tile;
    CtuList m_ctu;

    /* in use */
    Syntax::CABAD::State m_activeCabadState;
    std::array<
        Optional<Syntax::CABAD::State>,
        EnumRange<Syntax::CABAD::RestorePoint>::length()> m_cabadState;

    Handle<RPS> m_rps;
    PelLayers m_pelLayers;
public:
    /* in decoding order */
    /*------------------------------------------------------------------------*/
    /* POC */
    PicOrderCntSet order;
    int64_t decodingNo;
    int64_t outputNo;
    /*------------------------------------------------------------------------*/
    Tuple<
        // reference type
        RefPicType,
        /* POC of picture which was CURRENT PICTURE (in decoding order)
         * when current picture was marked with given type */
        PicOrderCntVal> reference;
public:
    Picture(
            Ptr<const VPS>,
            Ptr<const SPS>,
            Ptr<const PPS>,
            NalUnitType,
            bool,
            int64_t);

    Picture(const Picture &) = delete;
    Picture &operator= (const Picture &) = delete;
    /*------------------------------------------------------------------------*/
    /* Tile */
    const Tile &tile(int id) const
    {
        return m_tile[id];
    }

    Tile &tile(int id)
    {
        return m_tile[id];
    }

    const Tile &tile(CtbCoord at) const
    {
        return tile(tileId(at));
    }

    Tile &tile(CtbCoord at)
    {
        return tile(tileId(at));
    }

    const Tile &tile(PelCoord at) const
    {
        return tile(toCtb(at));
    }

    Tile &tile(PelCoord at)
    {
        return tile(toCtb(at));
    }
    /*------------------------------------------------------------------------*/
    /* Slice */
    Ptr<Slice> slice(CtbAddr addr)
    {
        m_slice.emplaceBack();
        m_slice.back().construct(addr);

        const Ptr<Slice> slice{m_slice.back()};

        tile(toCoord(addr.inRs)).add(slice);
        return slice;
    }

    void add(CtbAddr, Ptr<SSH>, Ptr<SSD>);

    Ptr<Slice> slice(CtbCoord at) const
    {
        return tile(at).slice(toAddrInTs(at));
    }

    Ptr<Slice> slice(PelCoord at) const
    {
        return slice(toCtb(at));
    }

    const SliceList &sliceList() const
    {
        return m_slice;
    }
    /*------------------------------------------------------------------------*/
    /* CABAD */
    Syntax::CABAD::State &getCabadState()
    {
        return m_activeCabadState;
    }

    void store(Syntax::CABAD::RestorePoint);
    void restore(Syntax::CABAD::RestorePoint, StreamAccessLayer &);
    /*------------------------------------------------------------------------*/
    /* Reference Picture Set */
    Ptr<RPS> rps() const
    {
        return Ptr<RPS>{m_rps};
    }
    /*------------------------------------------------------------------------*/
    /* Ctu */
    Ptr<Syntax::CodingTreeUnit> getCodingTreeUnit(Ctb addr) const
    {
        return m_ctu[toUnderlying(addr)];
    }

    Ptr<Syntax::CodingTreeUnit> getCodingTreeUnit(CtbCoord coord) const
    {
        return getCodingTreeUnit(toAddrInTs(toAddrInRs(coord)));
    }

    Ptr<Syntax::CodingTreeUnit> getCodingTreeUnit(PelCoord coord) const
    {
        return getCodingTreeUnit(toCtb(coord));
    }

    void add(Ptr<Syntax::CodingTreeUnit> ctu)
    {
        m_ctu.pushBack(ctu);
    }

    Ctb ctuNum() const
    {
        return Ctb(m_ctu.size());
    }
    /*------------------------------------------------------------------------*/
    Ptr<Syntax::SAO> getSAO(PelCoord) const;
    Ptr<Syntax::CodingQuadTree> getCodingQuadTree(PelCoord) const;
    Ptr<Syntax::CodingUnit> getCodingUnit(PelCoord) const;
    Ptr<Syntax::PredictionUnit> getPredictionUnit(PelCoord) const;
    Ptr<Syntax::MvdCoding> getMvdCoding(PelCoord) const;
    Ptr<Syntax::TransformTree> getTransformTree(PelCoord) const;
    Ptr<Syntax::TransformUnit> getTransformUnit(PelCoord) const;
    /*------------------------------------------------------------------------*/
    /* 6.4.1, "Derivation process for z-scan order block availability" */
    bool isAvailableInScanZ(PelCoord curr, PelCoord adj) const;
    /* 6.4.2, "Derivation process for prediction block availability" */
    bool isPredictionBlockAvailable(
            PelCoord cb, Pel nCbS,
            PelCoord pb, Pel nPbW, Pel nPbH,
            int partIdx,
            PelCoord nbY) const;

    const PelLayer &pelLayer(PelLayerId layer) const
    {
        return m_pelLayers[int(layer)];
    }

    PelLayer &pelLayer(PelLayerId layer)
    {
        return m_pelLayers[int(layer)];
    }

    const PelBuffer &pelBuffer(PelLayerId layer, Plane plane) const
    {
        return m_pelLayers[int(layer)][int(plane)];
    }

    PelBuffer &pelBuffer(PelLayerId layer, Plane plane)
    {
        return m_pelLayers[int(layer)][int(plane)];
    }

    void writeTo(std::ostream &, PelLayerId);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Structure

#endif // HEVC_Structure_Picture_h

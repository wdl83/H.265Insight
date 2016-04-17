#ifndef HEVC_Syntax_h
#define HEVC_Syntax_h

#include <HEVC.h>
#include <Fwd.h>
#include <Syntax/Fwd.h>
#include <Syntax/ElementId.h>
#include <Syntax/Element.h>
#include <Syntax/Embedded.h>
#include <Syntax/Aggregator.h>
#include <Syntax/VLD/Descriptors.h>
#include <Syntax/CABAD/Descriptors.h>
#include <Syntax/Parser.h>
#include <Syntax/check.h>
#include <Structure/Fwd.h>
#include <Decoder/Fwd.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
template <typename U, ElementId id>
class EmbeddedCoord: public Embedded
{
public:
    typedef U UnitType;
    typedef Coord<UnitType> CoordType;

    static const auto Id = id;
private:
    CoordType m_coord;
public:
    EmbeddedCoord(CoordType coord): m_coord(coord)
    {}

    CoordType inUnits() const
    {
        return m_coord;
    }

    UnitType x() const
    {
        return m_coord.x();
    }

    UnitType y() const
    {
        return m_coord.y();
    }
};

template <typename U, ElementId id>
class EmbeddedUnit: public Embedded
{
public:
    typedef U UnitType;

    static const auto Id = id;
private:
    UnitType m_unit;
public:
    EmbeddedUnit(UnitType u): m_unit(u)
    {}

    UnitType inUnits() const
    {
        return m_unit;
    }

    operator UnitType () const
    {
        return m_unit;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_h */

SET(
    utils_H
    BitArray.h
    BitIndex.h
    BitMatrix.h
    Coord.h
    Counter.h
    EnumUtils.h
    Fwd.h
    Handle.h
    HEVC.h
    HEVCDef.h
    HEVCLimits.h
    log.h
    Matrix.h
    MD5Hasher.h
    PicOrderCnt.h
    TerminationTools.h
    StreamAccessLayer.h
    runtime_assert.h
    units.h
    utils.h
    Range.h
    Pair.h
    StoreByValue.h
    Tuple.h
    VariadicTraits.h
    Vector.h
    Ptr.h)

SET(
    utils_SRC
    HEVCDef.cpp
    log.cpp
    TerminationTools.cpp
    StreamAccessLayer.cpp
    runtime_assert.cpp
    units.cpp)

ADD_LIBRARY(utils STATIC ${utils_SRC})
TARGET_LINK_LIBRARIES(utils third_party)

ADD_SUBDIRECTORY(Syntax)
ADD_SUBDIRECTORY(Structure)
ADD_SUBDIRECTORY(Decoder)
ADD_SUBDIRECTORY(ThirdParty)

SET(
    structure_H
    AccessUnit.h
    CPB.h
    CtbEdgeBS.h
    CtbEdgeFlags.h
    DPB.h
    Fwd.h
    PelBuffer.h
    Picture.h
    PictureProperties.h
    RPL.h
    RPS.h
    RPSP.h
    ScalingFactor.h
    ScalingList.h
    Slice.h
    Tile.h)

SET(
    structure_SRC
    AccessUnit.cpp
    CPB.cpp
    DPB.cpp
    PelBuffer.cpp
    Picture.cpp
    PictureProperties.cpp
    RPL.cpp
    RPS.cpp
    RPSP.cpp
    ScalingFactor.cpp
    ScalingList.cpp
    Slice.cpp
    Tile.cpp)

ADD_LIBRARY(structure STATIC ${structure_SRC})
TARGET_LINK_LIBRARIES( structure syntax utils)

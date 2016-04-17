SET(
    third_party_H
    MD5/md5.h)

SET(
    third_party_SRC
    MD5/md5.c)

ADD_LIBRARY(third_party STATIC ${third_party_SRC})

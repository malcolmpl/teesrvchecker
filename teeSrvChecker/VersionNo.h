#define TSVN_VERMAJOR      0
#define TSVN_VERMAJOR_STR "0"
#define TSVN_VERMINOR      1
#define TSVN_VERMINOR_STR "1"
#define TSVN_VERMICRO      3
#define TSVN_VERMICRO_STR "3"
#define TSVN_VERBUILD     2
#define TSVN_VERBUILD_STR "2"
#define TSVN_VERDATE      "2007/09/26 18:31:20"

#define FILEVER          TSVN_VERMAJOR,TSVN_VERMINOR,TSVN_VERMICRO,TSVN_VERBUILD
#define PRODUCTVER       TSVN_VERMAJOR,TSVN_VERMINOR,TSVN_VERMICRO,TSVN_VERBUILD


#define STRFILEVER       TSVN_VERMAJOR_STR "," TSVN_VERMINOR_STR "," TSVN_VERMICRO_STR "," TSVN_VERBUILD_STR
#define STRPRODUCTVER    TSVN_VERMAJOR_STR "," TSVN_VERMINOR_STR "," TSVN_VERMICRO_STR "," TSVN_VERBUILD_STR


#define TSVN_VERSPECIAL  "local modifications found, nomixed update revisions found"

#ifdef _WIN64
#define TSVN_PLATFORM    "64 Bit $DEVBUILD$"
#else
#define TSVN_PLATFORM    "32 Bit $DEVBUILD$"
#endif

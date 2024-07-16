
include_directories(

    ../../src/db/include
    ../../src/db
)


aux_source_directory(. DIR_UTILITY_SRCS)
add_library(utility OBJECT ${DIR_UTILITY_SRCS})
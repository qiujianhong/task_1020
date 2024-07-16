aux_source_directory(. DIR_COMMON_UTILITY_SRCS)
add_library(common.utility OBJECT ${DIR_COMMON_UTILITY_SRCS})
ADD_LIBRARY(xlutility STATIC ${DIR_COMMON_UTILITY_SRCS})
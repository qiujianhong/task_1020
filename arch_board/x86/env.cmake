cmake_minimum_required(VERSION 2.8)

include(${CODE_DIE}/build/product.cmake)
include(${CODE_DIE}/build/board.cmake)
include(${CODE_DIE}/build/area.cmake)

# 指定lib路径
set(LIB_DIR ${CODE_DIE}/release/lib)

# 指定hal_lib路径
set(HAL_DIR ${CODE_DIE}/release/hal_lib)

# 指定APP路径
set(APP_DIR ${CODE_DIE}/release/apps)

# 指定APP安装路径
set(RELEASE_DIR ${CODE_DIE}/release/apps)
set(INSTALL_DIR /usr/local/extapps)
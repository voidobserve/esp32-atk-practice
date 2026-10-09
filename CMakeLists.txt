# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# 额外组件目录: bsp 只作为"分类目录", 它下面的 bsp_xxx 各自是一个独立组件
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/components/bsp")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project)

# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)

project(bsec2_iaq)

# For more information, see: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/build-system.html#custom-cmake-variables
set(EXTRA_COMPONENT_DIRS
    components/Bosch-BME68x-Library
    components/Bosch-BSEC2-Library
)



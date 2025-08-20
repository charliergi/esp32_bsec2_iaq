# In components/Bosch-BME68x-Library/CMakeLists.txt

idf_component_register(SRCS "src/bme68x/bme68x.c" "src/bme68xLibrary.cpp"
                        INCLUDE_DIRS "." "src" "src/bme68x"
                        REQUIRES driver espressif__arduino-esp32)

target_compile_definitions(${COMPONENT_LIB} PRIVATE 
                           ARDUINO=200
                           )
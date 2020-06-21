
set(srcs "APDS9960.cpp" "I2cDevice.cpp")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include")

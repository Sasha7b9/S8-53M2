# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

set(CMAKE_UNITY_BUILD OFF)

include_directories(../common/_VS_KEIL/ARMCLANG
                    ../common/_VS_KEIL/CMSIS
                    ../common/_VS_KEIL/FatFS
                    ../STM32Cube_FW_F4/Drivers/STM32F4xx_HAL_Driver/Inc
                    ../STM32Cube_FW_F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
                    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
                    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
                    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
                    ../STM32Cube_FW_F4/Middlewares/Third_Party/FatFs/src
                    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc
                    ../STM32Cube_FW_F4/Middlewares/Third_Party/LwIP/src/include
                    ../STM32Cube_FW_F4/Middlewares/Third_Party/LwIP/system
                    src
                    src/libs)

add_definitions(-DSTM32F429xx -DUSE_HAL_DRIVER -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

    add_compile_options(/wd4100)
    add_compile_options(/wd4103)    # alignment changed after including header, may be due to missing #pragma pack(pop)
    add_compile_options(/wd4201)
    add_compile_options(/wd4211)
    add_compile_options(/wd4244)
    add_compile_options(/wd4255)
    add_compile_options(/wd4310)    # cast truncates constant value
    add_compile_options(/wd4505)    # 'function': unreferenced function with internal linkage has been removed
    add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
    add_compile_options(/wd4623)    # 'derived class' : default constructor was implicitly defined as deleted because a base
                                    # class default constructor is inaccessible or deleted
    add_compile_options(/wd4626)    # 'derived class' : assignment operator was implicitly defined as deleted because a base
                                    # class assignment operator is inaccessible or deleted
    add_compile_options(/wd4668)    # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    add_compile_options(/wd4774)    # 'function' : format string expected in argument 3 is not a string literal
    add_compile_options(/wd4820)    # 'bytes' bytes padding added after construct 'member_name'
    add_compile_options(/wd5027)    # 'derived class' : move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5039)    # 'function': pointer or reference to potentially throwing function passed to
                                    # 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws
                                    # an exception
    add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    add_compile_options(/wd5204)    # 'xxx': class has virtual functions, but its trivial destructor is not virtual;
                                    # instances of objects derived from this class may not be destructed correctly
    add_compile_options(/wd5219)    # implicit conversion from 'type1' to 'type2', possible loss of data
    add_compile_options(/wd5220)    # '<unnamed-tag>::SR': a non-static data member with a volatile qualified type no longer
                                    # implies that compiler generated copy/move constructors and copy/move assignment
                                    # operators are not trivia
    add_compile_options(/wd26812)   # Prefer 'enum class' over 'enum' (Enum.3)

else()


    add_definitions(-Wno-builtin-declaration-mismatch
                    -Wno-write-strings
                    -Wno-overflow
                    -Wno-int-to-pointer-cast
    )

endif()

file(GLOB SRC                 src/*.cpp src/*.h)
file(GLOB LIBS                src/libs/*.*)
file(GLOB DATA                src/Data/*.*)
file(GLOB DISPLAY             src/Display/*.*)
file(GLOB DISPLAY_FONT        src/Display/font/*.*)
file(GLOB ETHERNET            src/Ethernet/*.*)
file(GLOB FDRIVE              src/FDrive/*.*)
file(GLOB FPGA                src/FPGA/*.*)
file(GLOB HARDWARE            src/Hardware/*.*)
file(GLOB HARDWARE_HAL        src/Hardware/HAL/*.*)
file(GLOB MENU                src/Menu/*.*)
file(GLOB MENU_PAGES          src/Menu/Pages/*.*)
file(GLOB PANEL               src/Panel/*.*)
file(GLOB SETTINGS            src/Settings/*.*)
file(GLOB UTILS               src/Utils/*.*)
file(GLOB UTILS_CONTAINERS    src/Utils/Containers/*.*)
file(GLOB VCP                 src/VCP/*.*)
file(GLOB VCP_SCPI            src/VCP/SCPI/*.*)

source_group(src              FILES ${SRC})
source_group(libs             FILES ${LIBS})
source_group(Data             FILES ${DATA})
source_group(Display          FILES ${DISPLAY})
source_group(Display/font     FILES ${DISPLAY_FONT})
source_group(Ethernet         FILES ${ETHERNET})
source_group(FDrive           FILES ${FDRIVE})
source_group(FPGA             FILES ${FPGA})
source_group(Hardware         FILES ${HARDWARE})
source_group(Hardware/HAL     FILES ${HARDWARE_HAL})
source_group(Menu             FILES ${MENU})
source_group(Menu/Pages       FILES ${MENU_PAGES})
source_group(Panel            FILES ${PANEL})
source_group(Settings         FILES ${SETTINGS})
source_group(Utils            FILES ${UTILS})
source_group(Utils/Containers FILES ${UTILS_CONTAINERS})
source_group(VCP              FILES ${VCP})
source_group(VCP/SCPI         FILES ${VCP_SCPI})

set_source_files_properties(src/libs/ethernetif.cpp
                            src/libs/usbd_desc.cpp
                            src/libs/app_ethernet.cpp
                            src/libs/usbd_cdc_interface.cpp
                            src/libs/usbd_conf.c
                            src/Menu/Pages/PageHelp.cpp
                            src/Panel/PanelFunctions.cpp
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME} ${SRC} ${LIBS} ${DISPLAY} ${DISPLAY_FONT} ${ETHERNET}
                            ${DATA}
                            ${FDRIVE} ${FPGA} ${HARDWARE} ${HARDWARE_HAL} ${MENU} ${MENU_PAGES}
                            ${PANEL} ${SETTINGS}
                            ${UTILS} ${UTILS_CONTAINERS}
                            ${VCP} ${VCP_SCPI})

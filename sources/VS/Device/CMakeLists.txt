# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (S8-53M2)

set(CMAKE_UNITY_BUILD ON)

include_directories(../..
                    ../../Device/src
                    ../../Device/src/Hardware/HAL
                    ../../Device/src/Hardware/LwIP
                    ../../Device/src/Hardware/VCP
                                 src
                        ../../common/Hardware/FatFS
                        ../../common/Hardware/FDrive
                        ../../common/_VS_GUI/CMSIS
                        ../../common/_VS_GUI/FatFS
                        ../../common/STM32Cube_F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
                        ../../common/STM32Cube_F4/Drivers/STM32F4xx_HAL_Driver/Inc
                        ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src
                        ../../common/STM32Cube_F4/Middlewares/Third_Party/LwIP/src/include
                        ../../common/STM32Cube_F4/Middlewares/Third_Party/LwIP/system
                        ../../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
                        ../../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
                        ../../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
                        ../../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc
                        ../../VS/ThirdParty/wxWidgets/include 
                        ../../generated/ThirdParty/lib/vc_dll/mswud
                    )

link_directories(../../generated/ThirdParty/lib/vc_dll)

add_definitions(-DSTM32F429xx -DWXUSINGDLL -DGUI -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    
    add_compile_options(/wd4201)    # nonstandard extension used : nameless struct/union
    add_compile_options(/wd4127)    # conditional expression is constant
    add_compile_options(/wd4211)    # nonstandard extension used : redefined extern to static
    add_compile_options(/wd4255)    # 'function' : no function prototype given: converting '()' to '(void)'
    add_compile_options(/wd4310)    # cast truncates constant value
    add_compile_options(/wd4355)    # 'this' : used in base member initializer list
    add_compile_options(/wd4464)    # relative include path contains '..'
    add_compile_options(/wd4505)    # 'function': unreferenced function with internal linkage has been removed
    add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
    add_compile_options(/wd4623)    # 'derived class' : default constructor was implicitly defined as deleted because a base
                                    # class default constructor is inaccessible or deleted
    add_compile_options(/wd4625)    # 'derived class' : copy constructor was implicitly defined as deleted because a base
                                    # class copy constructor is inaccessible or deleted
    add_compile_options(/wd4626)    # 'xxx': assignment operator was implicitly defined as deleted
    add_compile_options(/wd4668)    # 'xxx' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
    add_compile_options(/wd4710)    # 'xxx': function not inlined
    add_compile_options(/wd4774)    # 'string' : format string expected in argument number is not a string literal
    add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
    add_compile_options(/wd4996)    # 'xxx': This function or variable may be unsafe.
    add_compile_options(/wd5026)    # 'type': move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5027)    # 'type': move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5039)    # 'xxx': pointer or reference to potentially throwing function passed to extern C
                                    # function under -EHc.
    add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    add_compile_options(/wd5204)
    add_compile_options(/wd5219)
    add_compile_options(/wd5220)    # a non-static data member with a volatile qualified type no longer implies that
                                    # compiler generated copy/move constructors and copy/move assignment operators are not
                                    # trivial
    add_compile_options(/wd26812)   # the enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum'
endif()

file(GLOB LIBS_CMSIS                      ../../common/_VS_GUI/CMSIS/*.*)
file(GLOB LIBS_FATFS                      ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src/diskio.*
                                          ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src/ff.*
                                          ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src/ff_gen_drv.*
                                          ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src/option/unicode.c
                                          ../../common/STM32Cube_F4/Middlewares/Third_Party/FatFs/src/option/syscall.c)
file(GLOB LIBS_HAL                        ../../common/_VS_GUI/HAL/*.*)
file(GLOB LIBS_USBH                       ../../common/_VS_GUI/USBH/*.*)
file(GLOB LIBS_USBD                       ../../common/_VS_GUI/USBD/*.*)
file(GLOB DATA                            ../../Device/src/Data/*.*)
file(GLOB DATA_INPUTSTORAGE               ../../Device/src/Data/InputStorage/*.*)
file(GLOB DATA_OUTPUTSTORAGE              ../../Device/src/Data/OutputStorage/*.*)
file(GLOB DISPLAY                         ../../Device/src/Display/Console.*
                                          ../../Device/src/Display/Colors_d.cpp
                                          ../../Device/src/Display/Display.*
                                          ../../Device/src/Display/Painter.*
                                          ../../Device/src/Display/PainterData.*
                                          ../../Device/src/Display/Warnings.*
                                                       src/Display/*.*
                                              ../../common/Display/Colors_.*)
file(GLOB DISPLAY_FONT                        ../../common/Display/Font/*.*)
file(GLOB DISPLAY_PAINTER                     ../../common/Display/Painter/Painter_.*
                                              ../../common/Display/Painter/Painter_stm_d_.*
                                              ../../common/Display/Painter/Primitives_.*
                                              ../../common/Display/Painter/Text_.*
                                          ../../Device/src/Display/Painter/DisplayPrimitives.*
                                          ../../Device/src/Display/Painter/PainterData.*)
file(GLOB DISPLAY_SCREEN                  ../../Device/src/Display/Screen/*.*)
file(GLOB FPGA                            ../../Device/src/FPGA/*.*)
file(GLOB HARDWARE                        ../../Device/src/Hardware/EPROM.h
                                                       src/Hardware/EPROM_win.cpp
                                          ../../Device/src/Hardware/RAM.*
                                              ../../common/Hardware/Sound_.*
                                              ../../common/Hardware/Timer_.*
                                              ../../common/Hardware/VCP_.*)
file(GLOB HARDWARE_HAL                        ../../common/Hardware/HAL/HAL_.*
                                              ../../common/Hardware/HAL/HAL_PINS_.*
                                              ../../common/Hardware/HAL/HAL_ADC1_.cpp
                                              ../../common/Hardware/HAL/HAL_CRC_.cpp
                                              ../../common/Hardware/HAL/HAL_DAC1_.cpp
                                              ../../common/Hardware/HAL/HAL_DAC2_.cpp
                                              ../../common/Hardware/HAL/HAL_LTDC_.cpp
                                              ../../common/Hardware/HAL/HAL_OTP_.cpp
                                              ../../common/Hardware/HAL/HAL_ROM_.cpp
                                              ../../common/Hardware/HAL/HAL_RTC_.cpp
                                              ../../common/Hardware/HAL/HAL_SPI1_.cpp
                                              ../../common/Hardware/HAL/HAL_SPI5_.cpp
                                              ../../common/Hardware/HAL/HAL_TIM2_.cpp
                                              ../../common/Hardware/HAL/HAL_TIM6_.cpp
                                              ../../common/Hardware/HAL/HAL_TIM7_.cpp
                                              ../../common/Hardware/HAL/HAL_USBD_.cpp
                                          ../../Device/src/Hardware/HAL/*.h
                                                       src/Hardware/HAL/HAL_FMC_win.cpp
                                                       src/Hardware/HAL/HAL_ROM_win.cpp)
file(GLOB HARDWARE_LAN                    ../../Device/src/Hardware/LAN/LAN.h
                                                       src/Hardware/LAN/LAN_win.cpp
                                          ../../Device/src/Hardware/LAN/SocketTCP.h
                                          ../../Device/src/Hardware/LAN/SocketTCP.cpp)
file(GLOB HARDWARE_MEMORY                     ../../common/Hardware/Memory/ROM_.*
                                              ../../common/Hardware/Memory/ROM_Data_.cpp
                                              ../../common/Hardware/Memory/ROM_Settings_.cpp
                                              ../../common/Hardware/Memory/Sector_.*
                                          ../../Device/src/Hardware/Memory/Sector.cpp)
file(GLOB HARDWARE_VCP                    ../../Device/src/Hardware/VCP/*.*)
file(GLOB HARDWARE_FDRIVE                     ../../common/Hardware/USBH/*.*
                                          ../../Device/src/Hardware/FDrive/*.h
                                                       src/Hardware/FDrive/FDrive_win.cpp)
file(GLOB MENU                            ../../Device/src/Menu/*.*)
file(GLOB MENU_ITEMS                      ../../Device/src/Menu/Items/*.*)
file(GLOB MENU_PAGES                      ../../Device/src/Menu/Pages/*.*)
file(GLOB MENU_PAGES_CURSORS              ../../Device/src/Menu/Pages/Cursors/*.*)
file(GLOB MENU_PAGES_DEBUG                ../../Device/src/Menu/Pages/Debug/*.*)
file(GLOB MENU_PAGES_DISPLAY              ../../Device/src/Menu/Pages/Display/*.*)
file(GLOB MENU_PAGES_MEASURES             ../../Device/src/Menu/Pages/Measures/*.*)
file(GLOB MENU_PAGES_MEMORY               ../../Device/src/Menu/Pages/Memory/*.*)
file(GLOB MENU_PAGES_SERVICE              ../../Device/src/Menu/Pages/Service/*.*)
file(GLOB MENU_PAGES_SERVICE_MATH         ../../Device/src/Menu/Pages/Service/Math/*.*)
file(GLOB PANEL                               ../../common/Panel/Controls_.*
                                          ../../Device/src/Panel/*.*)
file(GLOB SETTINGS                            ../../common/Settings/SettingsTypes_.h
                                          ../../Device/src/Settings/*.*)
file(GLOB SRC                                          src/main_win.cpp
                                          ../../Device/src/defines.*
                                          ../../Device/src/device.*
                                          ../../Device/src/Globals.*
                                          ../../Device/src/Tables.*
                                              ../../common/defines_.h
                                              ../../common/Log_.*
                                              ../../common/redefine_hal_.h)
file(GLOB SCPI                            ../../Device/src/SCPI/*.*)
file(GLOB SCPI_PROCESS                    ../../Device/src/SCPI/Process/*.*)
file(GLOB UTILS                           ../../Device/src/Utils/*.*
                                              ../../common/Utils/*.*)
file(GLOB UTILS_CONTAINERS                    ../../common/Utils/Containers/*.*)
file(GLOB GUI                                          src/GUI/*.*)
file(GLOB GUI_VCP                                      src/GUI/VCP/*.*)
file(GLOB RESOURCES                       resources/*)

set_source_files_properties(resources/*
                            PROPERTIES LANGUAGE RC)

set_source_files_properties(../../Device/src/Ethernet/Ethernet.cpp
                            ../../Device/src/Menu/Pages/PageHelp.cpp
                                         src/Hardware/HAL/HAL_ROM_win.cpp
                            ../../Device/src/Hardware/VCP/usbd_conf.cpp
                            PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(libs/CMSIS                 FILES ${LIBS_CMSIS})
source_group(libs/FatFS                 FILES ${LIBS_FATFS})
source_group(libs/HAL                   FILES ${LIBS_HAL})
source_group(libs/USBD                  FILES ${LIBS_USBD})
source_group(libs/USBH                  FILES ${LIBS_USBH})
source_group(Data                       FILES ${DATA})
source_group(Data/InputStorage          FILES ${DATA_INPUTSTORAGE})
source_group(Data/OutputStorage         FILES ${DATA_OUTPUTSTORAGE})
source_group(Display                    FILES ${DISPLAY})
source_group(Display/Font               FILES ${DISPLAY_FONT})
source_group(Display/Painter            FILES ${DISPLAY_PAINTER})
source_group(Display/Screen             FILES ${DISPLAY_SCREEN})
source_group(FPGA                       FILES ${FPGA})
source_group(GUI                        FILES ${GUI})
source_group(GUI/VCP                    FILES ${GUI_VCP})
source_group(Hardware                   FILES ${HARDWARE})
source_group(Hardware/HAL               FILES ${HARDWARE_HAL})
source_group(Hardware/LAN               FILES ${HARDWARE_LAN})
source_group(Hardware/Memory            FILES ${HARDWARE_MEMORY})
source_group(Hardware/VCP               FILES ${HARDWARE_VCP})
source_group(Hardware/FDrive            FILES ${HARDWARE_FDRIVE})
source_group(Menu                       FILES ${MENU})
source_group(Menu/Items                 FILES ${MENU_ITEMS})
source_group(Menu/Pages                 FILES ${MENU_PAGES})
source_group(Menu/Pages/Cursors         FILES ${MENU_PAGES_CURSORS})
source_group(Menu/Pages/Debug           FILES ${MENU_PAGES_DEBUG})
source_group(Menu/Pages/Display         FILES ${MENU_PAGES_DISPLAY})
source_group(Menu/Pages/Measures        FILES ${MENU_PAGES_MEASURES})
source_group(Menu/Pages/Memory          FILES ${MENU_PAGES_MEMORY})
source_group(Menu/Pages/Service         FILES ${MENU_PAGES_SERVICE})
source_group(Menu/Pages/Service/Math    FILES ${MENU_PAGES_SERVICE_MATH})
source_group(Panel                      FILES ${PANEL})
source_group(Resources                  FILES ${RESOURCES})
source_group(Settings                   FILES ${SETTINGS})
source_group(src                        FILES ${SRC})
source_group(SCPI                       FILES ${SCPI})
source_group(SCPI/Process               FILES ${SCPI_PROCESS})
source_group(Utils                      FILES ${UTILS})
source_group(Utils/Containers           FILES ${UTILS_CONTAINERS})

set_source_files_properties(    ../../common/Hardware/Memory/ROM_Data_.cpp
                                ../../common/Display/Colors_.cpp
                                ../../common/Hardware/Memory/ROM_Settings_.cpp
                                ../../common/Hardware/Memory/Sector_.cpp
                                ../../common/Display/Painter/Painter_stm_d_.cpp
                                ../../common/Hardware/HAL/HAL_.cpp
                                ../../common/Hardware/HAL/HAL_ADC1_.cpp
                                ../../common/Hardware/HAL/HAL_DAC1_.cpp
                                ../../common/Hardware/HAL/HAL_DAC2_.cpp
                                ../../common/Hardware/HAL/HAL_LTDC_.cpp
#                                ../../common/Hardware/HAL/HAL_OTP_.cpp
#                                ../../common/Hardware/HAL/HAL_PINS_.cpp
#                                ../../common/Hardware/HAL/HAL_ROM_.cpp
#                               ../../common/Hardware/HAL/HAL_RTC_.cpp
#                               ../../common/Hardware/HAL/HAL_CRC_.cpp
#                               ../../common/Hardware/HAL/HAL_SPI1_.cpp
#                               ../../common/Hardware/HAL/HAL_SPI5_.cpp
#                               ../../common/Hardware/HAL/HAL_TIM2_.cpp
#                               ../../common/Hardware/HAL/HAL_TIM6_.cpp
#                               ../../common/Hardware/HAL/HAL_TIM7_.cpp
                            ../../Device/src/Menu/Pages/Display/PageAveraging.cpp
                            ../../Device/src/Menu/Pages/Service/Math/PageFFT.cpp
                            ../../Device/src/Menu/Pages/Memory/PageLatest.cpp
                            ../../Device/src/Menu/Pages/Memory/PageInternal.cpp
                            ../../Device/src/Menu/Pages/Memory/PageSetMask.cpp
                            ../../Device/src/Menu/Pages/Display/PageGrid.cpp
                            ../../Device/src/Menu/Pages/Service/PageCursorsFFT.cpp
                            ../../Device/src/Menu/Pages/Memory/PageSetName.cpp
                            ../../Device/src/Menu/Pages/Service/PageInformation.cpp
                            ../../Device/src/Menu/Pages/Debug/PageADCBalance.cpp
                            ../../Device/src/Menu/Pages/Service/Math/PageFunction.cpp
                            ../../Device/src/Hardware/Memory/Sector.cpp
                            ../../Device/src/Settings/SettingsDisplay.cpp
                            ../../Device/src/Settings/SettingsNRST.cpp
                            ../../Device/src/FPGA/FreqMeter.cpp
                            ../../Device/src/FPGA/MathFPGA.cpp
                            ../../Device/src/FPGA/ReaderFPGA.cpp
                            ../../Device/src/FPGA/SettingsFPGA.cpp
                            ../../Device/src/FPGA/StructuresFPGA.cpp
                            ../../Device/src/Display/Screen/MemoryWindow.cpp
                            ../../Device/src/Hardware/VCP/USBD_.cpp
                            ../../Device/src/Hardware/VCP/VCP_.cpp
                            ../../Device/src/Menu/FileManager.cpp
                            ../../Device/src/Menu/MenuDrawing.cpp
                            ../../Device/src/Menu/Items/MenuItemsGraphics.cpp
                            ../../Device/src/FPGA/AutoFinderFPGA.cpp
                            ../../Device/src/Hardware/VCP/usbd_cdc_interface.cpp
                            ../../Device/src/Hardware/VCP/usbd_desc.cpp
                            ../../Device/src/FPGA/CalibrationFPGA.cpp
                            ../../Device/src/FPGA/FPGA.cpp
                                         src/GUI/Painter_win.cpp
                                         src/GUI/Application.cpp
                                         src/GUI/Dialog.cpp
                                         src/GUI/Governor.cpp
                                         src/GUI/Panel_win.cpp
                                         src/GUI/TuneGeneratorDialog.cpp
                                         src/Display/Display_win.cpp
                                         src/Display/Primitives_win.cpp
                                         src/main_win.cpp
                                         src/Hardware/LAN/LAN_win.cpp
                                         src/Hardware/FDrive/FDrive_win.cpp
                                         src/Hardware/HAL/HAL_FMC_win.cpp
                                         src/GUI/ConsoleSCPI.cpp
                                         src/GUI/VCP/ComPort.cpp
                            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

add_executable(${PROJECT_NAME} ${LIBS_CMSIS} ${LIBS_FATFS}
          ${LIBS_HAL} ${LIBS_USBD} ${LIBS_USBH} ${DATA} ${DATA_INPUTSTORAGE} ${DATA_OUTPUTSTORAGE}
          ${DISPLAY}
          ${DISPLAY_FONT} ${DISPLAY_PAINTER} ${DISPLAY_SCREEN} ${FPGA}
          ${GUI}
          ${GUI_VCP} ${HARDWARE}
          ${HARDWARE_HAL} ${HARDWARE_LAN} ${HARDWARE_MEMORY} ${HARDWARE_VCP} ${HARDWARE_FDRIVE} ${MENU} ${MENU_ITEMS}
          ${MENU_PAGES} ${MENU_PAGES_CURSORS} ${MENU_PAGES_DEBUG} ${MENU_PAGES_DISPLAY}
          ${MENU_PAGES_MEMORY} ${MENU_PAGES_MEASURES}
          ${MENU_PAGES_SERVICE} ${MENU_PAGES_SERVICE_MATH}
          ${PANEL} ${RESOURCES}
          ${SETTINGS}
          ${SRC} ${SCPI} ${SCPI_PROCESS} ${UTILS} ${UTILS_CONTAINERS})

target_link_libraries(${PROJECT_NAME} wxbase31ud wxmsw31ud_core)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxbase315ud_vc_custom.dll $(OutputPath) /Y)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxmsw315ud_core_vc_custom.dll $(OutputPath) /Y)

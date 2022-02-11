# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Loader)

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

add_definitions(-DSTM32F429xx -DUSE_HAL_DRIVER -DLOADER)

if(WIN32)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    
    add_compile_options(/wd4201)    # nonstandard extension used : nameless struct/union
    add_compile_options(/wd4505)    # 'function': unreferenced function with internal linkage has been removed
    add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
    add_compile_options(/wd4623)    # 'derived class' : default constructor was implicitly defined as deleted because a base
                                    # class default constructor is inaccessible or deleted
    add_compile_options(/wd4626)    # 'derived class' : assignment operator was implicitly defined as deleted because a base
                                    # class assignment operator is inaccessible or deleted
    add_compile_options(/wd4774)    # 'function' : format string expected in argument 3 is not a string literal
    add_compile_options(/wd4820)    # 'bytes' bytes padding added after construct 'member_name'
    add_compile_options(/wd5027)    # 'derived class' : move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5039)    # 'function': pointer or reference to potentially throwing function passed to 'extern
                                    # "C"' function under -EHc. Undefined behavior may occur if this function throws an
                                    # exception
    add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    add_compile_options(/wd5219)    # implicit conversion from 'type1' to 'type2', possible loss of data
    add_compile_options(/wd5220)    #
    add_compile_options(/wd26812)

else()

    add_definitions(-Wno-builtin-declaration-mismatch
                    -Wno-write-strings
    )

endif()

file(GLOB SRC             src/*.cpp src/*.h)

source_group(src          FILES ${SRC})

add_library(${PROJECT_NAME} ${SRC})

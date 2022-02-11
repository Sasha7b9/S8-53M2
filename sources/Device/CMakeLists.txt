# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

set(CMAKE_UNITY_BUILD OFF)

include_directories(../common/_VS_KEIL/ARMCLANG
                    ../common/_VS_KEIL/CMSIS
                    ../common/_VS_KEIL/FatFS
                    ../STM32Cube_FW_F4/Drivers/STM32F4xx_HAL_Driver/Inc
                    ../STM32Cube_FW_F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
                    src
                    src/libs
)

add_definitions(-DSTM32F429xx -DUSE_HAL_DRIVER -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

    add_compile_options(/wd4103)    # alignment changed after including header, may be due to missing #pragma pack(pop)
    add_compile_options(/wd4201)    # nonstandard extension used : nameless struct/union
    add_compile_options(/wd4211)    # nonstandard extension used : redefined extern to static
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

file(GLOB SRC        src/*.*)

source_group(src     FILES ${SRC})


add_library(${PROJECT_NAME} ${SRC})

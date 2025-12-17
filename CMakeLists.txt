cmake_minimum_required(VERSION 3.19)
project(NPass LANGUAGES C CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(NPass
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logic.h
    logic.cpp
    aes.h
    aes.c

)

target_link_libraries(NPass
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS NPass
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")

    set(ASAN_FLAGS
        -fsanitize=address
        -fno-omit-frame-pointer
        -g
    )

    add_compile_options(${ASAN_FLAGS})
    add_link_options(-fsanitize=address)
endif()
qt_generate_deploy_app_script(
    TARGET NPass
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

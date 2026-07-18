cmake_minimum_required(VERSION 3.24)

project(Vacuo
    VERSION 0.1.0
    DESCRIPTION "Native, safety-first Linux cache cleaner"
    HOMEPAGE_URL "https://github.com/Trendorin/Vacuo"
    LANGUAGES CXX
)

include(CTest)
include(GNUInstallDirs)
include(CheckPIESupported)
find_package(Threads REQUIRED)

option(VACUO_BUILD_GUI "Build the Qt 6 Widgets interface" ON)
option(VACUO_BUILD_HELPER "Build the restricted PolicyKit helper" ON)
option(VACUO_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(VACUO_WARNINGS_AS_ERRORS "Treat project warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(vacuo_harden target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
            $<$<BOOL:${VACUO_WARNINGS_AS_ERRORS}>:-Werror>
            $<$<NOT:$<CONFIG:Debug>>:-fstack-protector-strong>
        )
        target_compile_definitions(${target} PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:_FORTIFY_SOURCE=3>
        )
        target_link_options(${target} PRIVATE
            -Wl,-z,relro
            -Wl,-z,now
            -Wl,--as-needed
        )
        if(VACUO_ENABLE_SANITIZERS)
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

add_library(vacuo-core STATIC
    src/core/types.cpp
    src/core/system_detector.cpp
    src/core/rule_catalog.cpp
    src/core/scanner.cpp
    src/core/safe_delete.cpp
    src/core/process_runner.cpp
    src/core/helper_actions.cpp
    src/core/cleaner.cpp
    src/core/json.cpp
)
target_include_directories(vacuo-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(vacuo-core PUBLIC Threads::Threads)
target_compile_features(vacuo-core PUBLIC cxx_std_20)
set_target_properties(vacuo-core PROPERTIES POSITION_INDEPENDENT_CODE ON)
vacuo_harden(vacuo-core)

add_executable(vacuoctl src/cli/main.cpp)
target_link_libraries(vacuoctl PRIVATE vacuo-core)
set_target_properties(vacuoctl PROPERTIES POSITION_INDEPENDENT_CODE ON)
vacuo_harden(vacuoctl)

if(VACUO_BUILD_HELPER)
    add_executable(vacuo-helper src/helper/main.cpp)
    target_link_libraries(vacuo-helper PRIVATE vacuo-core)
    set_target_properties(vacuo-helper PROPERTIES POSITION_INDEPENDENT_CODE ON)
    vacuo_harden(vacuo-helper)

    configure_file(
        data/polkit/io.github.trendorin.Vacuo.policy.in
        ${CMAKE_CURRENT_BINARY_DIR}/io.github.trendorin.Vacuo.policy
        @ONLY
    )
endif()

if(VACUO_BUILD_GUI)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Concurrent)
    qt_standard_project_setup()

    qt_add_executable(vacuo
        src/gui/main.cpp
        src/gui/main_window.cpp
        src/gui/main_window.hpp
        src/gui/scan_model.cpp
        src/gui/scan_model.hpp
    )
    target_link_libraries(vacuo PRIVATE vacuo-core Qt6::Widgets Qt6::Concurrent)
    set_target_properties(vacuo PROPERTIES
        OUTPUT_NAME vacuo
        POSITION_INDEPENDENT_CODE ON
    )
    vacuo_harden(vacuo)
endif()

if(BUILD_TESTING)
    add_executable(vacuo-core-tests tests/test_main.cpp)
    target_link_libraries(vacuo-core-tests PRIVATE vacuo-core)
    vacuo_harden(vacuo-core-tests)
    add_test(NAME vacuo-core COMMAND vacuo-core-tests)
    set_tests_properties(vacuo-core PROPERTIES TIMEOUT 60)
endif()

install(TARGETS vacuoctl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(VACUO_BUILD_GUI)
    install(TARGETS vacuo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()
if(VACUO_BUILD_HELPER)
    install(TARGETS vacuo-helper
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.github.trendorin.Vacuo.policy
        DESTINATION ${CMAKE_INSTALL_DATADIR}/polkit-1/actions
    )
endif()

install(FILES data/io.github.trendorin.Vacuo.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
install(FILES data/io.github.trendorin.Vacuo.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
)
install(FILES data/icons/hicolor/scalable/apps/io.github.trendorin.Vacuo.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)
install(FILES README.md README_RU.md CHANGELOG.md SECURITY.md docs/PRIVACY.md docs/SECURITY_MODEL.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/vacuo
)

include(cmake/VacuoPackaging.cmake)

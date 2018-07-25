cmake_minimum_required (VERSION 3.10)
project (MandelbrotExplorer)
# The version number.
set (MandelbrotExplorer_VERSION_MAJOR 1)
set (MandelbrotExplorer_VERSION_MINOR 0)

find_package(wxWidgets 3.0 COMPONENTS core base adv propgrid REQUIRED)
include(${wxWidgets_USE_FILE})

include_directories(include)

IF(MSVC)
    # using Visual Studio C++
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

set(SOURCES src/main.cpp
	src/mandel_explore_frame.cpp
	src/mandel_properties_panel.cpp
	src/custom_resolution_dialog.cpp
	src/regex_validator.cpp
	src/fractal_render_panel.cpp
	src/palette_panel.cpp
  src/wave_control.cpp
  src/waves_color_mapper.cpp
  src/waves_palette_panel.cpp)

add_executable(mandelex ${SOURCES})

target_link_libraries(mandelex
    ${wxWidgets_LIBRARIES}
    )
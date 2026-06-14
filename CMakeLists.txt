# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.22)


project(KotoNotation VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        v4.6.2
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

add_subdirectory("JUCE" JUCE)

juce_add_gui_app(KotoNotation
    PRODUCT_NAME "Koto score creator"
)

target_sources(KotoNotation
    PRIVATE
        KotoNotation/KotoNotation.cpp
 )

 file(GLOB plugin_assets
    "assets/*.*")

juce_add_binary_data(BinaryData
    SOURCES 
    ${plugin_assets})

target_link_libraries(KotoNotation
    PRIVATE
    BinaryData
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_audio_basics
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_core
        juce::juce_graphics
        PDFHummus::PDFWriter
)

target_compile_definitions(KotoNotation
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)


# This project builds a separate tool for visualising TIN data
# It requires Qt libraries to be present on the build system

cmake_minimum_required(VERSION 3.15)

project(TinVisualiser)




find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)





qt_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES cs_CZ
    I18N_SOURCE_LANGUAGE en_US
)





add_subdirectory(Src)

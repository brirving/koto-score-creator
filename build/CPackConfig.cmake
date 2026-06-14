# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_ARCHIVE_GID "-1")
set(CPACK_ARCHIVE_UID "-1")
set(CPACK_BUILD_SOURCE_DIRS "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation;C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build")
set(CPACK_CMAKE_GENERATOR "Visual Studio 18 2026")
set(CPACK_COMPONENTS_ALL "Unspecified;dependencies;libraries")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-4.3/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "KotoNotation built using CMake")
set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE "ON")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INNOSETUP_ARCHITECTURE "x64")
set(CPACK_INSTALL_CMAKE_PROJECTS "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build;KotoNotation;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Program Files (x86)/KotoNotation")
set(CPACK_MODULE_PATH "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-src/cmake")
set(CPACK_NSIS_DISPLAY_NAME "KotoNotation 4.6.0")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_PACKAGE_NAME "KotoNotation 4.6.0")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-4.3/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KotoNotation built using CMake")
set(CPACK_PACKAGE_FILE_NAME "KotoNotation-4.6.0-win64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KotoNotation 4.6.0")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "KotoNotation 4.6.0")
set(CPACK_PACKAGE_NAME "KotoNotation")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "4.6.0")
set(CPACK_PACKAGE_VERSION_MAJOR "4")
set(CPACK_PACKAGE_VERSION_MINOR "6")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-src/LICENSE")
set(CPACK_RESOURCE_FILE_README "C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-4.3/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-4.3/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/CPackSourceConfig.cmake")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win64")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-src")
  file(MAKE_DIRECTORY "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-build"
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix"
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/tmp"
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/src/pdfhummus-populate-stamp"
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/src"
  "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/src/pdfhummus-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/src/pdfhummus-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/beei/Documents/Uni/NZSM496/KotoNotation/build/_deps/pdfhummus-subbuild/pdfhummus-populate-prefix/src/pdfhummus-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

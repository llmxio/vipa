cmake_minimum_required(VERSION 3.16)
project(VIPA LANGUAGES CXX VERSION 0.1.0)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(vipa INTERFACE)
add_library(llmx::vipa ALIAS vipa)

target_include_directories(vipa INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vipa>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(vipa INTERFACE cxx_std_23)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vipa"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

file(GLOB VIPA_PUBLIC_HDRS_TOP
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx"
)
if(VIPA_PUBLIC_HDRS_TOP)
  install(FILES ${VIPA_PUBLIC_HDRS_TOP} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vipa)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

install(TARGETS vipa
        EXPORT VIPATargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/VIPA")

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VIPAConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/VIPAConfig.cmake
  INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/VIPAConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/VIPAConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/VIPAConfigVersion.cmake
  DESTINATION ${CONFIG_INSTALL_DIR})

install(EXPORT VIPATargets
  FILE VIPATargets.cmake
  NAMESPACE llmx::
  DESTINATION ${CONFIG_INSTALL_DIR})

add_custom_target(export-vipa DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/VIPAConfig.cmake")

option(BUILD_TESTING "Enable tests" OFF)
if(BUILD_TESTING)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

option(BUILD_BENCHMARKING "Enable benchmark targets" OFF)
if(BUILD_BENCHMARKING)
  add_subdirectory(benchmarks)
endif()

option(BUILD_DOCS "Enable generating API documentation (Doxygen)" OFF)
if(BUILD_DOCS)
  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is required to build docs. Install Doxygen or set BUILD_DOCS=OFF.")
  endif()

  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
          DESTINATION ${CMAKE_INSTALL_DOCDIR}
          OPTIONAL)
endif()

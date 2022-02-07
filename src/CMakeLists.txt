project (jmedia)

include (${PROJECT_SOURCE_DIR}/../Defines.cmake)

set(SRCS
  jaudioconfigurationcontrol.cpp
  jaudiomixercontrol.cpp
  jcolorconversion.cpp
  jcontrol.cpp
  jframegrabberevent.cpp
  jframegrabberlistener.cpp
  jmedialib.cpp
  jplayer.cpp
  jplayerevent.cpp
  jplayerlistener.cpp
  jplayermanager.cpp
  jsynthesizer.cpp
  jvideodevicecontrol.cpp
  jvideoformatcontrol.cpp
  jvideosizecontrol.cpp
  jvolumecontrol.cpp
)

include (cmake/MediaProviders.cmake)

# find libcairo
pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)

# find jcanvas
pkg_check_modules(jCanvas REQUIRED IMPORTED_TARGET jcanvas)

# find jdemux
pkg_check_modules(jDemux REQUIRED IMPORTED_TARGET jdemux)

# core compilation
add_library(${PROJECT_NAME} SHARED 
  ${SRCS} 
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    ${MEDIA_PROVIDERS_CFLAGS} 
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_directories(${PROJECT_NAME}
  PRIVATE
    ${MEDIA_PROVIDERS_LIBRARY_DIRS} 
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    ${MEDIA_PROVIDERS_LIBRARIES}
  PUBLIC
    PkgConfig::Cairo
    PkgConfig::jCanvas
    PkgConfig::jDemux
)

message ("\tMedia Providers: ${MEDIA_LIST}")
message ("\t\tInclude Directories: ${MEDIA_PROVIDERS_INCLUDE_DIRS}")
message ("\t\tLibrary Directories: ${MEDIA_PROVIDERS_LIBRARY_DIRS}")
message ("\t\tLibraries: ${MEDIA_PROVIDERS_LIBRARIES}")

install(DIRECTORY include/jmedia DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


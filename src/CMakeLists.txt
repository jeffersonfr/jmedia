project (jmedia)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

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
pkg_check_modules(CAIRO REQUIRED cairo)

# find jcanvas
pkg_check_modules(JCANVAS REQUIRED jcanvas)

# find jdemux
pkg_check_modules(JDEMUX REQUIRED jdemux)

# core compilation
add_library(${PROJECT_NAME} SHARED 
  ${SRCS} 
)

include_directories(
  "${CMAKE_SOURCE_DIR}/src/include"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
  ${MEDIA_PROVIDERS_CFLAGS} 
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CAIRO_INCLUDE_DIRS}
  ${JCANVAS_INCLUDE_DIRS}
  ${JDEMUX_INCLUDE_DIRS}
  ${MEDIA_PROVIDERS_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${MEDIA_PROVIDERS_LIBRARY_DIRS} 
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${CAIRO_LIBRARIES}
  ${JCANVAS_LIBRARIES}
  ${JDEMUX_LIBRARIES}
  ${MEDIA_PROVIDERS_LIBRARIES}
)

message ("\tMedia Providers: ${MEDIA_LIST}")
message ("\t\tInclude Directories: ${MEDIA_PROVIDERS_INCLUDE_DIRS}")
message ("\t\tLibrary Directories: ${MEDIA_PROVIDERS_LIBRARY_DIRS}")
message ("\t\tLibraries: ${MEDIA_PROVIDERS_LIBRARIES}")

install(DIRECTORY src/include/jmedia DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


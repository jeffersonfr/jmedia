set (MEDIA_PROVIDER_LIST)

# gif
target_sources(${PROJECT_NAME} PRIVATE providers/gif/bind.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE GIF_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST gif)

# ilist
target_sources(${PROJECT_NAME} PRIVATE providers/ilist/bind.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE ILIST_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST ilist)

# alsa
pkg_check_modules(Alsa IMPORTED_TARGET alsa)

if (Alsa_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE providers/alsa/bind.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::Alsa)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ALSA_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST alsa)
endif()

# libvlc
pkg_check_modules(LibVlc IMPORTED_TARGET libvlc)

if (LibVlc_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE providers/libvlc/bind.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibVlc)
  target_compile_definitions(${PROJECT_NAME} PRIVATE LIBVLC_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST libvlc)
endif()

# libxine
pkg_check_modules(LibVlc IMPORTED_TARGET libxine)

if (LibXine_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE providers/libxine/bind.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibXine)
  target_compile_definitions(${PROJECT_NAME} PRIVATE LIBXINE_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST libxine)
endif()

# gstreamer
pkg_check_modules(GstreamerApp IMPORTED_TARGET gstreamer-app-1.0)
pkg_check_modules(GstreamerVideo IMPORTED_TARGET gstreamer-video-1.0)

if (GstreamerApp_FOUND AND GstreamerVideo_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE providers/gstreamer/bind.cpp)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
      PkgConfig::GstreamerApp
      PkgConfig::GstreamerVideo)
  target_compile_definitions(${PROJECT_NAME} PRIVATE GSTREAMER_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST gstreamer)
endif()

# libav
pkg_check_modules(LibAvCodec IMPORTED_TARGET libavcodec)
pkg_check_modules(LibAvDevice IMPORTED_TARGET libavdevice)
pkg_check_modules(LibAvFilter IMPORTED_TARGET libavfilter)
pkg_check_modules(LibAvResample IMPORTED_TARGET libavresample)

if (LibAvCodec_FOUND AND LibAvDevice_FOUND AND LibAvFilter_FOUND AND LibAvResample_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE providers/libav/bind.cpp)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
      PkgConfig::LibAvCodec
      PkgConfig::LibAvDevice
      PkgConfig::LibAvFilter
      PkgConfig::LibAvResample)
  target_compile_definitions(${PROJECT_NAME} PRIVATE LIBAV_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST libav)
endif()

# v4l2
pkg_check_modules(LibV4l2 IMPORTED_TARGET libv4l2)

if (LibV4l2_FOUND)
  target_sources(${PROJECT_NAME}
    PRIVATE
      providers/v4l2/bind.cpp
      providers/v4l2/videocontrol.cpp
      providers/v4l2/videograbber.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibV4l2)
  target_compile_definitions(${PROJECT_NAME} PRIVATE V4L2_MEDIA)
  list(APPEND MEDIA_PROVIDER_LIST v4l2)
endif()

message ("\tProviders: ${MEDIA_PROVIDER_LIST}")

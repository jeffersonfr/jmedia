set (MEDIA_LIST)

# gif
set(MEDIA_PROVIDERS_SRCS 
  ${MEDIA_PROVIDERS_SRCS}
  providers/gif/bind.cpp
)

add_definitions("-DGIF_MEDIA")

set (MEDIA_LIST ${MEDIA_LIST} gif)

# ilist
set(MEDIA_PROVIDERS_SRCS 
  ${MEDIA_PROVIDERS_SRCS}
  providers/ilist/bind.cpp
)

add_definitions("-DILIST_MEDIA")

set (MEDIA_LIST ${MEDIA_LIST} ilist)

# alsa
pkg_check_modules(ALSA alsa)

if (${ALSA_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/alsa/bind.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${ALSA_LIBRARIES} 
  )

  add_definitions("-DALSA_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} alsa)
endif()

# libvlc
pkg_check_modules(LIBVLC libvlc)

if (${LIBVLC_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/libvlc/bind.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${LIBVLC_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${LIBVLC_LIBRARIES} 
  )

  add_definitions("-DLIBVLC_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} libvlc)
endif()

# libxine
pkg_check_modules(LIBXINE libxine)

if (${LIBXINE_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/libxine/bind.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${LIBXINE_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${LIBXINE_LIBRARIES} 
  )

  add_definitions("-DLIBXINE_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} libxine)
endif()

# gstreamer
pkg_check_modules(GSTREAMERAPP gstreamer-app-1.0)
pkg_check_modules(GSTREAMERVIDEO gstreamer-video-1.0)

if (${GSTREAMERAPP_FOUND} AND ${GSTREAMERVIDEO_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/gstreamer/bind.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${GSTREAMERAPP_INCLUDE_DIRS}
    ${GSTREAMERVIDEO_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${GSTREAMERAPP_LIBRARIES} 
    ${GSTREAMERVIDEO_LIBRARIES} 
  )

  add_definitions("-DGSTREAMER_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} gstreamer)
endif()

# libav
pkg_check_modules(LIBAVCODEC libavcodec)
pkg_check_modules(LIBAVDEVICE libavdevice)
pkg_check_modules(LIBAVFILTER libavfilter)
pkg_check_modules(LIBAVRESAMPLE libavresample)

if (${LIBAVCODEC_FOUND} AND ${LIBAVDEVICE_FOUND} AND ${LIBAVFILTER_FOUND} AND ${LIBAVRESAMPLE_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/libav/bind.cpp
    providers/libav/libavplay.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${LIBAVCODEC_INCLUDE_DIRS}
    ${LIBAVDEVICE_INCLUDE_DIRS}
    ${LIBAVFILTER_INCLUDE_DIRS}
    ${LIBAVRESAMPLE_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${LIBAVCODEC_LIBRARIES} 
    ${LIBAVDEVICE_LIBRARIES} 
    ${LIBAVFILTER_LIBRARIES} 
    ${LIBAVRESAMPLE_LIBRARIES} 
  )

  add_definitions("-DLIBAV_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} libav)
endif()

# v4l2
pkg_check_modules(LIBV4L2 libv4l2)

if (${LIBV4L2_FOUND})
  set(MEDIA_PROVIDERS_SRCS 
    ${MEDIA_PROVIDERS_SRCS}
    providers/v4l2/bind.cpp
    providers/v4l2/videocontrol.cpp
    providers/v4l2/videograbber.cpp
  )

  set(MEDIA_PROVIDERS_INCLUDE_DIRS 
    ${MEDIA_PROVIDERS_INCLUDE_DIRS}
    ${LIBV4L2_INCLUDE_DIRS}
  )
  
  set(MEDIA_PROVIDERS_LIBRARIES
    ${MEDIA_PROVIDERS_LIBRARIES}
    ${LIBV4L2_LIBRARIES} 
  )

add_definitions("-DV4L2_MEDIA")

  set (MEDIA_LIST ${MEDIA_LIST} v4l2)
endif()

set(SRCS ${SRCS} ${MEDIA_PROVIDERS_SRCS})

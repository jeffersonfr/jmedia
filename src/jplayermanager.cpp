/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jmedia/jplayermanager.h"

#include "jdemux/jurl.h"

#if defined(LIBVLC_MEDIA)
#include "providers/libvlc/bind.h"
#endif

#if defined(LIBAV_MEDIA)
#include "providers/libav/bind.h"
#endif

#if defined(LIBXINE_MEDIA)
#include "providers/libxine/bind.h"
#endif

#if defined(V4L2_MEDIA)
#include "providers/v4l2/bind.h"
#endif

#if defined(GIF_MEDIA)
#include "providers/gif/bind.h"
#endif

#if defined(IMAGE_LIST_MEDIA)
#endif
#include "providers/ilist/bind.h"

#if defined(ALSA_MEDIA)
#include "providers/alsa/bind.h"
#endif

#if defined(GSTREAMER_MEDIA)
#include "providers/gstreamer/bind.h"
#endif

namespace jmedia {

std::map<jplayer_hints_t, bool> PlayerManager::_hints;

PlayerManager::PlayerManager()
{
}

PlayerManager::~PlayerManager()
{
}

Player * PlayerManager::CreatePlayer(std::string uri)
{
  if (_hints.size() == 0) {
    _hints[jplayer_hints_t::Caching] = false;
    _hints[jplayer_hints_t::Lightweight] = true;
    _hints[jplayer_hints_t::Security] = false;
    _hints[jplayer_hints_t::Plugins] = false;
  }

  jdemux::Url url{uri};

#if defined(V4L2_MEDIA)
  try {
    if (url.Protocol() == "v4l2") {
      return new V4L2LightPlayer(url.Path());
    }
  } catch (std::runtime_error &e) {
    return nullptr;
  }
#endif

#if defined(ALSA_MEDIA)
  try {
    return new AlsaLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

#if defined(IMAGE_LIST_MEDIA)
  try {
    return new ImageListLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

#if defined(GIF_MEDIA)
  try {
    return new GIFLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

#if defined(LIBVLC_MEDIA)
  try {
    return new LibVLCLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

#if defined(LIBAV_MEDIA)
  try {
    return new LibAVLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif
  
#if defined(LIBXINE_MEDIA)
  try {
    return new LibXineLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

#if defined(GSTREAMER_MEDIA)
  try {
    return new GStreamerLightPlayer(uri);
  } catch (std::runtime_error &e) {
  }
#endif

  return nullptr;
}
    
void PlayerManager::SetHint(jplayer_hints_t hint, bool value)
{
  _hints[hint] = value;
}

bool PlayerManager::GetHint(jplayer_hints_t hint)
{
  std::map<jplayer_hints_t, bool>::iterator i = _hints.find(hint);

  if (i != _hints.end()) {
    return i->second;
  }

  return false;
}

}


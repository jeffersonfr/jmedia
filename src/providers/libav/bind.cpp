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
#include "../providers/libav/bind.h"
#include "../providers/libav/libavplay.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"

#include "jcanvas/core/jbufferedimage.h"

#include "jdemux/jurl.h"

#include <cairo.h>

namespace jmedia {

class LibavPlayerComponentImpl : public jcanvas::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		cairo_surface_t *_surface;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		jcanvas::jrect_t<int> _src;
		/** \brief */
		jcanvas::jrect_t<int> _dst;
		/** \brief */
		jcanvas::jpoint_t<int> _frame_size;

	public:
		LibavPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jcanvas::Component({x, y, w, h})
		{
			_surface = nullptr;
			_player = player;
			
			_frame_size.x = w;
			_frame_size.y = h;

			_src = {
        0, 0, w, h
      };

			_dst = {
        0, 0, w, h
      };

			SetVisible(true);
		}

		virtual ~LibavPlayerComponentImpl()
		{
      if (_surface != nullptr) {
        cairo_surface_destroy(_surface);
      }
		}

		virtual jcanvas::jpoint_t<int> GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(uint8_t *buffer, int width, int height)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_src.size.x <= 0 || _src.size.y <= 0) {
				dynamic_cast<LibAVLightPlayer *>(_player)->_aspect = (double)width/(double)height;

				_frame_size.x = _src.size.x = width;
				_frame_size.y = _src.size.y = height;
			}

			int sw = width;
			int sh = height;

			_mutex.lock();

			_surface = cairo_image_surface_create_for_data(
					(uint8_t *)buffer, CAIRO_FORMAT_RGB24, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, sw));
			
      _mutex.unlock();

      Repaint();
		}

		virtual void Paint(jcanvas::Graphics *g)
		{
			jcanvas::Component::Paint(g);

      jcanvas::jpoint_t<int>
        size = GetSize();

			_mutex.lock();

      if (_surface == nullptr) {
        _mutex.unlock();

        return;
      }

      std::shared_ptr<jcanvas::Image> image = std::make_shared<jcanvas::BufferedImage>(_surface);

			_player->DispatchFrameGrabberEvent(new jmedia::FrameGrabberEvent(image, jmedia::jframeevent_type_t::Grab));

			cairo_surface_mark_dirty(_surface);

	    g->SetAntialias(jcanvas::jantialias_t::None);
	    g->SetCompositeFlags(jcanvas::jcomposite_flags_t::Src);
	    g->SetBlittingFlags(jcanvas::jblitting_flags_t::Nearest);

      if (_src.point.x == 0 and _src.point.y == 0 and _src.size.x == _frame_size.x and _src.size.y == _frame_size.y) {
			  g->DrawImage(image, {0, 0, size.x, size.y});
      } else {
			  g->DrawImage(image, _src, {0, 0, size.x, size.y});
      }

      cairo_surface_destroy(_surface);

      _surface = nullptr;

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class LibavVolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibAVLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		LibavVolumeControlImpl(LibAVLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;

			SetLevel(100);
		}

		virtual ~LibavVolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			int level = 0;

			/*
			if (_player->_provider != nullptr) {
				level = libvlc_audio_get_volume(_player->_provider);
			}
			*/

			return level;
		}

		virtual void SetLevel(int level)
		{
			_level = (level < 0)?0:(level > 100)?100:level;

			if (_level != 0) {
				_is_muted = true;
			} else {
				_is_muted = false;
			}

			/*
			if (_player->_provider != nullptr) {
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
				libvlc_audio_set_volume(_player->_provider, _level);
			}
			*/
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);
	
			_is_muted = b;
			
			if (_player->_provider != nullptr) {
				avplay_mute(_player->_provider, _is_muted);
			}
		}

};

class LibavVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibAVLightPlayer *_player;

	public:
		LibavVideoSizeControlImpl(LibAVLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~LibavVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
      std::shared_ptr<LibavPlayerComponentImpl> impl = std::dynamic_pointer_cast<LibavPlayerComponentImpl>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);
			
			impl->_src = {
        x, y, w, h
      };
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
      std::shared_ptr<LibavPlayerComponentImpl> impl = std::dynamic_pointer_cast<LibavPlayerComponentImpl>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jcanvas::jrect_t<int> GetSource()
		{
      return std::dynamic_pointer_cast<LibavPlayerComponentImpl>(_player->_component)->_src;
		}

		virtual jcanvas::jrect_t<int> GetDestination()
		{
      return std::dynamic_pointer_cast<LibavPlayerComponentImpl>(_player->_component)->GetBounds();
		}

};

class LibavVideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibAVLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		LibavVideoFormatControlImpl(LibAVLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = jvideo_mode_t::Full;
			_hd_video_format = jhd_video_format_t::Format1080p;
			_sd_video_format = jsd_video_format_t::PalM;
		}

		virtual ~LibavVideoFormatControlImpl()
		{
		}

		virtual void SetContentMode(jvideo_mode_t t)
		{
			_video_mode = t;
		}

		virtual void SetVideoFormatHD(jhd_video_format_t vf)
		{
			_hd_video_format = vf;
		}

		virtual void SetVideoFormatSD(jsd_video_format_t vf)
		{
			_sd_video_format = vf;
		}

		virtual jaspect_ratio_t GetAspectRatio()
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			double aspect = _player->_aspect;

			if (aspect == (1.0/1.0)) {
				return jaspect_ratio_t::Square;
			} else if (aspect == (3.0/2.0)) {
				return jaspect_ratio_t::Low;
			} else if (aspect == (4.0/3.0)) {
				return jaspect_ratio_t::Standard;
			} else if (aspect == (16.0/9.0)) {
			  return jaspect_ratio_t::Wide;
			}

			return jaspect_ratio_t::Wide;
		}

		virtual double GetFramesPerSecond()
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			return _player->_provider->frames_per_second;
		}

		virtual jvideo_mode_t GetContentMode()
		{
			return jvideo_mode_t::Full;
		}

		virtual jhd_video_format_t GetVideoFormatHD()
		{
			return jhd_video_format_t::Format1080p;
		}

		virtual jsd_video_format_t GetVideoFormatSD()
		{
			return jsd_video_format_t::PalM;
		}

};

static void render_callback(void *data, uint8_t *buffer, int width, int height)
{
	reinterpret_cast<LibavPlayerComponentImpl *>(data)->UpdateComponent(buffer, width, height);
}

static void endofmedia_callback(void *data)
{
	LibAVLightPlayer *player = reinterpret_cast<LibAVLightPlayer *>(data);
	
	player->DispatchPlayerEvent(new jmedia::PlayerEvent(player, jmedia::jplayerevent_type_t::Finish));
}

LibAVLightPlayer::LibAVLightPlayer(std::string uri):
	Player()
{
	_file = jdemux::Url{uri}.Path();
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	
	avplay_init();

	_provider = avplay_open(_file.c_str());

	if (_provider == nullptr) {
		throw std::runtime_error("Cannot recognize the media file");
	}

	_component = std::make_shared<LibavPlayerComponentImpl>(this, 0, 0, -1, -1);//iw, ih);

	avplay_set_rendercallback(_provider, render_callback, (void *)_component.get());
	avplay_set_endofmediacallback(_provider, endofmedia_callback, (void *)this);
		
	if (_provider->wanted_stream[AVMEDIA_TYPE_AUDIO] != -1) {
		_controls.push_back(new LibavVolumeControlImpl(this));
	}
	
	if (_provider->wanted_stream[AVMEDIA_TYPE_VIDEO] != -1) {
		_controls.push_back(new LibavVideoSizeControlImpl(this));
		_controls.push_back(new LibavVideoFormatControlImpl(this));
	}
}

LibAVLightPlayer::~LibAVLightPlayer()
{
	Close();
	
	_component = nullptr;

  while (_controls.size() > 0) {
		Control *control = *_controls.begin();

    _controls.erase(_controls.begin());

		delete control;
	}

  delete _provider;
  _provider = nullptr;
}

void LibAVLightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _provider != nullptr) {
		avplay_play(_provider);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Start));
	}
}

void LibAVLightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _provider != nullptr) {
		_is_paused = true;
		
		avplay_pause(_provider, true);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Pause));
	}
}

void LibAVLightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == true && _provider != nullptr) {
		_is_paused = false;
		
		avplay_pause(_provider, false);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Resume));
	}
}

void LibAVLightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibAVLightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_provider != nullptr) {
		avplay_close(_provider);

		_provider = nullptr;
	}
}

void LibAVLightPlayer::SetCurrentTime(uint64_t time)
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_setcurrentmediatime(_provider, time);
	}
}

uint64_t LibAVLightPlayer::GetCurrentTime()
{
	uint64_t time = 0LL;

  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		time = (uint64_t)avplay_getcurrentmediatime(_provider);
	}

	return time;
}

uint64_t LibAVLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	if (_provider != nullptr) {
		time = (uint64_t)avplay_getmediatime(_provider);
	}

	return time;
}

void LibAVLightPlayer::SetLoop(bool b)
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_setloop(_provider, b);
	}
}

bool LibAVLightPlayer::IsLoop()
{
	if (_provider != nullptr) {
		return avplay_isloop(_provider);
	}

	return false;
}

void LibAVLightPlayer::SetDecodeRate(double rate)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != nullptr) {
		// libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double LibAVLightPlayer::GetDecodeRate()
{
  std::unique_lock<std::mutex> lock(_mutex);

	double rate = 1.0;

	if (_provider != nullptr) {
		// rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

std::shared_ptr<jcanvas::Component> LibAVLightPlayer::GetVisualComponent()
{
	return _component;
}

}

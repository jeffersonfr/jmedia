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
#include "../providers/libxine/bind.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvideodevicecontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jmedia/jcolorconversion.h"

#include "jcanvas/core/jbufferedimage.h"

#include "jdemux/jurl.h"

#include <thread>

#include <cairo.h>

namespace jmedia {

class XinePlayerComponentImpl : public jcanvas::Component {

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
    std::shared_ptr<jcanvas::Image> _buffer[2];
		/** \brief */
		int _buffer_index;
		/** \brief */
		jcanvas::jpoint_t<int> _frame_size;

	public:
		XinePlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jcanvas::Component({x, y, w, h})
		{
			_buffer_index = 0;

			_surface = nullptr;
			_player = player;
			
			_frame_size.x = -1;
			_frame_size.y = -1;

			_src = {
        0, 0, -1, -1
      };

			SetVisible(true);
		}

		virtual ~XinePlayerComponentImpl()
		{
      if (_surface != nullptr) {
        cairo_surface_destroy(_surface);
      }
		}

		virtual jcanvas::jpoint_t<int> GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(int format, int width, int height, double aspect, void *data0, void *data1, void *data2)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_buffer == nullptr or width != _frame_size.x or height != _frame_size.y) {
				_frame_size.x = width;
				_frame_size.y = height;

        if (_src.size.x < 0) {
          _src.size.x = _frame_size.x;
        }

        if (_src.size.y < 0) {
          _src.size.y = _frame_size.y;
        }

				_buffer[0] = std::make_shared<jcanvas::BufferedImage>(jcanvas::jpixelformat_t::RGB32, jcanvas::jpoint_t<int>{width, height});
				_buffer[1] = std::make_shared<jcanvas::BufferedImage>(jcanvas::jpixelformat_t::RGB32, jcanvas::jpoint_t<int>{width, height});
			}
			
      std::shared_ptr<jcanvas::Image> image = _buffer[(_buffer_index++)%2];

			uint32_t *buffer = (uint32_t *)image->LockData();

			if (format == XINE_VORAW_YV12) {
				ColorConversion::GetRGB32FromYV12((uint8_t **)&data0, (uint8_t **)&data1, (uint8_t **)&data2, (uint32_t **)&buffer, width, height);
			} else if (format == XINE_VORAW_YUY2) {
				ColorConversion::GetRGB32FromYUYV((uint8_t **)&data0, (uint32_t **)&buffer, width, height);
			} else if (format == XINE_VORAW_RGB) {
				ColorConversion::GetRGB32FromRGB24((uint8_t **)&data0, (uint32_t **)&buffer, width, height);
			} 
	
			image->UnlockData();

      Repaint();
		}

		virtual void Paint(jcanvas::Graphics *g)
		{
			// jcanvas::Component::Paint(g);

      if (_buffer[0] == nullptr or _buffer[1] == nullptr) {
        return;
      }

      jcanvas::jpoint_t<int>
        size = GetSize();

      std::shared_ptr<jcanvas::Image> image = _buffer[(_buffer_index + 1)%2];

      image->LockData();

			_player->DispatchFrameGrabberEvent(new jmedia::FrameGrabberEvent(image, jmedia::jframeevent_type_t::Grab));

	    g->SetAntialias(jcanvas::jantialias_t::None);
	    g->SetCompositeFlags(jcanvas::jcomposite_flags_t::Src);
	    g->SetBlittingFlags(jcanvas::jblitting_flags_t::Nearest);

      if (_src.point.x == 0 and _src.point.y == 0 and _src.size.x == _frame_size.x and _src.size.y == _frame_size.y) {
			  g->DrawImage(image, {0, 0, size.x, size.y});
      } else {
			  g->DrawImage(image, _src, {0, 0, size.x, size.y});
      }
      
      image->UnlockData();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class XineVolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibXineLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		XineVolumeControlImpl(LibXineLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;

			SetLevel(100);
		}

		virtual ~XineVolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			int level = 0;

      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_stream != nullptr) {
				level = xine_get_param(_player->_stream, XINE_PARAM_AUDIO_VOLUME);
				
				// int amp = xine_get_param(_player->_stream, XINE_PARAM_AUDIO_AMP_LEVEL);

				// level = (int)(100.0*((float)level/100.0 * (float)amp/100.0));
			}

			return level;
		}

		virtual void SetLevel(int level)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_stream != nullptr) {
				_level = (level < 0)?0:(level > 100)?100:level;

				if (_level != 0) {
					_is_muted = true;
				} else {
					_is_muted = false;
				}

				/*
				level = (level < 0)?0:(level > 200)?200:level;

				int vol, amp;

				if (level > 100) {
					vol = 100;
					amp = level;
				} else {
					vol = level;
					amp = 100;
				}

				xine_set_param( data->stream, XINE_PARAM_AUDIO_VOLUME, vol );
				xine_set_param( data->stream, XINE_PARAM_AUDIO_AMP_LEVEL, amp );
				*/

				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_VOLUME, level);
				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_MUTE, (_is_muted == true)?1:0);
			}
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);
	
			_is_muted = b;
			
			if (_player->_stream != nullptr) {
				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_MUTE, (_is_muted == true)?1:0);
			}
		}

};

class XineVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibXineLightPlayer *_player;

	public:
		XineVideoSizeControlImpl(LibXineLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~XineVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
      XinePlayerComponentImpl *impl = dynamic_cast<XinePlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);
			
			impl->_src = {
        x, y, w, h
      };
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
      XinePlayerComponentImpl *impl = dynamic_cast<XinePlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jcanvas::jrect_t<int> GetSource()
		{
      return dynamic_cast<XinePlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jcanvas::jrect_t<int> GetDestination()
		{
      return dynamic_cast<XinePlayerComponentImpl *>(_player->_component)->GetBounds();
		}

};

class XineVideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibXineLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		XineVideoFormatControlImpl(LibXineLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = jvideo_mode_t::Full;
			_hd_video_format = jhd_video_format_t::Format1080p;
			_sd_video_format = jsd_video_format_t::PalM;
		}

		virtual ~XineVideoFormatControlImpl()
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

			return _player->_frames_per_second;
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

class XineVideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		LibXineLightPlayer *_player;
		std::map<jvideo_control_t, int> _default_values;

	public:
		XineVideoDeviceControlImpl(LibXineLightPlayer *player):
			VideoDeviceControl()
		{
			_player = player;

			_controls.push_back(jvideo_control_t::Contrast);
			_controls.push_back(jvideo_control_t::Saturation);
			_controls.push_back(jvideo_control_t::Hue);
			_controls.push_back(jvideo_control_t::Brightness);
			_controls.push_back(jvideo_control_t::Gamma);
		
			for (std::vector<jvideo_control_t>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
				_default_values[*i] = GetValue(*i);
			}
		}

		virtual ~XineVideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_stream != nullptr) {
				if (HasControl(id) == true) {
					int control = -1;

					if (id == jvideo_control_t::Contrast) {
						control = XINE_PARAM_VO_CONTRAST;
					} else if (id == jvideo_control_t::Saturation) {
						control = XINE_PARAM_VO_SATURATION;
					} else if (id == jvideo_control_t::Hue) {
						control = XINE_PARAM_VO_HUE;
					} else if (id == jvideo_control_t::Brightness) {
						control = XINE_PARAM_VO_BRIGHTNESS;
					} else if (id == jvideo_control_t::Gamma) {
						control = XINE_PARAM_VO_GAMMA;
					}
					
					return xine_get_param(_player->_stream, control);
				}
			}
				
			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_stream != nullptr) {
				if (HasControl(id) == true) {
					int control = -1;

					if (id == jvideo_control_t::Contrast) {
						control = XINE_PARAM_VO_CONTRAST;
					} else if (id == jvideo_control_t::Saturation) {
						control = XINE_PARAM_VO_SATURATION;
					} else if (id == jvideo_control_t::Hue) {
						control = XINE_PARAM_VO_HUE;
					} else if (id == jvideo_control_t::Brightness) {
						control = XINE_PARAM_VO_BRIGHTNESS;
					} else if (id == jvideo_control_t::Gamma) {
						control = XINE_PARAM_VO_GAMMA;
					}
					
					xine_set_param(_player->_stream, control, value);

					return true;
				}
			}

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
			SetValue(id, _default_values[id]);
		}

};

static void render_callback(void *data, int format, int width, int height, double aspect, void *data0, void *data1, void *data2)
{
	reinterpret_cast<XinePlayerComponentImpl *>(data)->UpdateComponent(format, width, height, aspect, data0, data1, data2);
}

static void overlay_callback(void *user_data, int num_ovl, raw_overlay_t *overlays_array)
{
}

static void events_callback(void *data, const xine_event_t *event) 
{
	LibXineLightPlayer *player = reinterpret_cast<LibXineLightPlayer *>(data);

	if (event->type == XINE_EVENT_UI_PLAYBACK_FINISHED) {
		player->DispatchPlayerEvent(new jmedia::PlayerEvent(player, jmedia::jplayerevent_type_t::Finish));

		if (player->IsLoop() == true) {
			player->Play();
		} else {
			player->Stop();
		}
  }
}
  
LibXineLightPlayer::LibXineLightPlayer(std::string uri):
	Player()
{
	_file = jdemux::Url{uri}.Path();
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_is_loop = false;
	_decode_rate = 1.0;
	_frames_per_second = 0.0;
	
	_component = new XinePlayerComponentImpl(this, 0, 0, -1, -1);

	raw_visual_t t;

	memset(&t, 0, sizeof(t));

	t.supported_formats = (int)(XINE_VORAW_RGB | XINE_VORAW_YV12 | XINE_VORAW_YUY2);
	t.raw_output_cb = render_callback;
	t.raw_overlay_cb = overlay_callback;
	t.user_data = _component;

  _xine = xine_new();
	
	char configfile[2048];

	sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");

	xine_config_load(_xine, configfile);
	xine_init(_xine);
 
	_post = nullptr;

  _ao_port = xine_open_audio_driver(_xine, "auto", nullptr);
  
  if (_ao_port == nullptr) {
    throw std::runtime_error("Unable to intialize 'auto' audio driver");
  }

  _vo_port = xine_open_video_driver(_xine , "auto", XINE_VISUAL_TYPE_RAW, &t);
  
  if (_vo_port == nullptr) {
    throw std::runtime_error("Unable to intialize 'auto' video driver");
  }

  _stream = xine_stream_new(_xine, _ao_port, _vo_port);
  _event_queue = xine_event_new_queue(_stream);
  
	xine_event_create_listener_thread(_event_queue, events_callback, this);

  if (xine_open(_stream, _file.c_str()) == 0) {
		xine_close(_stream);
		xine_event_dispose_queue(_event_queue);
		xine_dispose(_stream);
		xine_close_audio_driver(_xine, _ao_port);  
		xine_close_video_driver(_xine, _vo_port);  
		xine_exit(_xine);

		throw std::runtime_error("Unable to open the media file");
  }

	xine_set_param(_stream, XINE_PARAM_VERBOSITY, 1);
	xine_set_param(_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -1);

	_media_info.title = std::string(xine_get_meta_info(_stream, XINE_META_INFO_TITLE)?:"");
	_media_info.author = std::string(xine_get_meta_info(_stream, XINE_META_INFO_ARTIST) ?:"");
	_media_info.album = std::string(xine_get_meta_info(_stream, XINE_META_INFO_ALBUM)?:"");
	_media_info.genre = std::string(xine_get_meta_info(_stream, XINE_META_INFO_GENRE)?:"");
	_media_info.comments = std::string(xine_get_meta_info(_stream, XINE_META_INFO_COMMENT)?:"");
	_media_info.date = std::string(xine_get_meta_info(_stream, XINE_META_INFO_YEAR)?:"");

	if (xine_get_stream_info(_stream, XINE_STREAM_INFO_HAS_AUDIO)) {
		_has_audio = true;

		_controls.push_back(new XineVolumeControlImpl(this));
	}

	if (xine_get_stream_info(_stream, XINE_STREAM_INFO_HAS_VIDEO)) {
		_has_video = true;
		_aspect = xine_get_stream_info(_stream, XINE_STREAM_INFO_VIDEO_RATIO)/10000.0;
		_frames_per_second = xine_get_stream_info(_stream, XINE_STREAM_INFO_FRAME_DURATION);

		if (_frames_per_second) {
			_frames_per_second = 90000.0/_frames_per_second;
		}

		_controls.push_back(new XineVideoSizeControlImpl(this));
		_controls.push_back(new XineVideoFormatControlImpl(this));
		_controls.push_back(new XineVideoDeviceControlImpl(this));
	}

	if (_has_video == false && _has_audio == true) {
		const char *const *post_list;
		const char *post_plugin;
		xine_post_out_t *audio_source;

		post_list = xine_list_post_plugins_typed(_xine, XINE_POST_TYPE_AUDIO_VISUALIZATION);
		post_plugin = xine_config_register_string(_xine, "gui.post_audio_plugin", post_list[0], "Audio visualization plugin", nullptr, 0, nullptr, nullptr);
		_post = xine_post_init(_xine, post_plugin, 0, &_ao_port, &_vo_port);

		if (_post) {
			audio_source = xine_get_audio_source(_stream);
			xine_post_wire_audio_port(audio_source, _post->audio_input[0]);
		}
	}
				
	xine_set_param(_stream, XINE_PARAM_AUDIO_MUTE, 0);
  // xine_set_param(_stream, XINE_PARAM_AUDIO_VOLUME, 100);
}

LibXineLightPlayer::~LibXineLightPlayer()
{
	Close();
	
	_component = nullptr;

  while (_controls.size() > 0) {
		Control *control = *_controls.begin();

    _controls.erase(_controls.begin());

		delete control;
	}
}

void LibXineLightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _stream != nullptr) {
		int speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		xine_play(_stream, 0, 0);
		xine_set_param(_stream, XINE_PARAM_SPEED, speed);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Start));
	}
}

void LibXineLightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Pause));
	}
}

void LibXineLightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Resume));
	}
}

void LibXineLightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_stream != nullptr) {
		xine_stop(_stream);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibXineLightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_stream != nullptr) {
		xine_close(_stream);
		xine_event_dispose_queue(_event_queue);
		xine_dispose(_stream);
		xine_close_audio_driver(_xine, _ao_port);  
		xine_close_video_driver(_xine, _vo_port);  
		
		if (_post != nullptr) {
			xine_post_dispose(_xine, _post);
		}

		xine_exit(_xine);
	}
}

void LibXineLightPlayer::SetCurrentTime(uint64_t time)
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_stream != nullptr) {
		if (xine_get_stream_info(_stream, XINE_STREAM_INFO_SEEKABLE) == 0) {
			return;
		}

		int speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		xine_play(_stream, 0, time);

		xine_set_param(_stream, XINE_PARAM_SPEED, speed);

    std::this_thread::sleep_for(std::chrono::milliseconds((100)));
	}
}

uint64_t LibXineLightPlayer::GetCurrentTime()
{
  std::unique_lock<std::mutex> lock(_mutex);

	uint64_t time = 0LL;

	if (_stream != nullptr) {
		int pos = 0;

		for (int i=0; i<5; i++) {
			if (xine_get_pos_length(_stream, nullptr, &pos, nullptr)) {
				break;
			}

      std::this_thread::sleep_for(std::chrono::milliseconds((100)));
		}

		time = pos;
	}

	return time;
}

uint64_t LibXineLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	if (_stream != nullptr) {
		int length = 0;

		xine_get_pos_length(_stream, nullptr, nullptr, &length);

		time = (uint64_t)length;
	}

	return time;
}

void LibXineLightPlayer::SetLoop(bool b)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_is_loop = b;
}

bool LibXineLightPlayer::IsLoop()
{
	return _is_loop;
}

void LibXineLightPlayer::SetDecodeRate(double rate)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_stream != nullptr) {
		xine_set_param(_stream, XINE_PARAM_SPEED, rate*XINE_SPEED_NORMAL+0.5);
	}
}

double LibXineLightPlayer::GetDecodeRate()
{
  std::unique_lock<std::mutex> lock(_mutex);

	double rate = 1.0;

	if (_stream != nullptr) {
		int speed;

		speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		rate = (double)speed/ (double)XINE_SPEED_NORMAL;
	}

	return rate;
}

jcanvas::Component * LibXineLightPlayer::GetVisualComponent()
{
	return _component;
}

}

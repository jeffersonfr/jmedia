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
#include "../providers/v4l2/bind.h"
#include "../providers/v4l2/videocontrol.h"
#include "../providers/v4l2/videograbber.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvideodevicecontrol.h"
#include "jmedia/jcolorconversion.h"

#include "jcanvas/core/jbufferedimage.h"

#include "jdemux/jurl.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <cairo.h>

namespace jmedia {

class V4l2PlayerComponentImpl : public jcanvas::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
    std::mutex  _mutex;
		/** \brief */
		jcanvas::jrect_t<int> _src;
		/** \brief */
		uint32_t **_buffer;
		/** \brief */
		int _buffer_index;
		/** \brief */
		jcanvas::jpoint_t<int> _frame_size;

	public:
		V4l2PlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jcanvas::Component({x, y, w, h})
		{
			_buffer = nullptr;

			_buffer_index = 0;

			_player = player;
			
			_frame_size.x = w;
			_frame_size.y = h;

			_src = {
        0, 0, w, h
      };

			SetVisible(true);
		}

		virtual ~V4l2PlayerComponentImpl()
		{
			if (_buffer != nullptr) {
				delete [] _buffer[0];
				delete [] _buffer[1];

				delete [] _buffer;
        _buffer = nullptr;
			}
		}

		virtual jcanvas::jpoint_t<int> GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void Reset()
		{
			_frame_size.x = _src.size.x = -1;
			_frame_size.y = _src.size.y = -1;
		}

		virtual void UpdateComponent(const uint8_t *buffer, int width, int height, jcanvas::jpixelformat_t format)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			_mutex.lock();

      if (_buffer == nullptr or _frame_size.x != width or _frame_size.y != height) {
			  _frame_size.x = width;
			  _frame_size.y = height;
			
        if (_buffer != nullptr) {
          delete [] _buffer[0];
          delete [] _buffer[1];

          delete [] _buffer;
        }

        _buffer = new uint32_t*[2];

        _buffer[0] = new uint32_t[width*height];
        _buffer[1] = new uint32_t[width*height];

        if (_src.size.x < 0) {
			    _src.size.x = _frame_size.x;
        }

        if (_src.size.y < 0) {
			    _src.size.y = _frame_size.y;
        }
      }

      uint32_t *dst = _buffer[(_buffer_index++)%2];

			if (format == jcanvas::jpixelformat_t::UYVY) {
				ColorConversion::GetRGB32FromYUYV((uint8_t **)&buffer, (uint32_t **)&dst, width, height);
			} else if (format == jcanvas::jpixelformat_t::RGB24) {
				ColorConversion::GetRGB32FromRGB24((uint8_t **)&buffer, (uint32_t **)&dst, width, height);
			} else if (format == jcanvas::jpixelformat_t::RGB32) {
				memcpy(dst, buffer, width*height*4);
			}

      _mutex.unlock();

			Repaint();
		}

		virtual void Paint(jcanvas::Graphics *g)
		{
			// jcanvas::Component::Paint(g);

      jcanvas::jpoint_t<int>
        size = GetSize();

			_mutex.lock();

      if (_buffer == nullptr or _frame_size.x < 0 or _frame_size.y < 0) {
        _mutex.unlock();

        return;
      }

			cairo_surface_t *surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer[(_buffer_index)%2], CAIRO_FORMAT_RGB24, _frame_size.x, _frame_size.y, cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, _frame_size.x));
			
      std::shared_ptr<jcanvas::Image> image = std::make_shared<jcanvas::BufferedImage>(surface);

			_player->DispatchFrameGrabberEvent(new jmedia::FrameGrabberEvent(image, jmedia::jframeevent_type_t::Grab));

			cairo_surface_mark_dirty(surface);

	    g->SetAntialias(jcanvas::jantialias_t::None);
	    g->SetCompositeFlags(jcanvas::jcomposite_flags_t::Src);
	    g->SetBlittingFlags(jcanvas::jblitting_flags_t::Nearest);

      if (_src.point.x == 0 and _src.point.y == 0 and _src.size.x == _frame_size.x and _src.size.y == _frame_size.y) {
			  g->DrawImage(image, {0, 0, size.x, size.y});
      } else {
			  g->DrawImage(image, _src, {0, 0, size.x, size.y});
      }

			cairo_surface_destroy(surface);

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class V4l2VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		V4L2LightPlayer *_player;

	public:
		V4l2VideoSizeControlImpl(V4L2LightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~V4l2VideoSizeControlImpl()
		{
		}

		virtual void SetSize(int w, int h)
		{
      std::shared_ptr<V4l2PlayerComponentImpl> impl = std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component);
			VideoGrabber *grabber = _player->_grabber;

      impl->_mutex.lock();

			grabber->Stop();
			impl->Reset();
			grabber->Open();
			grabber->Configure(w, h);
			grabber->GetVideoControl()->Reset();
      
      impl->_mutex.unlock();
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
      std::shared_ptr<V4l2PlayerComponentImpl> impl = std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component);

      impl->_mutex.lock();
			
			impl->_src = {
        x, y, -1, -1
      };

      impl->_mutex.unlock();
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
      std::shared_ptr<V4l2PlayerComponentImpl> impl = std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component);

      impl->_mutex.lock();

			impl->SetBounds(x, y, w, h);
      
      impl->_mutex.unlock();
		}

		virtual jcanvas::jpoint_t<int> GetSize()
		{
      return std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component)->GetPreferredSize();
		}

		virtual jcanvas::jrect_t<int> GetSource()
		{
      return std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component)->_src;
		}

		virtual jcanvas::jrect_t<int> GetDestination()
		{
      return std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_player->_component)->GetBounds();
		}

};

class V4l2VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		V4L2LightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		V4l2VideoFormatControlImpl(V4L2LightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = jvideo_mode_t::Full;
			_hd_video_format = jhd_video_format_t::Format1080p;
			_sd_video_format = jsd_video_format_t::PalM;
		}

		virtual ~V4l2VideoFormatControlImpl()
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

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				return control->GetFramesPerSecond();
			}

			return 0.0;
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

class V4l2VideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		V4L2LightPlayer *_player;

	public:
		V4l2VideoDeviceControlImpl(V4L2LightPlayer *player):
			VideoDeviceControl()
		{
			_player = player;

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();
				std::vector<jvideo_control_t> controls = control->GetControls();

				for (std::vector<jvideo_control_t>::iterator i=controls.begin(); i!=controls.end(); i++) {
					_controls.push_back(*i);
				}
			}
		}

		virtual ~V4l2VideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					return control->GetValue(id);
				}
			}

			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					control->SetValue(id, value);

					return true;
				}
			}

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					control->Reset(id);
				}
			}
		}

};

V4L2LightPlayer::V4L2LightPlayer(std::string uri):
	Player()
{
	_file = jdemux::Url{uri}.Path();
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = true;
	_aspect = 1.0;
	_media_time = 0LL;
	_is_loop = false;
	_decode_rate = 1.0;
	_frame_rate = 0.0;
	_component = nullptr;
	
  if (_file.empty() == true) {
    _file = "/dev/video0";
  }

  _grabber = new VideoGrabber(this, _file);

	jcanvas::jpoint_t<int> size;
	
	size.x = 640;
	size.y = 480;

	_grabber->Open();
	_grabber->Configure(size.x, size.y);
	_grabber->GetVideoControl()->Reset();

	_controls.push_back(new V4l2VideoSizeControlImpl(this));
	_controls.push_back(new V4l2VideoFormatControlImpl(this));
	_controls.push_back(new V4l2VideoDeviceControlImpl(this));
	
	_component = std::make_shared<V4l2PlayerComponentImpl>(this, 0, 0, -1, -1);
}

V4L2LightPlayer::~V4L2LightPlayer()
{
	Close();
	
	_component = nullptr;

  while (_controls.size() > 0) {
		Control *control = *_controls.begin();

    _controls.erase(_controls.begin());

		delete control;
	}
}

void V4L2LightPlayer::ProcessFrame(const uint8_t *buffer, int width, int height, jcanvas::jpixelformat_t format)
{
  std::dynamic_pointer_cast<V4l2PlayerComponentImpl>(_component)->UpdateComponent(buffer, width, height, format);
}

void V4L2LightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _grabber != nullptr) {
		_grabber->Start();
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Start));
	}
}

void V4L2LightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _grabber != nullptr) {
		_is_paused = true;
		
		_grabber->Pause();
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Pause));
	}
}

void V4L2LightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == true && _grabber != nullptr) {
		_is_paused = false;
		
		_grabber->Resume();
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Resume));
	}
}

void V4L2LightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_grabber != nullptr) {
		_grabber->Stop();

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void V4L2LightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_grabber != nullptr) {
		delete _grabber;
		_grabber = nullptr;
	}
}

void V4L2LightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t V4L2LightPlayer::GetCurrentTime()
{
	return 0LL;
}

uint64_t V4L2LightPlayer::GetMediaTime()
{
	return 0LL;
}

void V4L2LightPlayer::SetLoop(bool b)
{
}

bool V4L2LightPlayer::IsLoop()
{
	return false;
}

void V4L2LightPlayer::SetDecodeRate(double rate)
{
}

double V4L2LightPlayer::GetDecodeRate()
{
	return 0.0;
}

std::shared_ptr<jcanvas::Component> V4L2LightPlayer::GetVisualComponent()
{
	return _component;
}

}


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
#include "../providers/ilist/bind.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jmedia/jaudioconfigurationcontrol.h"

#include "jcanvas/core/jbufferedimage.h"

#include "jdemux/jurl.h"

#include <algorithm>
#include <iostream>
#include <filesystem>

#include <cairo.h>

namespace jmedia {

class IlistPlayerComponentImpl : public jcanvas::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
    std::shared_ptr<jcanvas::Image> _image;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		jcanvas::jrect_t<int> _src;
		/** \brief */
		jcanvas::jrect_t<int> _dst;
		/** \brief */
		jcanvas::jpoint_t<int> _frame_size;

	public:
		IlistPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jcanvas::Component({x, y, w, h})
		{
			_image = nullptr;
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

		virtual ~IlistPlayerComponentImpl()
		{
			_image = nullptr;
		}

		virtual jcanvas::jpoint_t<int> GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(std::shared_ptr<jcanvas::Image> frame)
		{
			jcanvas::jpoint_t<int> isize = frame->GetSize();

			if (_frame_size.x != isize.x || _frame_size.y != isize.y) {
				if (_frame_size.x < 0 || _frame_size.y < 0) {
					_src.size = isize;
				}

				_frame_size = isize;
			}

			_player->DispatchFrameGrabberEvent(new jmedia::FrameGrabberEvent(frame, jmedia::jframeevent_type_t::Grab));

			_mutex.lock();

			_image = frame;

			Repaint();
		}

		virtual void Paint(jcanvas::Graphics *g)
		{
			jcanvas::Component::Paint(g);

      jcanvas::jpoint_t<int>
        size = GetSize();

	    g->SetAntialias(jcanvas::jantialias_t::None);
	    g->SetCompositeFlags(jcanvas::jcomposite_flags_t::Src);
	    g->SetBlittingFlags(jcanvas::jblitting_flags_t::Nearest);

      if (_src.point.x == 0 and _src.point.y == 0 and _src.size.x == _frame_size.x and _src.size.y == _frame_size.y) {
			  g->DrawImage(_image, {0, 0, size.x, size.y});
      } else {
			  g->DrawImage(_image, _src, {0, 0, size.x, size.y});
      }
				
      _image = nullptr;

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class IlistVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		ImageListLightPlayer *_player;

	public:
		IlistVideoSizeControlImpl(ImageListLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~IlistVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
      std::shared_ptr<IlistPlayerComponentImpl> impl = std::dynamic_pointer_cast<IlistPlayerComponentImpl>(_player->_component);

      impl->_mutex.lock();

			impl->_src = {
        x, y, w, h
      };

      impl->_mutex.unlock();
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
      std::shared_ptr<IlistPlayerComponentImpl> impl = std::dynamic_pointer_cast<IlistPlayerComponentImpl>(_player->_component);

      impl->_mutex.lock();

			impl->SetBounds(x, y, w, h);
      
      impl->_mutex.unlock();
		}

		virtual jcanvas::jrect_t<int> GetSource()
		{
			return std::dynamic_pointer_cast<IlistPlayerComponentImpl>(_player->_component)->_src;
		}

		virtual jcanvas::jrect_t<int> GetDestination()
		{
			return std::dynamic_pointer_cast<IlistPlayerComponentImpl>(_player->_component)->GetBounds();
		}

};

ImageListLightPlayer::ImageListLightPlayer(std::string uri):
	Player()
{
	_directory = jdemux::Url{uri}.Path();
	_is_paused = false;
	_is_playing = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	_frame_index = 0;
	
  std::filesystem::directory_entry entry {_directory};

  if (entry.exists() == false) {
		throw std::runtime_error("Media directory no exists");
	}

  for (auto const &entry : std::filesystem::directory_iterator(_directory)) {
    _image_list.push_back(entry.path().string());
  }

	if (_image_list.size() == 0) {
		throw std::runtime_error("There is no file in the directory");
	}

	std::sort(_image_list.begin(), _image_list.end(), [](std::string a, std::string b) {
    if (a < b) {
      return true;
    }

    return false;
  });

	_controls.push_back(new IlistVideoSizeControlImpl(this));

	_component = std::make_shared<IlistPlayerComponentImpl>(this, 0, 0, -1, -1);
}

ImageListLightPlayer::~ImageListLightPlayer()
{
	Close();
	
	_component = nullptr;

  while (_controls.size() > 0) {
    Control *control = *_controls.begin();

    _controls.erase(_controls.begin());

		delete control;
	}
}

void ImageListLightPlayer::ResetFrames()
{
	_frame_index = 0;
}

std::shared_ptr<jcanvas::Image> ImageListLightPlayer::GetFrame()
{
  std::string file = _image_list[_frame_index++];

  if (_frame_index >= (int)_image_list.size()) {
    _frame_index = 0;
  }

  return std::make_shared<jcanvas::BufferedImage>(file);
}

void ImageListLightPlayer::Run()
{
  std::shared_ptr<jcanvas::Image> frame;

	while (_is_playing == true) {
    std::unique_lock<std::mutex> lock(_mutex);

		frame = GetFrame();

		if (frame == nullptr) { 
			ResetFrames();

			if (_is_loop == false) {
				DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Finish));

				break;
			}
		}

    std::dynamic_pointer_cast<IlistPlayerComponentImpl>(_component)->UpdateComponent(frame);

		try {
			if (_decode_rate == 0) {
				_condition.wait(lock);
			} else {
				uint64_t us;

				us = 1000000; // 1.0 frame/sec

				if (_decode_rate != 1.0) {
					us = ((double)us / _decode_rate + 0.);
				}

        _condition.wait_for(lock, std::chrono::microseconds(us));
			}
		} catch (std::runtime_error &e) {
		}
	}
}

void ImageListLightPlayer::Play()
{
  _mutex.lock();

	if (_is_paused == false) {
		if (_is_playing == false) {
			_is_playing = true;

      _thread = std::thread(&ImageListLightPlayer::Run, this);
		}
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Pause()
{
  _mutex.lock();

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Pause));
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Resume()
{
  _mutex.lock();

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new jmedia::PlayerEvent(this, jmedia::jplayerevent_type_t::Resume));
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Stop()
{
  if (_is_playing == true) {
    _is_playing = false;

    _thread.join();
  }

	if (_has_video == true) {
		_component->Repaint();
	}

	_is_paused = false;
}

void ImageListLightPlayer::Close()
{
  Stop();

	_condition.notify_all();

	_is_closed = true;
}

void ImageListLightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t ImageListLightPlayer::GetCurrentTime()
{
	return -1LL;
}

uint64_t ImageListLightPlayer::GetMediaTime()
{
	return -1LL;
}

void ImageListLightPlayer::SetLoop(bool b)
{
	_is_loop = b;
}

bool ImageListLightPlayer::IsLoop()
{
	return _is_loop;
}

void ImageListLightPlayer::SetDecodeRate(double rate)
{
	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
			
		_condition.notify_one();
	}
}

double ImageListLightPlayer::GetDecodeRate()
{
	return _decode_rate;
}

std::shared_ptr<jcanvas::Component> ImageListLightPlayer::GetVisualComponent()
{
	return _component;
}

}

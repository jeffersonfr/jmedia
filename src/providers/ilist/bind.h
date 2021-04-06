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
#pragma once

#include "jmedia/jplayer.h"

#include "jcanvas/widgets/jcomponent.h"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace jmedia {

class ImageListLightPlayer : public Player {

	public:
		/** \brief */
		std::vector<std::string> _image_list;
		/** \brief */
    std::thread _thread;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
    std::condition_variable _condition;
		/** \brief */
		std::string _directory;
		/** \brief */
    jcanvas::Component *_component {nullptr};
		/** \brief */
		double _aspect;
		/** \brief */
		double _decode_rate;
		/** \brief */
		uint64_t _media_time;
		/** \brief */
		bool _is_paused;
		/** \brief */
		bool _is_closed;
		/** \brief */
		bool _is_loop;
		/** \brief */
		bool _has_audio;
		/** \brief */
		bool _has_video;
		/** \brief */
		bool _is_playing;
		/** \brief */
		int _frame_index;

	public:
		/**
		 * \brief
		 *
		 */
		ImageListLightPlayer(std::string uri);

		/**
		 * \brief
		 *
		 */
		virtual ~ImageListLightPlayer();

		/**
		 * \brief
		 *
		 */
		virtual void Play();

		/**
		 * \brief
		 *
		 */
		virtual void Pause();

		/**
		 * \brief
		 *
		 */
		virtual void Stop();

		/**
		 * \brief
		 *
		 */
		virtual void Resume();

		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentTime(uint64_t i);

		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetCurrentTime();

		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetMediaTime();

		/**
		 * \brief
		 *
		 */
		virtual void SetLoop(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsLoop();
		
		/**
		 * \brief
		 *
		 */
		virtual double GetDecodeRate();

		/**
		 * \brief
		 *
		 */
		virtual void SetDecodeRate(double rate);

		/**
		 * \brief
		 *
		 */
		virtual jcanvas::Component * GetVisualComponent();

		/**
		 * \brief
		 *
		 */
		virtual void ResetFrames();

		/**
		 * \brief
		 *
		 */
		virtual std::shared_ptr<jcanvas::Image> GetFrame();

		/**
		 * \brief
		 *
		 */
		virtual void Run();
};

}


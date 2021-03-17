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

#include "jmedia/jcontrol.h"

#include <istream>

namespace jmedia {

enum class jaudio_format_t {
   // 8-bit support
  S8, // signed 8-bit samples
  U8, // unsigned 8-bit samples

  // 16-bit support
  S16LSB, // signed 16-bit samples in little-endian byte order
  S16MSB, // signed 16-bit samples in big-endian byte order
  S16SYS, // signed 16-bit samples in native byte order
  S16, // S16LSB
  U16LSB, // unsigned 16-bit samples in little-endian byte order
  U16MSB, // unsigned 16-bit samples in big-endian byte order
  U16SYS, // unsigned 16-bit samples in native byte order
  U16, // U16LSB

  // 32-bit support
  S32LSB, // 32-bit integer samples in little-endian byte order
  S32MSB, // 32-bit integer samples in big-endian byte order
  S32SYS, // 32-bit integer samples in native byte order
  S32, // S32LSB
  
  // float support
  F32LSB, // 32-bit floating point samples in little-endian byte order
  F32MSB, // 32-bit floating point samples in big-endian byte order
  F32SYS, // 32-bit floating point samples in native byte order
  F32 // F32LSB
};

class Audio {
  
  public:

  public:
    /**
     * \brief 
     *
     */
    Audio();

    /**
     * \brief 
     *
     */
    virtual ~Audio();

    /**
     * \brief 
     *
     */
    virtual bool IsLoopEnabled();

    /**
     * \brief 
     *
     */
    virtual float GetVolume();

    /**
     * \brief 
     *
     */
    virtual void SetLoopEnabled(bool enabled);

    /**
     * \brief 
     *
     */
    virtual void SetVolume(float volume);

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class AudioMixerControl : public Control {

  public:
    /**
     * \brief 
     *
     */
    AudioMixerControl();

    /**
     * \brief
     *
     */
    virtual ~AudioMixerControl();

    /**
     * \brief
     *
     */
    virtual Audio * CreateAudio(std::string filename);

    /**
     * \brief
     *
     */
    virtual Audio * CreateAudio(std::istream &stream, jaudio_format_t format, int frequency, int channels);

    /**
     * \brief
     *
     */
    virtual void StartSound(Audio *audio);

    /**
     * \brief
     *
     */
    virtual void StopSound(Audio *audio);

};

}


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

namespace jmedia {

/**
 * \brief
 *
 */
enum class jaudio_config_mode_t {
  Pcm,
  PcmStereo,
  Compressed,
  Pcm21,
  Pcm51,
  Pcm71
};
  
/**
 * \brief
 *
 * \author Jeff Ferr
 */
class AudioConfigurationControl : public Control {

  public:
    /**
     * \brief 
     *
     */
    AudioConfigurationControl();

    /**
     * \brief
     *
     */
    virtual ~AudioConfigurationControl();

    /**
     * \brief 
     *
     */
    virtual void SetAudioMode(jaudio_config_mode_t);

    /**
     * \brief
     *
     */
    virtual jaudio_config_mode_t GetHDMIAudioMode();

    /**
     * \brief
     *
     */
    virtual void SetSPDIFPCM(bool pcm);

    /**
     * \brief
     *
     */
    virtual bool IsSPDIFPCM();

    /**
     * \brief
     *
     */
    virtual void SetAudioDelay(int64_t delay);

    /**
     * \brief
     *
     */
    virtual int64_t GetAudioDelay();

};

}

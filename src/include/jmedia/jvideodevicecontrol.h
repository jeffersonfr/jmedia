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

#include "jcanvas/core/jgraphics.h"

namespace jmedia {

/**
 * \brief
 *
 */
enum class jvideo_control_t {
  Unknown,
  Contrast,
  Saturation,
  Hue,
  Brightness,
  Sharpness,
  Gamma,
  AutoExposure,
  AutoFocus,
  Hflip,
  Vflip,
  Backlight,
  Zoom
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class VideoDeviceControl : public Control {

  protected:
    /** \brief */
    std::vector<jvideo_control_t> _controls;

  public:
    /**
     * \brief
     *
     */
    VideoDeviceControl();

    /**
     * \brief Destructor.
     *
     */
    virtual ~VideoDeviceControl();

    /**
     * \brief
     *
     */
    virtual const std::vector<jvideo_control_t> & GetControls();
    
    /**
     * \brief
     *
     */
    virtual bool HasControl(jvideo_control_t id);

    /**
     * \brief
     *
     */
    virtual int GetValue(jvideo_control_t id);

    /**
     * \brief
     *
     */
    virtual bool SetValue(jvideo_control_t id, int value);

    /**
     * \brief
     *
     */
    virtual void Reset(jvideo_control_t id);

};

}


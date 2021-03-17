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
enum class jaspect_ratio_t {
  Square,
  Low,
  Standard,
  Wide
};

/** 
 * \brief Tipos de adaptações de tela suportados:
 *
 * content_mode_zoom: Indica que o vídeo deve ser cortado (clipping) para adaptar a tela
 * content_mode_box: Indica que barras devem ser utilizadas para adaptação (letterbox ou pillar box) 
 * content_mode_panscan: Indica que o método PanScan deve ser utilizado (se as informações estiverem presentes no fluxo)
 * content_mode_full: Indica que o vídeo deve ser esticado (stretching) de forma linear para se adaptar à tela
 * content_mode_full: Indica que o vídeo deve ser esticado, porém de forma não-linear (as bordas podem ser mais esticadas que a parte central)
 * */
enum class jvideo_mode_t {
  Zoom,
  Box,
  PanScan,
  Full,
  FullNonLinear
};

/** 
 * \brief 
 *
 */
enum class jsd_video_format_t {
  Ntsc,
  NtscJapan,
  PalM,
  PalN,
  PalNC,
  PalB,
  PalB1,
  PalD,
  PalD1,
  PalG,
  PalH,
  PalK,
  PalI,
  Secam
};

/** 
 * \brief
 *
 */
enum class jhd_video_format_t {
  Format480p,
  Format576p,
  Format720p,
  Format1080i,
  Format1080p
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class VideoFormatControl : public Control {

  public:
    /**
     * \brief
     *
     */
    VideoFormatControl();

    /**
     * \brief Destructor.
     *
     */
    virtual ~VideoFormatControl();

    /**
     * \brief Define media's aspect ratio.
     *
     * \param t
     */
    virtual void SetAspectRatio(jaspect_ratio_t t);

    /**
     * \brief Define media's aspect ratio.
     *
     * \param t
     */
    virtual void SetFramesPerSecond(double fps);

    /**
     * \brief Define the video mode. The final result will depend from content mode and aspect ratio.
     *
     * \param t
     */
    virtual void SetContentMode(jvideo_mode_t t);

    /**
     * \brief Define the high definition format of media.
     *
     * \param t
     */
    virtual void SetVideoFormatHD(jhd_video_format_t vf);

    /**
     * \brief Define the standard definition format of media.
     *
     * \param t
     */
    virtual void SetVideoFormatSD(jsd_video_format_t vf);

    /**
     * \brief Returns the aspect ratio.
     *
     * \return 
     */
    virtual jaspect_ratio_t GetAspectRatio();

    /**
     * \brief Returns the frame rate.
     *
     * \return 
     */
    virtual double GetFramesPerSecond();

    /**
     * \brief Returns the content mode.
     *
     * \return 
     */
    virtual jvideo_mode_t GetContentMode();

    /**
     * \brief Returns high definition format.
     *
     * \return 
     */
    virtual jhd_video_format_t GetVideoFormatHD();

    /**
     * \brief Returns standard definition format.
     *
     * \return 
     */
    virtual jsd_video_format_t GetVideoFormatSD();

};

}


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
#include "jmedia/jvideoformatcontrol.h"

namespace jmedia {

VideoFormatControl::VideoFormatControl():
  Control("video.format")
{
}
    
VideoFormatControl::~VideoFormatControl()
{
}

void VideoFormatControl::SetAspectRatio(jaspect_ratio_t)
{
}

void VideoFormatControl::SetFramesPerSecond(double)
{
}

void VideoFormatControl::SetContentMode(jvideo_mode_t)
{
}

void VideoFormatControl::SetVideoFormatHD(jhd_video_format_t)
{
}

void VideoFormatControl::SetVideoFormatSD(jsd_video_format_t)
{
}

jaspect_ratio_t VideoFormatControl::GetAspectRatio()
{
  return jaspect_ratio_t::Wide;
}

double VideoFormatControl::GetFramesPerSecond()
{
  return 0.0;
}

jvideo_mode_t VideoFormatControl::GetContentMode()
{
  return jvideo_mode_t::Full;
}

jhd_video_format_t VideoFormatControl::GetVideoFormatHD()
{
  return jhd_video_format_t::Format1080i;
}

jsd_video_format_t VideoFormatControl::GetVideoFormatSD()
{
  return jsd_video_format_t::PalM;
}

}

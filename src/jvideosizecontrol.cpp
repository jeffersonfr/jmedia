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
#include "jmedia/jvideosizecontrol.h"

namespace jmedia {

VideoSizeControl::VideoSizeControl():
  Control("video.size")
{
}
    
VideoSizeControl::~VideoSizeControl()
{
}

void VideoSizeControl::SetSize(jcanvas::jpoint_t<int> t)
{
  SetSize(t);
}

void VideoSizeControl::SetSource(jcanvas::jrect_t<int> t)
{
  SetSource(t);
}

void VideoSizeControl::SetDestination(jcanvas::jrect_t<int> t)
{
  SetDestination(t);
}

jcanvas::jpoint_t<int> VideoSizeControl::GetSize()
{
  return {0, 0};
}

jcanvas::jrect_t<int> VideoSizeControl::GetSource()
{
  return {0, 0, 0, 0};
}

jcanvas::jrect_t<int> VideoSizeControl::GetDestination()
{
  return {0, 0, 0, 0};
}

}

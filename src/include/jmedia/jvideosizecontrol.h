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
 * \author Jeff Ferr
 */
class VideoSizeControl : public Control {

  public:
    /**
     * \brief
     *
     */
    VideoSizeControl();

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~VideoSizeControl();

    /**
     * \brief
     *
     */
    virtual void SetSize(jcanvas::jpoint_t<int> t);

    /**
     * \brief
     *
     */
    virtual void SetSource(jcanvas::jrect_t<int> t);

    /**
     * \brief
     *
     */
    virtual void SetDestination(jcanvas::jrect_t<int> t);

    /**
     * \brief
     *
     */
    virtual jcanvas::jpoint_t<int> GetSize();

    /**
     * \brief
     *
     */
    virtual jcanvas::jrect_t<int> GetSource();

    /**
     * \brief
     *
     */
    virtual jcanvas::jrect_t<int> GetDestination();

};

}


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

namespace jmedia {

/** 
 * \brief Supported events
 *
 */
enum class jplayerevent_type_t {
  Start,
  Pause,
  Resume,
  Stop,
  Finish
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class PlayerEvent {

  private:
    /** \brief */
    jplayerevent_type_t _type;

  public:
    /**
     * \brief 
     *
     */
    PlayerEvent(void *source, jplayerevent_type_t type);

    /**
     * \brief
     *
     */
    virtual ~PlayerEvent();

    /**
     * \brief
     *
     */
    jplayerevent_type_t GetType();

};

}


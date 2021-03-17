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

#include <map>

namespace jmedia {

enum class jplayer_hints_t {
  Caching,
  Lightweight,
  Security,
  Plugins
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class PlayerManager {

  private:
    /** \brief */
    static std::map<jplayer_hints_t, bool> _hints;

  private:
    /**
     * \brief
     *
     */
    PlayerManager();

  public:
    /**
     * \brief
     *
     */
    virtual ~PlayerManager();

    /**
     * \brief 
     *
     */
    static Player * CreatePlayer(std::string url);
    
    /**
     * \brief 
     *
     */
    static void SetHint(jplayer_hints_t hint, bool value);
    
    /**
     * \brief 
     *
     */
    static bool GetHint(jplayer_hints_t hint);

};

}


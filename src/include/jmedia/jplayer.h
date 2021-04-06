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
#include "jmedia/jplayerlistener.h"
#include "jmedia/jframegrabberlistener.h"

#include "jcanvas/widgets/jcomponent.h"

#include <vector>

namespace jmedia {

struct jmedia_info_t {
  std::string title;
  std::string author;
  std::string album;
  std::string genre;
  std::string comments;
  std::string date;
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Player {

  protected:
    /** \brief */
    std::vector<PlayerListener *> _player_listeners;
    /** \brief */
    std::vector<FrameGrabberListener *> _frame_listeners;
    /** \brief */
    std::vector<Control *> _controls;
    /** \brief */
    jmedia_info_t _media_info;

  protected:
    /**
     * \brief
     *
     */
    Player();

  public:
    /**
     * \brief
     *
     */
    virtual ~Player();

    /**
     * \brief Starts media decode.
     *
     */
    virtual void Play();

    /**
     * \brief
     *
     */
    virtual jmedia_info_t GetMediaInfo();

    /**
     * \brief Pause media decode.
     *
     */
    virtual void Pause();

    /**
     * \brief Stops media decode.
     *
     */
    virtual void Stop();

    /**
     * \brief Resume media decode.
     *
     */
    virtual void Resume();

    /**
     * \brief Release resources associated with this player.
     *
     */
    virtual void Close();

    /**
     * \brief Seeks media forward or back.
     *
     */
    virtual void SetCurrentTime(uint64_t time);

    /**
     * \brief Gets current media time.
     *
     */
    virtual uint64_t GetCurrentTime();

    /**
     * \brief Gets full media time.
     *
     */
    virtual uint64_t GetMediaTime();

    /**
     * \brief Set loop mode.
     *
     * \param b
     */
    virtual void SetLoop(bool b);

    /**
     * \brief 
     *
     */
    virtual bool IsLoop();
    
    /**
     * \brief Set a decode rate to this player.
     *
     * \param rate
     *
     */
    virtual double GetDecodeRate();

    /**
     * \brief Set a decode rate to this player.
     *
     * \param rate
     *
     */
    virtual void SetDecodeRate(double rate);

    /**
     * \brief Returns a list with all controls associated with this player.
     *
     * \return controls
     */
    virtual const std::vector<Control *> & GetControls();

    /**
     * \brief Return a specific control related with the type.
     *
     * \return
     */
    virtual Control * GetControl(std::string id);

    /**
     * \brief Return a specific control related with the type.
     *
     * \return
     */
    virtual jcanvas::Component * GetVisualComponent();

    /**
     * \brief Registry a listener.
     *
     * \param listener
     */
    virtual void RegisterPlayerListener(PlayerListener *listener);

    /**
     * \brief Remove a listener.
     *
     * \param listener
     */
    virtual void RemovePlayerListener(PlayerListener *listener);

    /**
     * \brief Remove a listener.
     *
     * \param listener
     */
    virtual const std::vector<PlayerListener *> & GetPlayerListeners();

    /*
     * \brief
     *
     */
    virtual void DispatchPlayerEvent(PlayerEvent *event);
    
    /**
     * \brief Registry a listener.
     *
     * \param listener
     */
    virtual void RegisterFrameGrabberListener(FrameGrabberListener *listener);

    /**
     * \brief Remove a listener.
     *
     * \param listener
     */
    virtual void RemoveFrameGrabberListener(FrameGrabberListener *listener);

    /**
     * \brief Remove a listener.
     *
     * \param listener
     */
    virtual const std::vector<FrameGrabberListener *> & GetFrameGrabberListeners();

    /*
     * \brief
     *
     */
    virtual void DispatchFrameGrabberEvent(FrameGrabberEvent *event);
    
};

}


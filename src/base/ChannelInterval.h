/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2011 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _CHANNELSEGMENT_H_
#define _CHANNELSEGMENT_H_

#include "base/Event.h"
#include "base/RealTime.h"

namespace Rosegarden
{

class FreeChannels;
class ChannelManager;

// @type ChannelId The index of a channel on some
// device.  A negative value indicates no channel.
// @author Tom Breton (Tehom)
typedef int ChannelId;

// @class ChannelInterval  A timewise section of a channel on some
// device
// @author Tom Breton (Tehom)
class ChannelInterval
{
    friend class FreeChannels;
    // Only want to friend ChannelManager::setChannelIdDirectly(void);
    // but C++ doesn't seem to allow forward-declaring member
    // functions. 
    friend class ChannelManager;
 public:
    
 ChannelInterval(void) :
    m_channel(-1),
        m_start(RealTime::zeroTime),
        m_end(RealTime::zeroTime)
            {}
    
 ChannelInterval(ChannelId channel, RealTime start, RealTime end) :
    m_channel(channel),
        m_start(start),
        m_end(end)
        {}

    // Comparison operation for sorting in FreeChannels
    struct Cmp
    {
        bool operator()(const ChannelInterval &cs1, const ChannelInterval &cs2) const {
            return cs1.m_start < cs2.m_start;
        }
        bool operator()(const ChannelInterval *cs1, const ChannelInterval *cs2) const {
            return operator()(*cs1, *cs2);
        }
    };

    ChannelId getChannelId(void) const
    { return m_channel; };
    void clearChannelId(void)
    { m_channel = -1; }
    bool validChannel(void) const
    { return m_channel >= 0; }

 private:
    void setChannelId(ChannelId channel)
    { m_channel = channel; };
    
    ChannelId m_channel;
    RealTime  m_start;
    RealTime  m_end;

 public:
  /* Reference times */

  // "before" and "after" exist so that all channel intervals we store
  // are bracketed by other channel intervals, which reduces special
  // cases.
  static const RealTime m_beforeEarliestTime; // Before the earliest time
  static const RealTime m_earliestTime;       // The earliest time (zero)
  static const RealTime m_latestTime;         // The latest time 
  static const RealTime m_afterLatestTime;    // After the latest time.
};
}

#endif /* ifndef _CHANNELSEGMENT_H_ */
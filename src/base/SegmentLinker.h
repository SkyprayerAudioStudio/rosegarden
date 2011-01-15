/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */


/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2010 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _SEGMENTLINKER_H
#define _SEGMENTLINKER_H

#include "Segment.h"
#include <QObject>

namespace Rosegarden 
{

class Command;
class Event;

class SegmentLinker : public QObject
{
    Q_OBJECT
    
public:
    typedef unsigned int SegmentLinkerId;
    
    SegmentLinker();
    SegmentLinker(SegmentLinkerId id);
    virtual ~SegmentLinker();
    
    void addLinkedSegment(Segment *s);
    void removeLinkedSegment(Segment *s);
    int getNumberOfLinkedSegments() const {
                                    return m_linkedSegmentParamsList.size(); }

    void clearRefreshStatuses();
    SegmentLinkerId getSegmentLinkerId() const { return m_id; }

    ///re-read one segment from any of the others
    void refreshSegment(Segment *segment);

    //factory functions for dealing with linking/unlinking of segments
    static Segment* createLinkedSegment(Segment *s);
    static bool unlinkSegment(Segment *s);

protected slots:
    void slotUpdateLinkedSegments(Command* command);

private:
    struct LinkedSegmentParams
    {
        LinkedSegmentParams(Segment *s);
        Segment *m_linkedSegment;
        uint m_refreshStatusId;
    };
    
    typedef std::list<LinkedSegmentParams> LinkedSegmentParamsList;

    void linkedSegmentChanged(Segment* s, const timeT from, const timeT to);
    void eraseNonIgnored(Segment *s, Segment::const_iterator itrFrom, 
                                     Segment::const_iterator itrTo);
    void insertMappedEvent(Segment *seg, const Event *e, timeT t, 
                                         int semitones, int steps);
    LinkedSegmentParamsList::iterator findParamsItrForSegment(Segment *s);
    static void handleImpliedCMajor(Segment *s);

    LinkedSegmentParamsList m_linkedSegmentParamsList;

    static SegmentLinkerId m_count;
    SegmentLinkerId m_id;
};
    
}

#endif // _SEGMENTLINKER_H

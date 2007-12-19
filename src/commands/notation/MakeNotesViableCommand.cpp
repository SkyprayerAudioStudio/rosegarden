/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2007
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "MakeNotesViableCommand.h"

#include "base/Segment.h"
#include "base/SegmentNotationHelper.h"
#include "base/Selection.h"
#include "document/BasicSelectionCommand.h"
#include "document/CommandRegistry.h"
#include <qstring.h>


namespace Rosegarden
{

void
MakeNotesViableCommand::registerCommand(CommandRegistry *r)
{
    r->registerCommand
        (getGlobalName(), "", "", "make_notes_viable",
         new SelectionCommandBuilder<MakeNotesViableCommand>());
}

void
MakeNotesViableCommand::modifySegment()
{
    Segment &segment(getSegment());
    SegmentNotationHelper helper(segment);

    if (m_selection) {
        EventSelection::RangeTimeList ranges(m_selection->getRangeTimes());
        for (EventSelection::RangeTimeList::iterator i = ranges.begin();
                i != ranges.end(); ++i) {
            helper.makeNotesViable(i->first, i->second, true);
            segment.normalizeRests(i->first, i->second);
        }
    } else {
        helper.makeNotesViable(getStartTime(), getEndTime(), true);
        segment.normalizeRests(getStartTime(), getEndTime());
    }
}

}

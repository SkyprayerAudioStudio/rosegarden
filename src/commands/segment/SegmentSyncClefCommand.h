
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2013 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_SEGMENTSYNCCLEFCOMMAND_H_
#define _RG_SEGMENTSYNCCLEFCOMMAND_H_

#include "document/Command.h"
#include "base/Event.h"
#include "base/NotationTypes.h"
#include "document/CommandHistory.h"

#include <QCoreApplication>


namespace Rosegarden
{

class Segment;
class SegmentSelection;


class SegmentSyncClefCommand : public MacroCommand
{
    Q_DECLARE_TR_FUNCTIONS(Rosegarden::SegmentSyncClefCommand)

public:
    SegmentSyncClefCommand(Segment &segment, const Clef& clef);

    virtual ~SegmentSyncClefCommand();

protected:
    void processSegment(Segment &segment, const Clef& clef);
};

}

#endif

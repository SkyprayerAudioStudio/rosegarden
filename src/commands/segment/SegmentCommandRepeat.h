
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_SEGMENTCOMMANDREPEAT_H_
#define _RG_SEGMENTCOMMANDREPEAT_H_

#include "SegmentCommand.h"
#include <vector>




namespace Rosegarden
{

class Segment;


/**
 * Repeat segment command from the SegmentParameterBox
 */
class SegmentCommandRepeat : public SegmentCommand
{
public:
    SegmentCommandRepeat(const std::vector<Segment*>&,
                         bool repeat);

    virtual void execute();
    virtual void unexecute();

protected:
    bool m_repeatState;
};

// Disabled until we find a better solution
//
// As it is, command creation happens on every change of the text
// value of the combo box
//
//
// class SegmentCommandChangeTransposeValue : public SegmentCommand
// {
// public:
//     SegmentCommandChangeTransposeValue(const std::vector<Segment*>&,
//                                        int transposeValue);

//     virtual void execute();
//     virtual void unexecute();

// protected:
//     int m_transposeValue;
//     std::vector<int> m_savedValues;
// };



}

#endif

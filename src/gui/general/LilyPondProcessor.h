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

#ifndef _LILYPOND_PROCESSOR_H_
#define _LILYPOND_PROCESSOR_H_

#include <QDialog>


namespace Rosegarden
{

/** Implement functionality equivalent to the old external
 *  rosegarden-lilypondview script.  The script used the external dcop and
 *  kdialog command line utlities to provide a user interface.  We'll do the
 *  user interface in real code, though we still have to use external helper
 *  applications to print, display a PDF, and of course it's all for naught
 *  without LilyPond itself being available.
 *
 *  I suppose we'll make the Conftest target just handle the "install some
 *  stuff" warnings inside itself, and I'll eliminate the redudant warnings
 *  elsewhere when I run across them.  We won't need a non-graphical mode for
 *  this, since it will always be running out of the GUI.  Other side benefits,
 *  this means it runs with the same stylesheet as the rest of Rosegarden super
 *  cheap.
 *
 *  \author D. Michael McIntyre
 */

class LilyPondProcessor : public QDialog
{
    Q_OBJECT

public:
    /** The old command line arguments are replaced with an int passed into the
     * ctor, using the following named constants to replace them, and avoid a
     * bunch of string parsing nonsense.  We no longer need a startup ConfTest
     * target.  We'll do the conftest every time we run instead, and only
     * complain if there is a problem.  We no longer need a version target,
     * since the version is tied to Rosegarden itself.  That means we really
     * only need a PDF preview mode and a direct print mode.
     *
     * The filename parameter should be a temporary file set elsewhere and
     * passed in.
     */
    static const int Preview   = 1;
    static const int Print     = 2;

    LilyPondProcessor(QWidget *parent,
                      int mode,
                      QString filename);
    ~LilyPondProcessor() { };
};


}

#endif

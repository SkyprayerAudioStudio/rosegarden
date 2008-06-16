
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2008 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_AUDIOROUTEMENU_H_
#define _RG_AUDIOROUTEMENU_H_

#include <qobject.h>
#include <qstring.h>


class QWidget;
class KComboBox;


namespace Rosegarden
{

class WheelyButton;
class Studio;
class Instrument;


class AudioRouteMenu : public QObject
{
    Q_OBJECT

public:
    enum Direction { In, Out };
    enum Format { Compact, Regular };

    AudioRouteMenu(QWidget *parent,
                   Direction direction,
                   Format format,
                   Studio *studio = 0,
                   Instrument *instrument = 0);

    QWidget *getWidget();

public slots:
    void slotRepopulate();
    void slotSetInstrument(Studio *, Instrument *);
    
protected slots:
    void slotWheel(bool up);
    void slotShowMenu();
    void slotEntrySelected(int);

signals:
    // The menu writes changes directly to the instrument, but it
    // also emits this to let you know something has changed
    void changed();

private:
    Studio *m_studio;
    Instrument *m_instrument;
    Direction m_direction;
    Format m_format;

    WheelyButton *m_button;
    KComboBox *m_combo;

    int getNumEntries();
    int getCurrentEntry(); // for instrument
    QString getEntryText(int n);
};




}

#endif

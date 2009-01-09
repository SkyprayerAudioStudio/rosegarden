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


#include "ClefInserter.h"

#include <klocale.h>
#include "base/Event.h"
#include "base/NotationTypes.h"
#include "base/ViewElement.h"
#include "commands/notation/ClefInsertionCommand.h"
#include "gui/general/EditTool.h"
#include "gui/general/LinedStaff.h"
#include "NotationElement.h"
#include "NotationTool.h"
#include "NotationView.h"
#include "NotePixmapFactory.h"
#include <kaction.h>
#include <qiconset.h>
#include <qstring.h>


namespace Rosegarden
{

ClefInserter::ClefInserter(NotationView* view)
        : NotationTool("ClefInserter", view),
        m_clef(Clef::Treble)
{
    QIconSet icon = QIconSet(NotePixmapFactory::toQPixmap(NotePixmapFactory::
                             makeToolbarPixmap("select")));
    new KAction(i18n("Switch to Select Tool"), icon, 0, this,
                SLOT(slotSelectSelected()), actionCollection(),
                "select");

    new KAction(i18n("Switch to Erase Tool"), "eraser", 0, this,
                SLOT(slotEraseSelected()), actionCollection(),
                "erase");

    icon = QIconSet
           (NotePixmapFactory::toQPixmap(NotePixmapFactory::
                                         makeToolbarPixmap("crotchet")));
    new KAction(i18n("Switch to Inserting Notes"), icon, 0, this,
                SLOT(slotNotesSelected()), actionCollection(),
                "notes");

    createMenu("clefinserter.rc");
}

void ClefInserter::slotNotesSelected()
{
    m_nParentView->slotLastNoteAction();
}

void ClefInserter::slotEraseSelected()
{
    m_parentView->actionCollection()->action("erase")->activate();
}

void ClefInserter::slotSelectSelected()
{
    m_parentView->actionCollection()->action("select")->activate();
}

void ClefInserter::ready()
{
    m_nParentView->setCanvasCursor(Qt::crossCursor);
    m_nParentView->setHeightTracking(false);
}

void ClefInserter::setClef(std::string clefType)
{
    m_clef = clefType;
}

void ClefInserter::handleLeftButtonPress(timeT,
        int,
        int staffNo,
        QMouseEvent* e,
        ViewElement*)
{
    if (staffNo < 0)
        return ;
    Event *clef = 0, *key = 0;

    LinedStaff *staff = m_nParentView->getLinedStaff(staffNo);

    NotationElementList::iterator closestElement =
        staff->getClosestElementToCanvasCoords(e->x(), (int)e->y(),
                                               clef, key, false, -1);

    if (closestElement == staff->getViewElementList()->end())
        return ;

    timeT time = (*closestElement)->event()->getAbsoluteTime(); // not getViewAbsoluteTime()


    ClefInsertionCommand *command =
        new ClefInsertionCommand(staff->getSegment(), time, m_clef);

    m_nParentView->addCommandToHistory(command);

    Event *event = command->getLastInsertedEvent();
    if (event)
        m_nParentView->setSingleSelectedEvent(staffNo, event);
}

const QString ClefInserter::ToolName     = "clefinserter";

}
#include "ClefInserter.moc"

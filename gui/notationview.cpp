/***************************************************************************
                          notationview.cpp  -  description
                             -------------------
    begin                : Fri Sep 22 2000
    copyright            : (C) 2000 by Guillaume Laurent, Chris Cannam, Rich Bown
    email                : glaurent@telegraph-road.org, cannam@all-day-breakfast.com, bownie@bownie.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/times.h>

#include <qcanvas.h>

#include <kmessagebox.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kapp.h>

#include "rosegardenguidoc.h"
#include "notationview.h"
#include "notationelement.h"

#include "staff.h"
#include "notepixmapfactory.h"
#include "qcanvaslinegroupable.h"
#include "qcanvassimplesprite.h"
#include "quantizer.h"
#include "resource.h"

#include "rosedebug.h"


NotationView::NotationView(RosegardenGUIDoc* doc, QWidget *parent)
    : KMainWindow(parent),
      m_config(kapp->config()),
      m_document(doc),
      m_canvasView(new NotationCanvasView(new QCanvas(width() * 2,
                                                      height() * 2),
                                          this)),
      m_mainStaff(new Staff(canvas())),
      m_currentStaff(m_mainStaff),
      m_hlayout(0),
      m_vlayout(0),
      m_currentSelectedNote(Quarter)
{

    kdDebug(KDEBUG_AREA) << "NotationView ctor" << endl;

    setupActions();

    setBackgroundMode(PaletteBase);

    setCentralWidget(m_canvasView);

    QObject::connect(m_canvasView, SIGNAL(noteInserted(int, QMouseEvent*)),
                     this, SLOT(insertNote(int, QMouseEvent*)));

    readOptions();

    if (doc) {

        setCaption(doc->getTitle());

        EventList &allEvents(doc->getEvents());

        m_notationElements = ViewElementsManager::notationElementList(allEvents.begin(),
                                                                      allEvents.end());

        m_mainStaff->move(20, 15);
        m_mainStaff->show();

        m_vlayout = new NotationVLayout(*m_mainStaff);
        m_hlayout = new NotationHLayout(*m_notationElements,
                                        (Staff::noteWidth + 2) * 4, // this shouldn't be constant
                                        4, // 4 beats per bar
                                        40);

        if (applyLayout()) {

            // Show all elements in the staff
            kdDebug(KDEBUG_AREA) << "Elements after layout : "
                                 << *m_notationElements << endl;
            showElements(m_notationElements->begin(), m_notationElements->end(), m_mainStaff);
            showBars(m_notationElements->begin(), m_notationElements->end());

        } else {
            KMessageBox::sorry(0, "Couldn't apply layout");
        }

    } else {
        kdDebug(KDEBUG_AREA) << "NotationView ctor : getDocument() returned 0" << endl;
        KMessageBox::sorry(0, "No document");
    }
}

NotationView::~NotationView()
{
    kdDebug(KDEBUG_AREA) << "-> ~NotationView()\n";

    // Delete canvas items.
    QCanvasItemList allItems = canvas()->allItems();
    QCanvasItemList::Iterator it;

    for(it = allItems.begin(); it != allItems.end(); ++it)
        delete *it;

    delete canvas();
    delete m_hlayout;
    delete m_vlayout;

    saveOptions();

    kdDebug(KDEBUG_AREA) << "<- ~NotationView()\n";
}

void
NotationView::saveOptions()
{	
    m_config->setGroup("Notation Options");
    m_config->writeEntry("Geometry", size());
    m_config->writeEntry("Show Toolbar", toolBar()->isVisible());
    m_config->writeEntry("Show Statusbar",statusBar()->isVisible());
    m_config->writeEntry("ToolBarPos", (int) toolBar()->barPos());
}

void
NotationView::readOptions()
{
    m_config->setGroup("Notation Options");
	
    QSize size(m_config->readSizeEntry("Geometry"));

    if(!size.isEmpty()) {
        resize(size);
    }
}

void
NotationView::setupActions()
{
    // setup Notes menu
    NotePixmapFactory npf;
    QIconSet icon(npf.makeNotePixmap(Whole));
    
    new KAction(i18n("Whole"), icon, 0, this,
                SLOT(slotWhole()), actionCollection(), "whole_note" );

    icon = QIconSet(npf.makeNotePixmap(Half));
    new KAction(i18n("Half"), icon, 0, this,
                SLOT(slotHalf()), actionCollection(), "half" );

    icon = QIconSet(npf.makeNotePixmap(Quarter));
    new KAction(i18n("Quarter"), icon, 0, this,
                SLOT(slotQuarter()), actionCollection(), "quarter" );

    icon = QIconSet(npf.makeNotePixmap(Eighth));
    new KAction(i18n("8th"), icon, 0, this,
                SLOT(slot8th()), actionCollection(), "8th" );

    icon = QIconSet(npf.makeNotePixmap(Sixteenth));
    new KAction(i18n("16th"), icon, 0, this,
                SLOT(slot16th()), actionCollection(), "16th" );

    icon = QIconSet(npf.makeNotePixmap(ThirtySecond));
    new KAction(i18n("32nd"), icon, 0, this,
                SLOT(slot32nd()), actionCollection(), "32nd" );

    icon = QIconSet(npf.makeNotePixmap(SixtyFourth));
    new KAction(i18n("64th"), icon, 0, this,
                SLOT(slot64th()), actionCollection(), "64th" );
    

    // setup edit menu
    KStdAction::undo     (this, SLOT(slotEditUndo()),       actionCollection());
    KStdAction::redo     (this, SLOT(slotEditRedo()),       actionCollection());
    KStdAction::cut      (this, SLOT(slotEditCut()),        actionCollection());
    KStdAction::copy     (this, SLOT(slotEditCopy()),       actionCollection());
    KStdAction::paste    (this, SLOT(slotEditPaste()),      actionCollection());

    // setup Settings menu
    KStdAction::showToolbar  (this, SLOT(slotToggleToolBar()),   actionCollection());
    KStdAction::showStatusbar(this, SLOT(slotToggleStatusBar()), actionCollection());

    KStdAction::saveOptions(this, SLOT(save_options()), actionCollection());
    KStdAction::preferences(this, SLOT(customize()),    actionCollection());

    KStdAction::keyBindings      (this, SLOT(editKeys()),     actionCollection());
    KStdAction::configureToolbars(this, SLOT(editToolbars()), actionCollection());

    createGUI("notation.rc");
}

bool
NotationView::showElements(NotationElementList::iterator from,
                           NotationElementList::iterator to)
{
    return showElements(from, to, 0, 0);
}

bool
NotationView::showElements(NotationElementList::iterator from,
                           NotationElementList::iterator to,
                           QCanvasItem *item)
{
    return showElements(from, to, item->x(), item->y());
}

bool
NotationView::showElements(NotationElementList::iterator from,
                           NotationElementList::iterator to,
                           double dxoffset, double dyoffset)
{
    kdDebug(KDEBUG_AREA) << "NotationElement::showElements()" << endl;

    if (from == to) return true;

    static ChordPixmapFactory npf(*m_mainStaff);

    for(NotationElementList::iterator it = from; it != to; ++it) {

        //
        // process event
        //
        try {
            Note note = Note((*it)->event()->get<Int>("Notation::NoteType"));

            QCanvasSimpleSprite *noteSprite = 0;
                
            if ((*it)->event()->type() == "note") {

                QCanvasPixmap notePixmap(npf.makeNotePixmap(note, true, true));
                noteSprite = new QCanvasSimpleSprite(&notePixmap, canvas());

            } else if ((*it)->event()->type() == "rest") {

                QCanvasPixmap notePixmap(npf.makeRestPixmap(note));
                noteSprite = new QCanvasSimpleSprite(&notePixmap, canvas());

            } else {
                    
                kdDebug(KDEBUG_AREA) << "NotationElement type is neither a note nor a rest - type is "
                                     << (*it)->event()->type()
                                     << endl;
                continue;
            }
                
                
            noteSprite->move(dxoffset + (*it)->x(),
                             dyoffset + (*it)->y());
            noteSprite->show();

            (*it)->setCanvasItem(noteSprite);

        } catch (...) {
            kdDebug(KDEBUG_AREA) << "NotationElement doesn't have a 'Notation::NoteType' property"
                                 << endl;
        }
    }

    kdDebug(KDEBUG_AREA) << "NotationElement::showElements() exiting" << endl;

    return true;
}

bool
NotationView::showBars(NotationElementList::iterator from,
                       NotationElementList::iterator to)
{
    if (from == to) return true;

    const NotationHLayout::barpositions& barPositions(m_hlayout->barPositions());

    NotationElementList::iterator lastElement = to;
    --lastElement;

//     kdDebug(KDEBUG_AREA) << "NotationView::showBars() : from->x = " <<(*from)->x()
//                          << " - lastElement->x = " << (*lastElement)->x() << endl
//                          << "lastElement : " << *(*lastElement) << endl;
    
    m_currentStaff->deleteBars((*from)->x(), (*lastElement)->x());
        
    
    for(NotationHLayout::barpositions::const_iterator it = barPositions.begin();
        it != barPositions.end(); ++it) {

        unsigned int barPos = *it;

        kdDebug(KDEBUG_AREA) << "Adding bar at pos " << barPos << endl;

        m_currentStaff->insertBar(barPos);

    }
    
    return true;
}


bool
NotationView::applyLayout()
{
    bool rch = applyHorizontalLayout();
    bool rcv = applyVerticalLayout();

    kdDebug(KDEBUG_AREA) << "NotationView::applyLayout() : done" << endl;

    return rch && rcv;
}


bool
NotationView::applyHorizontalLayout()
{
    if (!m_hlayout) {
        KMessageBox::error(0, "No Horizontal Layout engine");
        return false;
    }

    m_hlayout->reset();

    m_hlayout->layout(m_notationElements->begin(), m_notationElements->end());

    kdDebug(KDEBUG_AREA) << "NotationView::applyHorizontalLayout() : done" << endl;

    return m_hlayout->status() == 0;
}


bool 
NotationView::applyVerticalLayout()
{
    if (!m_vlayout) {
        KMessageBox::error(0, "No Vertical Layout engine");
        return false;
    }

    for (NotationElementList::iterator i = m_notationElements->begin();
         i != m_notationElements->end(); ++i)
        (*m_vlayout)(*i);
    
    kdDebug(KDEBUG_AREA) << "NotationView::applyVerticalLayout() : done" << endl;

    return m_vlayout->status() == 0;
}

//////////////////////////////////////////////////////////////////////
//                    Slots
//////////////////////////////////////////////////////////////////////

void
NotationView::slotEditUndo()
{
    slotStatusMsg(i18n("Undo..."));

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotEditRedo()
{
    slotStatusMsg(i18n("Redo..."));

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotEditCut()
{
    slotStatusMsg(i18n("Cutting selection..."));

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotEditCopy()
{
    slotStatusMsg(i18n("Copying selection to clipboard..."));

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotEditPaste()
{
    slotStatusMsg(i18n("Inserting clipboard contents..."));

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotToggleToolBar()
{
    slotStatusMsg(i18n("Toggle the toolbar..."));

    if (toolBar()->isVisible())
        toolBar()->hide();
    else
        toolBar()->show();

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}

void
NotationView::slotToggleStatusBar()
{
    slotStatusMsg(i18n("Toggle the statusbar..."));

    if (statusBar()->isVisible())
        statusBar()->hide();
    else
        statusBar()->show();

    slotStatusMsg(i18n(IDS_STATUS_DEFAULT));
}


void
NotationView::slotStatusMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message permanently
    statusBar()->clear();
    statusBar()->changeItem(text, ID_STATUS_MSG);
}


void
NotationView::slotStatusHelpMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message of whole statusbar temporary (text, msec)
    statusBar()->message(text, 2000);
}

//////////////////////////////////////////////////////////////////////

void
NotationView::slotWhole()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slotWhole()\n";
    m_currentSelectedNote = Whole;
}

void
NotationView::slotHalf()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slotHalf()\n";
    m_currentSelectedNote = Half;
}

void
NotationView::slotQuarter()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slotQuarter()\n";
    m_currentSelectedNote = Quarter;
}

void
NotationView::slot8th()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slot8th()\n";
    m_currentSelectedNote = Eighth;
}

void
NotationView::slot16th()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slot16th()\n";
    m_currentSelectedNote = Sixteenth;
}

void
NotationView::slot32nd()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slot32nd()\n";
    m_currentSelectedNote = ThirtySecond;
}

void
NotationView::slot64th()
{
    kdDebug(KDEBUG_AREA) << "NotationView::slot64th()\n";
    m_currentSelectedNote = SixtyFourth;
}

void
NotationView::insertNote(int pitch, QMouseEvent *e)
{
    // create new event
    //
    Event *insertedEvent = new Event;

    insertedEvent->setType("note"); // TODO : we can insert rests too
    
    // set its duration and pitch
    //
    insertedEvent->setTimeDuration(m_hlayout->quantizer().noteDuration(m_currentSelectedNote));
    insertedEvent->set<Int>("pitch", pitch);

    // Create associated notationElement and set its note type
    //
    NotationElement *newNotationElement = new NotationElement(insertedEvent);

    newNotationElement->event()->set<Int>("Notation::NoteType", m_currentSelectedNote);
    newNotationElement->event()->set<String>("Name", "INSERTED_NOTE");

    // give a "fake" X coord to the notation element (where the click was received)
    newNotationElement->setX(e->x());

    // readjust newNotationElement->X

    kdDebug(KDEBUG_AREA) << "NotationHLayout::insertNote : approx. x : "
                         << newNotationElement->x() << endl;

    NotationElementList::iterator closestNote = findClosestNote(newNotationElement->x());

    if (closestNote != m_notationElements->end()) {

//         newNotationElement->setX((*closestNote)->x());

//         kdDebug(KDEBUG_AREA) << "NotationHLayout::insertNote : adjusted x : "
//                              << newNotationElement->x() << endl;

        if ((*closestNote)->isRest()) {

            // replace rest (or part of it) with note
            // TODO
            kdDebug(KDEBUG_AREA) << "NotationHLayout::insertNote : insert over rest is not implemented yet"
                                 << endl;

        } else {

            kdDebug(KDEBUG_AREA) << "NotationHLayout::insertNote : insert over note - absoluteTime = "
                                 << (*closestNote)->absoluteTime()
                                 << endl;

            newNotationElement->setAbsoluteTime((*closestNote)->absoluteTime());
            m_notationElements->insert(newNotationElement);
            
        }
        

    } else { // note is inserted at end (appended)

        kdDebug(KDEBUG_AREA) << "NotationHLayout::insertNote : note appended" << endl;
        m_notationElements->insert(newNotationElement);
//         --closestNote;
//         newNotationElement->setX((*closestNote)->x() + Staff::noteWidth + m_noteMargin);
    }

    // TODO : insert insertedEvent too

    kdDebug(KDEBUG_AREA) << "NotationView::insertNote() : Elements before relayout : "
                         << endl << *m_notationElements << endl;

    (*m_vlayout)(newNotationElement);
    applyHorizontalLayout(); // TODO : be more subtle than this

    kdDebug(KDEBUG_AREA) << "NotationView::insertNote() : Elements after relayout : "
                         << endl << *m_notationElements << endl;

    // (*m_hlayout)(notationElement);

    // TODO : m_currentStaff should be updated by the mouse click 

    if (closestNote != m_notationElements->begin())
        showElements(--closestNote,
                     m_notationElements->end(),
                     m_currentStaff);
    else
        showElements(m_notationElements->begin(),
                     m_notationElements->end(),
                     m_currentStaff);
}


NotationElementList::iterator
NotationView::findClosestNote(double eventX)
{
    double minDist = 10e9,
        prevDist = 10e9;

    NotationElementList::iterator it, res;
    
    for (it = m_notationElements->begin(); it != m_notationElements->end(); ++it) {
        double dist;
        
        if ( (*it)->x() >= eventX)
            dist = (*it)->x() - eventX;
        else
            dist = eventX - (*it)->x();

        if (dist < minDist) {
            kdDebug(KDEBUG_AREA) << "NotationView::findClosestNote() : minDist was "
                                 << minDist << " now = " << dist << endl;
            minDist = dist;
            res = it;
        }
        
        if (dist > prevDist) break; // we can stop right now

        prevDist = dist;
    }

    kdDebug(KDEBUG_AREA) << "NotationView::findClosestNote(" << eventX << ") : found "
                         << *(*res) << endl;

    return res;
}


//////////////////////////////////////////////////////////////////////


void
NotationView::perfTest()
{
    // perf test - add many many notes
    clock_t st, et;
    struct tms spare;
    st = times(&spare);


    cout << "Adding 1000 notes" << endl;
    setUpdatesEnabled(false);

    QCanvasPixmapArray *notePixmap = new QCanvasPixmapArray("pixmaps/note-bodyfilled.xpm");

    for(unsigned int x = 0; x < 1000; ++x) {
        for(unsigned int y = 0; y < 100; ++y) {


            QCanvasSprite *clef = new QCanvasSprite(notePixmap, canvas());

            clef->move(x * 10, y * 10);
        }
    }
    setUpdatesEnabled(true);

    cout << "Done adding 1000 notes" << endl;
    et = times(&spare);

    cout << (et-st)*10 << "ms" << endl;
}

void
NotationView::test()
{
    //     QCanvasEllipse *t = new QCanvasEllipse(10, 10, canvas());
    //     t->setX(50);
    //     t->setY(50);

    //     QBrush brush(blue);
    //     t->setBrush(brush);

    Staff *staff = new Staff(canvas());
    staff->move(20, 15);
		
    staff = new Staff(canvas());
    staff->move(20, 130);
		
    // Add some notes

    QCanvasPixmapArray *notePixmap = new QCanvasPixmapArray("pixmaps/note-bodyfilled.xpm");

    for(unsigned int i = 0; i <= 17; ++i) {
        QCanvasSprite *note = new QCanvasSprite(notePixmap, canvas());
        note->move(20,14);
        note->moveBy(40 + i * 20, staff->pitchYOffset(i));
    }

    ChordPixmapFactory npf(*staff);

    for(unsigned int j = 0; j < 100; ++j) {

        for(unsigned int i = 0; i < 7; ++i) {


            QPixmap note(npf.makeNotePixmap(Note(i), true, true));

            QCanvasSimpleSprite *noteSprite = new QCanvasSimpleSprite(&note,
                                                                      canvas());

            noteSprite->move(50 + (i+j) * 20 , 100);

        }
    }
    
#if 0

    for(unsigned int i = 0; i < 7; ++i) {


        QPixmap note(npf.makeNotePixmap(i, true, false));

        QCanvasSprite *noteSprite = new QCanvasSimpleSpriteSprite(&note,
                                                                  canvas());

        noteSprite->move(50 + i * 20, 150);

    }
#endif

    chordpitches pitches;
    pitches.push_back(6); // something wrong here - pitches aren't in the right order
    pitches.push_back(4);
    pitches.push_back(0);

    QPixmap chord(npf.makeChordPixmap(pitches, Note(6), true, false));

    QCanvasSprite *chordSprite = new QCanvasSimpleSprite(&chord, canvas());

    chordSprite->move(50, 50);
   
}

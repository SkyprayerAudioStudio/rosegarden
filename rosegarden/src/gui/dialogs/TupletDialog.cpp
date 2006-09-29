/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2006
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


#include "TupletDialog.h"
#include <qlayout.h>

#include <klocale.h>
#include "base/NotationTypes.h"
#include "gui/editors/notation/NotationStrings.h"
#include "gui/editors/notation/NotePixmapFactory.h"
#include <kcombobox.h>
#include <kdialogbase.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qobject.h>
#include <qstring.h>
#include <qvbox.h>
#include <qwidget.h>


namespace Rosegarden
{

TupletDialog::TupletDialog(QWidget *parent, Note::Type defaultUnitType,
                           timeT maxDuration) :
        KDialogBase(parent, 0, true, i18n("Tuplet"), Ok | Cancel | Help),
        m_maxDuration(maxDuration)
{
    setHelp("nv-tuplets");
    QVBox *vbox = makeVBoxMainWidget();

    QGroupBox *timingBox = new QGroupBox
                           (1, Horizontal, i18n("New timing for tuplet group"), vbox);

    if (m_maxDuration > 0) {

        // bit of a sanity check
        if (maxDuration < Note(Note::Semiquaver).getDuration()) {
            maxDuration = Note(Note::Semiquaver).getDuration();
        }

        Note::Type maxUnitType =
            Note::getNearestNote(maxDuration / 2, 0).getNoteType();
        if (defaultUnitType > maxUnitType)
            defaultUnitType = maxUnitType;
    }

    QFrame *timingFrame = new QFrame(timingBox);
    QGridLayout *timingLayout = new QGridLayout(timingFrame, 3, 3, 5, 5);

    timingLayout->addWidget(new QLabel(i18n("Play "), timingFrame), 0, 0);

    m_untupledCombo = new KComboBox(timingFrame);
    timingLayout->addWidget(m_untupledCombo, 0, 1);

    m_unitCombo = new KComboBox(timingFrame);
    timingLayout->addWidget(m_unitCombo, 0, 2);

    for (Note::Type t = Note::Shortest; t <= Note::Longest; ++t) {
        Note note(t);
        timeT duration(note.getDuration());
        if (maxDuration > 0 && (2 * duration > maxDuration))
            break;
        timeT e; // error factor, ignore
        m_unitCombo->insertItem(NotePixmapFactory::toQPixmap
                                (NotePixmapFactory::makeNoteMenuPixmap(duration, e)),
                                NotationStrings::makeNoteMenuLabel(duration, false, e, true));
        if (defaultUnitType == t) {
            m_unitCombo->setCurrentItem(m_unitCombo->count() - 1);
        }
    }

    timingLayout->addWidget(new QLabel(i18n("in the time of  "), timingFrame), 1, 0);

    m_tupledCombo = new KComboBox(timingFrame);
    timingLayout->addWidget(m_tupledCombo, 1, 1);

    m_hasTimingAlready = new QCheckBox
                         (i18n("Timing is already correct: update display only"), timingFrame);
    m_hasTimingAlready->setChecked(false);
    timingLayout->addMultiCellWidget(m_hasTimingAlready, 2, 2, 0, 2);

    connect(m_hasTimingAlready, SIGNAL(clicked()), this, SLOT(slotHasTimingChanged()));

    updateUntupledCombo();
    updateTupledCombo();

    m_timingDisplayBox = new QGroupBox
                         (1, Horizontal, i18n("Timing calculations"), vbox);

    QGrid *timingDisplayGrid = new QGrid(3, QGrid::Horizontal, m_timingDisplayBox);

    if (maxDuration > 0) {

        new QLabel(i18n("Selected region:"), timingDisplayGrid);
        new QLabel("", timingDisplayGrid);
        m_selectionDurationDisplay = new QLabel("x", timingDisplayGrid);
        m_selectionDurationDisplay->setAlignment(int(QLabel::AlignVCenter |
                QLabel::AlignRight));
    } else {
        m_selectionDurationDisplay = 0;
    }

    new QLabel(i18n("Group with current timing:"), timingDisplayGrid);
    m_untupledDurationCalculationDisplay = new QLabel("x", timingDisplayGrid);
    m_untupledDurationDisplay = new QLabel("x", timingDisplayGrid);
    m_untupledDurationDisplay->setAlignment(int(QLabel::AlignVCenter |
                                            QLabel::AlignRight));

    new QLabel(i18n("Group with new timing:"), timingDisplayGrid);
    m_tupledDurationCalculationDisplay = new QLabel("x", timingDisplayGrid);
    m_tupledDurationDisplay = new QLabel("x", timingDisplayGrid);
    m_tupledDurationDisplay->setAlignment(int(QLabel::AlignVCenter |
                                          QLabel::AlignRight));

    new QLabel(i18n("Gap created by timing change:"), timingDisplayGrid);
    m_newGapDurationCalculationDisplay = new QLabel("x", timingDisplayGrid);
    m_newGapDurationDisplay = new QLabel("x", timingDisplayGrid);
    m_newGapDurationDisplay->setAlignment(int(QLabel::AlignVCenter |
                                          QLabel::AlignRight));

    if (maxDuration > 0) {

        new QLabel(i18n("Unchanged at end of selection:"), timingDisplayGrid);
        m_unchangedDurationCalculationDisplay = new QLabel
                                                ("x", timingDisplayGrid);
        m_unchangedDurationDisplay = new QLabel("x", timingDisplayGrid);
        m_unchangedDurationDisplay->setAlignment(int(QLabel::AlignVCenter |
                QLabel::AlignRight));

    } else {
        m_unchangedDurationDisplay = 0;
    }

    updateTimingDisplays();

    QObject::connect(m_unitCombo, SIGNAL(activated(const QString &)),
                     this, SLOT(slotUnitChanged(const QString &)));

    QObject::connect(m_untupledCombo, SIGNAL(activated(const QString &)),
                     this, SLOT(slotUntupledChanged(const QString &)));
    QObject::connect(m_untupledCombo, SIGNAL(textChanged(const QString &)),
                     this, SLOT(slotUntupledChanged(const QString &)));

    QObject::connect(m_tupledCombo, SIGNAL(activated(const QString &)),
                     this, SLOT(slotTupledChanged(const QString &)));
    QObject::connect(m_tupledCombo, SIGNAL(textChanged(const QString &)),
                     this, SLOT(slotTupledChanged(const QString &)));
}

void
TupletDialog::slotHasTimingChanged()
{
    updateUntupledCombo();
    updateTupledCombo();
    m_timingDisplayBox->setEnabled(!m_hasTimingAlready->isChecked());
}

TupletDialog::getUnitType() const
{
    return Note::Shortest + m_unitCombo->currentItem();
}

int
TupletDialog::getUntupledCount() const
{
    bool isNumeric = true;
    int count = m_untupledCombo->currentText().toInt(&isNumeric);
    if (count == 0 || !isNumeric)
        return 1;
    else
        return count;
}

int
TupletDialog::getTupledCount() const
{
    bool isNumeric = true;
    int count = m_tupledCombo->currentText().toInt(&isNumeric);
    if (count == 0 || !isNumeric)
        return 1;
    else
        return count;
}

bool
TupletDialog::hasTimingAlready() const
{
    return m_hasTimingAlready->isChecked();
}

void
TupletDialog::updateUntupledCombo()
{
    // Untupled combo can contain numbers up to the maximum
    // duration divided by the unit duration.  If there's no
    // maximum, we'll have to put in some likely values and
    // allow the user to edit it.  Both the numerical combos
    // should possibly be spinboxes, except I think I like
    // being able to "suggest" a few values

    int maxValue = 12;

    if (m_maxDuration) {
        if (m_hasTimingAlready->isChecked()) {
            maxValue = (m_maxDuration * 2) / Note(getUnitType()).getDuration();
        } else {
            maxValue = m_maxDuration / Note(getUnitType()).getDuration();
        }
    }

    QString previousText = m_untupledCombo->currentText();
    if (previousText.toInt() == 0) {
        if (maxValue < 3)
            previousText = QString("%1").arg(maxValue);
        else
            previousText = "3";
    }

    m_untupledCombo->clear();
    bool setText = false;

    for (int i = 1; i <= maxValue; ++i) {
        QString text = QString("%1").arg(i);
        m_untupledCombo->insertItem(text);
        if (m_hasTimingAlready->isChecked()) {
            if (i == (m_maxDuration * 3) / (Note(getUnitType()).getDuration()*2)) {
                m_untupledCombo->setCurrentItem(m_untupledCombo->count() - 1);
            }
        } else if (text == previousText) {
            m_untupledCombo->setCurrentItem(m_untupledCombo->count() - 1);
            setText = true;
        }
    }

    if (!setText) {
        m_untupledCombo->setEditText(previousText);
    }
}

void
TupletDialog::updateTupledCombo()
{
    // should contain all positive integers less than the
    // largest value in the untupled combo.  In principle
    // we can support values larger, but we can't quite
    // do the tupleting transformation yet

    int untupled = getUntupledCount();

    QString previousText = m_tupledCombo->currentText();
    if (previousText.toInt() == 0 ||
            previousText.toInt() > untupled) {
        if (untupled < 2)
            previousText = QString("%1").arg(untupled);
        else
            previousText = "2";
    }

    m_tupledCombo->clear();

    for (int i = 1; i < untupled; ++i) {
        QString text = QString("%1").arg(i);
        m_tupledCombo->insertItem(text);
        if (m_hasTimingAlready->isChecked()) {
            if (i == m_maxDuration / Note(getUnitType()).getDuration()) {
                m_tupledCombo->setCurrentItem(m_tupledCombo->count() - 1);
            }
        } else if (text == previousText) {
            m_tupledCombo->setCurrentItem(m_tupledCombo->count() - 1);
        }
    }
}

void
TupletDialog::updateTimingDisplays()
{
    timeT unitDuration = Note(getUnitType()).getDuration();

    int untupledCount = getUntupledCount();
    int tupledCount = getTupledCount();

    timeT untupledDuration = unitDuration * untupledCount;
    timeT tupledDuration = unitDuration * tupledCount;

    if (m_selectionDurationDisplay) {
        m_selectionDurationDisplay->setText(QString("%1").arg(m_maxDuration));
    }

    m_untupledDurationCalculationDisplay->setText
    (QString("  %1 x %2 = ").arg(untupledCount).arg(unitDuration));
    m_untupledDurationDisplay->setText
    (QString("%1").arg(untupledDuration));

    m_tupledDurationCalculationDisplay->setText
    (QString("  %1 x %2 = ").arg(tupledCount).arg(unitDuration));
    m_tupledDurationDisplay->setText
    (QString("%1").arg(tupledDuration));

    m_newGapDurationCalculationDisplay->setText
    (QString("  %1 - %2 = ").arg(untupledDuration).arg(tupledDuration));
    m_newGapDurationDisplay->setText
    (QString("%1").arg(untupledDuration - tupledDuration));

    if (m_selectionDurationDisplay && m_unchangedDurationDisplay) {
        if (m_maxDuration != untupledDuration) {
            m_unchangedDurationCalculationDisplay->setText
            (QString("  %1 - %2 = ").arg(m_maxDuration).arg(untupledDuration));
        } else {
            m_unchangedDurationCalculationDisplay->setText("");
        }
        m_unchangedDurationDisplay->setText
        (QString("%1").arg(m_maxDuration - untupledDuration));
    }
}

void
TupletDialog::slotUnitChanged(const QString &)
{
    updateUntupledCombo();
    updateTupledCombo();
    updateTimingDisplays();
}

void
TupletDialog::slotUntupledChanged(const QString &)
{
    updateTupledCombo();
    updateTimingDisplays();
}

void
TupletDialog::slotTupledChanged(const QString &)
{
    updateTimingDisplays();
}

}
#include "TupletDialog.moc"

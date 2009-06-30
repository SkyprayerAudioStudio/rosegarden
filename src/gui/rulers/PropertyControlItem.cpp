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

#include "PropertyControlItem.h"
#include "ControlRuler.h"
#include "gui/editors/matrix/MatrixElement.h"
#include "base/BaseProperties.h"
#include "base/Event.h"
#include "base/PropertyName.h"

namespace Rosegarden {

PropertyControlItem::PropertyControlItem(ControlRuler *controlRuler,
        PropertyName propertyname,
        MatrixElement *element,
        QPolygonF polygon)
: ControlItem(controlRuler,element->event(),polygon),
    m_element(element),
    m_propertyname(propertyname)
{
//    element->addObserver(this);
}

PropertyControlItem::~PropertyControlItem()
{
    if (m_element) {
//        m_element->removeObserver(this);
    }
}

void PropertyControlItem::update()
{
    if (!m_element) return;

    double x0 = m_element->getLayoutX();
    double x1 = m_element->getWidth() + x0;

    long val = 0;
    if (m_propertyname == BaseProperties::VELOCITY) {
        val = (long)m_element->getElementVelocity();
    } else {
        m_element->event()->get<Rosegarden::Int>(m_propertyname, val);
    }

    double y = (double) val / MIDI_CONTROL_MAX_VALUE;
    QPolygonF newpoly;
    newpoly << QPointF(x0,0) << QPointF(x0,y) << QPointF(x1,y) << QPointF(x1,0);
//    QPolygonF newpoly(QRectF(x0,0,x1-x0,y));
    this->clear();
    *this << newpoly;

    m_controlRuler->update();
}

}
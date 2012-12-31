/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2013 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _CONTROLPARAMETER_H_
#define _CONTROLPARAMETER_H_

#include <string>

#include "base/Event.h"
#include "XmlExportable.h"
#include "MidiProgram.h"

namespace Rosegarden
{
class Event;
    
class ControlParameter : public XmlExportable
{
public:
    ControlParameter();
    ControlParameter(const std::string &name,
                     const std::string &type,
                     const std::string &description,
                     int min = 0,
                     int max = 127,
                     int def = 0,
                     MidiByte controllerValue = 0,
                     unsigned int colour = 0,
                     int ipbPositon = -1);
    ControlParameter(const ControlParameter &control);
    ControlParameter& operator=(const ControlParameter &control);
    bool operator==(const ControlParameter &control);

    friend bool operator<(const ControlParameter &a, const ControlParameter &b);

    // ControlParameter comparison on IPB position
    //
    struct ControlPositionCmp
    {
        bool operator()(ControlParameter *c1,
                        ControlParameter *c2)
        {
            return (c1->getIPBPosition() < c2->getIPBPosition());
        }

        bool operator()(const ControlParameter &c1,
                        const ControlParameter &c2)
        {
            return (c1.getIPBPosition() < c2.getIPBPosition());
        }
    };

    std::string getName() const { return m_name; }
    std::string getType() const { return m_type; }
    std::string getDescription() const { return m_description; }

    int getMin() const { return m_min; }
    int getMax() const { return m_max; }
    int getDefault() const { return m_default; }
    int clamp(int value) const {
        if (value < m_min) { return m_min; }
        if (value > m_max) { return m_max; }
        return value;
    }

    MidiByte getControllerValue() const { return m_controllerValue; }

    unsigned int getColourIndex() const { return m_colourIndex; }

    int getIPBPosition() const { return m_ipbPosition; }

    void setName(const std::string &name) { m_name = name; }
    void setType(const std::string &type) { m_type = type; }
    void setDescription(const std::string &des) { m_description = des; }

    void setMin(int min) { m_min = min; }
    void setMax(int max) { m_max = max; }
    void setDefault(int def) { m_default = def; }

    void setControllerValue(MidiByte con) { m_controllerValue = con; }

    void setColourIndex(unsigned int colour) { m_colourIndex = colour; }

    void setIPBPosition(int position) { m_ipbPosition = position; }

    virtual std::string toXmlString();

    Event *newEvent(timeT time, int value) const;
    static const ControlParameter& getPitchBend(void);

protected:

    // ControlParameter name as it's displayed (eg "Velocity" or "Controller")
    std::string    m_name;

    // The type of event this controller controls (eg "controller" or
    // "pitchbend"); 
    std::string    m_type;

    // Descriptive name for this control parameter, or "<none>".
    std::string    m_description;

    int            m_min;
    int            m_max;
    int            m_default;

    MidiByte       m_controllerValue;

    unsigned int   m_colourIndex;

    int            m_ipbPosition; // position on Instrument Parameter Box


};

}

#endif // _CONTROLPARAMETER_H_

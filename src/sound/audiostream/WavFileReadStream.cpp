/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#ifdef HAVE_LIBSNDFILE

#include "WavFileReadStream.h"

#include <iostream>

#include "misc/Debug.h"

namespace Rosegarden
{

static QStringList
getSupportedExtensions()
{
    QStringList extensions;
    int count;
    
    if (sf_command(0, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(count))) {
        extensions.push_back("wav");
        extensions.push_back("aiff");
        extensions.push_back("aifc");
        extensions.push_back("aif");
        return extensions;
    }

    SF_FORMAT_INFO info;
    for (int i = 0; i < count; ++i) {
        info.format = i;
        if (!sf_command(0, SFC_GET_FORMAT_MAJOR, &info, sizeof(info))) {
            extensions.push_back(QString(info.extension).toLower());
        }
    }

    return extensions;
}

static
AudioReadStreamBuilder<WavFileReadStream>
wavbuilder(
    QUrl("http://breakfastquay.com/rdf/rosegarden/fileio/WavFileReadStream"),
    getSupportedExtensions()
    );

WavFileReadStream::WavFileReadStream(QString path) :
    m_file(0),
    m_path(path),
    m_offset(0)
{
    m_channelCount = 0;
    m_sampleRate = 0;

    m_fileInfo.format = 0;
    m_fileInfo.frames = 0;
    m_file = sf_open(m_path.toLocal8Bit().data(), SFM_READ, &m_fileInfo);

    if (!m_file || m_fileInfo.frames <= 0 || m_fileInfo.channels <= 0) {
	std::cerr << "WavFileReadStream::initialize: Failed to open file \""
                  << path << "\" ("
		  << sf_strerror(m_file) << ")" << std::endl;

	if (m_file) {
	    m_error = QString("Couldn't load audio file '") +
                m_path + "':\n" + sf_strerror(m_file);
	} else {
	    m_error = QString("Failed to open audio file '") +
		m_path + "'";
	}
	return;
    }

    m_channelCount = m_fileInfo.channels;
    m_sampleRate = m_fileInfo.samplerate;

    sf_seek(m_file, 0, SEEK_SET);
}

WavFileReadStream::~WavFileReadStream()
{
    if (m_file) sf_close(m_file);
}

size_t
WavFileReadStream::getFrames(size_t count, float *frames)
{
    if (!m_file || !m_channelCount) return 0;
    if (count == 0) return 0;

    if ((long)m_offset >= m_fileInfo.frames) {
	return 0;
    }

    sf_count_t readCount = 0;

    if ((readCount = sf_readf_float(m_file, frames, count)) < 0) {
        return 0;
    }

    m_offset = m_offset + readCount;

    return readCount;
}

}

#endif
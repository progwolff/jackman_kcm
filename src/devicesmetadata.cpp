/*
 *   Copyright 2013 by Julian Wolff <wolff@julianwolff.de>
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *  
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *  
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define TRANSLATION_DOMAIN "kcm-jackman"
#include <KLocalizedString>
#include "config.h"

#include "devicesmetadata.h"

#include <QSharedData>
#include <QSharedPointer>

#include <KDesktopFile>
#include <KConfigGroup>

class DevicesMetadataPrivate : public QSharedData
{
public:
    DevicesMetadataPrivate()
    {
        name = QString();
        device = QString();
        vendor = QString();
        nperiods = 2;
        hwmon = false;
        hwmeter = false;
        duplex = true;
        softmode = false;
        monitor = false;
        dither = DevicesMetadata::Dither::None;
        inchannels = 0;
        outchannels = 0;
        shorts = false;
        inputlatency = 0;
        outputlatency = 0;
        mididriver = DevicesMetadata::MIDIDriver::NoDriver;
        buffersize = 256;
        samplerate = 44100;
    }
    QString name;
    QString device;
    QString vendor;
    int nperiods;
    bool hwmon;
    bool hwmeter;
    bool duplex;
    bool softmode;
    bool monitor;
    int dither;
    int inchannels;
    int outchannels;
    bool shorts;
    int inputlatency;
    int outputlatency;
    int mididriver;
    int buffersize;
    int samplerate;
};

DevicesMetadata::DevicesMetadata(const QString &id, const QString &conf)
: d(new DevicesMetadataPrivate)
{
    d->name = QString();
    d->device = QString();
    d->vendor = QString();
    d->nperiods = 2;
    d->hwmon = false;
    d->hwmeter = false;
    d->duplex = true;
    d->softmode = false;
    d->monitor = false;
    d->dither = DevicesMetadata::Dither::None;
    d->inchannels = 0;
    d->outchannels = 0;
    d->shorts = false;
    d->inputlatency = 0;
    d->outputlatency = 0;
    d->mididriver = DevicesMetadata::MIDIDriver::NoDriver;
    d->samplerate = 44100;
    d->buffersize = 256;
    
    d->name = id;
    foreach (const QString& config, conf.split(';'))
    {
        const QString &key = config.split('=').first().trimmed().toLower();
        const QString &val = config.split('=').last().trimmed().toLower();
        if(key == "device") d->device = val;
        if(key == "nperiods") d->nperiods = val.toInt();
        else if(key == "hwmon") d->hwmon = (val=="true");
        else if(key == "hwmeter") d->hwmeter = (val=="true");
        else if(key == "duplex") d->duplex = (val=="true");
        else if(key == "softmode") d->softmode = (val=="true");
        else if(key == "monitor") d->monitor = (val=="true");
        else if(key == "dither") {
            if(val == "none" || val == "n") d->dither = Dither::None;
            else if(val == "rectangular" || val == "r") d->dither = Dither::Rectangular;
            else if(val == "triangular" || val == "t") d->dither = Dither::Triangular;
            else if(val == "shaped" || val == "s") d->dither = Dither::Shaped;
            else d->dither = Dither::None;
        }
        else if(key == "inchannels") d->inchannels = val.toInt();
        else if(key == "outchannels") d->outchannels = val.toInt();
        else if(key == "shorts") d->shorts = (val=="true");
        else if(key == "input-latency") d->inputlatency = val.toInt();
        else if(key == "output-latency") d->outputlatency = val.toInt();
        else if(key == "midi-driver") {
            if(val == "none") d->mididriver = MIDIDriver::NoDriver;
            else if(val == "raw") d->mididriver = MIDIDriver::Raw;
            else if(val == "seq") d->mididriver = MIDIDriver::Sequencer;
            else d->mididriver = MIDIDriver::NoDriver;
        }
        else if(key == "period") d->buffersize = val.toInt();
        else if(key == "rate") d->samplerate = val.toInt();
        else if(key == "vendor") d->vendor = val;
    }
}

DevicesMetadata::DevicesMetadata(const DevicesMetadata &other)
: d(other.d)
{
}

DevicesMetadata& DevicesMetadata::operator=( const DevicesMetadata &other )
{
    if ( this != &other )
        d = other.d;
    
    return *this;
}

DevicesMetadata::~DevicesMetadata()
{
}

QString DevicesMetadata::name() const
{
    return d->name;
}

QString DevicesMetadata::device() const
{
    return d->device;
}

QString DevicesMetadata::vendor() const
{
    return d->vendor;
}

int DevicesMetadata::nperiods() const
{
    return d->nperiods;
}
bool DevicesMetadata::hwmon() const
{
    return d->hwmon;
}
bool DevicesMetadata::hwmeter() const
{
    return d->hwmeter;
}
bool DevicesMetadata::duplex() const
{
    return d->duplex;
}
bool DevicesMetadata::softmode() const
{
    return d->softmode;
}
bool DevicesMetadata::monitor() const
{
    return d->monitor;
}
int DevicesMetadata::dither() const
{
    return d->dither;
}
int DevicesMetadata::inchannels() const
{
    return d->inchannels;
}
int DevicesMetadata::outchannels() const
{
    return d->outchannels;
}
bool DevicesMetadata::shorts() const
{
    return d->shorts;
}
int DevicesMetadata::inputlatency() const
{
    return d->inputlatency;
}
int DevicesMetadata::outputlatency() const
{
    return d->outputlatency;
}
int DevicesMetadata::mididriver() const
{
    return d->mididriver;
}
int DevicesMetadata::samplerate() const
{
    return d->samplerate;
}
int DevicesMetadata::buffersize() const
{
    return d->buffersize;
}

void DevicesMetadata::resetDevice()
{
    d->device.clear();
}




QString DevicesMetadata::dump() const 
{
    return "rate="+QString::number(samplerate())+";"
    +"period="+QString::number(buffersize())+";"
    +"nperiods="+QString::number(nperiods())+";"
    +"hwmon="+(hwmon()?"true":"false")+";"
    +"hwmeter="+(hwmeter()?"true":"false")+";"
    +"duplex="+(duplex()?"true":"false")+";"
    +"softmode="+(softmode()?"true":"false")+";"
    +"monitor="+(monitor()?"true":"false")+";"
    +"dither="+((dither()==Dither::None)?"n":((dither()==Dither::Rectangular)?"r":((dither()==Dither::Shaped)?"s":((dither()==Dither::Triangular)?"t":"n"))))+";"
    +((inchannels() > 0)?("inchannels="+QString::number(inchannels())+";"):QString())
    +((outchannels() > 0)?("outchannels="+QString::number(outchannels())+";"):QString())
    +"shorts="+(shorts()?"true":"false")+";"
    +"input-latency="+QString::number(inputlatency())+";"
    +"output-latency="+QString::number(outputlatency())+";"
    +"midi-driver="+((mididriver()==MIDIDriver::NoDriver)?"none":((mididriver()==MIDIDriver::Sequencer)?"seq":((mididriver()==MIDIDriver::Raw)?"raw":"none")));
}

/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
    Copyright 2014 by David Edmundson <davidedmundson@kde.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef THEMEMETADATA_H
#define THEMEMETADATA_H

#include <QString>
#include <QSharedDataPointer>

class DevicesMetadataPrivate;

class DevicesMetadata
{
public:
    enum Dither {None=0, Rectangular=1, Shaped=2, Triangular=3};
    enum MIDIDriver {NoDriver=0, Sequencer=1, Raw=2};
    
    explicit DevicesMetadata(const QString &id, const QString &conf = QString());
    DevicesMetadata(const DevicesMetadata &other);
    DevicesMetadata& operator=(const DevicesMetadata& other);

    ~DevicesMetadata();
    
    QString name() const;
    QString device() const;
    QString vendor() const;
    int nperiods() const;
    bool hwmon() const;
    bool hwmeter() const;
    bool duplex() const;
    bool softmode() const;
    bool monitor() const;
    int dither() const;
    int inchannels() const;
    int outchannels() const;
    bool shorts() const;
    int inputlatency() const;
    int outputlatency() const;
    int mididriver() const;
    int buffersize() const;
    int samplerate() const;
    
    void resetDevice();
    
    QString dump() const;
    
private:
    QSharedDataPointer<DevicesMetadataPrivate> d;
};
#endif //DEVICESMETADATA_H

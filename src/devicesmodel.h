/*
    Copyright 2016 Julian Wolff <wolff@julianwolff.de>

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

#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QAbstractListModel>

class DevicesMetadata;

class DevicesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole,
    NumberRole,
	DeviceRole,
        NPeriodsRole,
        HWMonRole,
        HWMeterRole,
        DuplexRole,
        SoftmodeRole,
        MonitorRole,
        DitherRole,
        InChannelsRole,
        OutChannelsRole,
        ShortsRole,
        InputLatencyRole,
        OutputLatencyRole,
        MidiDriverRole,
	BufferSizeRole,
	SampleRateRole,
	MasterRole,
	ConfigRole,
    AttachedRole,
    NotListedRole
    };
               
    explicit DevicesModel(QObject *parent=0);
    virtual ~DevicesModel();
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    
    void populate();
    void update();
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    
    
signals:
    void changed(bool);
    
private:
    void add(const QString &name, const QString &conf);
    const QStringList alsaInOut(const QString& name) const;
    const QString deviceName(const QString& card, int device) const;
    const QString deviceVendor(const QString& card, int device) const;
    const QString currentMaster() const;
    const QString nameFromNumber(int cardnum, int device) const;
    int numberFromName(const QString& name) const;
    
    QString m_currentMaster;
    
    QList<DevicesMetadata> mDevicesList;
    
    mutable QList<bool> mInOutCache;
    
    int movedindex;
    
    static QStringList vendorList;
};

#endif //DEVICESMODEL_H

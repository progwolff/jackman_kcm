/*
 *    Copyright 2016 by Julian Wolff <wolff@julianwolff.de>
 * 
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define TRANSLATION_DOMAIN "kcm-jackman"
#include <KLocalizedString>
#include "config.h"

#include "devicesmodel.h"

#include <QDir>
#include <QString>
#include <QIcon>
#include <QFont>

#include <KConfig>
#include <KConfigGroup>
#include <QDebug>
#include <KSharedConfig>
#include <QStandardPaths>
#include <QProcess>
#include <QRegularExpression>

#include <alsa/global.h>
#include <alsa/output.h>
#include <alsa/input.h>
#include <alsa/conf.h>
#include <alsa/pcm.h>
#include <alsa/control.h>

#include "devicesmetadata.h"

QStringList DevicesModel::vendorList = QStringList() //taken from http://www.alsa-project.org/main/index.php/Matrix:Main
<< "a-trend"<<"akai"<<"alsa"<<"ali"<<"amd"<<""<<"asound"<<"ati"<<"abit"<<"adlib"<<"airis"<<"alesis"<<"analog_devices"<<"aopen"<<"apple"<<"apple_computer_inc"<<"asound"<<"asus"<<"audioexcel"<<"audioscience"<<"audiotrak"<<"aureal"<<"auzentech"<<"avance_logic"<<"axago"<<"aztech_system_ltd"<<""<<"boss"<<"behringer"<<"best_union"<<"bgears"<<"brooktree"<<"c-media"<<"cme"<<"chic_technology"<<"cirrus_logic"<<"club3d"<<"compustar"<<"core_sound"<<"creative_labs"<<"cyrix"<<"diamond_multimedia"<<"digigram"<<"digital_audio_labs"<<"dream"<<"dynasonic"<<"e-mu"<<"esi"<<"ess_technology"<<"echo_corporation"<<"edirol"<<"ego_sys"<<"emagic"<<"ensoniq"<<"evolution"<<"focusrite"<<"fortemedia"<<"freescale"<<"frontier_design"<<"gadget_labs"<<"genius"<<"gibson"<<"gravis"<<"griffin"<<"guillemot"<<"his"<<"ht_omega"<<"hercules"<<"hoontech"<<"ic_ensemble"<<"idt"<<"intel"<<"jaton"<<"korg"<<"lacie"<<"labway"<<"lexicon"<<"lynx_studio_technology"<<"maudio"<<"mackie"<<"mad-dog-multimedia"<<"marian"<<"mark_of_the_unicorn"<<"mediatek"<<"miditech"<<"mixvibes"<<"nec"<<"native_instruments"<<"neomagic"<<"nuforce"<<"nvidia"<<"oak_technology"<<"onkyo"<<"opcode"<<"opti"<<"pine"<<"phillips"<<"powercolor"<<"rme"<<"razer"<<"roland_edirol"<<"s3"<<"sekd"<<"stb"<<"seasound"<<"sega"<<"sennheiser"<<"shark_multimedia"<<"shuttle"<<"sis"<<"sims"<<"sondigo"<<"sonorus"<<"sony"<<"steinberg"<<"stereo-link"<<"tascam"<<"tempotec"<<"terratec"<<"toshiba"<<"trident"<<"trust"<<"turtle_beach"<<"via"<<"vlsi"<<"videologic"<<"xitel"<<"yamaha"<<"zefiro"<<"zoltrix";

DevicesModel::DevicesModel(QObject *parent)
: QAbstractListModel(parent)
{
}

DevicesModel::~DevicesModel()
{
}

int DevicesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    
    return mDevicesList.size();
}

QVariant DevicesModel::data(const QModelIndex &index, int role) const
{
    const DevicesMetadata metadata = mDevicesList[index.row()];
    
    QString devicename;
    switch(role) {
        case Qt::DisplayRole:
            return metadata.name()+(metadata.device().isEmpty()?"":" ["+metadata.device()+"]");
        case Qt::DecorationRole:
            if(!metadata.device().isEmpty())
            {
                QIcon ico;
                if(!metadata.vendor().isEmpty())
                    ico = QIcon::fromTheme("vendor-"+metadata.vendor(),QIcon::fromTheme("audio-card"));
                return ico;
            }
            else
            {
                QIcon ico = QIcon::fromTheme("dialog-warning");//QVariant();
                return ico;
            }
        case Qt::FontRole:
            if(metadata.name() == m_currentMaster)  
            {
                QFont font;
                font.setBold(true);
                return font;
            }
        case DevicesModel::MasterRole:
            return QVariant::fromValue<bool>(metadata.name() == m_currentMaster);
        case DevicesModel::IdRole:
            return metadata.name();
        case DevicesModel::DeviceRole:
            return metadata.device();
        case DevicesModel::NPeriodsRole:
            return metadata.nperiods();
        case DevicesModel::HWMonRole:
            return metadata.hwmon();
        case DevicesModel::HWMeterRole:
            return metadata.hwmeter();
        case DevicesModel::DuplexRole:
            return metadata.duplex();
        case DevicesModel::SoftmodeRole:
            return metadata.softmode();
        case DevicesModel::MonitorRole:
            return metadata.monitor();
        case DevicesModel::DitherRole:
            return metadata.dither();
        case DevicesModel::InChannelsRole:
            return metadata.inchannels();
        case DevicesModel::OutChannelsRole:
            return metadata.outchannels();
        case DevicesModel::ShortsRole:
            return metadata.shorts();
        case DevicesModel::InputLatencyRole:
            return metadata.inputlatency();
        case DevicesModel::OutputLatencyRole:
            return metadata.outputlatency();
        case DevicesModel::MidiDriverRole:
            return metadata.mididriver();
        case DevicesModel::BufferSizeRole:
            return metadata.buffersize();
        case DevicesModel::SampleRateRole:
            return metadata.samplerate();
        case DevicesModel::ConfigRole:
            return metadata.dump();
    }
    
    return QVariant();
}

void DevicesModel::populate()
{
    //get connected devices, code stolen from aplay.c 
    QStringList devices;
    mDevicesList.clear();
    
    snd_ctl_t *handle;
    int card, err, dev, idx;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);
    
    card = -1;
    if (snd_card_next(&card) >= 0 && card >= 0) 
    {
        
        while (card >= 0) {
            char name[32];
            sprintf(name, "hw:%d", card);
            if ((err = snd_ctl_open(&handle, name, 0)) < 0) 
            {
                goto next_card;
            }
            if ((err = snd_ctl_card_info(handle, info)) < 0) 
            {
                snd_ctl_close(handle);
                goto next_card;
            }
            dev = -1;
            while (true) 
            {
                unsigned int count;
                //if (
                snd_ctl_pcm_next_device(handle, &dev);
                //<0)
                //error("snd_ctl_pcm_next_device");
                if (dev < 0)
                    break;
                snd_pcm_info_set_device(pcminfo, dev);
                snd_pcm_info_set_subdevice(pcminfo, 0);
                //snd_pcm_info_set_stream(pcminfo, stream);
                if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
                    //if (err != -ENOENT)
                    //  error("control digital audio info (%i): %s", card, snd_strerror(err));
                    continue;
                }
                
                devices.append(
                    QString::fromLatin1(snd_ctl_card_info_get_name(info))
                    +","+QString::number(dev)
                );
            }
            snd_ctl_close(handle);
            next_card:
            if (snd_card_next(&card) < 0) 
            {
                //error("snd_card_next");
                break;
            }
        }
    }
    
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    KSharedConfigPtr jackConfig = KSharedConfig::openConfig(dir+"/"+JACK_CONFIG_FILE);
    //read existing config
    devices.append( jackConfig->group("Devices").keyList() );
    //qDebug() << dir+"/"+JACK_CONFIG_FILE;
    
    foreach (const QString &device, devices) {
        //qDebug() << "found device config for " << device;
        const QString& conf = jackConfig->group("Devices").readEntry(device);
        add(device, conf);
    }
    
    m_currentMaster = currentMaster();
}

void DevicesModel::update()
{
    populate();
    /*
     *    m_currentMaster = currentMaster();
     *    for(DevicesMetadata& m : mDevicesList)
     *    {
     *"QString devicename = deviceName(m.name().split(',').first(), m.name().split(',').last().toInt());
     *"if(devicename.isEmpty())
     *"  m.resetDevice();
}
*/
}

const QString DevicesModel::deviceName(const QString& cardname, int device) const
{
    //qDebug() << "looking for " << cardname << "," << device;
    snd_ctl_t *handle;
    int card, err, dev, idx;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);
    
    QString devicename;
    
    card = -1;
    if (snd_card_next(&card) >= 0 && card >= 0) 
    {
        
        while (card >= 0) {
            char name[32];
            sprintf(name, "hw:%d", card);
            if ((err = snd_ctl_open(&handle, name, 0)) < 0) 
            {
                goto devicename_next_card;
            }
            if ((err = snd_ctl_card_info(handle, info)) < 0) 
            {
                snd_ctl_close(handle);
                goto devicename_next_card;
            }
            dev = -1;
            while (true) 
            {
                unsigned int count;
                //if (
                snd_ctl_pcm_next_device(handle, &dev);
                //<0)
                //error("snd_ctl_pcm_next_device");
                if (dev < 0)
                    break;
                snd_pcm_info_set_device(pcminfo, dev);
                snd_pcm_info_set_subdevice(pcminfo, 0);
                //snd_pcm_info_set_stream(pcminfo, stream);
                if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
                    //if (err != -ENOENT)
                    //  error("control digital audio info (%i): %s", card, snd_strerror(err));
                    continue;
                }
                if(QString::fromLatin1(snd_ctl_card_info_get_name(info)).trimmed() == cardname
                    && dev == device)
                {
                    devicename = QString::fromLatin1(snd_pcm_info_get_name(pcminfo));
                    //qDebug() << "device name of " << cardname << "," << device << " is " << devicename;
                }
                else
                {
                    //qDebug() << "not looking for " << snd_ctl_card_info_get_name(info) << "," << dev << " ...";
                }
            }
            snd_ctl_close(handle);
            devicename_next_card:
            if (!devicename.isEmpty() || snd_card_next(&card) < 0) 
            {
                //error("snd_card_next");
                break;
            }
        }
    }
    return devicename;
}

const QString DevicesModel::deviceVendor(const QString& cardname, int device) const
{
    //qDebug() << "looking for " << cardname << "," << device;
    snd_ctl_t *handle;
    int card, err, dev, idx;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);
    
    QString vendor;
    
    card = -1;
    if (snd_card_next(&card) >= 0 && card >= 0) 
    {
        
        while (card >= 0) {
            char name[32];
            sprintf(name, "hw:%d", card);
            if ((err = snd_ctl_open(&handle, name, 0)) < 0) 
            {
                goto devicename_next_card;
            }
            if ((err = snd_ctl_card_info(handle, info)) < 0) 
            {
                snd_ctl_close(handle);
                goto devicename_next_card;
            }
            dev = -1;
            while (true) 
            {
                unsigned int count;
                //if (
                snd_ctl_pcm_next_device(handle, &dev);
                //<0)
                //error("snd_ctl_pcm_next_device");
                if (dev < 0)
                    break;
                snd_pcm_info_set_device(pcminfo, dev);
                snd_pcm_info_set_subdevice(pcminfo, 0);
                //snd_pcm_info_set_stream(pcminfo, stream);
                if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
                    //if (err != -ENOENT)
                    //  error("control digital audio info (%i): %s", card, snd_strerror(err));
                    continue;
                }
                if(QString::fromLatin1(snd_ctl_card_info_get_name(info)).trimmed() == cardname
                    && dev == device)
                {
                    for(const QString& str : QString::fromLatin1(snd_ctl_card_info_get_longname(info)).toLower().split(' '))
                    {
                        if(vendorList.contains(str))
                        {
                            vendor = str;
                            break;
                        }
                    }
                }
                else
                {
                    //qDebug() << "not looking for " << snd_ctl_card_info_get_name(info) << "," << dev << " ...";
                }
            }
            snd_ctl_close(handle);
            devicename_next_card:
            if (!vendor.isEmpty() || snd_card_next(&card) < 0) 
            {
                //error("snd_card_next");
                break;
            }
        }
    }
    return vendor;
}


const QString DevicesModel::currentMaster() const
{
    // fuser -v /dev/snd/pcmC0D* 2>&1 | grep jack | grep -o -P '[^:]*' | head -1
    QStringList env;
    env.clear();
    QProcess *exec;
    env << "PATH=/usr/bin:/usr/local/bin";
    exec = new QProcess();
    exec->setEnvironment(env);
    exec->start("/bin/bash", QStringList() << "-c" << "/usr/bin/fuser -v /dev/snd/pcm* /dev/snd/dsp*");
    
    if(!exec->waitForFinished(3000))
        return QString();
    
    QString result = exec->readAllStandardError();
    
    
    for(const QString& line : result.split('\n'))
    {
        //qDebug() << "result: " << line;
        if(line.indexOf("jackdbus") != -1)
        {
            QRegularExpressionMatchIterator it = QRegularExpression("(\\d+)").globalMatch(line.split(':').first());
            
            if(!it.hasNext()) return QString();
            int cardnum = it.next().captured(1).toInt();
            if(!it.hasNext()) return QString();
            int device = it.next().captured(1).toInt();
            snd_ctl_t *handle;
            int card, err, dev, idx;
            snd_ctl_card_info_t *info;
            snd_pcm_info_t *pcminfo;
            snd_ctl_card_info_alloca(&info);
            snd_pcm_info_alloca(&pcminfo);
            
            card = -1;
            
            //qDebug() << "Jack master is " << cardnum << "," << device;
            
            QString ret;
            if (snd_card_next(&card) >= 0 && card >= 0) 
            {
                
                while (card >= 0) {
                    char name[32];
                    sprintf(name, "hw:%d", card);
                    if ((snd_ctl_open(&handle, name, 0)) < 0) 
                    {
                        qDebug() << "snd_ctl_open failed";
                        goto master_next_card;
                    }
                    if ((snd_ctl_card_info(handle, info)) < 0) 
                    {
                        qDebug() << "snd_ctl_card_info failed";
                        snd_ctl_close(handle);
                        goto master_next_card;
                    }
                    if(card < cardnum)
                        goto master_next_card;
                    ret = QString::fromLatin1(snd_ctl_card_info_get_name(info))+","+QString::number(device);
                    //qDebug() << "Jack master is " << ret;
                    snd_ctl_close(handle);
                    return ret;
                    master_next_card:
                    if (snd_card_next(&card) < 0) 
                    {
                        //error("snd_card_next");
                        break;
                    }
                }
            }
        }
    }
    
    return QString();
}

void DevicesModel::add(const QString &id, const QString &conf)
{
    beginInsertRows(QModelIndex(), mDevicesList.count(), mDevicesList.count());
    
    for(int i=0; i<mDevicesList.size(); ++i)
    {
        if(mDevicesList.at(i).name() == id)
            mDevicesList.removeAt(i);
    }
    
    QString _conf = conf+";device="+deviceName(id.split(',').first(), id.split(',').last().toInt());
    _conf = _conf+";vendor="+deviceVendor(id.split(',').first(), id.split(',').last().toInt());
    mDevicesList.insert( conf.split("priority=").last().split(';').first().toInt(), DevicesMetadata(id, _conf) );
    
    endInsertRows();
}

bool DevicesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    movedindex = row;
    
    emit changed(true);
    
    return true;
}

Qt::DropActions DevicesModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags DevicesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | defaultFlags;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | defaultFlags;
}

bool DevicesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(QModelIndex(), mDevicesList.count(), mDevicesList.count());
    
    DevicesMetadata d = mDevicesList.at(row);
    
    if(row > movedindex)
        mDevicesList.removeAt(row);
    
    mDevicesList.insert(movedindex, d);
    
    qDebug() << "moved " << d.name() << " from " << row << " to " << movedindex;
    
    if(row <= movedindex)
        mDevicesList.removeAt(row);
    
    endInsertRows();
    
    return true;
}

void DevicesModel::dump(const QString &id)
{
    Q_UNUSED(id)
    
    /*ThemeMetadata metadata(path);
     * 
     *    qDebug() << "Theme Path:" << metadata.path();
     *    qDebug() << "Name: " << metadata.name();
     *    qDebug() << "Version: " << metadata.version();
     *    qDebug() << "Author: " << metadata.author();
     *    qDebug() << "Description: " << metadata.description();
     *    qDebug() << "Email: " << metadata.email();
     *    qDebug() << "License: " << metadata.license();
     *    qDebug() << "Copyright: " << metadata.copyright();
     *    qDebug() << "Screenshot: " << metadata.screenshot();*/
    //TODO
}

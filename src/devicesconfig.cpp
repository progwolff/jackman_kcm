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

#include "devicesconfig.h"
#include "ui_devicesconfig.h"
#include "devicesmodel.h"
#include "devicesmetadata.h"
//#include "devicesdelegate.h"

#include <QFile>
#include <QQuickView>
#include <QQuickItem>
#include <QListView>
#include <QQmlContext>
#include <QDebug>
#include <QStandardPaths>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QToolButton>

#include <QMenu>
#include <QProcess>

#include <KMessageBox>
#include <KConfigGroup>
#include <QIcon>

#include <kdeclarative/kdeclarative.h>


DevicesConfig::DevicesConfig(QWidget *parent) :
QWidget(parent)
{
    //mConfig = KSharedConfig::openConfig(JACK_CONFIG_FILE, KConfig::SimpleConfig);
    
    configUi = new Ui::DevicesConfig();
    configUi->setupUi(this);
    configUi->quickWidget->setVisible(false);
    
    KDeclarative::KDeclarative kdeclarative;
    //view refers to the QDeclarativeView
    kdeclarative.setDeclarativeEngine(configUi->quickWidget->engine());
    //binds things like kconfig and icons
    kdeclarative.setupBindings();
    
    
    DevicesModel *model = new DevicesModel(this);
    configUi->devicesListView->setModel(model);
    
    //TODO
    QStyledItemDelegate *delegate = new QStyledItemDelegate(configUi->devicesListView);
    configUi->devicesListView->setItemDelegate(delegate);
    //DevicesDelegate *delegate = new DevicesDelegate(configUi->devicesListView);
    //delegate->setPreviewSize(QSize(128,128));
    //configUi->devicesListView->setItemDelegate(delegate);
    
    configUi->devicesListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(configUi->devicesListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    
    connect(configUi->devicesListView, SIGNAL(activated(QModelIndex)), SLOT(deviceSelected(QModelIndex)));
    connect(configUi->devicesListView, SIGNAL(clicked(QModelIndex)), SLOT(deviceSelected(QModelIndex)));
    connect(model, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
    //connect(configUi->selectBackgroundButton, SIGNAL(imagePathChanged(QString)), SLOT(backgroundChanged(QString)));
    
    connect(configUi->preferButton, SIGNAL(clicked()), SLOT(prefer()));
    connect(configUi->deferButton, SIGNAL(clicked()), SLOT(defer()));
    connect(configUi->testButton, SIGNAL(clicked()), SLOT(test()));
    
    mTestPlaying = false;
    
    reset();
    
    //prepareInitialDevice();
}

void DevicesConfig::prefer()
{
    QModelIndex index = configUi->devicesListView->currentIndex();
    if(!index.isValid() || index.row() <= 0)
        return;
    configUi->devicesListView->model()->dropMimeData(NULL, Qt::DropAction(), index.row()-1, index.column(), index.parent());
    configUi->devicesListView->model()->removeRows(index.row(), 1, index.parent());
    index = configUi->devicesListView->model()->index(index.row()-1, index.column());
    configUi->devicesListView->setCurrentIndex(index);
    //deviceSelected(index);
    configUi->deferButton->setEnabled(false);
    configUi->preferButton->setEnabled(false);
    if(index.row() < configUi->devicesListView->model()->rowCount()-1)
        configUi->deferButton->setEnabled(true);
    if(index.row() > 0)
        configUi->preferButton->setEnabled(true);
}

void DevicesConfig::defer()
{
    QModelIndex index = configUi->devicesListView->currentIndex();
    if(!index.isValid() || index.row() >= configUi->devicesListView->model()->rowCount()-1)
        return;
    configUi->devicesListView->model()->dropMimeData(NULL, Qt::DropAction(), index.row()+2, index.column(), index.parent());
    configUi->devicesListView->model()->removeRows(index.row(), 1, index.parent());
    index = configUi->devicesListView->model()->index(index.row()+1, index.column());
    configUi->devicesListView->setCurrentIndex(index);
    //deviceSelected(index);
    configUi->deferButton->setEnabled(false);
    configUi->preferButton->setEnabled(false);
    if(index.row() < configUi->devicesListView->model()->rowCount()-1)
        configUi->deferButton->setEnabled(true);
    if(index.row() > 0)
        configUi->preferButton->setEnabled(true);
}

void DevicesConfig::remove()
{
    QModelIndex index = configUi->devicesListView->currentIndex();
    if(!index.isValid())
        return;
    configUi->devicesListView->model()->removeRow(index.row());
    configUi->devicesListView->clearSelection();
    configUi->devicesListView->setCurrentIndex(QModelIndex());
    emit saveconfig();
}

void DevicesConfig::showContextMenu(const QPoint &pos)
{
    
    QModelIndex index = configUi->devicesListView->indexAt(pos);
    DevicesModel* model = (DevicesModel*)configUi->devicesListView->model();
    if(index.isValid())
    {
        deviceSelected(index);
        
        
        // Handle global position
        QPoint globalPos = configUi->devicesListView->mapToGlobal(pos);
        
        // Create menu and insert some actions
        QMenu myMenu;
        QAction *action;
        
        action = myMenu.addAction(QIcon::fromTheme("audio-card"), i18n("Switch Master"), this, SLOT(switchMaster()));
        if(index.data(DevicesModel::DeviceRole).toString().isEmpty())
            action->setEnabled(false);
        if(index.data(DevicesModel::MasterRole).toBool())
            action->setEnabled(false);
        
        myMenu.addSeparator();
        
        action = myMenu.addAction(QIcon::fromTheme("go-up"), i18n("Prefer"), this, SLOT(prefer()));
        if(index.row() <= 0)
            action->setEnabled(false);
        
        action = myMenu.addAction(QIcon::fromTheme("go-down"), i18n("Defer"), this, SLOT(defer()));
        if(index.row() >= configUi->devicesListView->model()->rowCount()-1)
            action->setEnabled(false);
        
        if(index.data(DevicesModel::DeviceRole).toString().isEmpty())
        {
            myMenu.addSeparator();
            myMenu.addAction(QIcon::fromTheme("list-remove"), i18n("Remove entry"), this, SLOT(remove()));
        }
        
        // Show context menu at handling position
        myMenu.exec(globalPos);
    }
}

void DevicesConfig::switchMaster(QModelIndex index)
{
    if(!index.isValid())
        index = configUi->devicesListView->currentIndex();
    if(index.isValid())
    {
        QStringList env = QProcess::systemEnvironment();
        QStringList args;
        
        QProcess *exec;
        
        args << "-d" << index.data(DevicesModel::IdRole).toString().split(',').last();
        args << "-m" << index.data(DevicesModel::IdRole).toString().split(',').first();
        args << "-f";
        
        exec = new QProcess(this);
        exec->setEnvironment(env);
        
        qDebug() << "switch master";
        
        connect(exec, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(switchMasterFinished(int, QProcess::ExitStatus)));
        
        exec->start("bash",QStringList() << "jackman" << args);
    }
    else
    {
        qDebug() << "can't switch master to invalid index";
    }
}

void DevicesConfig::test()
{
    QModelIndex index = configUi->devicesListView->currentIndex();
    if(index.isValid())
    {
        QStringList env = QProcess::systemEnvironment();
        
        QProcess *exec;
        exec = new QProcess(this);
        exec->setEnvironment(env);
        
        QString port = index.data(DevicesModel::IdRole).toString();
        if(index.data(DevicesModel::MasterRole).toBool())
            port = "system";
        
        QString soundfile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sounds/KDE-Sys-Log-In.ogg");
        
        connect(exec, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(testFinished(int, QProcess::ExitStatus)));
        
        exec->start("mplayer", QStringList() << "-ao" << "jack:port="+port << "-volume" << "80" << soundfile);
        qDebug() << "mplayer" << exec->arguments().join(' ');
        
        mTestPlaying = true;
        configUi->testButton->setEnabled(false);
        
    }
    else
    {
        qDebug() << "can't test card with invalid index";
    }
}

void DevicesConfig::switchMasterFinished(int exitcode, QProcess::ExitStatus status)
{
    if(QProcess::NormalExit != status || 0 != exitcode)
    {
        QMessageBox msgBox;
        msgBox.setText(i18n("Jack failed to start"));
        msgBox.setInformativeText(i18n("This audio device is not compatible with the current settings. Please try other values."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
    
    QObject::sender()->deleteLater();
    
    /*else
     *    {
     *	QMessageBox msgBox;
     *	msgBox.setText(i18n("Jack started"));
     *	msgBox.exec();
}*/
    reset();
}

void DevicesConfig::testFinished(int exitcode, QProcess::ExitStatus status)
{
    mTestPlaying = false;
    QObject::sender()->deleteLater();
    
    QModelIndex index = configUi->devicesListView->currentIndex();
    if(!index.data(DevicesModel::DeviceRole).toString().isEmpty())
        configUi->testButton->setEnabled(true);
}


DevicesConfig::~DevicesConfig()
{
    delete configUi;
}

void DevicesConfig::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    emit changed(true);
    mChanged = true;
}


void DevicesConfig::reset()
{
    configUi->deferButton->setEnabled(false);
    configUi->preferButton->setEnabled(false);
    configUi->testButton->setEnabled(false);
    
    DevicesModel* model = (DevicesModel*)configUi->devicesListView->model();
    model->populate();
    
    if(configUi->devicesListView->currentIndex().isValid())
        deviceSelected(configUi->devicesListView->currentIndex());
    //configUi->quickWidget->setVisible(false);
    
    emit changed(false);
    mChanged = false;
    
}


#define PROP(a) configUi->quickWidget->rootObject()->property(a)
QVariantMap DevicesConfig::save()
{
    mChanged = false;
    
    QVariantMap args;
    
    const DevicesModel* model = (DevicesModel*)configUi->devicesListView->model();
    QModelIndex index = configUi->devicesListView->currentIndex();
    for(int i=0; i<model->rowCount(index); ++i)
    {
        index = model->index(i, index.column(), index.parent());
        args[index.data(DevicesModel::IdRole).toString().split('[').first().trimmed()] = index.data(DevicesModel::ConfigRole).toString()+";priority="+QString::number(i);
        qDebug() << " found " << index.data(DevicesModel::IdRole).toString() << " at " << i;
    }
    
    index = configUi->devicesListView->currentIndex();
    
    if (!index.isValid() || !configUi->quickWidget->isVisible()) {
        return args;
    }
    
    QVariant data = "rate="+(PROP("samplerate").toString())+";"
    +"period="+(PROP("buffersize").toString())+";"
    +"nperiods="+(PROP("nperiods").toString())+";"
    +"hwmon="+(PROP("hwmon").toBool()?"true":"false")+";"
    +"hwmeter="+(PROP("hwmeter").toBool()?"true":"false")+";"
    +"duplex="+(PROP("duplex").toBool()?"true":"false")+";"
    +"softmode="+(PROP("softmode").toBool()?"true":"false")+";"
    +"monitor="+(PROP("monitor").toBool()?"true":"false")+";"
    +"dither="+((PROP("dither").toInt()==DevicesMetadata::Dither::None)?"n":((PROP("dither").toInt()==DevicesMetadata::Dither::Rectangular)?"r":((PROP("dither").toInt()==DevicesMetadata::Dither::Shaped)?"s":((PROP("dither").toInt()==DevicesMetadata::Dither::Triangular)?"t":"n"))))+";"
    +((PROP("inchannels").toInt() > 0)?("inchannels="+(PROP("inchannels").toString())+";"):QString())
    +((PROP("outchannels").toInt() > 0)?("outchannels="+(PROP("outchannels").toString())+";"):QString())
    +"shorts="+(PROP("shorts").toBool()?"true":"false")+";"
    +"input-latency="+(PROP("inputlatency").toString())+";"
    +"output-latency="+(PROP("outputlatency").toString())+";"
    +"midi-driver="+((PROP("mididriver").toInt()==DevicesMetadata::MIDIDriver::NoDriver)?"none":((PROP("mididriver").toInt()==DevicesMetadata::MIDIDriver::Sequencer)?"seq":((PROP("mididriver").toInt()==DevicesMetadata::MIDIDriver::Raw)?"raw":"none")))+";"
    +"priority="+QString::number(index.row());
    
    
    args[index.data(DevicesModel::IdRole).toString().split('[').first().trimmed()] = data;//index.data(DevicesModel::ConfigRole);
    
    if(index.data(DevicesModel::MasterRole).toBool())
        QTimer::singleShot(200, this, [this,index](){switchMaster(index);});
    
    return args;
}

/*QString DevicesConfig::devicesConfigPath() const
 * {
 *    return mDevicesConfigPath;
 * }*/

/*void DevicesConfig::prepareInitialDevice()
 * {
 *    const QString initialDevice = mConfig->group("Device").readEntry("Current");
 *    
 *    const QModelIndex index = findDeviceIndex(initialDevice);
 *    if (!index.isValid()) {
 *        //KMessageBox::error(this, i18n("Could not find any themes. \nPlease install SDDM themes."), i18n("No SDDM themes"));
 *        return;
 *    }
 *    configUi->themesListView->setCurrentIndex(index);
 *    themeSelected(index);
 * }*/

QModelIndex DevicesConfig::findDeviceIndex(const QString &id) const
{
    QAbstractItemModel* model = configUi->devicesListView->model();
    
    for (int i=0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        if (index.data(DevicesModel::IdRole).toString() == id) {
            return index;
        }
    }
    
    return QModelIndex();
}

void DevicesConfig::deviceSelected(const QModelIndex &index)
{	
    if(mChanged)
    {
        if(index == mChangedIndex)
            return;
        
        QMessageBox msgBox;
        msgBox.setText(i18n("The configuration of the currently selected device has been modified."));
        msgBox.setInformativeText(i18n("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:
                configUi->devicesListView->setCurrentIndex(mChangedIndex);
                emit saveconfig();
                mChanged = false;
                configUi->devicesListView->setCurrentIndex(index);
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
                configUi->devicesListView->setCurrentIndex(mChangedIndex);
                return;
            default:
                break;
        }
    }
    mChanged = false;
    
    configUi->deferButton->setEnabled(false);
    configUi->preferButton->setEnabled(false);
    configUi->testButton->setEnabled(false);
    
    if(index.row() < configUi->devicesListView->model()->rowCount()-1)
        configUi->deferButton->setEnabled(true);
    if(index.row() > 0)
        configUi->preferButton->setEnabled(true);
    
    if(!mTestPlaying && !index.data(DevicesModel::DeviceRole).toString().isEmpty())
        configUi->testButton->setEnabled(true);
    
    if(!configUi->quickWidget->source().isValid())
    {
        const QString mainQmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "jackman_kcm/main.qml");
        configUi->quickWidget->setSource(QUrl::fromLocalFile(mainQmlPath));
    }
    configUi->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    configUi->quickWidget->setEnabled(true);
    connect(configUi->quickWidget->rootObject(), SIGNAL(configChanged(bool)), this, SLOT(widgetChanged(bool)));
    
    
    configUi->quickWidget->rootObject()->setProperty("name", index.data(DevicesModel::IdRole).toString());
    configUi->quickWidget->rootObject()->setProperty("device", index.data(DevicesModel::DeviceRole).toString());
    configUi->quickWidget->rootObject()->setProperty("nperiods", index.data(DevicesModel::NPeriodsRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("hwmon", index.data(DevicesModel::HWMonRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("hwmeter", index.data(DevicesModel::HWMeterRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("duplex", index.data(DevicesModel::DuplexRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("softmode", index.data(DevicesModel::SoftmodeRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("monitor", index.data(DevicesModel::MonitorRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("inchannels", index.data(DevicesModel::InChannelsRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("outchannels", index.data(DevicesModel::OutChannelsRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("shorts", index.data(DevicesModel::ShortsRole).toBool());
    configUi->quickWidget->rootObject()->setProperty("inputlatency", index.data(DevicesModel::InputLatencyRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("outputlatency", index.data(DevicesModel::OutputLatencyRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("mididriver", index.data(DevicesModel::MidiDriverRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("dither", index.data(DevicesModel::DitherRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("buffersize", index.data(DevicesModel::BufferSizeRole).toInt());
    configUi->quickWidget->rootObject()->setProperty("samplerate", index.data(DevicesModel::SampleRateRole).toInt());
    
    QMetaObject::invokeMethod(configUi->quickWidget->rootObject(), "update");
    ((DevicesModel*)configUi->devicesListView->model())->update();
    
    //Check if we need to display configuration group
    QString configPath;
    prepareConfigurationUi(configPath);
    
}

void DevicesConfig::prepareConfigurationUi(const QString &configPath)
{
    
    configUi->quickWidget->setVisible(true);
    //configUi->quickWidget->updateGeometry();
}

void DevicesConfig::widgetChanged(bool change)
{
    emit changed(change);
    mChanged = change;
    if(change)
    {
        mChangedIndex = configUi->devicesListView->currentIndex();
    }
}

void DevicesConfig::dump()
{
    //dump jack conf
    //TODO
}

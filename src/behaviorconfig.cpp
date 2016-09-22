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

#include "behaviorconfig.h"
#include "ui_behaviorconfig.h"
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

#include <QMessageBox>
#include <QProgressDialog>
#include <KConfigGroup>
#include <QIcon>

#include <kdeclarative/kdeclarative.h>
#include <QGraphicsScene>

#include <cfloat>
#include <limits>
#include <cmath>


#define PROP(a) configUi->quickWidget->rootObject()->property(a)



BehaviorConfig::BehaviorConfig(QWidget *parent) :
QWidget(parent)
{
    configUi = new Ui::BehaviorConfig();
    configUi->setupUi(this);
    configUi->configArea->setVisible(false);
    
    KDeclarative::KDeclarative kdeclarative;
    //view refers to the QDeclarativeView
    kdeclarative.setDeclarativeEngine(configUi->quickWidget->engine());
    //binds things like kconfig and icons
    kdeclarative.setupBindings();
    
    mHotplug = true;
    mAttachOthers = true;
    
    reset();
}

BehaviorConfig::~BehaviorConfig()
{
    delete configUi;
}

void BehaviorConfig::reset()
{
    emit changed(false);
    
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    KSharedConfigPtr jackConfig = KSharedConfig::openConfig(dir+"/"+JACK_CONFIG_FILE);
    for( const QString& key : jackConfig->group("Behavior").keyList())
    {
        const QString& val = jackConfig->group("Behavior").readEntry(key);
     
        qDebug() << key << ": " << val;
        
        if (key == "hotplug")
        {
            mHotplug = !(val == "false" || val == "0");
        }
        else if (key == "attach_others")
        {
            mAttachOthers = !(val == "false" || val == "0");
        }
    }
    
    updateConfigurationUi();
}


QVariantMap BehaviorConfig::save()
{
    QVariantMap args;
    
    args["hotplug"] = PROP("hotplug");
    args["attach_others"] = PROP("attach_others");
    
    return args;
}

void BehaviorConfig::updateConfigurationUi()
{
    if(!configUi->quickWidget->source().isValid())
    {
        const QString mainQmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "jackman_kcm/behavior.qml");
        configUi->quickWidget->setSource(QUrl::fromLocalFile(mainQmlPath));
    }
    
    configUi->quickWidget->setEnabled(true);
    connect(configUi->quickWidget->rootObject(), SIGNAL(configChanged(bool)), this, SIGNAL(changed(bool)));
    
    configUi->quickWidget->rootObject()->setProperty("hotplug", mHotplug);
    configUi->quickWidget->rootObject()->setProperty("attach_others", mAttachOthers);
    
    QMetaObject::invokeMethod(configUi->quickWidget->rootObject(), "update");
    
    configUi->configArea->setVisible(true);
}

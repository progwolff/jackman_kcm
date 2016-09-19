/*
    Copyright 2016 by Julian Wolff <wolff@julianwolff.de>
 
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


#include "jackmankcm.h"

#include <QHBoxLayout>
#include <QTabWidget>

#include <KPluginFactory>

#include <KAboutData>

#define TRANSLATION_DOMAIN "kcm-jackman"
#include <KLocalizedString>
#include <QDebug>

#include "config.h"
#include "devicesconfig.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kdeclarative.h>

K_PLUGIN_FACTORY(JackKcmFactory, registerPlugin<JackKcm>();)
//K_EXPORT_PLUGIN(JackKcmFactory("kcm-jackman"))

JackKcm::JackKcm(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args)
{
    KAboutData* aboutData = new KAboutData("kcm-jackman", i18n("JACK Audio Devices Config"), PROJECT_VERSION);

    aboutData->setShortDescription(i18n("Configure audio devices used with Jack"));
    aboutData->setLicense(KAboutLicense::GPL_V2);
    aboutData->setHomepage("https://github.com/progwolff/jackman_kcm");

    aboutData->addAuthor("Julian Wolff", i18n("Author"), "wolff@julianwolff.de");

    setAboutData(aboutData);

    KLocalizedString::setApplicationDomain("kcm-jackman");
    
    prepareUi();
}

JackKcm::~JackKcm()
{
}

void JackKcm::load()
{
    mDevicesConfig->reset();
}

void JackKcm::defaults()
{
    mDevicesConfig->reset();
}

void JackKcm::save()
{
    QVariantMap args;
    
    args.unite(mDevicesConfig->save());
    
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    KSharedConfigPtr jackConfig = KSharedConfig::openConfig(dir+"/"+JACK_CONFIG_FILE);
    
    if(!args.empty())
    {
      QMap<QString, QVariant>::const_iterator iterator;
      
	  QString groupName = "Devices";  
      jackConfig->deleteGroup(groupName);
      
      for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
	  
	  QString keyName = iterator.key();

	  jackConfig->group(groupName).writeEntry(keyName, iterator.value());
	  
	  qDebug() << "[" << keyName << "]=" << iterator.value();
      }
    }

    jackConfig->sync();
    //load();
}

void JackKcm::prepareUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QTabWidget* tabHolder = new QTabWidget(this);
    layout->addWidget(tabHolder);
    
    mDevicesConfig = new DevicesConfig(this);
    connect(mDevicesConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    connect(mDevicesConfig, SIGNAL(saveconfig()), this, SLOT(save()), Qt::DirectConnection);
    
    tabHolder->addTab(mDevicesConfig, i18n("Audio Devices"));
    /*
    mAdvanceConfig = new AdvanceConfig(this);
    connect(mAdvanceConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    
    tabHolder->addTab(mAdvanceConfig, i18n("Advanced"));*/

}

#include "jackmankcm.moc"

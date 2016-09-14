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
#ifndef JACKKCM_H
#define JACKKCM_H

#include <KCModule>

#include <KConfig>
#include <KConfigGroup>
#include <QFile>

class DevicesConfig;

class JackKcm : public KCModule
{
    Q_OBJECT
public:
    explicit JackKcm(QWidget *parent, const QVariantList &args);
    ~JackKcm();

public slots:
    void save();
    void load();
    void defaults();
    
private:
    void prepareUi();
    
private:
    DevicesConfig *mDevicesConfig;
};

#endif // JACKKCM_H

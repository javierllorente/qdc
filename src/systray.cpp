/*
 *  qRAE - Un cliente del diccionario castellano de la RAE
 *
 *  Copyright (C) 2015 Javier Llorente <javier@opensuse.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "systray.h"

const QString monochromeIcon = ":/icons/qrae_72x72_mono.png";
const QString colourIcon = ":/icons/qrae_72x72.png";

SysTray::SysTray(QWidget *parent) :
    associatedWidget(parent)
{
    readSettings();
    init();
}

SysTray::~SysTray()
{
    writeSettings();
}

void SysTray::init()
{
    actionRestore = new QAction(tr("&Ocultar"), this);
    connect(actionRestore, SIGNAL(triggered()), associatedWidget, SLOT(toggleVisibility()));

    actionQuit = new QAction(tr("&Salir"), this);
    actionQuit->setIcon(QIcon(":/icons/16x16/application-exit.png"));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(associatedWidget);
    trayIconMenu->addAction(actionRestore);
    trayIconMenu->addAction(actionQuit);

    setToolTip("Diccionario de la RAE");
    setContextMenu(trayIconMenu);

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), associatedWidget,
            SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));
    show();
}

void SysTray::setMonochromeIcon(bool monochrome)
{
    QString icon = monochrome ? monochromeIcon : colourIcon;
    setIcon(QIcon(icon));
    m_monochrome = monochrome;
}

bool SysTray::isMonochromeIcon()
{
    return m_monochrome;
}

void SysTray::readSettings()
{
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("SysTray");

    bool plasma5 = QFile::decodeName(qgetenv("KDE_SESSION_VERSION"))=="5";

    if (settings.value("Monochrome").isNull()) {
        setMonochromeIcon(plasma5);
    } else {
        setMonochromeIcon(settings.value("Monochrome").toBool());
    }
    settings.endGroup();
}

void SysTray::writeSettings()
{
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("SysTray");
    settings.setValue("Monochrome", m_monochrome);
    settings.endGroup();
}

void SysTray::toggleVisibilityText(bool visible)
{
    QString actionRestoreText = visible ?  "&Ocultar" : "&Mostrar";
    actionRestore->setText(actionRestoreText);
}


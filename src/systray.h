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

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QDebug>

class SysTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SysTray(QWidget *parent = 0);
    ~SysTray();
    void toggleVisibilityText(bool visible);
    void setMonochromeIcon(bool monochrome);
    bool isMonochromeIcon();

private:
    QWidget *associatedWidget;
    void init();
    QMenu *trayIconMenu;
    QAction *actionQuit;
    QAction *actionRestore;
    bool m_monochrome;
    void readSettings();
    void writeSettings();

};

#endif // SYSTRAY_H

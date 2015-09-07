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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QWebSettings>
#include "systray.h"
#include "history.h"
#include "proxysettings.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0, SysTray *sysTray = 0, History *history = 0, ProxySettings *proxySettings = 0);
    ~Settings();

private slots:
    void on_radioButtonManualProxy_toggled(bool checked);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushButtonBorrarHistorial_clicked();

private:
    Ui::Settings *ui;
    void loadProxySettings();
    void saveProxySettings();
    void restoreProxySettings();
    enum ProxyType { NoProxy, SystemProxy, ManualProxy };
    QNetworkProxy proxy;
    SysTray *m_sysTray;
    History *m_history;
    ProxySettings *m_proxySettings;
    void loadAutostartValue();
    void saveAutostartValue();
    void setLaunchOnStartup(bool enable);
    void loadMonochromeIconValue();

};

#endif // SETTINGS_H

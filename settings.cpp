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

#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent, History *history, ProxySettings *proxySettings) :
    QDialog(parent),
    ui(new Ui::Settings),
    m_history(history),
    m_proxySettings(proxySettings)
{
    ui->setupUi(this);

    loadProxySettings();

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    ui->listWidget->setCurrentRow(0);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_radioButtonManualProxy_toggled(bool checked)
{
    ui->comboBoxProxyType->setEnabled(checked);
    ui->lineEditProxyServer->setEnabled(checked);
    ui->spinBoxProxyPort->setEnabled(checked);
    ui->lineEditProxyUser->setEnabled(checked);
    ui->lineEditProxyPassword->setEnabled(checked);
}

void Settings::on_buttonBox_accepted()
{
    saveProxySettings();
}

void Settings::on_buttonBox_rejected()
{
    restoreProxySettings();
}

void Settings::loadProxySettings()
{
    ui->buttonGroupProxyType->setId(ui->radioButtonNoProxy, 0);
    ui->buttonGroupProxyType->setId(ui->radioButtonSystemProxy, 1);
    ui->buttonGroupProxyType->setId(ui->radioButtonManualProxy, 2);
    ui->comboBoxProxyType->addItem("Socks 5");
    ui->comboBoxProxyType->addItem("HTTP");

    proxy = m_proxySettings->getProxy();

    int proxyType = proxy.type();
    bool manualProxy = false;

    switch (proxyType) {
    case QNetworkProxy::NoProxy:
        ui->buttonGroupProxyType->button(0)->setChecked(true);
        break;
    case QNetworkProxy::DefaultProxy:
        ui->buttonGroupProxyType->button(1)->setChecked(true);
        break;
    case QNetworkProxy::Socks5Proxy:
        ui->buttonGroupProxyType->button(2)->setChecked(true);
        ui->comboBoxProxyType->setCurrentIndex(0);
        manualProxy = true;
        break;
    case QNetworkProxy::HttpProxy:
        ui->buttonGroupProxyType->button(2)->setChecked(true);
        ui->comboBoxProxyType->setCurrentIndex(1);
        manualProxy = true;
        break;
    default:
        break;
    }

    if (manualProxy) {
        ui->comboBoxProxyType->setEnabled(true);
        ui->lineEditProxyServer->setText(proxy.hostName());
        ui->spinBoxProxyPort->setValue(proxy.port());
        ui->lineEditProxyUser->setText(proxy.user());
        ui->lineEditProxyPassword->setText(proxy.password());
    }
}

void Settings::saveProxySettings()
{
    QNetworkProxy::ProxyType proxyType = QNetworkProxy::NoProxy;
    int checkedId = ui->buttonGroupProxyType->checkedId();

    switch (checkedId) {
    case NoProxy: // QNetworkProxy::NoProxy
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        proxy.setType(proxyType);
        break;
    case SystemProxy: // QNetworkProxy::DefaultProxy:
        QNetworkProxyFactory::setUseSystemConfiguration(true);
        proxy.setType(QNetworkProxy::DefaultProxy);
        break;
    case ManualProxy: // QNetworkProxy::Socks5Proxy || QNetworkProxy::HttpProxy
        proxyType = ui->comboBoxProxyType->currentIndex() == 0 ?
                    QNetworkProxy::Socks5Proxy : QNetworkProxy::HttpProxy;
        proxy.setType(proxyType);
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        proxy.setHostName(ui->lineEditProxyServer->text());
        proxy.setPort(ui->spinBoxProxyPort->text().toInt());
        proxy.setUser(ui->lineEditProxyUser->text());
        proxy.setPassword(ui->lineEditProxyPassword->text());
        QNetworkProxy::setApplicationProxy(proxy);
        break;
    default:
        break;
    }
    m_proxySettings->setProxy(proxy);
}

void Settings::restoreProxySettings()
{
    if (ui->radioButtonManualProxy->isChecked()) {
        int index = proxy.type() == QNetworkProxy::Socks5Proxy ? 0 : 1;
        ui->comboBoxProxyType->setCurrentIndex(index);
        ui->spinBoxProxyPort->setValue(proxy.port());
        ui->lineEditProxyServer->setText(proxy.hostName());
        ui->lineEditProxyUser->setText(proxy.user());
        ui->lineEditProxyPassword->setText(proxy.password());
    }
}

void Settings::on_pushButtonBorrarHistorial_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Borrar historial");
    msgBox.setText("¿Estás seguro de que quieres borrar el historial?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    if (ret==QMessageBox::Ok) {
        qDebug() << "Borrando historial...";
        m_history->clear();
    }
}

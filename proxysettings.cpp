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

#include "proxysettings.h"

ProxySettings::ProxySettings()
{
    readConfig();
}

ProxySettings::~ProxySettings()
{
    writeConfig();
}

void ProxySettings::readConfig()
{
    qDebug() << "Leyendo configuración del proxy...";
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("Proxy");
    QNetworkProxy::ProxyType proxyType = (QNetworkProxy::ProxyType) settings.value("Type").toInt();
    proxy.setType(proxyType);

    if (proxyType == QNetworkProxy::Socks5Proxy || proxyType == QNetworkProxy::HttpProxy) {
        proxy.setHostName(settings.value("HostName").toString());
        proxy.setPort(settings.value("Port").toInt());
        proxy.setUser(settings.value("User").toString());
        proxy.setPassword(settings.value("Password").toString());
    }

    if (proxyType == QNetworkProxy::DefaultProxy) {
         QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else {
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        QNetworkProxy::setApplicationProxy(proxy);
    }

    settings.endGroup();
}

void ProxySettings::writeConfig()
{
    qDebug() << "Escribiendo configuración del proxy...";
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("Proxy");
    settings.setValue("Type", proxy.type());

    if (proxy.type() == QNetworkProxy::Socks5Proxy || proxy.type() == QNetworkProxy::HttpProxy) {
        settings.setValue("HostName", proxy.hostName());
        settings.setValue("Port", proxy.port());
        settings.setValue("User", proxy.user());
        settings.setValue("Password", proxy.password());
    }
    settings.endGroup();
}

QNetworkProxy ProxySettings::getProxy() const
{
    return proxy;
}

void ProxySettings::setProxy(const QNetworkProxy &proxy)
{
    this->proxy = proxy;
}

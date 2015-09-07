/*
 *  libqrae - Biblioteca para consultar el diccionario castellano de la RAE
 *
 *  Copyright (C) 2013-2015 Javier Llorente <javier@opensuse.org>
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

#include "libqrae.h"

#include <QtCore/QDebug>

#if QT_VERSION >= 0x050000
#include <QtCore/QUrlQuery>
#endif

DRAE::DRAE()
{
      
}
    
QUrl DRAE::consultar(QString termino)
{  
    QUrl url("http://lema.rae.es/drae/srv/search"); 
    const QString draeQuery = "val";
  
    if (!termino.isEmpty()) {
      
        QByteArray valBA;
        QByteArray terBA;

        // codificación de caracteres
        termino.replace(QString::fromUtf8("á"), "%E1");
        termino.replace(QString::fromUtf8("é"), "%E9");
        termino.replace(QString::fromUtf8("í"), "%ED");
        termino.replace(QString::fromUtf8("ó"), "%F3");
        termino.replace(QString::fromUtf8("ú"), "%FA");
        termino.replace(QString::fromUtf8("ñ"), "%F1");
        termino.replace(QString::fromUtf8("ü"), "%FC");

        termino.replace(QString::fromUtf8("Á"), "%C1");
        termino.replace(QString::fromUtf8("É"), "%C9");
        termino.replace(QString::fromUtf8("Í"), "%CD");
        termino.replace(QString::fromUtf8("Ó"), "%D3");
        termino.replace(QString::fromUtf8("Ú"), "%DA");
        termino.replace(QString::fromUtf8("Ñ"), "%D1");
        termino.replace(QString::fromUtf8("Ü"), "%DC");

        valBA.append(draeQuery);
        terBA.append(termino);

#if QT_VERSION >= 0x050000
        QUrlQuery urlQuery;
        urlQuery.addQueryItem(valBA, terBA);
        url.setQuery(urlQuery);
#else
        url.addEncodedQueryItem(valBA, terBA);
#endif

    }
    return url;
}

QString DRAE::getErrorMsg() const
{
    return QString(
                "<html>"
                "<head>"
                "<title>Error</title>"
                "<style type=\"text/css\">"
                "body {"
                "font-family: \"Arial Unicode MS\",\"TITUS Cyberbit Basic\",\"Lucida Sans Unicode\";"
                "font-size: 13pt;"
                "text-align: left;"
                "}"
                "img {"
                "display: block;"
                 "margin: 0 auto;"
                "}"
                "h1 {"
                "font-size: 13pt;"
                "color: #9C0204;"
                "margin-top: 20px;"
                "text-align: center"
                "}"
                "#mensaje {"
                "display: block;"
                "width: 300px;"
                "margin: 0 auto;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<img src=\"qrc:/icons/128x128/dialog-warning.png\">"
                "<h1>No se ha podido cargar la p&aacute;gina</h1>"
                "<div id=\"mensaje\">"
                "Por favor, comprueba que tu ordenador est&eacute; conectado a Internet."
                "<br>"
                "En caso afirmativo, int&eacute;ntalo de nuevo."
                "</div>"
                "</body>"
                "</html>"
                );
}

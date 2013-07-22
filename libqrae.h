/*
 *  libqrae - Biblioteca para consultar el diccionario castellano de la RAE
 *
 *  Copyright (C) 2013 Javier Llorente <javier@opensuse.org>
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

#ifndef LIBQRAE_H
#define LIBQRAE_H

#include <QString>
#include <QUrl>
#include <QDebug>

class DRAE
{

public:
    DRAE();
    ~DRAE();
    
    QUrl consultar(QString);
    QString getErrorMsg();
  
      
};
#endif // LIBQRAE_H
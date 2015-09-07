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

#include "history.h"

const QString dbName = "historial.sqlite";

History::History()
{
    init();
}

void History::init()
{
#if QT_VERSION >= 0x050000
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/data/" + QCoreApplication::applicationName();
#else
    const QString dataDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QCoreApplication::applicationName();
#endif
    QDir dir(dataDir);

    if (!dir.exists()) {
        dir.mkpath(dataDir);
    }

    QString dbPath = dir.filePath(dbName);
    qDebug() << "db:" << dbPath;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Error. No se ha podido abrir la base de datos";
    } else {

        if (db.tables().isEmpty()) {
            qDebug() << "Creando tabla en BD...";
            QSqlQuery query(db);
            query.exec("CREATE TABLE historial (id INTEGER PRIMARY KEY, termino TEXT, fecha TEXT)");
        }
    }
}

QStringList History::get() const
{
    QSqlQuery query(db);
    query.exec("SELECT termino FROM historial ORDER BY id DESC");

    QStringList stringList;
    while (query.next()) {
        stringList.append(query.value(0).toString());
    }
    return stringList;
}

void History::update(const QString &termino)
{
    QDate fecha = QDate::currentDate();
    QSqlQuery query(db);
    query.exec("SELECT EXISTS (SELECT termino FROM 'historial' WHERE termino='" + termino + "')");
    bool yaExiste = false;

    while (query.next()) {
        yaExiste = query.value(0).toBool();
    }

    if (yaExiste) {
        query.exec("UPDATE historial SET fecha='" + fecha.toString() + "' WHERE termino='" + termino + "'");
        qDebug() << "db: update";
    } else {
        qDebug() << "db: insert";
        query.exec("INSERT INTO historial (termino, fecha) VALUES ('" + termino + "', '" + fecha.toString() + "')");
    }
}

void History::clear()
{
    QSqlQuery query(db);
    query.exec("DELETE FROM historial");
}

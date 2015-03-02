/*
 *  qRAE - Un cliente del diccionario castellano de la RAE
 *
 *  Copyright (C) 2012-2015 Javier Llorente <javier@opensuse.org>
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtSql/QSqlQuery>

#if QT_VERSION >= 0x050000
#include <QtCore/QStandardPaths>
#include <QtCore/QStringListModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextEdit>
#include <QtWebKitWidgets/QWebView>
#else
#include <QtGui/QDesktopServices>
#include <QtGui/QStringListModel>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QTextEdit>
#include <QtWebKit/QWebView>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    appVersion = "0.4.2";
    draeUrl = "http://lema.rae.es/drae/srv/search";
    draeQuery = "val";
    ayudaAbreviaturasYsignos = "qrc:/html/abreviaturas_y_signos_empleados.html";
    ayudaCastellano = "qrc:/html/castellano.html";

    createTrayIcon();
    ui->setupUi(this);

    QLocale castellano(QLocale::Spanish, QLocale::Spain);
    ui->webView->setLocale(castellano);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));

    connect(ui->webView, SIGNAL(loadProgress(int)), this, SLOT(progresoCarga(int)));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(resultadoCarga(bool)));

    connect(ui->webView, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(errorAlCargar()));

    ui->webView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->webView->show();

    readSettings();

    m_drae = new DRAE();

    cargarBD();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon();
    trayIconMenu = new QMenu();
    trayIcon->setIcon(QIcon(":/iconos/logo_app.png"));
    trayIcon->setToolTip("Diccionario de la RAE");
    trayIcon->setContextMenu(trayIconMenu);

    trayIconMenu->addSeparator();

    actionRestore = new QAction(tr("&Ocultar"), trayIcon);
    connect(actionRestore, SIGNAL(triggered()), this, SLOT(toggleVisibility()));
    trayIconMenu->addAction(actionRestore);

    actionQuit = new QAction(tr("&Salir"), trayIcon);
    actionQuit->setIcon(QIcon(":/iconos/application-exit.png"));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu->addAction(actionQuit);

    trayIcon->show();
}

void MainWindow::cargarBD()
{
    #if QT_VERSION >= 0x050000
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/data/qraeqRAE";
    #else
    QString dataDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "qraeqRAE";
    #endif
    QDir dir(dataDir);

    if (!dir.exists()) {
        dir.mkpath(dataDir);
    }

    QString dbName = dir.filePath("historial.sqlite");
    qDebug() << "db:" << dbName;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Error. No se ha podido abrir la base de datos";
    } else {

        if (db.tables().isEmpty()) {
            qDebug() << "Creando tabla en BD...";
            QSqlQuery query(db);
            query.exec("CREATE TABLE historial (id INTEGER PRIMARY KEY, termino TEXT, fecha TEXT)");
        }
    }

    // Puesta en marcha del autocompletado
    QSqlQuery query(db);
    query.exec("SELECT termino FROM historial ORDER BY id DESC");

    QStringList stringList;
    while (query.next()) {
        stringList.append(query.value(0).toString());
    }

    QStringListModel *model = new QStringListModel(stringList);
    completer = new QCompleter(model, this);

    ui->lineEditConsultar->setCompleter(completer);

    connect(ui->lineEditConsultar, SIGNAL(textEdited(const QString&)),
            this, SLOT(actualizarAutocompletado(const QString&)));

    connect(completer, SIGNAL(activated(const QString&)),
            this, SLOT(terminoAutocompletado_clicked(const QString&)));
}

void MainWindow::terminoAutocompletado_clicked(const QString&)
{
    consultar();
}

void MainWindow::actualizarAutocompletado(const QString&)
{
    QStringListModel *model = (QStringListModel*)(completer->model());
    QStringList stringList;
    QSqlQuery query(db);
    query.exec("SELECT termino FROM historial ORDER BY id DESC");

    while (query.next()) {
        stringList.append(query.value(0).toString());
    }
    model->setStringList(stringList);
}

void MainWindow::actualizarBD(const QString& termino)
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

void MainWindow::progresoCarga(int progreso)
{

    statusBar()->showMessage("Cargando...");

    if(progreso!=100 && !timer->isActive()) {

        // El termporizador se activará sólo una vez
        timer->setSingleShot(true);

        // Se inicia con el valor de 30 segundos como tiempo de espera
        // para cargar el resultado de la consulta
        timer->start(30000);

    } else if(progreso==100) {

        timer->stop();
    }

    qDebug() << "progreso: " << progreso;

    if (progreso==100) {

        qDebug() << "(cargado)";
        ui->lineEditConsultar->setText("");

    }
}

void MainWindow::resultadoCarga(bool ok)
{

    statusBar()->showMessage("Cargado", 5000);

    if(!ok) {
        errorAlCargar();
        qDebug() <<  "resultadoCarga(bool): !ok";
    }
}

void MainWindow::errorAlCargar()
{

    ui->webView->setHtml( m_drae->getErrorMsg() );

    qDebug() << "Ha fallado la carga";
}

void MainWindow::consultar()
{
    if (ui->lineEditConsultar->text()!="") {

        ui->webView->load( QUrl( m_drae->consultar( ui->lineEditConsultar->text() ) ));
        actualizarBD(ui->lineEditConsultar->text());

    }
}

void MainWindow::on_lineEditConsultar_returnPressed()
{
    consultar();
}

void MainWindow::on_pushButtonConsultar_clicked()
{
    consultar();
}

void MainWindow::ocultarVentana()
{
    hide();
    actionRestore->setText("&Mostrar");
}

void MainWindow::mostrarVentana()
{
    showNormal();
    actionRestore->setText("&Ocultar");
}

void MainWindow::toggleVisibility()
{
    if (this->isVisible()) {
        ocultarVentana();

    } else {
        mostrarVentana();
    }
}

void MainWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason==QSystemTrayIcon::Trigger) {
        toggleVisibility();
    }
}

void MainWindow::showContextMenu(const QPoint& position)
{

    QPoint globalPosition = ui->webView->mapToGlobal(position);

    QMenu webViewMenu;

    QAction *actionCopy = ui->webView->pageAction((QWebPage::Copy));
    actionCopy->setIcon(QIcon(":/iconos/edit-copy.png"));
    actionCopy->setText("Copiar");

    QAction *actionBack = ui->webView->pageAction((QWebPage::Back));
    actionBack->setText(QString::fromUtf8("Atrás"));

    QAction *actionForward = ui->webView->pageAction((QWebPage::Forward));
    actionForward->setText("Adelante");

    webViewMenu.addAction(actionCopy);
    webViewMenu.addAction(actionBack);
    webViewMenu.addAction(actionForward);
    webViewMenu.exec(globalPosition);

}

void MainWindow::on_actionAcerca_de_triggered()
{

    QMessageBox::about(this,"Acerca de qRAE",
                       "<h2 align=\"left\">qRAE</h2>"\
                       "Diccionario castellano de la RAE<br>"\
                       "Versi&oacute;n: " + appVersion +
                       "<div align=\"left\">"
                       "<p>"
                       "&copy; 2013-2015 Javier Llorente <br>"
                       "<a href=\"http://www.javierllorente.com/qrae/\">www.javierllorente.com/qrae/</a><br><br>"
                       "Proyecto Oxygen (iconos de acciones)<br>"
                       "<a href=\"http://www.oxygen-icons.org/\">www.oxygen-icons.org</a><br><br>"
                       "<b>Licencia:</b> <br>"
                       "<nobr>Este programa est&aacute; bajo la GPL v3</nobr>"
                       "</p>"
                       "</div>"
                       );
}

void MainWindow::on_actionSalir_triggered()
{
    qApp->quit();
}

void MainWindow::writeSettings()
{
    qDebug() << "Escribiendo ajustes...";
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("MainWindow");

    settings.setValue("Maximized", isMaximized());
    settings.setValue("Visibility", this->isVisible());

    if (!isMaximized()) {
        settings.setValue("pos", pos());
        settings.setValue("Geometry", saveGeometry());
    }

    settings.endGroup();
}

void MainWindow::readSettings()
{
    qDebug() << "Leyendo ajustes...";
    QSettings settings("qRAE","Diccionario castellano de la RAE");
    settings.beginGroup("MainWindow");

    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreGeometry(settings.value("Geometry", saveGeometry()).toByteArray());

    if (settings.value(("Maximized")).toBool()) {
        showMaximized();

    } else if (settings.value("Visibility").toBool()) {
        show();

    } else if (settings.value("Visibility").toString().isNull()) {
        // Valor por defecto
        show();

    } else {
        ocultarVentana();

    }

    settings.endGroup();
}

void MainWindow::on_actionAbreviaturas_y_signos_triggered()
{
    ui->webView->load(QUrl(ayudaAbreviaturasYsignos));
}

void MainWindow::on_actionAlgunos_datos_triggered()
{
    ui->webView->load(QUrl(ayudaCastellano));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    toggleVisibility();
}

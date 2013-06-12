/*
 *  qRAE - Un cliente del diccionario castellano de la RAE
 *
 *  Copyright (C) 2012-2013 Javier Llorente <javier@opensuse.org>
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    appVersion = "0.2.4";
    draeUrl = "http://lema.rae.es/drae/srv/search";
    draeQuery = "val";
    ayudaUsoDrae = "http://lema.rae.es/drae/html/advertencia.html";
    ayudaAbreviaturasYsignos = "http://lema.rae.es/drae/html/abrevt11.html";
    ayudaNovedades = "http://lema.rae.es/drae/html/novedades22.html";
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

void MainWindow::progresoCarga(int progreso) {

    statusBar()->showMessage("Cargando...");

    if(progreso!=100 && !timer->isActive()) {

        timer->setSingleShot(true);
        timer->start(10000);

    } else if(progreso==100) {

        timer->stop();
    }

    qDebug() << "progreso: " << progreso;

    if (progreso==100) {

        qDebug() << "(cargado)";

    }
}

void MainWindow::resultadoCarga(bool ok) {

    statusBar()->showMessage("Cargado", 5000);

    if(!ok) {
        errorAlCargar();
        qDebug() <<  "resultadoCarga(bool): !ok";
    }
}

void MainWindow::errorAlCargar() {
    qDebug() << "Ha fallado la carga";


    QString errorMsg(
                "<html>"
                "<head>"
                "<title>Error</title>"
                "<style type=\"text/css\">"
                "body {"
                "font-family: \"Arial Unicode MS\",\"TITUS Cyberbit Basic\",\"Lucida Sans Unicode\";"
                "font-size: 13pt;"
                "text-align: left;"
                "}"
                "h1 {"
                "font-size: 13pt;"
                "color: #9C0204;"
                "margin-top: 20px;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<h1>Error: No se ha podido cargar la p&aacute;gina</h1>"
                "Por favor, comprueba que tu ordenador est&eacute; conectado a Internet."
                "<br>"
                "En caso afirmativo, int&eacute;ntalo de nuevo."
                "</body>"
                "</html>"
                );
    ui->webView->setHtml(errorMsg);

}

void MainWindow::consultar(QString termino)
{
    if (!termino.isEmpty()) {

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

        QUrl url(draeUrl);
        QByteArray valBA;
        QByteArray terBA;
        valBA.append(draeQuery);
        terBA.append(termino);
        url.addEncodedQueryItem(valBA, terBA);
        ui->webView->load(QUrl(url));
        ui->lineEditConsultar->setText("");

    }
}

void MainWindow::on_lineEditConsultar_returnPressed()
{
    consultar((ui->lineEditConsultar->text()));
}

void MainWindow::on_pushButtonConsultar_clicked()
{
    consultar(ui->lineEditConsultar->text());
}

void MainWindow::ocultarVentana()
{
    hide();
    actionRestore->setText("Mostrar");
}

void MainWindow::mostrarVentana()
{
    showNormal();
    actionRestore->setText("Ocultar");
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

void MainWindow::showContextMenu(const QPoint& position) {

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
                       "<b>Desarrollador:</b><br>"
                       "Javier Llorente<br>"
                       "javier@opensuse.org<br>"
                       "<a href=\"http://www.javierllorente.com/\">www.javierllorente.com</a><br><br>"
                       "<b>Temas art&iacute;sticos:</b> <br>"
                       "Iconos de acciones:<br>"
                       "Proyecto Oxygen<br>"
                       "<a href=\"http://www.oxygen-icons.org/\">www.oxygen-icons.org</a><br><br>"
                       "<b>Licencia:</b> <br>"
                       "<nobr>Este programa est&aacute; bajo la GNU General Public License v3</nobr>"
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

void MainWindow::on_actionUso_del_diccionario_triggered()
{
    ui->webView->load(QUrl(ayudaUsoDrae));
}

void MainWindow::on_actionAbreviaturas_y_signos_triggered()
{
    ui->webView->load(QUrl(ayudaAbreviaturasYsignos));
}

void MainWindow::on_actionNovedades_triggered()
{
    ui->webView->load(QUrl(ayudaNovedades));
}

void MainWindow::on_actionAlgunos_datos_triggered()
{
    ui->webView->load(QUrl(ayudaCastellano));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    toggleVisibility();
}

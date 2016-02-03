/*
 *  qdc - Un cliente del diccionario castellano de la RAE
 *
 *  Copyright (C) 2012-2016 Javier Llorente <javier@opensuse.org>
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

const QString ayudaGuiaConsulta = "http://dle.rae.es/?t=/docs/ayuda.html";
const QString ayudaFormulario = "http://www.rae.es/consultas-linguisticas/formulario";
const QString ayudaPreguntasFrecuentes = "http://www.rae.es/consultas-linguisticas/preguntas-frecuentes";
const QString ayudaAbreviaturasYsignos = "http://www.rae.es/diccionario-de-la-lengua-espanola/sobre-la-22a-edicion-2001/abreviaturas-y-signos-empleados";
const QString ayudaCastellano = "qrc:/html/castellano.html";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_sysTray(new SysTray(this)),
    m_drae(new DRAE()),
    m_timer(new QTimer(this)),
    m_history(new History()),
    m_searchWidget(0),
    m_proxySettings(new ProxySettings())
{
    ui->setupUi(this);

    createMenuEditarActions();
    setupWebView();
    readSettings();
    inicializarAutocompletado();
    setupMenuVer();

    connect(ui->lineEditConsultar, SIGNAL(returnPressed()), this, SLOT(consultar()));
    connect(ui->pushButtonConsultar, SIGNAL(clicked(bool)), this, SLOT(consultar()));
}

MainWindow::~MainWindow()
{
    delete m_sysTray;
    delete m_proxySettings;
    writeSettings();
    delete ui;
}

void MainWindow::createMenuEditarActions()
{
    actionDeshacer = new QAction(ui->menuEditar);
    actionDeshacer->setIcon(QIcon(":/icons/16x16/edit-undo.png"));
    actionDeshacer->setText("Deshacer");
    actionDeshacer->setShortcut(QKeySequence::Undo);
    ui->menuEditar->addAction(actionDeshacer);
    connect(actionDeshacer, SIGNAL(triggered(bool)), ui->lineEditConsultar, SLOT(undo()));

    actionRehacer = new QAction(ui->menuEditar);
    actionRehacer->setIcon(QIcon(":/icons/16x16/edit-redo.png"));
    actionRehacer->setText("Rehacer");
    actionRehacer->setShortcut(QKeySequence::Redo);
    ui->menuEditar->addAction(actionRehacer);
    connect(actionRehacer, SIGNAL(triggered(bool)), ui->lineEditConsultar, SLOT(redo()));

    actionCopiar = ui->webView->pageAction((QWebPage::Copy));
    actionCopiar->setIcon(QIcon(":/icons/16x16/edit-copy.png"));
    actionCopiar->setText("Copiar");
    actionCopiar->setShortcut(QKeySequence::Copy);
    ui->menuEditar->addAction(actionCopiar);

    actionPegar = new QAction(ui->menuEditar);
    actionPegar->setIcon(QIcon(":/icons/16x16/edit-paste.png"));
    actionPegar->setText("Pegar");
    actionPegar->setShortcut(QKeySequence::Paste);
    ui->menuEditar->addAction(actionPegar);
    connect(actionPegar, SIGNAL(triggered(bool)), ui->lineEditConsultar, SLOT(paste()));

    actionSeleccionarTodo = ui->webView->pageAction((QWebPage::SelectAll));
    actionSeleccionarTodo->setIcon(QIcon(":/icons/16x16/edit-select-all.png"));
    actionSeleccionarTodo->setText("Seleccionar todo");
    actionSeleccionarTodo->setShortcut(QKeySequence::SelectAll);
    ui->menuEditar->addAction(actionSeleccionarTodo);

    ui->menuEditar->addSeparator();

    actionBuscarTexto = new QAction(this);
    actionBuscarTexto->setIcon(QIcon(":/icons/16x16/edit-find.png"));
    actionBuscarTexto->setText("Buscar texto");
    actionBuscarTexto->setShortcut(QKeySequence("Ctrl+F"));
    ui->menuEditar->addAction(actionBuscarTexto);
    connect(actionBuscarTexto, SIGNAL(triggered(bool)), this, SLOT(buscarTexto()));

    ui->menuEditar->addSeparator();

    actionAjustes = new QAction(ui->menuEditar);
    actionAjustes->setIcon(QIcon(":/icons/16x16/configure.png"));
    actionAjustes->setText("Ajustes");
    actionAjustes->setShortcut(QKeySequence::Preferences);
    ui->menuEditar->addAction(actionAjustes);
    connect(actionAjustes, SIGNAL(triggered(bool)), this, SLOT(showSettings()));
}

void MainWindow::setupWebView()
{
    QLocale castellano(QLocale::Spanish, QLocale::Spain);
    ui->webView->setLocale(castellano);

    connect(ui->webView, SIGNAL(loadProgress(int)), this, SLOT(progresoCarga(int)));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(resultadoCarga(bool)));
    connect(ui->webView, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(errorAlCargar()));

    ui->webView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->webView->show();
}

void MainWindow::inicializarAutocompletado()
{
    QStringList stringList = m_history->get();

    QStringListModel *model = new QStringListModel(stringList);
    completer = new QCompleter(model, this);

    ui->lineEditConsultar->setCompleter(completer);

    connect(ui->lineEditConsultar, SIGNAL(textEdited(const QString&)),
            this, SLOT(actualizarAutocompletado(const QString&)));

    connect(completer, SIGNAL(activated(const QString&)),
            this, SLOT(consultar()));
}

void MainWindow::setupMenuVer()
{
    ui->actionAumentar->setShortcut(QKeySequence("Ctrl++"));
    ui->actionReducir->setShortcut(QKeySequence("Ctrl+-"));
    ui->actionNormal->setShortcut(QKeySequence("Ctrl+0"));
    connect(ui->actionAumentar, SIGNAL(triggered(bool)), this, SLOT(aumentarTamano()));
    connect(ui->actionReducir, SIGNAL(triggered(bool)), this, SLOT(reducirTamano()));
    connect(ui->actionNormal, SIGNAL(triggered(bool)), this, SLOT(tamanoNormal()));
}

void MainWindow::actualizarAutocompletado(const QString&)
{
    QStringListModel *model = (QStringListModel*)(completer->model());
    QStringList stringList = m_history->get();
    model->setStringList(stringList);
}

void MainWindow::buscarTexto()
{
    if (m_searchWidget) {
        m_searchWidget->selectAll();
    } else {
        m_searchWidget = new SearchWidget(this, ui->webView, ui->lineEditConsultar);
        ui->verticalLayout->insertWidget(2, m_searchWidget);
        m_searchWidget->focusLineEdit();
    }
}

void MainWindow::aumentarTamano()
{
    ui->webView->setZoomFactor(qreal(ui->webView->zoomFactor())+0.10);
}

void MainWindow::reducirTamano()
{
    qreal zoomFactor = ui->webView->zoomFactor();
    if (zoomFactor>0.60) {
        ui->webView->setZoomFactor((zoomFactor)-0.10);
    }
}

void MainWindow::tamanoNormal()
{
    ui->webView->setZoomFactor(qreal(1));
}

void MainWindow::progresoCarga(int progreso)
{
    statusBar()->showMessage("Cargando...");

    if (progreso!=100 && !m_timer->isActive()) {

        // El termporizador se activará sólo una vez
        m_timer->setSingleShot(true);

        // Se inicia con el valor de 30 segundos como tiempo de espera
        // para cargar el resultado de la consulta
        m_timer->start(30000);

    } else if (progreso==100) {
        m_timer->stop();
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
    ui->statusBar->clearMessage();
    qDebug() << "Ha fallado la carga";
}

void MainWindow::consultar()
{
    if (ui->lineEditConsultar->text()!="") {
        ui->webView->load( QUrl( m_drae->consultar( ui->lineEditConsultar->text() ) ));
        m_history->update(ui->lineEditConsultar->text());
    }
}

void MainWindow::ocultarVentana()
{
    hide();
    m_sysTray->toggleVisibilityText(isVisible());
}

void MainWindow::mostrarVentana()
{
    showNormal();
    m_sysTray->toggleVisibilityText(isVisible());
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
    actionCopy->setIcon(QIcon(":/icons/16x16/edit-copy.png"));
    actionCopy->setText("Copiar");

    QAction *actionBack = ui->webView->pageAction((QWebPage::Back));
    actionBack->setText(QString::fromUtf8("Atrás"));

    QAction *actionForward = ui->webView->pageAction((QWebPage::Forward));
    actionForward->setText("Adelante");

    webViewMenu.addAction(actionBack);
    webViewMenu.addAction(actionForward);
    webViewMenu.addSeparator();
    webViewMenu.addAction(actionCopy);
    webViewMenu.addAction(actionSeleccionarTodo);
    webViewMenu.exec(globalPosition);
}

void MainWindow::on_actionAcerca_de_triggered()
{

    QMessageBox::about(this,"Acerca de qdc",
                       "<h2 align=\"left\">qdc</h2>"\
                       "Diccionario castellano<br>"\
                       "Versi&oacute;n: " + QString(QDC_VERSION) +
                       "<div align=\"left\">"
                       "<p>"
                       "&copy; 2013-2016 Javier Llorente <br>"
                       "<a href=\"http://www.javierllorente.com/qdc/\">www.javierllorente.com/qdc/</a>"
                       "<br><br>"
                        "&copy; Proyecto Oxygen (iconos de los menús) &nbsp; &nbsp; <br>"
                       "<a href=\"https://techbase.kde.org/Projects/Oxygen\">techbase.kde.org/Projects/Oxygen</a>"
                       "<br><br>"
                       "&copy; Real Academia Espa&ntilde;ola<br>"
                       "Las definiciones son propiedad de la RAE &nbsp; &nbsp;  <br>"
                       "<a href=\"http://www.rae.es\">www.rae.es</a>"
                       "<br><br>"
                       "<b>Licencia</b> <br>"
                       "<nobr>Este programa est&aacute; bajo la licencia GPLv3</nobr> &nbsp; &nbsp;"
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
    QSettings settings;
    settings.beginGroup("MainWindow");

    settings.setValue("Maximized", isMaximized());
    settings.setValue("Visibility", this->isVisible());

    if (!isMaximized()) {
        settings.setValue("pos", pos());
        settings.setValue("Geometry", saveGeometry());
    }

    settings.setValue("ZoomFactor", ui->webView->zoomFactor());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    qDebug() << "Leyendo ajustes...";
    QSettings settings;
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

    qreal zoomFactor = settings.value("ZoomFactor").toDouble();
    if (zoomFactor >= 0.50) {
        ui->webView->setZoomFactor(zoomFactor);
    }

    settings.endGroup();
}

void MainWindow::showSettings()
{
    Settings *settings = new Settings(this, m_sysTray, m_history, m_proxySettings);
    if (settings->exec()) {

    }
    delete settings;
}

void MainWindow::on_actionGuia_de_consulta_triggered()
{
    ui->webView->load(QUrl(ayudaGuiaConsulta));
}

void MainWindow::on_actionFormulario_triggered()
{
    ui->webView->load(QUrl(ayudaFormulario));
}

void MainWindow::on_actionPreguntas_frecuentes_triggered()
{
    ui->webView->load(QUrl(ayudaPreguntasFrecuentes));
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

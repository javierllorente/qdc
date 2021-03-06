/* 
 *  qdc - Un cliente del diccionario castellano de la RAE
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QTimer>
#include <QtGui/QCloseEvent>
#include <QtCore/QPointer>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMenu>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#else
#include <QtGui/QMenu>
#include <QtGui/QCompleter>
#include <QtGui/QMainWindow>
#include <QtGui/QApplication>
#endif

#include "systray.h"
#include "libqrae.h"
#include "proxysettings.h"
#include "settings.h"
#include "history.h"
#include "searchwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    SysTray *m_sysTray;
    void createMenuEditarActions();
    QAction *actionDeshacer;
    QAction *actionRehacer;
    QAction *actionCopiar;
    QAction *actionPegar;
    QAction *actionSeleccionarTodo;
    QAction *actionBuscarTexto;
    QAction *actionAjustes;
    DRAE *m_drae;
    QTimer *m_timer;
    History *m_history;
    QPointer <SearchWidget> m_searchWidget;
    ProxySettings *m_proxySettings;
    QSqlDatabase db;
    QCompleter *completer;
    void setupWebView();
    void inicializarAutocompletado();
    void setupMenuVer();
    void ocultarVentana();
    void mostrarVentana();
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent*);

private slots:
    void consultar();
    void showContextMenu(const QPoint&);
    void toggleVisibility();
    void on_actionAcerca_de_triggered();
    void on_actionSalir_triggered();
    void showSettings();
    void trayIconClicked(QSystemTrayIcon::ActivationReason);
    void on_actionGuia_de_consulta_triggered();
    void on_actionFormulario_triggered();
    void on_actionPreguntas_frecuentes_triggered();
    void on_actionAbreviaturas_y_signos_triggered();
    void on_actionAlgunos_datos_triggered();
    void progresoCarga(int);
    void resultadoCarga(bool);
    void errorAlCargar();
    void actualizarAutocompletado(const QString &);
    void buscarTexto();
    void aumentarTamano();
    void reducirTamano();
    void tamanoNormal();
};

#endif // MAINWINDOW_H

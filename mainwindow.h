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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QCloseEvent>
#include <QTextEdit>
#include <libqrae.h>

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
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *actionQuit;
    QAction *actionRestore;
    DRAE *m_drae;
    void consultar();
    QString appVersion;
    QString draeUrl;
    QString draeQuery;
    QString ayudaUsoDrae;
    QString ayudaAbreviaturasYsignos;
    QString ayudaNovedades;
    QString ayudaCastellano;
    void ocultarVentana();
    void mostrarVentana();
    void writeSettings();
    void readSettings();
    QTimer *timer;
    void closeEvent(QCloseEvent*);

private slots:
    void on_lineEditConsultar_returnPressed();
    void on_pushButtonConsultar_clicked();
    void showContextMenu(const QPoint&);
    void toggleVisibility();
    void on_actionAcerca_de_triggered();
    void on_actionSalir_triggered();
    void trayIconClicked(QSystemTrayIcon::ActivationReason);
    void on_actionUso_del_diccionario_triggered();
    void on_actionAbreviaturas_y_signos_triggered();
    void on_actionNovedades_triggered();
    void on_actionAlgunos_datos_triggered();
    void progresoCarga(int);
    void resultadoCarga(bool);
    void errorAlCargar();
};

#endif // MAINWINDOW_H

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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWebView>
#include <QLineEdit>
#include <QShortcut>
#include <QKeyEvent>

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = 0, QWebView *webView = 0, QLineEdit *lineEdit = 0);
    ~SearchWidget();
    void focusLineEdit();
    void selectAll();

private slots:
    void findNext();
    void findPrevious();

private:
    QWebView *m_webView;
    QLineEdit *lineEditConsultar;
    Ui::SearchWidget *ui;
    void findText(const QString &text, QWebPage::FindFlags findFlags);
    void keyPressEvent(QKeyEvent *event);

};

#endif // SEARCHWIDGET_H

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

#include "searchwidget.h"
#include "ui_searchwidget.h"

SearchWidget::SearchWidget(QWidget *parent, QWebView *webView, QLineEdit *lineEdit) :
    QWidget(parent),
    m_webView(webView),
    lineEditConsultar(lineEdit),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

#if QT_VERSION >= 0x050200
    ui->lineEdit->setClearButtonEnabled(true);
#endif

    QShortcut *findNextAction = new QShortcut(QKeySequence("F3"), this);
    QShortcut *findPreviousAction = new QShortcut(QKeySequence("Shift+F3"), this);

    connect(findNextAction, SIGNAL(activated()), this, SLOT(findNext()));
    connect(findPreviousAction, SIGNAL(activated()), this, SLOT(findPrevious()));
    connect(ui->pushButtonNext, SIGNAL(clicked(bool)), this, SLOT(findNext()));
    connect(ui->pushButtonPrevious, SIGNAL(clicked(bool)), this, SLOT(findPrevious()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(findNext()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(ui->pushButtonClose, SIGNAL(clicked(bool)), this, SLOT(close()));
}

SearchWidget::~SearchWidget()
{
    findText(QString(), QWebPage::FindWrapsAroundDocument);
    lineEditConsultar->setFocus();
    delete ui;
}

void SearchWidget::focusLineEdit()
{
    ui->lineEdit->setFocus();
}

void SearchWidget::selectAll()
{
    ui->lineEdit->selectAll();
    focusLineEdit();
}

void SearchWidget::findNext()
{
    findText(ui->lineEdit->text(), QWebPage::FindWrapsAroundDocument);
}

void SearchWidget::findPrevious()
{
    findText(ui->lineEdit->text(), QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
}

void SearchWidget::findText(const QString &text, QWebPage::FindFlags findFlags)
{
    bool found = m_webView->findText(text, findFlags);
    if (text.isEmpty()) {
        found = true;
    }
    found ? ui->lineEdit->setStyleSheet("") : ui->lineEdit->setStyleSheet("QLineEdit { background-color: #FF6666 }");
}

void SearchWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

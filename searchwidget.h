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

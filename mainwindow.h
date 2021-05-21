#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QByteArray>
#include <QTime>
#include <QTimer>
#include <QTextCursor>
#include <QSyntaxHighlighter>
#include <QTextStream>
#include <time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    unsigned short getSpeed() const;
    float getAccuracy() const;

private:
    Ui::MainWindow *ui;

    unsigned short pos;
    unsigned short mistakes;
    unsigned short chars_entered;
    unsigned short speed;
    bool           input_is_active;

    QString        full_text;
    QTimer*        label_timer;
    QTime          input_time;
    QList<int>     modifiers;
private slots:
    void onInpFinished();
    void keyHandle(int code);
    void timerUpd();
public slots:
    void InsertText();
    void MoveCursor();

signals:
    void sig_text_size(int size);
    void input_finished();
};

class Filter : public QObject
{
    Q_OBJECT
public:
    Filter(QObject* parent=0);
    ~Filter();
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
signals:
    void sig_key_code(int code);
};

class HL : public QSyntaxHighlighter
{
public:
    HL(int size, QObject *parent=0);
    ~HL();
protected:
    void highlightBlock(const QString &text);
private:
    int text_size;
};

class RandomTextLoader : public QObject
{
public:
    RandomTextLoader(QObject *parent=0);
    ~RandomTextLoader();

    static QByteArray loadText(const QString lang="eng");
    static QString SimplifyChars(QString &text);
};


#endif // MAINWINDOW_H

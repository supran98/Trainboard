#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pos = 0;
    full_text = RandomTextLoader::loadText("rus");
    mistakes = 0;
    chars_entered = 0;
    input_is_active = false;
    modifiers << 16777248 << 16777249 << 16777252 << 16777217 << 16777251 << 16777250;

    ui->textEdit->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->textEdit->setPlainText(full_text);
    filter = new Filter(ui->textEdit);
    ui->textEdit->installEventFilter(filter);
    label_timer = new QTimer();

    connect(filter,             SIGNAL(sig_key_code(int)), this, SLOT(HandleKey(int)));
    connect(ui->restart_button, SIGNAL(clicked(bool)), SIGNAL(input_finished()));
    connect(ui->restart_button, SIGNAL(clicked(bool)), ui->textEdit, SLOT(setFocus()));
    connect(ui->restart_button, SIGNAL(clicked(bool)), SLOT(ShowShortcutInfo()));
    connect(ui->insert_txt_btn, SIGNAL(clicked(bool)), SLOT(InsertText()));
    connect(this,               SIGNAL(input_finished()), SLOT(onInpFinished()));
    connect(label_timer,        SIGNAL(timeout()), SLOT(timerUpd()));
    connect(ui->textEdit,       SIGNAL(textChanged()), SLOT(MoveCursor()));

    label_timer->start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete label_timer;
    delete filter;
}

float MainWindow::getAccuracy() const
{
    if (mistakes == 0)
        return 100.0;

    float accuracy = 100 - ((float)mistakes / chars_entered) * 100.0;

    return  mistakes < chars_entered ? accuracy : 0;

}

unsigned short MainWindow::getSpeed() const
{
    if (!input_time.elapsed())
        return 0;
    else
        return chars_entered / (float)(input_time.elapsed() / 1000.0) * 60;
}

void MainWindow::HandleKey(int code)
{
    if (modifiers.contains(code))
        return ;
    if (!input_is_active)
    {
        input_is_active = true;
        label_timer->start(500);
        if (!input_time.restart())
            input_time.start();
    }

    if (code == full_text[chars_entered])
    {
        if(chars_entered > 0)
            delete highlighter;
        chars_entered++;
        highlighter = new HL(chars_entered, ui->textEdit);

    }
    else
        mistakes++;

    if (chars_entered == full_text.size())
    {
        label_timer->stop();
        emit input_finished();
    }
}

void MainWindow::onInpFinished()
{
    input_is_active = false;
    if (chars_entered)
        delete highlighter;
    mistakes = 0;
    chars_entered = 0;
}

void MainWindow::timerUpd()
{
    if (input_is_active)
    {
        ui->label->setText("Speed: " + QString::number(getSpeed()) +
                           "    Mistakes: " + QString::number(mistakes) +
                           "    Accuracy: " + QString::number(getAccuracy(), 'f', 1));
    }
    else
        ui->label->setText("Start typing as soon as you're ready");
}

void MainWindow::InsertText()
{
    ui->statusBar->showMessage("\"Ctrl+F\"", 500);
    emit input_finished();

    if (ui->setEngLayout->isChecked() && !ui->setRusLayout->isChecked())
        ui->textEdit->setPlainText(RandomTextLoader::loadText("eng"));

    else if ((ui->setEngLayout->isChecked() && ui->setRusLayout->isChecked())
            || (!ui->setRusLayout->isChecked() && !ui->setRusLayout->isChecked()))
    {
        time(0) % 2 ? //simple random
                 ui->textEdit->setPlainText(RandomTextLoader::loadText("rus")) :
                 ui->textEdit->setPlainText(RandomTextLoader::loadText("eng"));
    }
    else
        ui->textEdit->setPlainText(RandomTextLoader::loadText("rus"));

    full_text = ui->textEdit->toPlainText();
    if (full_text.isEmpty())
        ui->statusBar->showMessage("File with texts not found", 3000);

    ui->textEdit->setFocus();
}

void MainWindow::MoveCursor()
{
    QTextCursor cursor = ui->textEdit->textCursor();

    if (!input_is_active)
        cursor.setPosition(0);
    else
        cursor.setPosition(chars_entered);

    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::ShowShortcutInfo()
{
    ui->statusBar->showMessage("\"Ctrl+R\"", 500);
}

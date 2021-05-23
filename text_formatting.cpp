#include "mainwindow.h"

Filter::Filter(QObject *parent) : QObject(parent)
{

}

Filter::~Filter()
{

}

bool Filter::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    int key = static_cast<QKeyEvent*>(event)->key();

    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->modifiers() & Qt::ShiftModifier)
            emit sig_key_code(key);

        else if (key == 1025) // "ё"-handling
            emit sig_key_code(1105);

        else if ((key >= 65 && key <= 90) || (key >= 1040 && key <= 1071))
            emit sig_key_code(key + 32);

        else
            emit sig_key_code(key);

        return true;
    }

    return false;
}

HL::HL(QObject *parent) : QSyntaxHighlighter(parent)
{

}

HL::HL(int size, QObject *parent) : QSyntaxHighlighter(parent), highlighted_chars(size)
{

}

HL::~HL()
{

}

void HL::highlightBlock(const QString &text)
{
    Q_UNUSED(text)

    QTextCharFormat format;
    format.setBackground(Qt::yellow);

    setFormat(0, highlighted_chars, format);
}

RandomTextLoader::RandomTextLoader(QObject *parent) : QObject(parent)
{

}

RandomTextLoader::~RandomTextLoader()
{

}

QByteArray RandomTextLoader::loadText(const QString lang)
{
    QTextStream out;
    QVector<QString> all_texts;
    QFile texts_rus("texts_rus.txt");
    QFile texts_eng("texts_eng.txt");

    if (lang == "rus")
    {
        if (texts_rus.open(QIODevice::ReadOnly))
        {
            out.setDevice(&texts_rus);
            out.setCodec("UTF-8");
            all_texts = out.readAll().split(QRegExp("@+! END_OF_THE_TEXT+ !+@")).toVector();
            texts_rus.close();
        }
        else
            return "";
    }
    else if (lang == "eng")
    {
        if (texts_eng.open(QIODevice::ReadOnly))
        {
            out.setDevice(&texts_eng);
            out.setCodec("UTF-8");
            all_texts = out.readAll().split(QRegExp("@+! END_OF_THE_TEXT+ !+@")).toVector();
            texts_eng.close();
        }
        else
            return "";
    }
    qsrand(static_cast<unsigned int>(time(0)));

    return SimplifyChars(all_texts[qrand() % all_texts.size()]).toUtf8();
}

QString RandomTextLoader::SimplifyChars(QString &text)
{
    for (int pos = 0; pos < text.size(); pos++)
    {
        if (text[pos] == 0xAB || text[pos] == 0xBB) // ordinary quotes instead of angle quotes
            text[pos] = '\"';

        if (text[pos].category() == QChar::Punctuation_Dash)
            text[pos] = '-';
    }

    return text.simplified();
}

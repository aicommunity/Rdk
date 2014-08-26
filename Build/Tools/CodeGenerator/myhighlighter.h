#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class MyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit MyHighlighter(QObject *parent = 0);
    explicit MyHighlighter(QTextDocument *parent);
protected:
    void highlightBlock(const QString &text);

signals:

public slots:

};

#endif // MYHIGHLIGHTER_H

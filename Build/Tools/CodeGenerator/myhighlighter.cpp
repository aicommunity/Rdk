#include "myhighlighter.h"

MyHighlighter::MyHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

MyHighlighter::MyHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

void MyHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat keyWordFormat;
    keyWordFormat.setFontWeight(QFont::Bold);
    keyWordFormat.setForeground(Qt::darkBlue);
    QString keyWordPattern = "foreach|bool|int|void|double|float|char|delete|class|const|virtual|mutable|this|struct|union|throw|^for|if|else|false|namespace|switch|case|public|private|protected|new|return|using|true|emit|connect|SIGNAL|SLOT|slots|signals|^#ifndef|^#define|^#endif|^#include|^#pragma";

    QRegExp expression(keyWordPattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, keyWordFormat);
        index = text.indexOf(expression, index + length);
     }

    QTextCharFormat lineCommentFormat;
    lineCommentFormat.setForeground(Qt::darkGreen);
    QRegExp startCommentExp("//");
    QRegExp endCommentExp("\r");

    setCurrentBlockState(0);

    int startIndex2 = 0;
    if (previousBlockState() != 1)
        startIndex2 = text.indexOf(startCommentExp);

    while (startIndex2 >= 0) {
       int endIndex2 = text.indexOf(endCommentExp, startIndex2);
       int commentLength2;
       if (endIndex2 == -1) {
           setCurrentBlockState(1);
           commentLength2 = text.length() - startIndex2;
       } else {
           commentLength2 = endIndex2 - startIndex2
                           + endCommentExp.matchedLength();
       }
       setFormat(startIndex2, commentLength2, lineCommentFormat);
       startIndex2 = text.indexOf(startCommentExp,
                                 startIndex2 + commentLength2);
    }





    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setForeground(Qt::darkGreen);

    QRegExp startExpression("/\\*");
    QRegExp endExpression("\\*/");

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(startExpression);

    while (startIndex >= 0) {
       int endIndex = text.indexOf(endExpression, startIndex);
       int commentLength;
       if (endIndex == -1) {
           setCurrentBlockState(1);
           commentLength = text.length() - startIndex;
       } else {
           commentLength = endIndex - startIndex
                           + endExpression.matchedLength();
       }
       setFormat(startIndex, commentLength, multiLineCommentFormat);
       startIndex = text.indexOf(startExpression,
                                 startIndex + commentLength);
    }
 }

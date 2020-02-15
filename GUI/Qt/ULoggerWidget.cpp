#include "ULoggerWidget.h"

ULoggerWidget::ULoggerWidget(QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app)
{
    setAccessibleName("ULoggerWidget");
    UpdateInterval = 200;
    CheckModelFlag = false; // for updating logger when model not loaded yet

    layout = new QVBoxLayout(this);
    textEdit = new QPlainTextEdit(this);
    textEdit->clear();
    layout->addWidget(textEdit);
    layout->setMargin(0);

    UpdateInterface(true);
}

ULoggerWidget::~ULoggerWidget()
{
    delete textEdit;
}

void ULoggerWidget::AUpdateInterface()
{
    if(!application) return;
    std::list<std::string> log = application->GetEngineControl()->GetEngineStateThread()->ReadGuiUnsentLog();

    for(std::list<std::string>::iterator i = log.begin(); i != log.end(); ++i)
    {
     int log_level=-1;
     std::string::size_type pos1=i->find_first_of('>');
     if(pos1 != std::string::npos)
     {
      std::string::size_type pos2=i->find_first_of('>',pos1+1);
      if(pos2 != std::string::npos && pos2 > 0)
      {
       log_level=RDK::atoi(i->substr(pos2-1,1));
      }
     }
     AddString(log_level,QString::fromLocal8Bit(i->c_str()));
    }
}

void ULoggerWidget::AddString(int log_level, const QString &string)
{
 Qt::GlobalColor color;
 switch(log_level)
 {
 case RDK_EX_APP:
  color=Qt::blue;
 break;

 case RDK_EX_INFO:
  color=Qt::darkGreen;
 break;

 case RDK_EX_DEBUG:
  color=Qt::darkBlue;
 break;

 case RDK_EX_WARNING:
  color=Qt::darkYellow;
 break;

 case RDK_EX_ERROR:
  color=Qt::darkRed;
 break;

 case RDK_EX_FATAL:
  color=Qt::red;
 break;

 case RDK_EX_UNKNOWN:
  color=Qt::magenta;
 break;

 default:
  color=Qt::black;
 }
 QTextCharFormat tf=textEdit->currentCharFormat();
 tf.setForeground(QBrush(color));
 textEdit->setCurrentCharFormat(tf);
 textEdit->appendPlainText(string);
}

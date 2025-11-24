#include "ULoggerWidget.h"

#include "../../Core/Engine/UGlogGuiSink.h"
#include "../../Deploy/Include/rdk_error_codes.h"

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif

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

namespace
{
int MapLogSeverity(const RDK::UGlogGuiMessage& message)
{
 int level=RDK_EX_INFO;
#ifdef RDK_USE_GLOG
 switch(message.Severity)
 {
 case google::GLOG_FATAL:
  level=RDK_EX_FATAL;
  break;
 case google::GLOG_ERROR:
  level=RDK_EX_ERROR;
  break;
 case google::GLOG_WARNING:
  level=RDK_EX_WARNING;
  break;
 case google::GLOG_INFO:
 default:
  level=RDK_EX_INFO;
  break;
 }
#endif

 if(message.Text.find("[APP]") != std::string::npos)
  level=RDK_EX_APP;
 else
 if(message.Text.find("[DEBUG]") != std::string::npos)
  level=RDK_EX_DEBUG;

 return level;
}
}

void ULoggerWidget::AUpdateInterface()
{
 if(!application)
  return;

 const std::vector<RDK::UGlogGuiMessage> messages = RDK::UGlogGuiSink::Instance().ReadMessages(512);
 for(const RDK::UGlogGuiMessage& message : messages)
 {
  const int severity = MapLogSeverity(message);
  AddString(severity, QString::fromLocal8Bit(message.Text.c_str()));
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

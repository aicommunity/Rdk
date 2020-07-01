#include "UClassGenerator.h"
#include "ui_UClassGenerator.h"
#include <QFileDialog>
#include <fstream>
#include <string>

using namespace std;

UClassGenerator::UClassGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UClassGenerator)
{
    ui->setupUi(this);

    connect(ui->GenerateBtn, SIGNAL(clicked()), this, SLOT(generate()));
    connect(ui->BrowsePathBtn, SIGNAL(clicked()), this, SLOT(browsePath()));

}

UClassGenerator::~UClassGenerator()
{
    delete ui;
}

string stringToUpper(const string& src)
{
    string newStr(src);
    string::iterator it = newStr.begin();
    for(; it != newStr.end(); ++it){
        if(*it == '_')
            continue;
        if(!std::isupper(*it)){
            *it = static_cast<char>(std::toupper(static_cast<unsigned char>(*it)));
        } else {
            if(it != newStr.begin())
            {
                string newPart(it, newStr.end());
                *it++ = '_';
                string recursResult = stringToUpper(newPart);
                string out(newStr.begin(), it);
                out += recursResult;
                return out;
            }
        }
    }
    return newStr;
}

string makeCodeGuardHead(const string& className)
{
    string define = stringToUpper(className);
    define += string("_H");
    string guardHead("#ifndef ");
    guardHead += define;
    guardHead += string("\n");
    guardHead += string("#define ");
    guardHead += define;
    guardHead += string("\n\n");
    return guardHead;
}


string makeIncludeRdk(int depth){
    string up("../");
    string out("#include \"");
    for(int i = 0; i != depth; ++i)
        out += up;
    out += string("Rdk/Deploy/Include/rdk.h\"\n");
    return out;
}

string makeClassHeader(const string& className, int depth)
{
    string out(makeCodeGuardHead(className));
    out += makeIncludeRdk(depth);
    out += string("\nnamespace RTV\n{\n\t");
    out += string("using namespace RDK;\n\t");
    out += string("class RDK_LIB_TYPE ");
    out += className;
    out += string(": public UNet\n\t{\n\t");
    out += string("\tbool ADefault(void);\n\t");
    out += string("\tbool ABuild(void);\n\t");
    out += string("\tbool AReset(void);\n\t");
    out += string("\tbool ACalculate(void);\n\t");
    out += string("public:\n\t\t");
    out += className;
    out += string("();\n\t\t~");
    out += className;
    out += string("();\n\t\t");
    out += className;
    out += string("* New(void);\n\t};\n}\n#endif");
    return out;
}

string classSpace(const string& className)
{
    string out(className);
    out += "::";
    return out;
}

string boolClassSpace(const string& className)
{
    string out("bool ");
    out += className;
    out += "::";
    return out;
}

string defOut(const bool& fill=false)
{
    string out("()\n\t{\n\t");
    if(fill)
        out += string("\treturn true;\n\t");
    out += string("}\n\n\t");
    return out;
}

string makeClassDeff(const string& className)
{
    string out("#include \"");
    out += className;
    out += string(".h\"\n\n");
    out += string("namespace RTV\n{\n\n\t");
    out += classSpace(className);
    out += className;
    out += defOut();
    out += classSpace(className);
    out += string("~");
    out += className;
    out += defOut();
    out += className;
    out += string("* ");
    out += classSpace(className);
    out += string("New()\n\t{\n\t\treturn new ");
    out += className;
    out += string(";\n\t}\n\n\t");
    out += boolClassSpace(className);
    out += string("ADefault");
    out += defOut(true);
    out += boolClassSpace(className);
    out += string("ABuild");
    out += defOut(true);
    out += boolClassSpace(className);
    out += string("AReset");
    out += defOut(true);
    out += boolClassSpace(className);
    out += string("ACalculate");
    out += defOut(true);
    out += string("\n}");
    return out;
}

void UClassGenerator::generate()
{
    ui->ResultEdit->setText("");
    string className(ui->NameEdit->text().toStdString());
    int depthPath = ui->LevelBox->value();

    QString nameWithPath(ui->PathEdit->text());
    if(nameWithPath.lastIndexOf('/') != (nameWithPath.size()-1))
        nameWithPath += '/';
    nameWithPath += className.c_str();

    string headerFileName(nameWithPath.toStdString());
    headerFileName += string(".h");
    string cppFileName(nameWithPath.toStdString());
    cppFileName += string(".cpp");

    std::ofstream header(headerFileName, std::ios_base::out);
    if(!header.is_open()){
        string errMsg("Can't open ");
        errMsg += headerFileName;
        ui->ResultEdit->setText(errMsg.c_str());
        return;
    }
    header <<  makeClassHeader(className, depthPath);
    header.close();

    ofstream cpp(cppFileName, ios_base::out);
    if(!cpp.is_open()){
        string errMsg("Can't open ");
        errMsg += cppFileName;
        ui->ResultEdit->setText(errMsg.c_str());
        return;
    }
    cpp << makeClassDeff(className);
    cpp.close();
    ui->ResultEdit->setText("Complite");
}

void UClassGenerator::browsePath()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    dialog.exec();

    ui->PathEdit->setText(dialog.directory().path());
}

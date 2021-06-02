#include "testform.hpp"

#include "header.hpp"

#include <QMessageBox>

TestForm::TestForm(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    object.child->childString.ReadBind(lineEdit, "text");
    object.child->childString.ReadBind(label, "text");
    object.child->childString.WriteBind(lineEdit, "text", &QLineEdit::textChanged);
}

void TestForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void TestForm::on_pushButton_clicked()
{
    QMessageBox::information(this, "Values should be the same", "UI: " + lineEdit->text() + "\n" + "Object: " + object.child->childString);
}

void TestForm::on_pushButton_2_clicked()
{
    object.child->childString = "val";
}

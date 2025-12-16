#ifndef CALAMUSMAIN_H
#define CALAMUSMAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class CalamusMain;
}
QT_END_NAMESPACE

class CalamusMain : public QMainWindow
{
    Q_OBJECT

public:
    CalamusMain(QWidget *parent = nullptr);
    ~CalamusMain();

private:
    Ui::CalamusMain *ui;
};
#endif // CALAMUSMAIN_H

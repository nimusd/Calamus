#include "container.h"
#include "ui_container.h"
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QShowEvent>
#include <QEvent>
#include <QMouseEvent>

Container::Container(QWidget *parent, const QString &name, const QColor &color,
                     const QStringList &inputs, const QStringList &outputs)
    : QWidget(parent)
    , ui(new Ui::Container)
    , dragging(false)
{
    ui->setupUi(this);
    containerColor = color;
    // Set the container name
    ui->labelInstanceName->setText(name);

    // Set the header color
    QString style = QString("background-color: %1; color: white;").arg(color.name());
    ui->containerFrameHeader->setStyleSheet(style);

    // Set pale background for the body
    QColor paleColor = color.lighter(180);
    QString bodyStyle = QString("background-color: %1;").arg(paleColor.name());
    ui->frameInputs->setStyleSheet(bodyStyle);
    ui->frameOutputs->setStyleSheet(bodyStyle);

    // Clear placeholder labels from inputs frame
    QLayoutItem *item;
    while ((item = ui->frameInputs->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Clear placeholder labels from outputs frame
    while ((item = ui->frameOutputs->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add input port labels
    for (const QString &input : inputs) {
        QLabel *label = new QLabel(input, this);
        ui->frameInputs->layout()->addWidget(label);

        // Create circle on the container itself
        QLabel *circle = new QLabel("●", this);
        circle->setFixedSize(24, 24);
        circle->setStyleSheet("color: #666; font-size: 22px; background-color: #e8d0f0;");
        ports.append({input, false, circle});
    }

    // Add output port labels
    for (const QString &output : outputs) {
        QLabel *label = new QLabel(output, this);
        label->setAlignment(Qt::AlignRight);
        ui->frameOutputs->layout()->addWidget(label);

        // Create circle on the container itself
        QLabel *circle = new QLabel("●", this);
        circle->setFixedSize(24, 24);
        circle->setStyleSheet("color: #666; font-size: 22px; background-color: #e8d0f0;");
        ports.append({output, true, circle});
    }

    // Store container name
    containerName = name;

    // Install event filter on all port circles
    for (const PortInfo &port : ports) {
        port.circle->installEventFilter(this);
    }
}



Container::~Container()
{
    delete ui;
}


void Container::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    positionPortCircles();
}

void Container::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    positionPortCircles();
}

void Container::positionPortCircles()
{
    int inputIndex = 0;
    int outputIndex = 0;

    for (const PortInfo &port : ports) {
        QLayout *layout;
        int index;
        int xPos;

        if (port.isOutput) {
            layout = ui->frameOutputs->layout();
            index = outputIndex++;
            xPos = width() - 24;
        } else {
            layout = ui->frameInputs->layout();
            index = inputIndex++;
            xPos = 0;
        }

        if (QLayoutItem *item = layout->itemAt(index)) {
            QWidget *label = item->widget();
            if (label) {
                int y = label->mapTo(this, QPoint(0, label->height()/2)).y() - 12;
                port.circle->move(xPos, y);
            }
        }
    }
}

bool Container::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *circle = qobject_cast<QLabel*>(watched);
        if (circle) {
            for (const PortInfo &port : ports) {
                if (port.circle == circle) {
                    emit portClicked(this, port.name, port.isOutput, circle->mapToGlobal(QPoint(12, 12)));
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Container::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->pos();
    }
    emit clicked(this);
}

void Container::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPoint newPos = mapToParent(event->pos() - dragStartPosition);
        move(newPos);
    }
}

void Container::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}

void Container::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    emit moved();
}


void Container::setSelected(bool selected)
{
    isSelected = selected;
    if (isSelected) {
        setStyleSheet("QFrame { border: 1px solid #0066CC; border-radius: 5px; }");
    } else {
        setStyleSheet("");  // Clear to original style
    }
}

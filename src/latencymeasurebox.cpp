#include "latencymeasurebox.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QApplication>
#include <qstyle.h>

LatencyMeasureBox::LatencyMeasureBox(QWidget *parent, const QString& text, const QString& detailtext, int inmax, int outmax)
    : QDialog(parent)
{

    
    input = new QSpinBox(this);
    output = new QSpinBox(this);
    input->setMinimum(1);
    input->setMaximum(inmax);
    output->setMinimum(1);
    output->setMaximum(outmax);
    
    inputLabel = new QLabel(i18n("Input: "), this);
    outputLabel = new QLabel(i18n("Output: "), this);
    
    QGridLayout *g = new QGridLayout;
    
    g->addWidget(new QLabel(text, this),  g->rowCount(), 1, 1, 4);
    
    g->addWidget(inputLabel, g->rowCount(), 1, 1, 1, Qt::AlignRight);
    g->addWidget(input, g->rowCount()-1, 2, 1, 1, Qt::AlignLeft);
    g->addWidget(outputLabel, g->rowCount()-1, 3, 1, 1, Qt::AlignRight);
    g->addWidget(output, g->rowCount()-1, 4, 1, 1, Qt::AlignLeft);
    
    g->addWidget(new QLabel(detailtext, this),  g->rowCount(), 1, 1, g->columnCount()-1);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No, this);
    g->addWidget(buttonBox, g->rowCount(), 1, 1, g->columnCount()-1);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    icon = new QLabel(this);
    int iconsize = QApplication::style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
    icon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(iconsize,iconsize));
    icon->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    g->addWidget(icon, 0, 0, g->rowCount(), 1);
    
    setLayout(g);
}

LatencyMeasureBox::~LatencyMeasureBox()
{
    delete input;
    delete output;
    delete inputLabel;
    delete outputLabel;
    delete buttonBox;
    delete icon;
}

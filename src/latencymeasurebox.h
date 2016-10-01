#ifndef LATENCYMEASUREBOX_H
#define LATENCYMEASUREBOX_H

#include <QDialog>
#include <QSpinBox>
#include <QGroupBox>
#include <QLabel>

class QDialogButtonBox;

class LatencyMeasureBox : public QDialog
{
    Q_OBJECT
    
public :
    LatencyMeasureBox(QWidget *parent = nullptr, const QString& text=QString(), const QString& detailtext=QString(), int inmax=2, int outmax=2);
    ~LatencyMeasureBox();
    
    QSpinBox *input;
    QSpinBox *output;
    
private : 
    QLabel *inputLabel;
    QLabel *outputLabel;
    QLabel *icon;
    QDialogButtonBox *buttonBox;

};

#endif

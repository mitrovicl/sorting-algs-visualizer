#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QComboBox;
class QRadioButton;
class SortVisualizer;
class QPushButton;
class QSpinBox;
class QLCDNumber;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void disableButtons();
    void enableButtons();

private:
    SortVisualizer *sortVisualizer;
    QComboBox *algorithmsComboBox;
    QRadioButton *radioIncreasing;
    QRadioButton *radioDecreasing;
    QSpinBox *intervalSpinBox;
    QComboBox *chooseRectsNumberCombo;
    QPushButton *randomizeButton;
    QPushButton *startButton;
    QPushButton *stopButton;
    QLCDNumber *timeLCD;
    int algorithmIndex;

private slots:
    void start();
    void stop();
    void randomize();
    void numRectsChanged();

public slots:
    void setElapsedTime(int time);

};

#endif // MAINWINDOW_H

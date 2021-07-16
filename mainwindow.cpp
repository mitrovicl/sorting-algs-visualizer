#include "mainwindow.h"
#include "sortvisualizer.h"
#include <QLayout>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    // QComboBox

    QWidget *widget = new QWidget;

    algorithmsComboBox = new QComboBox;
    algorithmsComboBox->addItem(tr("Selection sort"));
    algorithmsComboBox->addItem(tr("Bubble Sort"));
    algorithmsComboBox->addItem(tr("Insertion sort"));
    algorithmsComboBox->addItem(tr("Merge sort"));
    algorithmsComboBox->addItem(tr("Quick Sort"));

    radioIncreasing = new QRadioButton(tr("Increasing"), this);
    radioIncreasing->setChecked(true);
    radioDecreasing = new QRadioButton(tr("Decreasing"), this);

    intervalSpinBox = new QSpinBox;
    intervalSpinBox->setRange(0, 10000);
    intervalSpinBox->setValue(5);

    chooseRectsNumberCombo = new QComboBox;
    chooseRectsNumberCombo->addItem(QString::number(10), 10);
    chooseRectsNumberCombo->addItem(QString::number(20), 20);
    chooseRectsNumberCombo->addItem(QString::number(50), 50);
    chooseRectsNumberCombo->addItem(QString::number(100), 100);
    chooseRectsNumberCombo->addItem(QString::number(200), 200);
    chooseRectsNumberCombo->addItem(QString::number(250), 250);
    chooseRectsNumberCombo->addItem(QString::number(500), 500);
    chooseRectsNumberCombo->addItem(QString::number(1000), 1000);
    chooseRectsNumberCombo->setCurrentIndex(4);

    randomizeButton = new QPushButton;
    randomizeButton->setText(tr("Randomize"));

    startButton = new QPushButton;
    startButton->setText(tr("Start"));


    stopButton = new QPushButton;
    stopButton->setText(tr("Stop"));

    timeLCD = new QLCDNumber;
    QPalette palette = timeLCD->palette();
    palette.setColor(palette.WindowText, Qt::red);
    palette.setColor(palette.Foreground, Qt::red);
    palette.setColor(palette.Background, Qt::red);
    timeLCD->setPalette(palette);
    timeLCD->setSegmentStyle(QLCDNumber::Flat);
    qDebug() << timeLCD->segmentStyle() << "\n";
    qDebug() << timeLCD->palette() << "\n";



    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel(tr("Algorithm:"), this));
    rightLayout->addWidget(algorithmsComboBox);
    rightLayout->addWidget(radioIncreasing);
    rightLayout->addWidget(radioDecreasing);
    rightLayout->addWidget(new QLabel(tr("Interval [ms]:"), this));
    rightLayout->addWidget(intervalSpinBox);
    rightLayout->addWidget(new QLabel(tr("Number of elements:"), this));
    rightLayout->addWidget(chooseRectsNumberCombo);
    rightLayout->addWidget(randomizeButton);
    rightLayout->addWidget(startButton);
    rightLayout->addWidget(stopButton);
    rightLayout->addStretch();
    rightLayout->addWidget(timeLCD);


    sortVisualizer = new SortVisualizer();
    //leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(sortVisualizer, &SortVisualizer::enableButtons, this, &MainWindow::enableButtons);
    connect(sortVisualizer, &SortVisualizer::disableButtons, this, &MainWindow::disableButtons);

    connect(randomizeButton, &QPushButton::clicked, this, &MainWindow::randomize);

    connect(sortVisualizer, &SortVisualizer::setTimeElapsed, this, &MainWindow::setElapsedTime);

    connect(chooseRectsNumberCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::numRectsChanged);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::start);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stop);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(sortVisualizer);
    mainLayout->addLayout(rightLayout);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
    /*
    qDebug() << widget->width() << " " << widget->height() << "\n";
    qDebug() << width() << " " << height() << "\n";
    qDebug() << size() << "\n";
    */
}

void MainWindow::start() {
    algorithmIndex = algorithmsComboBox->currentIndex();
    qDebug() << "currentIndex: " << algorithmsComboBox->currentIndex();
    qDebug() << "algorithmIndex: " << algorithmIndex;
    bool increasing = radioIncreasing->isChecked();
    int interval = intervalSpinBox->value();
    int numberOfRects = chooseRectsNumberCombo->currentData().toInt();
    disableButtons();
    sortVisualizer->startSort(algorithmIndex, increasing, interval, numberOfRects);
}

void MainWindow::stop() {
    sortVisualizer->stopSort();
}

void MainWindow::disableButtons() {
    startButton->setDisabled(true);
    randomizeButton->setDisabled(true);
    chooseRectsNumberCombo->setDisabled(true);
}

void MainWindow::enableButtons() {
    startButton->setDisabled(false);
    randomizeButton->setDisabled(false);
    chooseRectsNumberCombo->setDisabled(false);
}

void MainWindow::setElapsedTime(int time) {
    if (time/1000 != timeLCD->intValue()) {
        timeLCD->display(time/1000);
    }
}

void MainWindow::randomize() {
    sortVisualizer->randomize();
}

void MainWindow::numRectsChanged() {
    int numRects = chooseRectsNumberCombo->currentData().toInt();
    sortVisualizer->changeNumberOfRectangles(numRects);
}











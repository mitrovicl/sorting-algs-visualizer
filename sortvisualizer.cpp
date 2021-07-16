#include "sortvisualizer.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QColor>

SortVisualizer::SortVisualizer() {

    setFixedSize(1000, 500);
    rectangleWidth = 5;
    setNumberOfElements(width() / rectangleWidth);
    generateRectangles();
    intervalLength = 5;

}

void SortVisualizer::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (rectangleWidth >= 3) {
        painter.setPen(colors[DefaultBorderColor]);
        //qDebug() << painter.pen();
        //painter.setPen(Qt::blue);
        //qDebug() << painter.pen();
    } else {
        painter.setPen(Qt::NoPen);
        //qDebug() << painter.pen();
    }
    painter.setBrush(colors[DefaultFillColor]);
    //qDebug() << painter.brush();

    for (int i = 0; i < numberOfElements; i++) {
        if (rectangleColors[i] != DefaultFillColor) {
            colorRectangle(i, &painter, colors[rectangleColors[i]]);
        } else {
            painter.drawRect(rectanglesArray[i]);
        }
    }

}

void SortVisualizer::setNumberOfElements(int newNumberOfElements) {
    numberOfElements = newNumberOfElements;
}

void SortVisualizer::generateRectangles() {
    generateArray();
    rectanglesArray.clear();
    for (int i = 0; i < numberOfElements; i++) {
        rectanglesArray.append(QRect(i * rectangleWidth, height() - numbersArray[i], rectangleWidth, numbersArray[i]));
    }
    qDebug() << "rectanglesArray size: " << rectanglesArray.size();
}

void SortVisualizer::generateArray() {
    numbersArray.clear();
    rectangleColors.clear();
    for (int i = 0; i < numberOfElements; i++) {
        numbersArray.append(QRandomGenerator::global()->bounded(height()));
        rectangleColors.append(DefaultFillColor);
    }
}

/*!
 * \brief SortVisualizer::copyNumbersArray copies numbersArray into sortArray (for sorting purposes)
 */
void SortVisualizer::copyNumbersArray() {
    sortArray.clear();
    for (int i = 0; i < numbersArray.size(); i++) {
        sortArray.append(numbersArray[i]);
    }
}

void SortVisualizer::setRectangleWidth(int newRectangleWidth) {
    if (newRectangleWidth != rectangleWidth) {
        rectangleWidth = newRectangleWidth;
        randomize();
    }
}

void SortVisualizer::randomize() {
    generateRectangles();
    update();
}

/*!
 * \brief SortVisualizer::swapRectangles swaps two rectangles (their height and y coordinate)
*/
void SortVisualizer::swapRectangles(int i, int j) {

    QRect rectI = rectanglesArray[i];
    QRect rectJ = rectanglesArray[j];

    rectanglesArray[i].setY(rectJ.y());
    rectanglesArray[i].setHeight(rectJ.height());

    rectanglesArray[j].setY(rectI.y());
    rectanglesArray[j].setHeight(rectI.height());

    qSwap(numbersArray[i], numbersArray[j]);
    //qSwap(rectangleColors[i], rectangleColors[j]);
}

/*!
 * \brief SortVisualizer::setRectangle sets i rectangle's height and y coordinate to j rectangle's height and y coordinate
*/
void SortVisualizer::setRectangle(int i, int j) {
    rectanglesArray[i].setY(rectanglesArray[j].y());
    rectanglesArray[i].setHeight(rectanglesArray[j].height());
    numbersArray[i] = numbersArray[j]; ///////
}

void SortVisualizer::setRectangle(int i, QRect rect) {
    rectanglesArray[i].setY(rect.y());
    rectanglesArray[i].setHeight(rect.height());
    numbersArray[i] = rect.height(); /////////////
}

void SortVisualizer::setRectangleToHeight(int i, int height) {
    rectanglesArray[i].setY(this->height() - height);
    rectanglesArray[i].setHeight(height);


    //qDebug() << "height: " << rectanglesArray[i].height() << " " << rectanglesArray[i].y();

    numbersArray[i] = height;
}

void SortVisualizer::colorRectangle(int i, QPainter *painter, QColor color) {
    //qDebug() << color;
    //qDebug() << colors[DefaultBorderColor];
    //qDebug() << colors[HighlightColor];
    painter->setBrush(color);
    painter->drawRect(rectanglesArray[i]);
    painter->setBrush(colors[DefaultFillColor]);
}

void SortVisualizer::startSort(int algorithmIndex, bool increasing, int interval, int numRects) {
    isIncreasingSort = increasing;
    intervalLength = interval;
    setNumberOfElements(numRects);
    setRectangleWidth(width() / numRects);

    elapsedTimer = new QElapsedTimer;
    elapsedTimer->start();

    //qDebug() << algorithmIndex;
    copyNumbersArray();

    switch(algorithmIndex) {
    case 0:
        startSelectionSort();
        break;
    case 1:
        startBubbleSort();
        break;
    case 2:
        startInsertionSort();
        break;
    case 3:
        startMergeSort();
        break;
    case 4:
        startQuickSort();
        break;
    default:
        break;

    }
}

void SortVisualizer::stopSort() {
    qDebug() << "Stopped sort";

    timer->stop();
    timerUpdateElapsed->stop();
    int elapsedTime = elapsedTimer->elapsed();

    emit setTimeElapsed(elapsedTime);

    rectangleColors.fill(DefaultFillColor);

    update();
    emit enableButtons();
}

void SortVisualizer::updateTimeElapsed() {
    emit setTimeElapsed(elapsedTimer->elapsed());
}

void SortVisualizer::changeNumberOfRectangles(int newNumberOfRectangles) {
    setNumberOfElements(newNumberOfRectangles);
    setRectangleWidth(width() / newNumberOfRectangles);
}

void SortVisualizer::connectTimers() {
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timerUpdateElapsed = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &SortVisualizer::processCurrentStep);
    connect(timerUpdateElapsed, &QTimer::timeout, this, &SortVisualizer::updateTimeElapsed);

    timer->start(intervalLength);
    timerUpdateElapsed->start(50);

    qDebug() << timer->interval();
}



#define HIGHLIGHT 0
#define SWAP 1
#define SET_HEIGHT 2
#define SET 3
#define HIGHLIGHT_RANGE 4
#define HIGHLIGHT_ONE 5

void SortVisualizer::processCurrentStep() {
    if (currIndex >= stepsVector.size()) {
        stopSort();
        return;
    }

    int i = std::get<1>(stepsVector[currIndex]);
    int j = std::get<2>(stepsVector[currIndex]);

    //qDebug() << i << " " << j << " " << currIndex << ": ";

    if (currIndex > 0) {
        unHighlight(std::get<0>(stepsVector[currIndex-1]), i, j);
    }

    switch (std::get<0>(stepsVector[currIndex])) {
    case HIGHLIGHT:
        processHighlight(i, j);
        break;
    case SWAP:
        processSwap(i, j);
        break;
    case SET_HEIGHT:
        processSetHeight(i, j);
        break;
    case SET:
        processSet(i, j);
        break;
    case HIGHLIGHT_RANGE:
        processHighlightRange(i, j);
        break;
    case HIGHLIGHT_ONE:
        rectangleColors[i] = HighlightColor;
        break;
    default:
        break;
    }

    //qDebug() << rectangleColors;
    update();
    currIndex++;
}

void SortVisualizer::unHighlight(int prevCommand, int i, int j) {
    if (prevCommand == HIGHLIGHT_RANGE) {
        //unHighlightRange(std::get<1>(stepsVector[currIndex-1]), std::get<2>(stepsVector[currIndex-1]));
    } else {
        rectangleColors[std::get<1>(stepsVector[currIndex-1])] = DefaultFillColor;
        if (prevCommand != SET_HEIGHT) {
            rectangleColors[std::get<2>(stepsVector[currIndex-1])] = DefaultFillColor;
        }
    }
}

void SortVisualizer::unHighlightRange(int begin, int end) {
    for (int i = begin; i <= end; i++) {
        rectangleColors[i] = DefaultFillColor;
    }
}


void SortVisualizer::processHighlight(int i, int j) {
    //qDebug() << i << " " << j;
    rectangleColors[i] = HighlightColor;
    rectangleColors[j] = HighlightColor;
}

void SortVisualizer::processSwap(int i, int j) {
    processHighlight(i, j);
    swapRectangles(i, j);
}

void SortVisualizer::processSetHeight(int i, int j) {
    rectangleColors[i] = HighlightColor;
    //qDebug() << ".. " << rectanglesArray;
    setRectangleToHeight(i, j);
    //qDebug() << ".. " << rectanglesArray;
}

void SortVisualizer::processSet(int i, int j) {
    processHighlight(i, j);
    setRectangle(i, j);
}

void SortVisualizer::processHighlightRange(int begin, int end) {
    for (int i = begin; i <= end; i++) {
        rectangleColors[i] = HighlightRangeColor;
    }
}

void SortVisualizer::processInsertionSet(int i, int j) {
    processHighlight(i, j);
    setRectangle(j, j-1);
}


void SortVisualizer::startSelectionSort() {
    currIndex = 0; // move to startSort()
    stepsVector.clear(); // move to startSort()

    if (isIncreasingSort) {
        selectionSortIncreasing();
    } else {
        selectionSortDecreasing();
    }

    connectTimers();

}

void SortVisualizer::startBubbleSort() {
    currIndex = 0; // move to startSort()
    stepsVector.clear(); // move to startSort()

    if (isIncreasingSort) {
        bubbleSortIncreasing();
    } else {
        bubbleSortDecreasing();
    }

    connectTimers();
}

void SortVisualizer::startInsertionSort() {
    currIndex = 0; // move to startSort()
    stepsVector.clear(); // move to startSort()

    if (isIncreasingSort) {
        insertionSortIncreasing();
    } else {
        insertionSortDecreasing();
    }

    connectTimers();
}

void SortVisualizer::startMergeSort() {
    currIndex = 0; // move to startSort()
    stepsVector.clear(); // move to startSort()

    if (isIncreasingSort) {
        mergeSortIncreasing(0, numberOfElements-1);
    } else {
        mergeSortDecreasing(0, numberOfElements-1);
    }

    //qDebug() << sortArray;
    connectTimers();
}

void SortVisualizer::startQuickSort() {
    currIndex = 0; // move to startSort()
    stepsVector.clear(); // move to startSort()

    if (isIncreasingSort) {
        quickSortIncreasing(0, numberOfElements-1);
    } else {
        quickSortDecreasing(0, numberOfElements-1);
    }

    //qDebug() << sortArray;
    connectTimers();
}

/**** ALGORITHMS ******/

void SortVisualizer::selectionSortIncreasing() {
    for (int i = 0; i < numberOfElements; i++) {
        for (int j = i+1; j < numberOfElements; j++) {
            if (sortArray[i] > sortArray[j]) {
                stepsVector.append(std::make_tuple(SWAP, i, j));
                std::swap(sortArray[i], sortArray[j]);
            } else {
                stepsVector.append(std::make_tuple(HIGHLIGHT, i, j));
            }
        }
    }
}

void SortVisualizer::selectionSortDecreasing() {
    for (int i = 0; i < numberOfElements; i++) {
        for (int j = i+1; j < numberOfElements; j++) {
            if (sortArray[i] < sortArray[j]) {
                stepsVector.append(std::make_tuple(SWAP, i, j));
                std::swap(sortArray[i], sortArray[j]);
            } else {
                stepsVector.append(std::make_tuple(HIGHLIGHT, i, j));
            }
        }
    }
}

void SortVisualizer::bubbleSortIncreasing() {
    for (int i = 0; i < numberOfElements - 1; i++) {
        for (int j = 0; j < numberOfElements - i - 1; j++) {
            if (sortArray[j] > sortArray[j+1]) {
                stepsVector.append(std::make_tuple(SWAP, j, j+1));
                std::swap(sortArray[j], sortArray[j+1]);
            } else {
                stepsVector.append(std::make_tuple(HIGHLIGHT, j, j+1));
            }
        }
    }
}

void SortVisualizer::bubbleSortDecreasing() {
    for (int i = 0; i < numberOfElements - 1; i++) {
        for (int j = 0; j < numberOfElements - i - 1; j++) {
            if (sortArray[j] < sortArray[j+1]) {
                stepsVector.append(std::make_tuple(SWAP, j, j+1));
                std::swap(sortArray[j], sortArray[j+1]);
            } else {
                stepsVector.append(std::make_tuple(HIGHLIGHT, j, j+1));
            }
        }
    }
}

void SortVisualizer::insertionSortIncreasing() {
    for (int i = 1; i < numberOfElements; i++) {
        int key = sortArray[i];
        int j = i - 1;

        while (j >= 0 && sortArray[j] > key) {
            sortArray[j+1] = sortArray[j];
            stepsVector.append(std::make_tuple(SET, j+1, j)); // HIGHLIGHT_SET
            j--;
        }
        sortArray[j+1] = key;
        stepsVector.append(std::make_tuple(SET_HEIGHT, j+1, key)); // SET_VALUE
        //qDebug() << "key " << key;
    }
    qDebug() << stepsVector.size();
}

void SortVisualizer::insertionSortDecreasing() {
    for (int i = 1; i < numberOfElements; i++) {

        int key = sortArray[i];
        int j = i - 1;

        while (j >= 0 && sortArray[j] < key) {
            sortArray[j+1] = sortArray[j];
            stepsVector.append(std::make_tuple(SET, j+1, j));
            j--;
        }
        sortArray[j+1] = key;
        stepsVector.append(std::make_tuple(SET_HEIGHT, j+1, key));
    }
}

void SortVisualizer::mergeSortIncreasing(int begin, int end) {
    stepsVector.append(std::make_tuple(HIGHLIGHT, begin, end));

    if (begin >= end)
        return; // Returns recursivly

    auto mid = begin + (end - begin) / 2;
    mergeSortIncreasing(begin, mid);
    mergeSortIncreasing(mid + 1, end);
    mergeIncreasing(begin, mid, end);
}

void SortVisualizer::mergeSortDecreasing(int begin, int end) {
    stepsVector.append(std::make_tuple(HIGHLIGHT, begin, end));

    if (begin >= end)
        return; // Returns recursivly

    auto mid = begin + (end - begin) / 2;
    mergeSortDecreasing(begin, mid);
    mergeSortDecreasing(mid + 1, end);
    mergeDecreasing(begin, mid, end);
}

void SortVisualizer::mergeIncreasing(int left, int mid, int right) {
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temp arrays
    QVector<int> leftArray(subArrayOne);
    QVector<int> rightArray(subArrayTwo);

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++) {
        leftArray[i] = sortArray[left + i];
        //stepsVector.append(std::make_tuple(HIGHLIGHT_ONE, left+i, left+i));
        stepsVector.append(std::make_tuple(HIGHLIGHT_RANGE, left, left+i));
    }
    for (auto j = 0; j < subArrayTwo; j++) {
        rightArray[j] = sortArray[mid + 1 + j];
        //stepsVector.append(std::make_tuple(HIGHLIGHT_ONE, mid+1+j, mid+1+j));
        stepsVector.append(std::make_tuple(HIGHLIGHT_RANGE, mid+1, mid+1+j));
    }

    auto indexOfSubArrayOne = 0, // Initial index of first sub-array
        indexOfSubArrayTwo = 0; // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            sortArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, leftArray[indexOfSubArrayOne]));
            indexOfSubArrayOne++;
        }
        else {
            sortArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, rightArray[indexOfSubArrayTwo]));
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        sortArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, leftArray[indexOfSubArrayOne]));
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        sortArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, rightArray[indexOfSubArrayTwo]));
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}

void SortVisualizer::mergeDecreasing(int left, int mid, int right) {
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temp arrays
    QVector<int> leftArray(subArrayOne);
    QVector<int> rightArray(subArrayTwo);

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++) {
        leftArray[i] = sortArray[left + i];
        stepsVector.append(std::make_tuple(HIGHLIGHT_ONE, left+i, left+i));
    }
    for (auto j = 0; j < subArrayTwo; j++) {
        rightArray[j] = sortArray[mid + 1 + j];
        stepsVector.append(std::make_tuple(HIGHLIGHT_ONE, mid+1+j, mid+1+j));
    }

    auto indexOfSubArrayOne = 0, // Initial index of first sub-array
        indexOfSubArrayTwo = 0; // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] >= rightArray[indexOfSubArrayTwo]) {  ///
            sortArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, leftArray[indexOfSubArrayOne]));
            indexOfSubArrayOne++;
        }
        else {
            sortArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, rightArray[indexOfSubArrayTwo]));
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        sortArray[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, leftArray[indexOfSubArrayOne]));
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        sortArray[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        stepsVector.append(std::make_tuple(SET_HEIGHT, indexOfMergedArray, rightArray[indexOfSubArrayTwo]));
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}


void SortVisualizer::quickSortIncreasing(int low, int high) {
    if (low < numberOfElements && high < numberOfElements) {
        stepsVector.append(std::make_tuple(HIGHLIGHT, (low < numberOfElements ? low : numberOfElements - 1), (high < numberOfElements ? high : numberOfElements - 1)));
    }
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partitionIncreasing(low, high);

        // Separately sort elements before
        // partition and after partition
        quickSortIncreasing(low, pi - 1);
        quickSortIncreasing(pi + 1, high);
    }
}

void SortVisualizer::quickSortDecreasing(int low, int high) {

}

int SortVisualizer::partitionIncreasing(int low, int high) {
    int pivot = sortArray[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    //qDebug() << low << " " << high << " ASD";

    if (low < numberOfElements && high < numberOfElements) {
        stepsVector.append(std::make_tuple(HIGHLIGHT, low, high));
    }

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (j < numberOfElements && high < numberOfElements) {
            stepsVector.append(std::make_tuple(HIGHLIGHT, j, high));
        }
        //stepsVector.append(std::make_tuple(HIGHLIGHT, j, high));

        if (sortArray[j] <= pivot)
        {
            i++;    // increment index of smaller element
            qSwap(sortArray[i], sortArray[j]);
            if (i < numberOfElements && j < numberOfElements) {
                stepsVector.append(std::make_tuple(SWAP, i, j));
            }

            //qDebug() << i << " " << j << "ij";
        }
    }
    qSwap(sortArray[i + 1], sortArray[high]);
    if (i+1 < numberOfElements && high < numberOfElements) {
        stepsVector.append(std::make_tuple(SWAP, i+1, high));
    }
    //stepsVector.append(std::make_tuple(SWAP, i+1, high));
    //qDebug() << i+1 << " " << high << "swap";
    return (i + 1);
}

int SortVisualizer::partitionDecreasing(int low, int high) {

}

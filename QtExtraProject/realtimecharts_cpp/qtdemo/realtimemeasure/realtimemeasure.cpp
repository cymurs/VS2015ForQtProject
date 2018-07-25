#include <QApplication>
#include <QButtonGroup>
#include <QIcon>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QMouseEvent>
#include "realtimemeasure.h"
#include "chartdir.h"
#include <math.h>
#include <string>
#include <sstream>

using namespace std;


static const int DataInterval = 250;
static const int ChartUpdateInterval = 250;

// The mouse can drag the track line if it is within the GrabDistance to the line.
static const int GrabDistance = 8;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet("* {font-family:arial;font-size:11px}");
    RealTimeMeasure demo;
    demo.show();
    return app.exec();
}


RealTimeMeasure::RealTimeMeasure(QWidget *parent) :
    QDialog(parent)
{
    //
    // Set up the GUI
    //

    setFixedSize(772, 380);
    setWindowTitle("Real-Time Chart with Measurement Cursors");

    QFrame *frame = new QFrame(this);
    frame->setGeometry(4, 4, 120, 372);
    frame->setFrameShape(QFrame::StyledPanel);

    // Pointer push button
    QPushButton *pointerPB = new QPushButton(QIcon(":/pointer.png"), "Pointer", frame);
    pointerPB->setGeometry(4, 8, 112, 28);
    pointerPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
    pointerPB->setCheckable(true);

    // Zoom In push button
    QPushButton *zoomInPB = new QPushButton(QIcon(":/zoomin.png"), "Zoom In", frame);
    zoomInPB->setGeometry(4, 36, 112, 28);
    zoomInPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
    zoomInPB->setCheckable(true);

    // Zoom Out push button
    QPushButton *zoomOutPB = new QPushButton(QIcon(":/zoomout.png"), "Zoom Out", frame);
    zoomOutPB->setGeometry(4, 64, 112, 28);
    zoomOutPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
    zoomOutPB->setCheckable(true);

    // Save push button
    QPushButton *savePB = new QPushButton(QIcon(":/save.png"), "Save", frame);
    savePB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
    savePB->setGeometry(4, 120, 112, 28);
    connect(savePB, SIGNAL(clicked(bool)), SLOT(onSave(bool)));

    // The Pointer/Zoom In/Zoom Out buttons form a button group
    QButtonGroup *mouseUsage = new QButtonGroup(frame);
    mouseUsage->addButton(pointerPB, Chart::MouseUsageScroll);
    mouseUsage->addButton(zoomInPB, Chart::MouseUsageZoomIn);
    mouseUsage->addButton(zoomOutPB, Chart::MouseUsageZoomOut);
    connect(mouseUsage, SIGNAL(buttonPressed(int)), SLOT(onMouseUsageChanged(int)));

    m_TrackLine1Enable = new QCheckBox("Track Line 1", frame);
    m_TrackLine1Enable->setGeometry(8, 180, 108, 20);
    m_TrackLine1Enable->setChecked(true);
    connect(m_TrackLine1Enable, SIGNAL(clicked()), SLOT(onTrackLineCheckBox()));

    m_TrackLine2Enable = new QCheckBox("Track Line 2", frame);
    m_TrackLine2Enable->setGeometry(8, 204, 108, 20);
    m_TrackLine2Enable->setChecked(true);
    connect(m_TrackLine2Enable, SIGNAL(clicked()), SLOT(onTrackLineCheckBox()));

    // Chart Viewer
    m_ChartViewer = new QChartViewer(this);
    m_ChartViewer->setGeometry(128, 4, 640, 350);
    connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(m_ChartViewer, SIGNAL(mouseMoveChart(QMouseEvent*)),
        SLOT(onMouseMoveChart(QMouseEvent*)));

    // Horizontal scroll bar
    m_HScrollBar = new QScrollBar(Qt::Horizontal, this);
    m_HScrollBar->setGeometry(128, 358, 640, 17);
    connect(m_HScrollBar, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));

    // Clear data arrays to Chart::NoValue
    for (int i = 0; i < sampleSize; ++i)
        m_timeStamps[i] = m_dataSeriesA[i] = m_dataSeriesB[i] = Chart::NoValue;
    m_currentIndex = 0;
    m_firstChartTime = m_chartTimeLimit = Chart::NoValue;

    // Variables to keep track of the mouse for dragging track lines
    m_nearestTrackLine = -1;
    m_nearestDistance = 0;

    // Set m_nextDataTime to the current time. It is used by the real time random number
    // generator so it knows what timestamp should be used for the next data point.
    m_nextDataTime = QDateTime::currentDateTime();

    // Initially set the mouse to drag to scroll mode.
    pointerPB->click();

    // Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
    m_ChartViewer->setMouseWheelZoomRatio(1.1);

    // Set up the data acquisition mechanism. In this demo, we just use a timer to get a
    // sample every 250ms.
    QTimer *dataRateTimer = new QTimer(this);
    connect(dataRateTimer, SIGNAL(timeout()), SLOT(onDataTimer()));
    dataRateTimer->start(DataInterval);

    // Set up the chart update timer
    m_ChartUpdateTimer = new QTimer(this);
    connect(m_ChartUpdateTimer, SIGNAL(timeout()), SLOT(onChartUpdateTimer()));
    m_ChartUpdateTimer->start(ChartUpdateInterval);
}

RealTimeMeasure::~RealTimeMeasure()
{
    delete m_ChartViewer->getChart();   
}

//
// The Pointer, Zoom In or Zoom out button is pressed
//
void RealTimeMeasure::onMouseUsageChanged(int mouseUsage)
{
    m_ChartViewer->setMouseUsage(mouseUsage);
}

//
// The Save button is pressed
//
void RealTimeMeasure::onSave(bool)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save", "chartdirector_demo",
        "PNG (*.png);;JPG (*.jpg);;GIF (*.gif);;BMP (*.bmp);;SVG (*.svg);;PDF (*.pdf)");

    if (!fileName.isEmpty())
    {
        // Save the chart
        BaseChart *c = m_ChartViewer->getChart();
        if (0 != c)
            c->makeChart(fileName.toUtf8().constData());
    }
}

//
// User changes the track line checkbox
//
void RealTimeMeasure::onTrackLineCheckBox()
{
    // Simply redraw the track lines
    trackLineLabel((XYChart*)m_ChartViewer->getChart());
    m_ChartViewer->updateDisplay();
}

//
// The data acquisition routine. In this demo, this is invoked every 250ms.
//
void RealTimeMeasure::onDataTimer()
{
    // The current time
    QDateTime now = QDateTime::currentDateTime();

    // This is our formula for the random number generator
    do
    {
        // We need the currentTime in millisecond resolution
        double currentTime = Chart::chartTime2(m_nextDataTime.toTime_t())
                             + m_nextDataTime.time().msec() / 250 * 0.25;

        // Get a data sample
        double p = currentTime * 4;
        double dataA = 150 + 100 * sin(p / 27.7) * sin(p / 10.1);
        double dataB = 150 + 100 * cos(p / 6.7) * cos(p / 11.9);

        // In this demo, if the data arrays are full, the oldest 5% of data are discarded.
        if (m_currentIndex >= sampleSize)
        {
            m_currentIndex = sampleSize * 95 / 100 - 1;

            for(int i = 0; i < m_currentIndex; ++i)
            {
                int srcIndex = i + sampleSize - m_currentIndex;
                m_timeStamps[i] = m_timeStamps[srcIndex];
                m_dataSeriesA[i] = m_dataSeriesA[srcIndex];
                m_dataSeriesB[i] = m_dataSeriesB[srcIndex];
            }
        }

        // Remember the first timestamps to compute the elapsed time
        if (m_firstChartTime == Chart::NoValue)
            m_firstChartTime = currentTime;

        // Store the new values in the current index position, and increment the index.
        m_timeStamps[m_currentIndex] = currentTime - m_firstChartTime;
        m_dataSeriesA[m_currentIndex] = dataA;
        m_dataSeriesB[m_currentIndex] = dataB;
        ++m_currentIndex;

        m_nextDataTime = m_nextDataTime.addMSecs(DataInterval);
    }
    while (m_nextDataTime < now);
}

//
// Update the chart and the viewport periodically
//
void RealTimeMeasure::onChartUpdateTimer()
{
    if (m_currentIndex > 0)
    {
        //
        // As we added more data, we may need to update the full range of the viewport.
        //

        double startDate = m_timeStamps[0];
        double endDate = m_timeStamps[m_currentIndex - 1];

        // Use the initialFullRange (which is 60 seconds in this demo) if this is sufficient.
        double duration = endDate - startDate;
        if (duration < initialFullRange)
            endDate = startDate + initialFullRange;

        // Update the full range to reflect the actual duration of the data. In this case,
        // if the view port is viewing the latest data, we will scroll the view port as new
        // data are added. If the view port is viewing historical data, we would keep the
        // axis scale unchanged to keep the chart stable.
        int updateType = Chart::ScrollWithMax;
        if (m_ChartViewer->getViewPortLeft() + m_ChartViewer->getViewPortWidth() < 0.999)
            updateType = Chart::KeepVisibleRange;
        bool scaleHasChanged = m_ChartViewer->updateFullRangeH("x", startDate, endDate, updateType);

        // Set the zoom in limit as a ratio to the full range
        m_ChartViewer->setZoomInWidthLimit(zoomInLimit / (m_ChartViewer->getValueAtViewPort("x", 1) -
            m_ChartViewer->getValueAtViewPort("x", 0)));

        // Trigger the viewPortChanged event to update the display if the axis scale has changed
        // or if new data are added to the existing axis scale.
        if (scaleHasChanged || (duration < initialFullRange))
            m_ChartViewer->updateViewPort(true, false);
    }
}

//
// View port changed event
//
void RealTimeMeasure::onViewPortChanged()
{
    // In addition to updating the chart, we may also need to update other controls that
    // changes based on the view port.
    updateControls(m_ChartViewer);

    // Update the chart if necessary
    if (m_ChartViewer->needUpdateChart())
        drawChart(m_ChartViewer);
}

//
// User clicks on the the horizontal scroll bar
//
void RealTimeMeasure::onHScrollBarChanged(int value)
{
    if (!m_ChartViewer->isInViewPortChangedEvent())
    {
        // Set the view port based on the scroll bar
        int scrollBarLen = m_HScrollBar->maximum() + m_HScrollBar->pageStep();
        m_ChartViewer->setViewPortLeft(value / (double)scrollBarLen);

        // Update the chart display without updating the image maps. (We can delay updating
        // the image map until scrolling is completed and the chart display is stable.)
        m_ChartViewer->updateViewPort(true, false);
    }
}

//
// Update controls in the user interface when the view port changed
//
void RealTimeMeasure::updateControls(QChartViewer *viewer)
{
    // The logical length of the scrollbar. It can be any large value. The actual value does
    // not matter.
    const int scrollBarLen = 1000000000;

    // Update the horizontal scroll bar
    m_HScrollBar->setEnabled(viewer->getViewPortWidth() < 1);
    m_HScrollBar->setPageStep((int)ceil(viewer->getViewPortWidth() * scrollBarLen));
    m_HScrollBar->setSingleStep(min(scrollBarLen / 100, m_HScrollBar->pageStep()));
    m_HScrollBar->setRange(0, scrollBarLen - m_HScrollBar->pageStep());
    m_HScrollBar->setValue((int)(0.5 + viewer->getViewPortLeft() * scrollBarLen));
}

//
// Draw chart
//
void RealTimeMeasure::drawChart(QChartViewer *viewer)
{
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
    double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft() +
        viewer->getViewPortWidth());

    // Extract the part of the data arrays that are visible.
    DoubleArray viewPortTimeStamps;
    DoubleArray viewPortDataSeriesA;
    DoubleArray viewPortDataSeriesB;

    if (m_currentIndex > 0)
    {
        // Get the array indexes that corresponds to the visible start and end dates
        int startIndex = (int)floor(Chart::bSearch(DoubleArray(m_timeStamps, m_currentIndex), viewPortStartDate));
        int endIndex = (int)ceil(Chart::bSearch(DoubleArray(m_timeStamps, m_currentIndex), viewPortEndDate));
        int noOfPoints = endIndex - startIndex + 1;

        // Extract the visible data
        viewPortTimeStamps = DoubleArray(m_timeStamps + startIndex, noOfPoints);
        viewPortDataSeriesA = DoubleArray(m_dataSeriesA + startIndex, noOfPoints);
        viewPortDataSeriesB = DoubleArray(m_dataSeriesB + startIndex, noOfPoints);
        m_chartTimeLimit = m_timeStamps[m_currentIndex - 1];
    }

    //
    // At this stage, we have extracted the visible data. We can use those data to plot the chart.
    //

    //================================================================================
    // Configure overall chart appearance.
    //================================================================================

    // Create an XYChart object of size 640 x 350 pixels
    XYChart *c = new XYChart(640, 350);

    // Set the position, size and colors of the plot area
    c->setPlotArea(23, 33, c->getWidth() - 41, c->getHeight() - 53, c->linearGradientColor(0, 33, 0,
        c->getHeight() - 53, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

    // As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
    c->setClipping();

    // Add a title to the chart using 18pt Arial font
    c->addTitle("Real-Time Chart with Measurement Cursors", "arial.ttf", 18);

    // Add a legend box at (60, 28) using horizontal layout. Use 10pt Arial Bold as font. Set the
    // background and border color to transparent and use line style legend key.
    LegendBox *b = c->addLegend(60, 28, false, "arialbd.ttf", 10);
    b->setBackground(Chart::Transparent);
    b->setLineStyleKey();

    // Set the x and y axis stems to transparent and the label font to 10pt Arial
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    c->xAxis()->setLabelStyle("arial.ttf", 10);
    c->yAxis()->setLabelStyle("arialbd.ttf", 10, 0x336699);

    // Set the y-axis tick length to 0 to disable the tick and put the labels closer to the axis.
    c->yAxis()->setLabelGap(-1);
    c->yAxis()->setLabelAlignment(1);
    c->yAxis()->setTickLength(0);
    c->yAxis()->setMargin(20);

    // Add axis title using 12pt Arial Bold Italic font
    c->yAxis()->setTitle("Ionic Temperature (C)", "arialbd.ttf", 12);

    // Configure the x-axis tick length to 1 to put the labels closer to the axis.
    c->xAxis()->setTickLength(1);

    //================================================================================
    // Add data to chart
    //================================================================================

    //
    // In this example, we represent the data by lines. You may modify the code below to use other
    // representations (areas, scatter plot, etc).
    //

    // Add a line layer for the lines, using a line width of 2 pixels
    LineLayer *layer = c->addLineLayer();
    layer->setLineWidth(2);
    layer->setFastLineMode();

    // Now we add the 3 data series to a line layer, using the color red (ff0000), green (00cc00)
    // and blue (0000ff)
    layer->setXData(viewPortTimeStamps);
    layer->addDataSet(viewPortDataSeriesA, 0x00cc00, "Alpha");
    layer->addDataSet(viewPortDataSeriesB, 0x0000ff, "Beta");

    //================================================================================
    // Configure axis scale and labelling
    //================================================================================

    // Set the x-axis as a date/time axis with the scale according to the view port x range.
    if (m_currentIndex > 0)
        c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

    // For the automatic axis labels, set the minimum spacing to 75/30 pixels for the x/y axis.
    c->xAxis()->setTickDensity(75);
    c->yAxis()->setTickDensity(30);

    // We use "hh:nn:ss" as the axis label format.
    c->xAxis()->setLabelFormat("{value|nn:ss}");

    // We make sure the tick increment must be at least 1 second.
    c->xAxis()->setMinTickInc(1);

    //================================================================================
    // Output the chart
    //================================================================================

    // We need to update the track line too.
    trackLineLabel(c);

    // Set the chart image to the WinChartViewer
    delete viewer->getChart();
    viewer->setChart(c);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void RealTimeMeasure::onMouseMoveChart(QMouseEvent *e)
{
    // Mouse can drag the track lines if it is in scroll mode
    int mouseUsage = m_ChartViewer->getMouseUsage();
    if (((mouseUsage != Chart::MouseUsageScroll) && (mouseUsage != Chart::MouseUsageDefault))
        || (m_trackLinePos.size() == 0))
        return;

    int mouseX = m_ChartViewer->getChartMouseX();

    // Check if mouse button is down
    if ((Qt::LeftButton & e->buttons()) != 0)
    {
        // If mouse is near track line, then it is dragging the track line
        if (m_nearestTrackLine >= 0)
        {
            XYChart *c = (XYChart *)m_ChartViewer->getChart();
            PlotArea *p = c->getPlotArea();

            // move the track line while ensuring the track line is in the plot area
            m_trackLinePos[m_nearestTrackLine] =
                (std::min)(p->getRightX(), (std::max)(p->getLeftX(), mouseX - m_nearestDistance));

            // repaint the track lines
            trackLineLabel(c);
            m_ChartViewer->updateDisplay();
        }
    }
    else
    {
        // Check which track line is nearest to the mouse
        m_nearestTrackLine = -1;
        m_nearestDistance = GrabDistance + 1;
        for (int i = 0; i < (int)m_trackLinePos.size(); ++i)
        {
            if (abs(mouseX - m_trackLinePos[i]) < abs(m_nearestDistance))
            {
                m_nearestTrackLine = i;
                m_nearestDistance = mouseX - m_trackLinePos[i];
            }
        }

        // If mouse is near the track line, it is used to drag the line, so disable drag to scroll.
        m_ChartViewer->setMouseUsage((m_nearestTrackLine >= 0) ? Chart::MouseUsageDefault :
            Chart::MouseUsageScroll);
    }
}

//
// Draw the track line with data point labels
//
void RealTimeMeasure::trackLineLabel(XYChart *c)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    c->initDynamicLayer();

    // In this example, we have two track lines.
    const int trackLineCount = 2;

    if (m_trackLinePos.size() == 0)
    {
        // Initialize the track line position by distributing them on the plot area
        PlotArea *p = c->getPlotArea();
        for (int i = 0; i < trackLineCount; ++i)
            m_trackLinePos.push_back(p->getLeftX() + (int)(p->getWidth() * (i + 0.5) / trackLineCount));
    }

    // Record the positions with the track lines
    std::map<std::string, double> trackLineLog[trackLineCount];

    // Draw the track lines if enabled
    if (m_TrackLine1Enable->isChecked())
        drawTrackLine(c, m_trackLinePos[0], trackLineLog[0]);
    if (m_TrackLine2Enable->isChecked())
        drawTrackLine(c, m_trackLinePos[1], trackLineLog[1]);

    // Draw the differences beteween the first two track lines
    drawTrackDiff(c, trackLineLog[0], trackLineLog[1]);
}

void RealTimeMeasure::drawTrackLine(XYChart *c, int lineX, std::map<std::string, double> &log)
{
    // The drawarea and plotarea objects
    DrawArea *d = c->getDrawArea();
    PlotArea *plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the track line, and find its pixel coordinate.
    double xValue = c->getNearestXValue(lineX);
    int xCoor = c->getXCoor(xValue);

    // Draw empty track line if it is ahead of the data
    if ((m_currentIndex <= 0) || ((xCoor < lineX) && (xValue >= m_chartTimeLimit)))
    {
        d->vline(plotArea->getTopY(), plotArea->getBottomY(), lineX, 0x888888);
        return;
    }

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, 0x888888);

    // Draw a label on the x-axis to show the track line position.
    ostringstream xlabel;
    xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue, "nn:ss.ff")
        << " <*/font*>";
    TTFText *t = d->text(xlabel.str().c_str(), "arialbd.ttf", 10);
    log["x"] = xValue;

    // Restrict the x-pixel position of the label to make sure it stays inside the chart image.
    int xLabelPos = max(0, min(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
    t->draw(xLabelPos, plotArea->getBottomY() + 3, 0xffffff);
    t->destroy();

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i)
    {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j)
        {
            DataSet *dataSet = layer->getDataSetByZ(j);
            const char *dataSetName = dataSet->getDataName();

            // Get the color, name and position of the data label
            int color = dataSet->getDataColor();
            int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

            // Draw a track dot with a label next to it for visible data points in the plot area
            if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color !=
                Chart::Transparent) && dataSetName && *dataSetName)
            {
                d->circle(xCoor, yCoor, 4, 4, color, color);

                ostringstream label;
                label << "<*font,bgColor=" << hex << color << "*> "
                    << c->formatValue(dataSet->getValue(xIndex), "{value|P4}") << " <*font*>";
                t = d->text(label.str().c_str(), "arialbd.ttf", 10);
                log[dataSetName] = dataSet->getValue(xIndex);

                // Draw the label on the right side of the dot if the mouse is on the left side the
                // chart, and vice versa. This ensures the label will not go outside the chart image.
                if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
                    t->draw(xCoor + 6, yCoor, 0xffffff, Chart::Left);
                else
                    t->draw(xCoor - 6, yCoor, 0xffffff, Chart::Right);

                t->destroy();
            }
        }
    }
}

//
// Draw the differences between the track lines
//
void RealTimeMeasure::drawTrackDiff(XYChart *c, std::map<std::string, double> &log0, std::map<std::string, double> &log1)
{
    std::map<std::string, double>::iterator x0 = log0.find("x");
    std::map<std::string, double>::iterator x1 = log1.find("x");
    if ((x0 == log0.end()) || (x1 == log1.end()))
        return;  // Not all track lines have data

    // Two columns in the table
    ostringstream leftCol;
    ostringstream rightCol;

    leftCol << "Change in x: ";
    rightCol << c->formatValue(x1->second - x0->second, "{value|2}");

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i)
    {
        Layer *layer = c->getLayerByZ(i);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j)
        {
            const char *dataSetName = layer->getDataSet(j)->getDataName();
            std::map<std::string, double>::iterator i0 = log0.find(dataSetName);
            std::map<std::string, double>::iterator i1 = log1.find(dataSetName);
            if ((i0 == log0.end()) || (i1 == log1.end()))
                continue;
            leftCol << "\nChange in " << dataSetName << ": ";
            rightCol << "\n" << c->formatValue(i1->second - i0->second, "{value|2}");
        }
    }

    ostringstream table;
    table << "<*block,bgColor=80ffffff,margin=4*><*block*>" << leftCol.str()
        << "<*/*><*block,halign=right*>" << rightCol.str() << "<*/*><*/*>";

    TTFText *t = c->getDrawArea()->text(table.str().c_str(), "arial.ttf", 10);
    t->draw(c->getPlotArea()->getRightX() - t->getWidth(), c->getPlotArea()->getTopY(), 0x000000);
    t->destroy();
}


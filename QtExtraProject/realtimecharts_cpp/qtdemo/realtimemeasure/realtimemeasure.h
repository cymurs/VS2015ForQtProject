#ifndef RealTimeMeasure_H
#define RealTimeMeasure_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QScrollBar>
#include <QCheckBox>
#include "qchartviewer.h"
#include <vector>


class RealTimeMeasure : public QDialog {
    Q_OBJECT
public:
    RealTimeMeasure(QWidget *parent = 0);
    ~RealTimeMeasure();

private:
    // The number of samples per data series used in this demo
    static const int sampleSize = 10000;

    // The initial full range is set to 60 seconds of data.
    static const int initialFullRange = 60;

    // The maximum zoom in is 10 seconds.
    static const int zoomInLimit = 10;

    double m_timeStamps[sampleSize];	// The timestamps for the data series
    double m_dataSeriesA[sampleSize];	// The values for the data series A
    double m_dataSeriesB[sampleSize];	// The values for the data series B

    int m_currentIndex;                 // Index of the array position to which new values are added.

    QDateTime m_nextDataTime;           // Used by the random number generator to generate realtime data.

    double m_firstChartTime;            // The first chart time
    double m_chartTimeLimit;            // The last chart time

    QChartViewer *m_ChartViewer;        // QChartViewer control
    QTimer *m_ChartUpdateTimer;         // The chart update timer
    QScrollBar *m_HScrollBar;           // The scroll bar
    QCheckBox *m_TrackLine1Enable;      // Enable track line 1
    QCheckBox *m_TrackLine2Enable;      // Enable track line 2

    // Draw chart
    void drawChart(QChartViewer *viewer);

    // The position of the track lines
    std::vector<int> m_trackLinePos;

    // Draw track lines
    void trackLineLabel(XYChart *c);
    void drawTrackLine(XYChart *c, int i, std::map<std::string, double> &log);
    void drawTrackDiff(XYChart *c, std::map<std::string, double> &log0, std::map<std::string, double> &log1);

    // Keep track of the mouse for dragging the track lines
    int m_nearestTrackLine;
    int m_nearestDistance;

    // Update other controls as viewport changes
    void updateControls(QChartViewer *viewer);

private slots:
    void onMouseUsageChanged(int mouseUsage);       // Pointer/zoom in/zoom out button clicked
    void onSave(bool);                              // Save button clicked
    void onMouseMoveChart(QMouseEvent *event);      // Mouse move on chart
    void onDataTimer();                             // Get new data values
    void onChartUpdateTimer();                      // Update the chart.
    void onViewPortChanged();                       // Viewport has changed
    void onHScrollBarChanged(int value);            // Scrollbar changed
    void onTrackLineCheckBox();
};

#endif // RealTimeMeasure_H

// realtimemeasureDlg.h : header file
//

#pragma once

#include "ChartViewer.h"
#include "afxwin.h"
#include <map>
#include <string>
#include <vector>


// CRealtimemeasureDlg dialog
class CRealtimemeasureDlg : public CDialog
{
// Construction
public:
    CRealtimemeasureDlg(CWnd* pParent = NULL);  // standard constructor
    ~CRealtimemeasureDlg();

// Dialog Data
    enum { IDD = IDD_REALTIMEMEASURE_DIALOG };
    CButton m_PointerPB;
    CChartViewer m_ChartViewer;
    CScrollBar m_HScrollBar;
    CButton m_TrackLine1Enable;
    CButton m_TrackLine2Enable;

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnViewPortChanged();
	afx_msg void OnMouseMoveChart();
    afx_msg void OnPointerPB();
    afx_msg void OnZoomInPB();
    afx_msg void OnZoomOutPB();
    afx_msg void OnSavePB();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnTrackLineChanged();

    DECLARE_MESSAGE_MAP()

    void OnDataRateTimer();
    void OnChartUpdateTimer();

private:

    // The number of samples per data series used in this demo
    static const int sampleSize = 10000;
    
    // The initial full range is set to 60 seconds of data.
    static const int initialFullRange = 60;

    // The maximum zoom in is 10 seconds.
    static const int zoomInLimit = 10;

    double m_timeStamps[sampleSize];    // The timestamps for the data series
    double m_dataSeriesA[sampleSize];   // The values for the data series B
    double m_dataSeriesB[sampleSize];   // The values for the data series C

    // The index of the array position to which new data values are added.
    int m_currentIndex;

    // Used by the random number generator to generate real time data.
    double m_nextDataTime;

    // The first and last chart time
    double m_firstChartTime;
    double m_chartTimeLimit;

    // Draw chart
    void drawChart(CChartViewer *viewer);

    // The position of the track lines
    std::vector<int> m_trackLinePos;

    // Draw the track lines
    void trackLineLabel(XYChart *c);
    void drawTrackLine(XYChart *c, int i, std::map<std::string, double> &log);
    void drawTrackDiff(XYChart *c, std::map<std::string, double> &log0, std::map<std::string, double> &log1);
    
    // Keep track of the mouse for dragging the track lines
    int m_nearestTrackLine;
    int m_nearestDistance;

    // Update controls when the view port changed
    void updateControls(CChartViewer *viewer);
    // Moves the scroll bar when the user clicks on it
    double moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    // utility to load icon resource to a button
    void loadButtonIcon(int buttonId, int iconId, int width, int height);
};

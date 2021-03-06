#include "stdafx.h"
#include "realtimemultithread.h"
#include "realtimemultithreadDlg.h"
#include "randomwalk.h"
#include <math.h>
#include <sstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CRealtimemultithreadDlg dialog

//
// Constructor
//
CRealtimemultithreadDlg::CRealtimemultithreadDlg(CWnd* pParent)
	: CDialog(CRealtimemultithreadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// The random series generator.
	dataSource = new RandomWalk(OnData, this);
}

//
// Destructor
//
CRealtimemultithreadDlg::~CRealtimemultithreadDlg()
{
    delete m_ChartViewer.getChart();
	delete dataSource;
}

void CRealtimemultithreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
	DDX_Control(pDX, IDC_PointerPB, m_PointerPB);
	DDX_Control(pDX, IDC_HScrollBar, m_HScrollBar);
}

BEGIN_MESSAGE_MAP(CRealtimemultithreadDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, OnViewPortChanged)
    ON_CONTROL(CVN_MouseMovePlotArea, IDC_ChartViewer, OnMouseMovePlotArea)
	ON_BN_CLICKED(IDC_PointerPB, OnPointerPB)
	ON_BN_CLICKED(IDC_ZoomInPB, OnZoomInPB)
	ON_BN_CLICKED(IDC_ZoomOutPB, OnZoomOutPB)
	ON_BN_CLICKED(IDC_SavePB, OnSavePB)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRealtimemultithreadDlg message handlers

//
// Initialization
//
BOOL CRealtimemultithreadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    //
    // Initialize member variables
    //
    m_currentIndex = 0;

	// Initially, auto-move the track line to make it follow the data series
    trackLineEndPos = 0;
    trackLineIsAtEnd = true;

    //
    // Initialize controls
    //

	// Load icons for the Run/Freeze buttons
    loadButtonIcon(IDC_PointerPB, IDI_PointerPB, 100, 20);  
    loadButtonIcon(IDC_ZoomInPB, IDI_ZoomInPB, 100, 20);    
    loadButtonIcon(IDC_ZoomOutPB, IDI_ZoomOutPB, 100, 20);
    loadButtonIcon(IDC_SavePB, IDI_SavePB, 100, 20);

	// Initially set the mouse to drag to scroll mode.
    m_PointerPB.SetCheck(1);
    m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
	
	// Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
	m_ChartViewer.setMouseWheelZoomRatio(1.1);

    // Start the random data generator
	dataSource->start();

    // The chart update rate is set to 100ms
	SetTimer(ChartUpdateTimer, 100, 0);

    return TRUE;
}

// *** code automatically generated by VC++ MFC AppWizard ***
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CRealtimemultithreadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// *** code automatically generated by VC++ MFC AppWizard ***
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRealtimemultithreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//
// User clicks on the Pointer pushbutton
//
void CRealtimemultithreadDlg::OnPointerPB()
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);   
}

//
// User clicks on the Zoom In pushbutton
//
void CRealtimemultithreadDlg::OnZoomInPB()
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomIn);   
}

//
// User clicks on the Zoom Out pushbutton
//
void CRealtimemultithreadDlg::OnZoomOutPB()
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomOut);   
}

//
// User clicks on the Save pushbutton
//
void CRealtimemultithreadDlg::OnSavePB()
{
	// Supported formats = PNG, JPG, GIF, BMP, SVG and PDF
	TCHAR szFilters[]= _T("PNG (*.png)|*.png|JPG (*.jpg)|*.jpg|GIF (*.gif)|*.gif|")
		_T("BMP (*.bmp)|*.bmp|SVG (*.svg)|*.svg|PDF (*.pdf)|*.pdf||");

	// The standard CFileDialog
    CFileDialog fileDlg(FALSE, _T("png"), _T("chartdirector_demo"), OFN_HIDEREADONLY | 
		OFN_OVERWRITEPROMPT, szFilters);
	if(fileDlg.DoModal() != IDOK)
		return;

	// Save the chart
	CString path = fileDlg.GetPathName();
	BaseChart *c = m_ChartViewer.getChart();
	if (0 != c)
		c->makeChart(TCHARtoUTF8(path));
}

//
// User clicks on the the horizontal scroll bar 
//
void CRealtimemultithreadDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Update the view port if the scroll bar has really moved
	double newViewPortLeft = moveScrollBar(nSBCode, nPos, pScrollBar);
	if (newViewPortLeft != m_ChartViewer.getViewPortLeft()) 
	{
		m_ChartViewer.setViewPortLeft(newViewPortLeft);
		m_ChartViewer.updateViewPort(true, false);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//
// Handles timer events
//
void CRealtimemultithreadDlg::OnTimer(UINT_PTR nIDEvent) 
{
	// The timer event for updating the chart
    if (nIDEvent == ChartUpdateTimer)
        OnChartUpdateTimer();

    CDialog::OnTimer(nIDEvent);
}

//
// View port changed event
//
void CRealtimemultithreadDlg::OnViewPortChanged()
{
    // In addition to updating the chart, we may also need to update other controls that
    // changes based on the view port.
	updateControls(&m_ChartViewer);

	// Update the chart if necessary
    if (m_ChartViewer.needUpdateChart())
		drawChart(&m_ChartViewer);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void CRealtimemultithreadDlg::OnMouseMovePlotArea()
{
    double trackLinePos = trackLineLabel((XYChart *)m_ChartViewer.getChart(), 
        m_ChartViewer.getPlotAreaMouseX()); 
    trackLineIsAtEnd = (m_currentIndex <= 0) || (trackLinePos == trackLineEndPos);
    m_ChartViewer.updateDisplay();
}


/////////////////////////////////////////////////////////////////////////////
// CRealtimemultithreadDlg methods

//
// Handles realtime data from RandomWalk. The RandomWalk will call this method from its own thread.
// This is a static method.
//
void CRealtimemultithreadDlg::OnData(void *self, double elapsedTime, double series0, double series1)
{
	// Copy the data into a structure and store it in the queue.
	DataPacket packet;
	packet.elapsedTime = elapsedTime;
	packet.series0 = series0;
	packet.series1 = series1;

	((CRealtimemultithreadDlg *)self)->buffer.put(packet);
}

//
// Get data from the queue, update the viewport and update the chart display if necessary.
//
void CRealtimemultithreadDlg::OnChartUpdateTimer()
{
    CChartViewer *viewer = &m_ChartViewer;

    // Enables auto scroll if the viewport is showing the latest data before the update
    bool autoScroll = (m_currentIndex > 0) && (0.001 + viewer->getValueAtViewPort("x", 
        viewer->getViewPortLeft() + viewer->getViewPortWidth()) >= m_timeStamps[m_currentIndex - 1]);
    
    //
	// Get new data from the queue and append them to the data arrays
	//
	int count;
	DataPacket *packets;
	if ((count = buffer.get(&packets)) <= 0)
		return;

	// if data arrays have insufficient space, we need to remove some old data.
	if (m_currentIndex + count >= sampleSize)
	{
		// For safety, we check if the queue contains too much data than the entire data arrays. If
		// this is the case, we only use the latest data to completely fill the data arrays.
		if (count > sampleSize)
		{
			packets += count - sampleSize;
			count = sampleSize;
		}

		// Remove oldest data to leave space for new data. To avoid frequent removal, we ensure at
        // least 5% empty space available after removal. 
        int originalIndex = m_currentIndex;
        m_currentIndex = sampleSize * 95 / 100 - 1;
		if (m_currentIndex > sampleSize - count)
			m_currentIndex = sampleSize - count;

		for (int i = 0; i < m_currentIndex; ++i)
		{
            int srcIndex = i + originalIndex - m_currentIndex;
			m_timeStamps[i] = m_timeStamps[srcIndex];
			m_dataSeriesA[i] = m_dataSeriesA[srcIndex];
			m_dataSeriesB[i] = m_dataSeriesB[srcIndex];
		}
	}

    // Append the data from the queue to the data arrays
	for (int n = 0; n < count; ++n)
	{
		m_timeStamps[m_currentIndex] = packets[n].elapsedTime;
		m_dataSeriesA[m_currentIndex] = packets[n].series0;
		m_dataSeriesB[m_currentIndex] = packets[n].series1;
		++m_currentIndex;
	}

	//
    // As we added more data, we may need to update the full range of the viewport. 
    //

    double startDate = m_timeStamps[0];
    double endDate = m_timeStamps[m_currentIndex - 1];

    // Use the initialFullRange (which is 60 seconds in this demo) if this is sufficient.
    double duration = endDate - startDate;
    if (duration < initialFullRange)
        endDate = startDate + initialFullRange;

    // Update the new full data range to include the latest data
	bool axisScaleHasChanged = viewer->updateFullRangeH("x", startDate, endDate, 
        Chart::KeepVisibleRange);
    
    if (autoScroll)
    {
        // Scroll the viewport if necessary to display the latest data
        double viewPortEndPos = viewer->getViewPortAtValue("x", m_timeStamps[m_currentIndex - 1]);
        if (viewPortEndPos > viewer->getViewPortLeft() + viewer->getViewPortWidth())
        {
            viewer->setViewPortLeft(viewPortEndPos - viewer->getViewPortWidth());
            axisScaleHasChanged = true;
        }
    }
    
    // Set the zoom in limit as a ratio to the full range
    viewer->setZoomInWidthLimit(zoomInLimit / (viewer->getValueAtViewPort("x", 1) - 
        viewer->getValueAtViewPort("x", 0)));

    // Trigger the viewPortChanged event. Updates the chart if the axis scale has changed
    // (scrolling or zooming) or if new data are added to the existing axis scale.
    viewer->updateViewPort(axisScaleHasChanged || (duration < initialFullRange), false);
}

//
// Handle scroll bar events
//
double CRealtimemultithreadDlg::moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    //
    // Get current scroll bar position
    //
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    pScrollBar->GetScrollInfo(&info);

    //
    // Compute new position based on the type of scroll bar events
    //
    int newPos = info.nPos;
    switch (nSBCode)
    {
    case SB_LEFT:
        newPos = info.nMin;
        break;
    case SB_RIGHT:
        newPos = info.nMax;
        break;
    case SB_LINELEFT:
        newPos -= (info.nPage > 10) ? info.nPage / 10 : 1;
        break;
    case SB_LINERIGHT:
        newPos += (info.nPage > 10) ? info.nPage / 10 : 1;
        break;
    case SB_PAGELEFT:
        newPos -= info.nPage;
        break;
    case SB_PAGERIGHT:
        newPos += info.nPage;
        break;
    case SB_THUMBTRACK:
        newPos = info.nTrackPos;
        break;
    }
    if (newPos < info.nMin) newPos = info.nMin;
    if (newPos > info.nMax) newPos = info.nMax;
    
    // Update the scroll bar with the new position
    pScrollBar->SetScrollPos(newPos);

    // Returns the position of the scroll bar as a ratio of its total length
    return ((double)(newPos - info.nMin)) / (info.nMax - info.nMin);
}

//
// Update controls when the view port changed
//
void CRealtimemultithreadDlg::updateControls(CChartViewer *viewer)
{
    // Update the scroll bar to reflect the view port position and width of the view port.
    m_HScrollBar.EnableWindow(viewer->getViewPortWidth() < 1);
    if (viewer->getViewPortWidth() < 1)
    {
        SCROLLINFO info;
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_ALL;
        info.nMin = 0;
        info.nMax = 0x1fffffff;
	    info.nPage = (int)ceil(viewer->getViewPortWidth() * (info.nMax - info.nMin));
        info.nPos = (int)(0.5 + viewer->getViewPortLeft() * (info.nMax - info.nMin)) + info.nMin;
        m_HScrollBar.SetScrollInfo(&info);
    }
}

//
// Draw the chart and display it in the given viewer
//
void CRealtimemultithreadDlg::drawChart(CChartViewer *viewer)
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
		viewPortTimeStamps = DoubleArray(m_timeStamps+ startIndex, noOfPoints);
		viewPortDataSeriesA = DoubleArray(m_dataSeriesA + startIndex, noOfPoints);
		viewPortDataSeriesB = DoubleArray(m_dataSeriesB + startIndex, noOfPoints);

        // Keep track of the latest available data at chart plotting time
        trackLineEndPos = m_timeStamps[m_currentIndex - 1];
    }

    //
    // At this stage, we have extracted the visible data. We can use those data to plot the chart.
    //

    //================================================================================
    // Configure overall chart appearance.
    //================================================================================

    // Create an XYChart object of size 640 x 350 pixels
    XYChart *c = new XYChart(640, 350);

    // Set the plotarea at (55, 50) with width 80 pixels less than chart width, and height 80 pixels
    // less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
    // as background. Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(55, 50, c->getWidth() - 85, c->getHeight() - 80, c->linearGradientColor(0, 50, 0,
        c->getHeight() - 35, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

    // As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
    c->setClipping();

    // Add a title to the chart using 18pt Arial font
    c->addTitle("   Multithreading Real-Time Chart", "arial.ttf", 18);

    // Add a legend box at (55, 25) using horizontal layout. Use 10pt Arial Bold as font. Set the
    // background and border color to transparent and use line style legend key.
    LegendBox *b = c->addLegend(55, 25, false, "arialbd.ttf", 10);
    b->setBackground(Chart::Transparent);
    b->setLineStyleKey();

    // Set the x and y axis stems to transparent and the label font to 10pt Arial
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("arial.ttf", 10);
	c->yAxis()->setLabelStyle("arial.ttf", 10);
	
	// Set the y-axis tick length to 0 to disable the tick and put the labels closer to the axis.
	c->yAxis()->setTickLength(0);

    // Add axis title using 12pt Arial Bold Italic font
    c->yAxis()->setTitle("Ionic Temperature (C)", "arialbd.ttf", 12);

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

    // Now we add the 2 data series to the line layer with red (ff0000) and green (00cc00) colors
    layer->setXData(viewPortTimeStamps);
    layer->addDataSet(viewPortDataSeriesA, 0xff0000, "Alpha");
    layer->addDataSet(viewPortDataSeriesB, 0x00cc00, "Beta");

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
    c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

    // We make sure the tick increment must be at least 1 second.
    c->xAxis()->setMinTickInc(1);

    // Set the auto-scale margin to 0.05, and the zero affinity to 0.6
    c->yAxis()->setAutoScale(0.05, 0.05, 0.6);

    //================================================================================
    // Output the chart
    //================================================================================

	// We need to update the track line too. If the mouse is moving on the chart (eg. if 
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if (!viewer->isInMouseMoveEvent())
    	trackLineLabel(c, trackLineIsAtEnd ? c->getWidth() : viewer->getPlotAreaMouseX());

	// Set the chart image to the CChartViewer
	delete viewer->getChart();
    viewer->setChart(c);
}

//
// Draw track line with data labels
//
double CRealtimemultithreadDlg::trackLineLabel(XYChart *c, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);
	if (xCoor < plotArea->getLeftX())
		return xValue;

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, 0x888888);

    // Draw a label on the x-axis to show the track line position.
	ostringstream xlabel;
	xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue + 0.00499,
		"hh:nn:ss.ff") << " <*/font*>";
	TTFText *t = d->text(xlabel.str().c_str(), "arialbd.ttf", 10);

    // Restrict the x-pixel position of the label to make sure it stays inside the chart image.
    int xLabelPos = max(0, min(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
	t->draw(xLabelPos, plotArea->getBottomY() + 6, 0xffffff);
	t->destroy();	

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i) {
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

    return xValue;
}


/////////////////////////////////////////////////////////////////////////////
// General utilities

//
// Load an icon resource into a button
//
void CRealtimemultithreadDlg::loadButtonIcon(int buttonId, int iconId, int width, int height)
{
    GetDlgItem(buttonId)->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)::LoadImage(
        AfxGetResourceHandle(), MAKEINTRESOURCE(iconId), IMAGE_ICON, width, height, 
        LR_DEFAULTCOLOR));  
}

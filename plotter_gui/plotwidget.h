#ifndef DragableWidget_H
#define DragableWidget_H

#include <map>
#include <QObject>
#include <QTextEdit>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <deque>
#include <QMessageBox>
#include <QTime>
#include "plotmagnifier.h"
#include "plotzoomer.h"
#include <qwt_plot_panner.h>
#include <QDomDocument>
#include "timeseries_qwt.h"
#include "customtracker.h"
#include "axis_limits_dialog.h"
#include "transforms/transform_selector.h"
#include "transforms/custom_function.h"
#include <qwt_plot_legenditem.h>

class PlotWidget : public QwtPlot
{
    Q_OBJECT

public:

    PlotWidget(PlotDataMapRef& datamap, QWidget *parent=nullptr);

    virtual ~PlotWidget() override;

    bool isEmpty() const;

    const std::map<std::string, QwtPlotCurve*> &curveList() const;

    QDomElement xmlSaveState(QDomDocument &doc) const;

    bool xmlLoadState(QDomElement &element);

    QRectF currentBoundingRect() const;

    PlotData::RangeTime getMaximumRangeX() const;

    PlotData::RangeValue getMaximumRangeY( PlotData::RangeTime range_X, bool absolute_time ) const;

    void setZoomRectangle( QRectF rect, bool emit_signal );

    void reloadPlotData( );

    void changeAxisX(QString curve_name);

    bool isXYPlot() const;

    void changeBackgroundColor(QColor color);

    void setLegendSize(int size);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

signals:
    void swapWidgetsRequested(PlotWidget* source, PlotWidget* destination);
    void rectChanged(PlotWidget* self, QRectF rect );
    void undoableChange();
    void trackerMoved(QPointF pos);
    void curveListChanged();
    void curvesDropped();
    void legendSizeChanged(int new_size);

public slots:

    void replot() override;

    void updateCurves();

    void detachAllCurves();

    void on_panned(int dx, int dy);

    void zoomOut(bool emit_signal);

    void on_zoomOutHorizontal_triggered(bool emit_signal = true);

    void on_zoomOutVertical_triggered(bool emit_signal = true);

    void removeCurve(const std::string &name);

    void activateLegent(bool activate);

    void activateGrid(bool activate);

    void configureTracker(CurveTracker::Parameter val);

    void enableTracker(bool enable);

    void setTrackerPosition(double abs_time);

    void on_changeTimeOffset(double offset);

private slots:

    void on_changeToBuiltinTransforms(QString new_transform);

    void on_convertToXY_triggered(bool checked);

    void on_customTransformsDialog();

    void on_savePlotToFile();

    void on_editAxisLimits_triggered();

private slots:
    void launchRemoveCurveDialog();

    void canvasContextMenuTriggered(const QPoint &pos);

    void on_changeColorsDialog_triggered();

    void on_changeColor(QString curve_name, QColor new_color);

    void on_showPoints_triggered(bool checked);

    void on_externallyResized(const QRectF &new_rect);

private:

    std::map<std::string, QwtPlotCurve* > _curve_list;
    std::map<std::string, QwtPlotMarker*> _point_marker;

    QAction *_action_removeCurve;
    QAction *_action_removeAllCurves;
    QAction *_action_changeColorsDialog;
    QAction *_action_showPoints;
    QAction *_action_zoomOutHorizontally;
    QAction *_action_zoomOutVertically;
    QAction *_action_noTransform;
    QAction *_action_1stDerivativeTransform;
    QAction *_action_2ndDerivativeTransform;
    QAction *_action_phaseXY;
    QAction *_action_custom_transform;
    QAction *_action_saveToFile;
    QAction *_action_editLimits;

    PlotZoomer* _zoomer;
    PlotMagnifier* _magnifier;
    QwtPlotPanner* _panner1;
    QwtPlotPanner* _panner2;

    CurveTracker* _tracker;
    QwtPlotLegendItem* _legend;
    QwtPlotGrid* _grid;

    PlotDataMapRef& _mapped_data;
    QString _default_transform;
    std::map<std::string, QString> _curves_transform;

    struct DragInfo{
        enum{ NONE, CURVES, NEW_X, SWAP_PLOTS} mode;
        std::vector<QString> curves;
        QObject* source;
    };

    DragInfo _dragging;

    bool addCurve(const std::string &name);

    void buildActions();

    void buildLegend();

    void updateAvailableTransformers();

    bool _show_line_and_points;

    void setDefaultRangeX();
    
    DataSeriesBase* createSeriesData(const QString& ID, const PlotData *data);

    double _time_offset;

    const PlotData* _axisX = nullptr;

    PlotData::RangeValue _custom_Y_limits;

    AxisLimitsDialog* _axis_limits_dialog;

    TransformSelector* _transform_select_dialog;

    SnippetsMap _snippets;
    void transformCustomCurves();
};

#endif

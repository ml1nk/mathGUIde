/*-------------------------------------------------------------------+
 |     Copyright (c) 2006-2010 Hartmut Ring, University Siegen       |
 |     http://www.mathgui.de                                         |
 |                                                                   |
 |     This file is part of the mathGUIde package.                   |
 |                                                                   |
 | mathGUIde is free software: you can redistribute it and/or modify |
 | it under the terms of the GNU General Public License as published |
 | by the Free Software Foundation, either version 3 of the License, |
 | or (at your option) any later version.                            |
 |                                                                   |
 | You should have received a copy of the GNU General Public License |
 | along with this program. If not, see http://www.gnu.org/licenses/.|
 +-------------------------------------------------------------------*/

#ifndef __PLOTWINDOW_H__
#define __PLOTWINDOW_H__

#include <qwidget.h>

class PlotWindow : public QWidget {
    Q_OBJECT
public:
    PlotWindow(QWidget* parent=0);

    void beginPlot(double xMin, double xMax, int nValues,
        bool lines=true, bool dots=false, bool bars=false);
    bool addPlotValues(QVector<double> values);
    bool addMarker(QPointF p);
    void endPlot();

public slots:
signals:

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    static void round(double& y, bool up, int digits=2);
    void drawGrid(QPainter& p, int d, int e);
    enum PLOTSTATE {NONE, PLOT, BAR};
    PLOTSTATE   _state;

    double _xMin, _xMax;
    QVector<QVector<double> > _y;
    QVector<QPointF>          _markers;

    double _yMin, _yMax;
    bool   _lines;
    bool   _dots;
    bool   _bars;

    double _scaleX, _scaleY;
    int    _fontH;

    int    _iValue, _nValues, _nFunctions;
    int    _left, _right, _top, _bottom;
};

#endif //__PLOTWINDOW_H__

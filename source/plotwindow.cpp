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

#include "plotwindow.h"

#include <math.h>
#include <float.h>
#include <qpainter.h>
#include <qfontmetrics.h>


PlotWindow::PlotWindow(QWidget* parent/*=0*/)
:   QWidget(parent),
    _state(NONE),
    _nValues(0)
{
    setAutoFillBackground(true);
    setPalette(Qt::white);
}

void PlotWindow::beginPlot(double xMin, double xMax, int nValues,
                           bool lines/*=true*/, bool dots/*=false*/, bool bars/*=false*/) {
    _lines = lines;
    _dots = dots;
    _bars = bars;
    if (nValues != _y.count()) {
        _y.resize(nValues+1);
        _nValues = nValues;
    }
    _markers.clear();
    _iValue = 0;
    _xMin = xMin;
    _xMax = xMax;
    _yMin = DBL_MAX;
    _yMax = DBL_MIN;
}

bool PlotWindow::addPlotValues(QVector<double> values) {
    if (_iValue == 0)
        _nFunctions = values.count();
    if (_iValue > _nValues)
        return false;
    _y[_iValue++] = values;
    foreach (double y, values) {
        if (y < _yMin) _yMin = y;
        if (y > _yMax) _yMax = y;
    }
    return true;
}

bool PlotWindow::addMarker(QPointF p) {
    _markers.append(p);
    if (p.y() < _yMin) _yMin = p.y();
    if (p.y() > _yMax) _yMax = p.y();
    return true;
}

void PlotWindow::endPlot () {
    _state = PLOT;
    // round y interval using 2 significant digits
    round(_yMin, false, 2); // round down
    round(_yMax, true,  2); // round up
    repaint();
}

void PlotWindow::round(double& y, bool up, int digits/*=2*/) {
    if (y != 0) {
        bool neg = y < 0;
        if (neg) y = -y;
        double factor = pow(10.0, floor(log10(y))-digits+1);
        y /= factor;
        if (neg) y = -y;
        y = (up ? ceil(y) : floor(y)) * factor;
    }
}

static double gridBase(int d, double scale) {
    // return min{10**n | scale * 10**n >= d}
    return pow(10.0, ceil(log10(d/scale)));
}

void PlotWindow::drawGrid(QPainter& p, int d, int e) {
    // vertical grid lines
    double dxGrid = e * gridBase(d, _scaleX);
    double xGrid = dxGrid * ceil(_xMin/dxGrid);
    while (xGrid <= _xMax) {
        double x0 = _left + (xGrid-_xMin) * _scaleX;
        //p.drawLine(x0, _top, x0, _bottom);
        p.drawLine(QPointF(x0, _top), QPointF(x0, _bottom));
        xGrid += dxGrid;
    }
    // horizontal grid lines
    double dyGrid = e * gridBase(d, _scaleY);
    double yGrid = dyGrid * floor(_yMin/dyGrid);
    while (yGrid <= _yMax) {
        double y0 = _bottom - (yGrid-_yMin) * _scaleY;
        p.drawLine(QPointF(_left, y0), QPointF(_right, y0));
        yGrid += dyGrid;
    }
}

void PlotWindow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QString sxMin; sxMin.setNum(_xMin);
    QString sxMax; sxMax.setNum(_xMax);
    QString syMin; syMin.setNum(_yMin);
    QString syMax; syMax.setNum(_yMax);

    QFontMetrics fm = p.fontMetrics();
    _fontH = fm.height();
    _left = fm.width(syMin);
    if (fm.width(syMax) > _left)
        _left = fm.width(syMax);
    _left += 4;

    _top  =  8;
    _right =  width() - 8;
    _bottom = height() - _fontH;

    _scaleX = (_right - _left) / (_xMax - _xMin);
    _scaleY = (_bottom - _top) / (_yMax - _yMin);

    int w = (_right-_left)/2;

    switch (_state) {
    case PLOT: {
        double dx = (double)(_right-_left)/(_nValues-1);
        // grid lines
        p.setPen(QColor(240,240,240));  drawGrid(p, 4, 1);
        p.setPen(QColor(224,224,224));  drawGrid(p, 4, 5);
        p.setPen(QColor(192,192,192));  drawGrid(p, 4, 10);

        // coordinate axes
        double y0 = _bottom + _yMin * _scaleY;
        p.setPen(Qt::blue);
        if (_yMin <= 0.0 && _yMax >= 0.0) {
            double y0 = _bottom + _yMin * _scaleY;
            p.drawLine(QPointF(0, y0), QPointF(width(), y0));
        }
        if (_xMin <= 0.0 && _xMax >= 0.0) {
            double x0 = _left  -_xMin * _scaleX;
            p.drawLine(QPointF(x0, 0), QPointF(x0, height()));
        }
#ifdef RAHMEN_ZEICHNEN
        p.setPen(Qt::green);
        p.drawRect(_left, _top, _right-_left, _bottom-_top);
#endif

        // labels
        p.setPen(Qt::black);

        p.drawText(0, _top,           _left-4, _fontH,  Qt::AlignRight, syMax);
        p.drawText(0, _bottom-_fontH, _left-4, _fontH,  Qt::AlignRight, syMin);

        p.drawText(_left+2,    _bottom, w-2,   _fontH,  Qt::AlignLeft,  sxMin);
        p.drawText(_right-w-2, _bottom, w-2,   _fontH,  Qt::AlignRight, sxMax);

        // plot
        if (_lines) {
            QList<QColor> clr;
            clr << QColor(0,32,128) << QColor(192,0,0) << QColor(0,128,32) << QColor(96,96,0);
            for (int k=0; k<_nFunctions; k++) {
                QPen pen;
                pen.setWidth(2);
                pen.setColor(clr[k % clr.count()]);
                p.setPen(pen);
                QPainterPath path;
                path.moveTo(_left, _bottom - (_y[0][k]-_yMin) *_scaleY);
                for (int i=1; i<_nValues; i++)
                    path.lineTo(_left+i*dx, _bottom - (_y[i][k]-_yMin)*_scaleY);
                p.drawPath(path);
            }
        }
        if (_bars) {
            int rBar = (int)(dx/2)-1;
            if (rBar > 0) {
                QList<QColor> clr;
                clr << QColor(0,0,255,128) << QColor(255,0,0,128) << QColor(0,255,0,128) << QColor(96,96,0,128);
                for (int k=0; k<_nFunctions; k++) {
                    QColor penClr(192,192,255);
                    p.setPen(penClr);
                    for (int i=0; i<=_nValues; i++) {
                        QLinearGradient g;
                        g = QLinearGradient(0, y0, 0, y0-_y[i][k]*_scaleY);
                        g.setColorAt(0, QColor(255,255,255,0));
                        g.setColorAt(1, clr[k % clr.count()]);
                        p.setBrush(g);
                        p.drawRect(QRectF(
                            QPointF(_left+i*dx - rBar,  y0),
                            QSizeF(2*rBar,             -_y[i][k]*_scaleY)));
                    }
                }
            }
        }
        if (_dots) {
            int rDot = (int)(dx/2)-1;
            if (rDot < 2)
                rDot = 2;
            QList<QColor> clr;
            clr << QColor(0,32,128) << QColor(192,0,0) << QColor(0,128,32) << QColor(96,96,0);
            for (int k=0; k<_nFunctions; k++) {
                p.setBrush(clr[k % clr.count()]);
                p.setPen(clr[k % clr.count()]);
                for (int i=0; i<_nValues; i++)
                    p.drawEllipse(QRectF(
                        QPointF(_left+i*dx-2,  _bottom - (_y[i][k]-_yMin)*_scaleY - 2),
                        QSizeF(4,             4)));
            }
        }
        p.setBrush(Qt::red);
        p.setPen(Qt::red);
        foreach (QPointF point, _markers) {
            double x = _left + (point.x() - _xMin) * _scaleX;
            double y = _top  + (_yMax - point.y()) * _scaleY;
            p.drawEllipse(QRectF(QPointF(x-4, y-4),  QSizeF(8, 8)));
        }
        break;
    }
    case BAR:
        break;
    default:
        p.setPen(Qt::black);
        p.drawText(0, (height()-_fontH)/2, width(), _fontH,
            Qt::AlignCenter,
            tr("No plot defined (Menu: Insert -- Plot)"));
    }
}

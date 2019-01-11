/***************************************************************************
    qgsmaptooladdcircle.h  -  map tool for adding circle
    ---------------------
    begin                : July 2017
    copyright            : (C) 2017
    email                : lbartoletti at tuxfamily dot org
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMAPTOOLADDCIRCLE_H
#define QGSMAPTOOLADDCIRCLE_H

#include "qgsmaptoolcapture.h"
#include "qgscircle.h"
#include "qgis_app.h"

class QgsGeometryRubberBand;
class QgsSnapIndicator;

struct EdgesOnlyFilter : public QgsPointLocator::MatchFilter
{
  bool acceptMatch( const QgsPointLocator::Match &m ) override { return m.hasEdge(); }
};

class APP_EXPORT QgsMapToolAddCircle: public QgsMapToolCapture
{
    Q_OBJECT

  public:
    QgsMapToolAddCircle( QgsMapToolCapture *parentTool, QgsMapCanvas *canvas, CaptureMode mode = CaptureLine );
    ~QgsMapToolAddCircle() override;

    void keyPressEvent( QKeyEvent *e ) override;
    void keyReleaseEvent( QKeyEvent *e ) override;

    void deactivate() override;
    void activate() override;
    void clean() override;

  protected:
    explicit QgsMapToolAddCircle( QgsMapCanvas *canvas ) = delete; //forbidden

    /**
     * The parent map tool, e.g. the add feature tool.
     *  Completed circle will be added to this tool by calling its addCurve() method.
     **/
    QgsMapToolCapture *mParentTool = nullptr;
    //! Circle points (in map coordinates)
    QgsPointSequence mPoints;
    //! The rubberband to show the circular string currently working on
    QgsGeometryRubberBand *mTempRubberBand = nullptr;
    //! Circle
    QgsCircle mCircle;
    //! Layer type which will be used for rubberband
    QgsWkbTypes::GeometryType mLayerType = QgsWkbTypes::LineGeometry;

    //! Snapping indicators
    std::unique_ptr<QgsSnapIndicator> mSnapIndicator;

};

#endif // QGSMAPTOOLADDCIRCLE_H

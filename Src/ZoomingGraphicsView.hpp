#pragma once

#include <QGraphicsView>





/** A QGraphicsView descendant that provides zooming by mouse wheel. */
class ZoomingGraphicsView:
	public QGraphicsView
{
	Q_OBJECT

	using Super = QGraphicsView;


public:

	/** The factor by which to zoom in each direction. Must be 0 < mZoomFactor < 1. */
	double mZoomFactor = 0.9;


	ZoomingGraphicsView(QWidget * aParent = nullptr);

	void wheelEvent(QWheelEvent * aWheelEvent) override;
};

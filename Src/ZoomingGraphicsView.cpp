#include "ZoomingGraphicsView.hpp"

#include <QWheelEvent>





ZoomingGraphicsView::ZoomingGraphicsView(QWidget * aParent):
	Super(aParent)
{
	setMouseTracking(true);
}





void ZoomingGraphicsView::wheelEvent(QWheelEvent * aWheelEvent)
{
	if (aWheelEvent->modifiers() == Qt::NoModifier)
	{
		const auto anchor = transformationAnchor();
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		int angle = aWheelEvent->angleDelta().y();
		qreal factor;
		if (angle > 0)
		{
			factor = 1 / mZoomFactor;
		}
		else
		{
			factor = mZoomFactor;
		}
		scale(factor, factor);
		setTransformationAnchor(anchor);
	}
}

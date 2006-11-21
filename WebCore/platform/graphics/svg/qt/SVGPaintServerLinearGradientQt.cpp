/*
    Copyright (C) 2006 Nikolas Zimmermann <wildfox@kde.org>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "config.h"

#ifdef SVG_SUPPORT
#include "SVGPaintServerLinearGradient.h"

#include "KRenderingDeviceQt.h"
#include "RenderPath.h"

#include <QLinearGradient>

namespace WebCore {

bool SVGPaintServerLinearGradient::setup(KRenderingDeviceContext* context, const RenderObject* object, SVGPaintTargetType type) const
{
    KRenderingDeviceContextQt* qtContext = static_cast<KRenderingDeviceContextQt*>(context);
    Q_ASSERT(qtContext != 0);

    if (listener())
        listener()->resourceNotification();

    RenderStyle* renderStyle = object->style();

    double x1, x2, y1, y2;
    if (boundingBoxMode()) {
        QRectF bbox = qtContext->pathBBox();
        x1 = double(bbox.left()) + (double(gradientStart().x() / 100.0) * double(bbox.width()));
        y1 = double(bbox.top()) + (double(gradientStart().y() / 100.0) * double(bbox.height()));
        x2 = double(bbox.left()) + (double(gradientEnd().x() / 100.0)  * double(bbox.width()));
        y2 = double(bbox.top()) + (double(gradientEnd().y() / 100.0) * double(bbox.height()));
    } else {
        x1 = gradientStart().x();
        y1 = gradientStart().y();
        x2 = gradientEnd().x();
        y2 = gradientEnd().y();
    }

    qtContext->painter().setPen(Qt::NoPen);
    qtContext->painter().setBrush(Qt::NoBrush);

    QLinearGradient gradient(QPointF(x1, y1), QPointF(x2, y2));
    if (spreadMethod() == SPREADMETHOD_REPEAT)
        gradient.setSpread(QGradient::RepeatSpread);
    else if (spreadMethod() == SPREADMETHOD_REFLECT)
        gradient.setSpread(QGradient::ReflectSpread);
    else
        gradient.setSpread(QGradient::PadSpread);

    double opacity = 1.0;

    // TODO: Gradient transform + opacity fixes!

    if ((type & APPLY_TO_FILL) && renderStyle->svgStyle()->hasFill()) {
        fillColorArray(gradient, gradientStops(), opacity);

        QBrush brush(gradient);

        qtContext->painter().setBrush(brush);
        qtContext->setFillRule(renderStyle->svgStyle()->fillRule());
    }

    if ((type & APPLY_TO_STROKE) && renderStyle->svgStyle()->hasStroke()) {
        fillColorArray(gradient, gradientStops(), opacity);

        QPen pen;
        QBrush brush(gradient);

        setPenProperties(object, renderStyle, pen);
        pen.setBrush(brush);

        qtContext->painter().setPen(pen);
    }

    return true;
}

} // namespace WebCore

#endif

// vim:ts=4:noet

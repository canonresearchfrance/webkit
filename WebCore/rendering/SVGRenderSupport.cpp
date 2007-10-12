/**
 * This file is part of the DOM implementation for WebKit.
 *
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 *           (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 *           (C) 2007 Eric Seidel <eric@webkit.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"

#if ENABLE(SVG)

#include "AffineTransform.h"
#include "RenderObject.h"
#include "SVGResourceClipper.h"
#include "SVGResourceFilter.h"
#include "SVGResourceMasker.h"
#include "SVGStyledElement.h"
#include "SVGURIReference.h"

namespace WebCore {

#if ENABLE(SVG_EXPERIMENTAL_FEATURES)
void prepareToRenderSVGContent(RenderObject* object, RenderObject::PaintInfo& paintInfo, const FloatRect& boundingBox, SVGResourceFilter*& filter)
#else
void prepareToRenderSVGContent(RenderObject* object, RenderObject::PaintInfo& paintInfo, const FloatRect& boundingBox, void*)
#endif
{    
    SVGElement* svgElement = static_cast<SVGElement*>(object->element());
    ASSERT(svgElement && svgElement->document() && svgElement->isStyled());
    
    SVGStyledElement* styledElement = static_cast<SVGStyledElement*>(svgElement);
    const SVGRenderStyle* svgStyle = object->style()->svgStyle();
    
    AtomicString filterId(SVGURIReference::getTarget(svgStyle->filter()));
    AtomicString clipperId(SVGURIReference::getTarget(svgStyle->clipPath()));
    AtomicString maskerId(SVGURIReference::getTarget(svgStyle->maskElement()));
    
    Document* document = object->document();
#if ENABLE(SVG_EXPERIMENTAL_FEATURES)
    filter = getFilterById(document, filterId);
#endif
    SVGResourceClipper* clipper = getClipperById(document, clipperId);
    SVGResourceMasker* masker = getMaskerById(document, maskerId);
    
#if ENABLE(SVG_EXPERIMENTAL_FEATURES)
    if (filter)
        filter->prepareFilter(paintInfo.context, boundingBox);
    else if (!filterId.isEmpty())
        svgElement->document()->accessSVGExtensions()->addPendingResource(filterId, styledElement);
#endif
    
    if (clipper) {
        clipper->addClient(styledElement);
        clipper->applyClip(paintInfo.context, boundingBox);
    } else if (!clipperId.isEmpty())
        svgElement->document()->accessSVGExtensions()->addPendingResource(clipperId, styledElement);
    
    if (masker) {
        masker->addClient(styledElement);
        masker->applyMask(paintInfo.context, boundingBox);
    } else if (!maskerId.isEmpty())
        svgElement->document()->accessSVGExtensions()->addPendingResource(maskerId, styledElement);
}

} // namespace WebCore

#endif // ENABLE(SVG)

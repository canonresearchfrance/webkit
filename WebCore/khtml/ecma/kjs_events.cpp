// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "kjs_window.h"
#include "kjs_events.h"
#include "kjs_events.lut.h"
#include "kjs_views.h"
#include "kjs_proxy.h"
#include "xml/dom_nodeimpl.h"
#include "xml/dom_docimpl.h"
#include "xml/dom2_eventsimpl.h"
#include "rendering/render_object.h"

#include <kdebug.h>

using namespace KJS;

// -------------------------------------------------------------------------

JSEventListener::JSEventListener(Object _listener, const Object &_win, bool _html)
{
    listener = _listener;
    //fprintf(stderr,"JSEventListener::JSEventListener this=%p listener=%p\n",this,listener.imp());
    html = _html;
    win = _win;
    static_cast<Window*>(win.imp())->jsEventListeners.append(this);
}

JSEventListener::~JSEventListener()
{
    static_cast<Window*>(win.imp())->jsEventListeners.removeRef(this);
    //fprintf(stderr,"JSEventListener::~JSEventListener this=%p listener=%p\n",this,listener.imp());
}

void JSEventListener::handleEvent(DOM::Event &evt)
{
#ifdef KJS_DEBUGGER
  if (KJSDebugWin::instance() && KJSDebugWin::instance()->inSession())
    return;
#endif
  KHTMLPart *part = static_cast<Window*>(win.imp())->part();
  KJSProxy *proxy = 0L;
  if (part)
      proxy = KJSProxy::proxy( part );

  if (proxy && listener.implementsCall()) {
    ref();

    KJS::ScriptInterpreter *interpreter = static_cast<KJS::ScriptInterpreter *>(proxy->interpreter());
    ExecState *exec = interpreter->globalExec();

    List args;
    args.append(getDOMEvent(exec,evt));

    // Add the event's target element to the scope
    // (and the document, and the form - see KJS::HTMLElement::eventHandlerScope)
    Object thisObj = Object::dynamicCast(getDOMNode(exec,evt.currentTarget()));
    List scope;
    List oldScope = listener.scope();
    //if (thisVal.type() != NullType)
    if ( !thisObj.isNull() ) {
      scope = static_cast<DOMNode*>(thisObj.imp())->eventHandlerScope(exec);
      if ( !scope.isEmpty() ) {
        List curScope = oldScope.copy();
        curScope.prependList( scope );
        listener.setScope( curScope );
      }
    }

#ifdef KWQ_DEBUG_JS
	fprintf (stdout, "handleEvent() thisObj = %s\n", thisObj.toString(exec).qstring().latin1());
	{
		int i;
		
		for (i = 0; i < args.size(); i++)
			fprintf (stdout, "handleEvent() args[%d] = %s\n", i, args.at(i).toString(exec).qstring().latin1());
	}
#endif

    Window *window = static_cast<Window*>(win.imp());
    // Set the event we're handling in the Window object
    window->setCurrentEvent( &evt );
    // ... and in the interpreter
    interpreter->setCurrentEvent( &evt );

    Value retval = listener.call(exec, thisObj, args);

    if ( !scope.isEmpty() ) {
      listener.setScope( oldScope );
    }

    window->setCurrentEvent( 0 );
    interpreter->setCurrentEvent( 0 );
    if ( exec->hadException() )
        exec->clearException();
    else
    {
        QVariant ret = ValueToVariant(exec, retval);
        if (ret.type() == QVariant::Bool && ret.toBool() == false)
            evt.preventDefault();
    }
    DOM::DocumentImpl::updateDocumentsRendering();
    deref();
  }
}

DOM::DOMString JSEventListener::eventListenerType()
{
    if (html)
	return "_khtml_HTMLEventListener";
    else
	return "_khtml_JSEventListener";
}

Value KJS::getNodeEventListener(DOM::Node n, int eventId)
{
    DOM::EventListener *listener = n.handle()->getHTMLEventListener(eventId);
    if (listener)
	return static_cast<JSEventListener*>(listener)->listenerObj();
    else
	return Null();
}

// -------------------------------------------------------------------------

const ClassInfo EventConstructor::info = { "EventConstructor", 0, &EventConstructorTable, 0 };
/*
@begin EventConstructorTable 3
  CAPTURING_PHASE	DOM::Event::CAPTURING_PHASE	DontDelete|ReadOnly
  AT_TARGET		DOM::Event::AT_TARGET		DontDelete|ReadOnly
  BUBBLING_PHASE	DOM::Event::BUBBLING_PHASE	DontDelete|ReadOnly
# Reverse-engineered from Netscape
  MOUSEDOWN		1				DontDelete|ReadOnly
  MOUSEUP		2				DontDelete|ReadOnly
  MOUSEOVER		4				DontDelete|ReadOnly
  MOUSEOUT		8				DontDelete|ReadOnly
  MOUSEMOVE		16				DontDelete|ReadOnly
  MOUSEDRAG		32				DontDelete|ReadOnly
  CLICK			64				DontDelete|ReadOnly
  DBLCLICK		128				DontDelete|ReadOnly
  KEYDOWN		256				DontDelete|ReadOnly
  KEYUP			512				DontDelete|ReadOnly
  KEYPRESS		1024				DontDelete|ReadOnly
  DRAGDROP		2048				DontDelete|ReadOnly
  FOCUS			4096				DontDelete|ReadOnly
  BLUR			8192				DontDelete|ReadOnly
  SELECT		16384				DontDelete|ReadOnly
  CHANGE		32768				DontDelete|ReadOnly
@end
*/

Value EventConstructor::tryGet(ExecState *exec, const UString &p) const
{
  return DOMObjectLookupGetValue<EventConstructor, DOMObject>(exec,p,&EventConstructorTable,this);
}

Value EventConstructor::getValueProperty(ExecState *, int token) const
{
  // We use the token as the value to return directly
  return Number(token);
}

Value KJS::getEventConstructor(ExecState *exec)
{
  return cacheGlobalObject<EventConstructor>(exec, "[[event.constructor]]");
}

// -------------------------------------------------------------------------

const ClassInfo DOMEvent::info = { "Event", 0, &DOMEventTable, 0 };
/*
@begin DOMEventTable 7
  type		DOMEvent::Type		DontDelete|ReadOnly
  target	DOMEvent::Target	DontDelete|ReadOnly
  currentTarget	DOMEvent::CurrentTarget	DontDelete|ReadOnly
  srcElement	DOMEvent::SrcElement	DontDelete|ReadOnly
  eventPhase	DOMEvent::EventPhase	DontDelete|ReadOnly
  bubbles	DOMEvent::Bubbles	DontDelete|ReadOnly
  cancelable	DOMEvent::Cancelable	DontDelete|ReadOnly
  timeStamp	DOMEvent::TimeStamp	DontDelete|ReadOnly
  returnValue   DOMEvent::ReturnValue   DontDelete
  cancelBubble  DOMEvent::CancelBubble  DontDelete
@end
@begin DOMEventProtoTable 3
  stopPropagation 	DOMEvent::StopPropagation	DontDelete|Function 0
  preventDefault 	DOMEvent::PreventDefault	DontDelete|Function 0
  initEvent		DOMEvent::InitEvent		DontDelete|Function 3
@end
*/
DEFINE_PROTOTYPE("DOMEvent", DOMEventProto)
IMPLEMENT_PROTOFUNC(DOMEventProtoFunc)
IMPLEMENT_PROTOTYPE(DOMEventProto, DOMEventProtoFunc)

DOMEvent::DOMEvent(ExecState *exec, DOM::Event e)
  : DOMObject(DOMEventProto::self(exec)), event(e) { }

DOMEvent::~DOMEvent()
{
  ScriptInterpreter::forgetDOMObject(event.handle());
}

Value DOMEvent::tryGet(ExecState *exec, const UString &p) const
{
#ifdef KJS_VERBOSE
  kdDebug() << "KJS::DOMEvent::tryGet " << p.qstring() << endl;
#endif
  return DOMObjectLookupGetValue<DOMEvent,DOMObject>(exec, p, &DOMEventTable, this );
}

Value DOMEvent::getValueProperty(ExecState *exec, int token) const
{
  switch (token) {
  case Type:
    return String(event.type());
  case Target:
  case SrcElement: /*MSIE extension - "the object that fired the event"*/
    return getDOMNode(exec,event.target());
  case CurrentTarget:
    return getDOMNode(exec,event.currentTarget());
  case EventPhase:
    return Number((unsigned int)event.eventPhase());
  case Bubbles:
  case CancelBubble: // MSIE extension. not sure if readable. and returnValue ?
    return Boolean(event.bubbles());
  case Cancelable:
    return Boolean(event.cancelable());
  case TimeStamp:
    return Number((long unsigned int)event.timeStamp()); // ### long long ?
  default:
    kdWarning() << "Unhandled token in DOMEvent::getValueProperty : " << token << endl;
    return Value();
  }
}

void DOMEvent::tryPut(ExecState *exec, const UString &propertyName,
                      const Value& value, int attr)
{
  DOMObjectLookupPut<DOMEvent, DOMObject>(exec, propertyName, value, attr,
                                          &DOMEventTable, this);
}

void DOMEvent::putValue(ExecState *exec, int token, const Value& value, int)
{
  switch (token) {
  case ReturnValue:
    if (value.toBoolean(exec))
      event.preventDefault();
    break;
  case CancelBubble:
    if (value.toBoolean(exec))
      event.stopPropagation();
    break;
  default:
    break;
  }
}

Value DOMEventProtoFunc::tryCall(ExecState *exec, Object & thisObj, const List &args)
{
  if (!thisObj.inherits(&KJS::DOMEvent::info)) {
    Object err = Error::create(exec,TypeError);
    exec->setException(err);
    return err;
  }
  DOM::Event event = static_cast<DOMEvent *>( thisObj.imp() )->toEvent();
  switch (id) {
    case DOMEvent::StopPropagation:
      event.stopPropagation();
    case DOMEvent::PreventDefault:
      event.preventDefault();
      return Undefined();
    case DOMEvent::InitEvent:
      event.initEvent(args[0].toString(exec).string(),args[1].toBoolean(exec),args[2].toBoolean(exec));
      return Undefined();
  };
  return Undefined();
}

Value KJS::getDOMEvent(ExecState *exec, DOM::Event e)
{
  DOMObject *ret;
  if (e.isNull())
    return Null();
  ScriptInterpreter* interp = static_cast<ScriptInterpreter *>(exec->interpreter());
  if ((ret = interp->getDOMObject(e.handle())))
    return Value(ret);

  DOM::DOMString module = e.eventModuleName();
  if (module == "UIEvents")
    ret = new DOMUIEvent(exec, static_cast<DOM::UIEvent>(e));
  else if (module == "MouseEvents")
    ret = new DOMMouseEvent(exec, static_cast<DOM::MouseEvent>(e));
  else if (module == "MutationEvents")
    ret = new DOMMutationEvent(exec, static_cast<DOM::MutationEvent>(e));
  else
    ret = new DOMEvent(exec, e);

  interp->putDOMObject(e.handle(),ret);
  return Value(ret);
}

DOM::Event KJS::toEvent(const Value& val)
{
  Object obj = Object::dynamicCast(val);
  if (obj.isNull() || !obj.inherits(&DOMEvent::info))
    return DOM::Event();

  const DOMEvent *dobj = static_cast<const DOMEvent*>(obj.imp());
  return dobj->toEvent();
}

// -------------------------------------------------------------------------


const ClassInfo EventExceptionConstructor::info = { "EventExceptionConstructor", 0, &EventExceptionConstructorTable, 0 };
/*
@begin EventExceptionConstructorTable 1
  UNSPECIFIED_EVENT_TYPE_ERR    DOM::EventException::UNSPECIFIED_EVENT_TYPE_ERR DontDelete|ReadOnly
@end
*/
Value EventExceptionConstructor::tryGet(ExecState *exec, const UString &p) const
{
  return DOMObjectLookupGetValue<EventExceptionConstructor, DOMObject>(exec,p,&EventExceptionConstructorTable,this);
}

Value EventExceptionConstructor::getValueProperty(ExecState *, int token) const
{
  // We use the token as the value to return directly
  return Number(token);
}

Value KJS::getEventExceptionConstructor(ExecState *exec)
{
  return cacheGlobalObject<EventExceptionConstructor>(exec, "[[eventException.constructor]]");
}

// -------------------------------------------------------------------------

const ClassInfo DOMUIEvent::info = { "UIEvent", &DOMEvent::info, &DOMUIEventTable, 0 };
/*
@begin DOMUIEventTable 2
  view		DOMUIEvent::View	DontDelete|ReadOnly
  detail	DOMUIEvent::Detail	DontDelete|ReadOnly
@end
@begin DOMUIEventProtoTable 1
  initUIEvent	DOMUIEvent::InitUIEvent	DontDelete|Function 5
@end
*/
DEFINE_PROTOTYPE("DOMUIEvent",DOMUIEventProto)
IMPLEMENT_PROTOFUNC(DOMUIEventProtoFunc)
IMPLEMENT_PROTOTYPE_WITH_PARENT(DOMUIEventProto,DOMUIEventProtoFunc,DOMEventProto)

DOMUIEvent::~DOMUIEvent()
{
}

Value DOMUIEvent::tryGet(ExecState *exec, const UString &p) const
{
  return DOMObjectLookupGetValue<DOMUIEvent,DOMEvent>(exec,p,&DOMUIEventTable,this);
}

Value DOMUIEvent::getValueProperty(ExecState *exec, int token) const
{
  switch (token) {
  case View:
    return getDOMAbstractView(exec,static_cast<DOM::UIEvent>(event).view());
  case Detail:
    return Number(static_cast<DOM::UIEvent>(event).detail());
  default:
    kdWarning() << "Unhandled token in DOMUIEvent::getValueProperty : " << token << endl;
    return Value();
  }
}

Value DOMUIEventProtoFunc::tryCall(ExecState *exec, Object &thisObj, const List &args)
{
  if (!thisObj.inherits(&KJS::DOMUIEvent::info)) {
    Object err = Error::create(exec,TypeError);
    exec->setException(err);
    return err;
  }
  DOM::UIEvent uiEvent = static_cast<DOMUIEvent *>(thisObj.imp())->toUIEvent();
  switch (id) {
    case DOMUIEvent::InitUIEvent: {
      DOM::AbstractView v = toAbstractView(args[3]);
      static_cast<DOM::UIEvent>(uiEvent).initUIEvent(args[0].toString(exec).string(),
                                                     args[1].toBoolean(exec),
                                                     args[2].toBoolean(exec),
                                                     v,
                                                     args[4].toInteger(exec));
      }
      return Undefined();
  }
  return Undefined();
}

// -------------------------------------------------------------------------

const ClassInfo DOMMouseEvent::info = { "MouseEvent", &DOMUIEvent::info, &DOMMouseEventTable, 0 };

/*
@begin DOMMouseEventTable 2
  screenX	DOMMouseEvent::ScreenX	DontDelete|ReadOnly
  screenY	DOMMouseEvent::ScreenY	DontDelete|ReadOnly
  clientX	DOMMouseEvent::ClientX	DontDelete|ReadOnly
  x		DOMMouseEvent::X	DontDelete|ReadOnly
  clientY	DOMMouseEvent::ClientY	DontDelete|ReadOnly
  y		DOMMouseEvent::Y	DontDelete|ReadOnly
  offsetX	DOMMouseEvent::OffsetX	DontDelete|ReadOnly
  offsetY	DOMMouseEvent::OffsetY	DontDelete|ReadOnly
  ctrlKey	DOMMouseEvent::CtrlKey	DontDelete|ReadOnly
  shiftKey	DOMMouseEvent::ShiftKey	DontDelete|ReadOnly
  altKey	DOMMouseEvent::AltKey	DontDelete|ReadOnly
  metaKey	DOMMouseEvent::MetaKey	DontDelete|ReadOnly
  button	DOMMouseEvent::Button	DontDelete|ReadOnly
  relatedTarget	DOMMouseEvent::RelatedTarget DontDelete|ReadOnly
  fromElement	DOMMouseEvent::FromElement DontDelete|ReadOnly
  toElement	DOMMouseEvent::ToElement	DontDelete|ReadOnly
@end
@begin DOMMouseEventProtoTable 1
  initMouseEvent	DOMMouseEvent::InitMouseEvent	DontDelete|Function 15
@end
*/
DEFINE_PROTOTYPE("DOMMouseEvent",DOMMouseEventProto)
IMPLEMENT_PROTOFUNC(DOMMouseEventProtoFunc)
IMPLEMENT_PROTOTYPE_WITH_PARENT(DOMMouseEventProto,DOMMouseEventProtoFunc,DOMUIEventProto)

DOMMouseEvent::~DOMMouseEvent()
{
}

Value DOMMouseEvent::tryGet(ExecState *exec, const UString &p) const
{
#ifdef KJS_VERBOSE
  kdDebug(6070) << "DOMMouseEvent::tryGet " << p.qstring() << endl;
#endif
  return DOMObjectLookupGetValue<DOMMouseEvent,DOMUIEvent>(exec,p,&DOMMouseEventTable,this);
}

Value DOMMouseEvent::getValueProperty(ExecState *exec, int token) const
{
  switch (token) {
  case ScreenX:
    return Number(static_cast<DOM::MouseEvent>(event).screenX());
  case ScreenY:
    return Number(static_cast<DOM::MouseEvent>(event).screenY());
  case ClientX:
  case X:
    return Number(static_cast<DOM::MouseEvent>(event).clientX());
  case ClientY:
  case Y:
    return Number(static_cast<DOM::MouseEvent>(event).clientY());
  case OffsetX:
  case OffsetY: // MSIE extension
  {
    DOM::Node node = event.target();
    node.handle()->getDocument()->updateRendering();
    khtml::RenderObject *rend = node.handle() ? node.handle()->renderer() : 0L;
    int x = static_cast<DOM::MouseEvent>(event).clientX();
    int y = static_cast<DOM::MouseEvent>(event).clientY();
    if ( rend ) {
      int xPos, yPos;
      if ( rend->absolutePosition( xPos, yPos ) ) {
        kdDebug() << "DOMMouseEvent::getValueProperty rend=" << rend << "  xPos=" << xPos << "  yPos=" << yPos << endl;
        x -= xPos;
        y -= yPos;
      }
    }
    return Number( token == OffsetX ? x : y );
  }
  case CtrlKey:
    return Boolean(static_cast<DOM::MouseEvent>(event).ctrlKey());
  case ShiftKey:
    return Boolean(static_cast<DOM::MouseEvent>(event).shiftKey());
  case AltKey:
    return Boolean(static_cast<DOM::MouseEvent>(event).altKey());
  case MetaKey:
    return Boolean(static_cast<DOM::MouseEvent>(event).metaKey());
  case Button:
  {
    // Tricky. The DOM (and khtml) use 0 for LMB, 1 for MMB and 2 for RMB
    // but MSIE uses 1=LMB, 2=RMB, 4=MMB, as a bitfield
    int domButton = static_cast<DOM::MouseEvent>(event).button();
    int button = domButton==0 ? 1 : domButton==1 ? 4 : domButton==2 ? 2 : 0;
    return Number( (unsigned int)button );
  }
  case ToElement:
    // MSIE extension - "the object toward which the user is moving the mouse pointer"
    if (event.handle()->id() == DOM::EventImpl::MOUSEOUT_EVENT)
      return getDOMNode(exec,static_cast<DOM::MouseEvent>(event).relatedTarget());
    return getDOMNode(exec,static_cast<DOM::MouseEvent>(event).target());
  case FromElement:
    // MSIE extension - "object from which activation
    // or the mouse pointer is exiting during the event" (huh?)
    if (event.handle()->id() == DOM::EventImpl::MOUSEOUT_EVENT)
      return getDOMNode(exec,static_cast<DOM::MouseEvent>(event).target());
    /* fall through */
  case RelatedTarget:
    return getDOMNode(exec,static_cast<DOM::MouseEvent>(event).relatedTarget());
  default:
    kdWarning() << "Unhandled token in DOMMouseEvent::getValueProperty : " << token << endl;
    return Value();
  }
}

Value DOMMouseEventProtoFunc::tryCall(ExecState *exec, Object &thisObj, const List &args)
{
  if (!thisObj.inherits(&KJS::DOMMouseEvent::info)) {
    Object err = Error::create(exec,TypeError);
    exec->setException(err);
    return err;
  }
  DOM::MouseEvent mouseEvent = static_cast<DOMMouseEvent *>(thisObj.imp())->toMouseEvent();
  switch (id) {
    case DOMMouseEvent::InitMouseEvent:
      mouseEvent.initMouseEvent(args[0].toString(exec).string(), // typeArg
                                args[1].toBoolean(exec), // canBubbleArg
                                args[2].toBoolean(exec), // cancelableArg
                                toAbstractView(args[3]), // viewArg
                                args[4].toInteger(exec), // detailArg
                                args[5].toInteger(exec), // screenXArg
                                args[6].toInteger(exec), // screenYArg
                                args[7].toInteger(exec), // clientXArg
                                args[8].toInteger(exec), // clientYArg
                                args[9].toBoolean(exec), // ctrlKeyArg
                                args[10].toBoolean(exec), // altKeyArg
                                args[11].toBoolean(exec), // shiftKeyArg
                                args[12].toBoolean(exec), // metaKeyArg
                                args[13].toInteger(exec), // buttonArg
                                toNode(args[14])); // relatedTargetArg
      return Undefined();
  }
  return Undefined();
}

// -------------------------------------------------------------------------

const ClassInfo MutationEventConstructor::info = { "MutationEventConstructor", 0, &MutationEventConstructorTable, 0 };
/*
@begin MutationEventConstructorTable 3
  MODIFICATION	DOM::MutationEvent::MODIFICATION	DontDelete|ReadOnly
  ADDITION	DOM::MutationEvent::ADDITION		DontDelete|ReadOnly
  REMOVAL	DOM::MutationEvent::REMOVAL		DontDelete|ReadOnly
@end
*/
Value MutationEventConstructor::tryGet(ExecState *exec, const UString &p) const
{
  return DOMObjectLookupGetValue<MutationEventConstructor,DOMObject>(exec,p,&MutationEventConstructorTable,this);
}

Value MutationEventConstructor::getValueProperty(ExecState *, int token) const
{
  // We use the token as the value to return directly
  return Number(token);
}

Value KJS::getMutationEventConstructor(ExecState *exec)
{
  return cacheGlobalObject<MutationEventConstructor>(exec, "[[mutationEvent.constructor]]");
}

// -------------------------------------------------------------------------

const ClassInfo DOMMutationEvent::info = { "MutationEvent", &DOMEvent::info, &DOMMutationEventTable, 0 };
/*
@begin DOMMutationEventTable 5
  relatedNode	DOMMutationEvent::RelatedNode	DontDelete|ReadOnly
  prevValue	DOMMutationEvent::PrevValue	DontDelete|ReadOnly
  newValue	DOMMutationEvent::NewValue	DontDelete|ReadOnly
  attrName	DOMMutationEvent::AttrName	DontDelete|ReadOnly
  attrChange	DOMMutationEvent::AttrChange	DontDelete|ReadOnly
@end
@begin DOMMutationEventProtoTable 1
  initMutationEvent	DOMMutationEvent::InitMutationEvent	DontDelete|Function 8
@end
*/
DEFINE_PROTOTYPE("DOMMutationEvent",DOMMutationEventProto)
IMPLEMENT_PROTOFUNC(DOMMutationEventProtoFunc)
IMPLEMENT_PROTOTYPE_WITH_PARENT(DOMMutationEventProto,DOMMutationEventProtoFunc,DOMEventProto)

DOMMutationEvent::~DOMMutationEvent()
{
}

Value DOMMutationEvent::tryGet(ExecState *exec, const UString &p) const
{
  return DOMObjectLookupGetValue<DOMMutationEvent,DOMEvent>(exec,p,&DOMMutationEventTable,this);
}

Value DOMMutationEvent::getValueProperty(ExecState *exec, int token) const
{
  switch (token) {
  case RelatedNode:
    return getDOMNode(exec,static_cast<DOM::MutationEvent>(event).relatedNode());
  case PrevValue:
    return String(static_cast<DOM::MutationEvent>(event).prevValue());
  case NewValue:
    return String(static_cast<DOM::MutationEvent>(event).newValue());
  case AttrName:
    return String(static_cast<DOM::MutationEvent>(event).attrName());
  case AttrChange:
    return Number((unsigned int)static_cast<DOM::MutationEvent>(event).attrChange());
  default:
    kdWarning() << "Unhandled token in DOMMutationEvent::getValueProperty : " << token << endl;
    return Value();
  }
}

Value DOMMutationEventProtoFunc::tryCall(ExecState *exec, Object &thisObj, const List &args)
{
  if (!thisObj.inherits(&KJS::DOMMutationEvent::info)) {
    Object err = Error::create(exec,TypeError);
    exec->setException(err);
    return err;
  }
  DOM::MutationEvent mutationEvent = static_cast<DOMMutationEvent *>(thisObj.imp())->toMutationEvent();
  switch (id) {
    case DOMMutationEvent::InitMutationEvent:
      mutationEvent.initMutationEvent(args[0].toString(exec).string(), // typeArg,
                                      args[1].toBoolean(exec), // canBubbleArg
                                      args[2].toBoolean(exec), // cancelableArg
                                      toNode(args[3]), // relatedNodeArg
                                      args[4].toString(exec).string(), // prevValueArg
                                      args[5].toString(exec).string(), // newValueArg
                                      args[6].toString(exec).string(), // attrNameArg
                                      args[7].toInteger(exec)); // attrChangeArg
      return Undefined();
  }
  return Undefined();
}

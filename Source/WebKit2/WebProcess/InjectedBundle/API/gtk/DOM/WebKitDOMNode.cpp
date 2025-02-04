/*
 *  This file is part of the WebKit open source project.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "WebKitDOMNode.h"

#include "ConvertToUTF8String.h"
#include "DOMObjectCache.h"
#include "GObjectEventListener.h"
#include "WebKitDOMDocumentPrivate.h"
#include "WebKitDOMElementPrivate.h"
#include "WebKitDOMEventPrivate.h"
#include "WebKitDOMEventTarget.h"
#include "WebKitDOMNodeListPrivate.h"
#include "WebKitDOMNodePrivate.h"
#include "WebKitDOMPrivate.h"
#include <WebCore/CSSImportRule.h>
#include <WebCore/Document.h>
#include <WebCore/ExceptionCode.h>
#include <WebCore/ExceptionCodeDescription.h>
#include <WebCore/JSMainThreadExecState.h>
#include <WebCore/SVGTests.h>
#include <wtf/GetPtr.h>
#include <wtf/RefPtr.h>

#define WEBKIT_DOM_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, WEBKIT_DOM_TYPE_NODE, WebKitDOMNodePrivate)

typedef struct _WebKitDOMNodePrivate {
    RefPtr<WebCore::Node> coreObject;
} WebKitDOMNodePrivate;

namespace WebKit {

WebKitDOMNode* kit(WebCore::Node* obj)
{
    if (!obj)
        return 0;

    if (gpointer ret = DOMObjectCache::get(obj))
        return WEBKIT_DOM_NODE(ret);

    return wrap(obj);
}

WebCore::Node* core(WebKitDOMNode* request)
{
    return request ? static_cast<WebCore::Node*>(WEBKIT_DOM_OBJECT(request)->coreObject) : 0;
}

WebKitDOMNode* wrapNode(WebCore::Node* coreObject)
{
    ASSERT(coreObject);
    return WEBKIT_DOM_NODE(g_object_new(WEBKIT_DOM_TYPE_NODE, "core-object", coreObject, nullptr));
}

}

static gboolean webkit_dom_node_dispatch_event(WebKitDOMEventTarget* target, WebKitDOMEvent* event, GError** error)
{
    WebCore::Event* coreEvent = WebKit::core(event);
    if (!coreEvent)
        return false;
    WebCore::Node* coreTarget = static_cast<WebCore::Node*>(WEBKIT_DOM_OBJECT(target)->coreObject);

    auto result = coreTarget->dispatchEventForBindings(*coreEvent);
    if (result.hasException()) {
        WebCore::ExceptionCodeDescription description(result.releaseException().code());
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), description.code, description.name);
        return false;
    }
    return result.releaseReturnValue();
}

static gboolean webkit_dom_node_add_event_listener(WebKitDOMEventTarget* target, const char* eventName, GClosure* handler, gboolean useCapture)
{
    WebCore::Node* coreTarget = static_cast<WebCore::Node*>(WEBKIT_DOM_OBJECT(target)->coreObject);
    return WebKit::GObjectEventListener::addEventListener(G_OBJECT(target), coreTarget, eventName, handler, useCapture);
}

static gboolean webkit_dom_node_remove_event_listener(WebKitDOMEventTarget* target, const char* eventName, GClosure* handler, gboolean useCapture)
{
    WebCore::Node* coreTarget = static_cast<WebCore::Node*>(WEBKIT_DOM_OBJECT(target)->coreObject);
    return WebKit::GObjectEventListener::removeEventListener(G_OBJECT(target), coreTarget, eventName, handler, useCapture);
}

static void webkit_dom_event_target_init(WebKitDOMEventTargetIface* iface)
{
    iface->dispatch_event = webkit_dom_node_dispatch_event;
    iface->add_event_listener = webkit_dom_node_add_event_listener;
    iface->remove_event_listener = webkit_dom_node_remove_event_listener;
}

G_DEFINE_TYPE_WITH_CODE(WebKitDOMNode, webkit_dom_node, WEBKIT_DOM_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(WEBKIT_DOM_TYPE_EVENT_TARGET, webkit_dom_event_target_init))

enum {
    PROP_0,
    PROP_NODE_NAME,
    PROP_NODE_VALUE,
    PROP_NODE_TYPE,
    PROP_PARENT_NODE,
    PROP_CHILD_NODES,
    PROP_FIRST_CHILD,
    PROP_LAST_CHILD,
    PROP_PREVIOUS_SIBLING,
    PROP_NEXT_SIBLING,
    PROP_OWNER_DOCUMENT,
    PROP_BASE_URI,
    PROP_TEXT_CONTENT,
    PROP_PARENT_ELEMENT,
};

static void webkit_dom_node_finalize(GObject* object)
{
    WebKitDOMNodePrivate* priv = WEBKIT_DOM_NODE_GET_PRIVATE(object);

    WebKit::DOMObjectCache::forget(priv->coreObject.get());

    priv->~WebKitDOMNodePrivate();
    G_OBJECT_CLASS(webkit_dom_node_parent_class)->finalize(object);
}

static void webkit_dom_node_set_property(GObject* object, guint propertyId, const GValue* value, GParamSpec* pspec)
{
    WebKitDOMNode* self = WEBKIT_DOM_NODE(object);

    switch (propertyId) {
    case PROP_NODE_VALUE:
        webkit_dom_node_set_node_value(self, g_value_get_string(value), nullptr);
        break;
    case PROP_TEXT_CONTENT:
        webkit_dom_node_set_text_content(self, g_value_get_string(value), nullptr);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static void webkit_dom_node_get_property(GObject* object, guint propertyId, GValue* value, GParamSpec* pspec)
{
    WebKitDOMNode* self = WEBKIT_DOM_NODE(object);

    switch (propertyId) {
    case PROP_NODE_NAME:
        g_value_take_string(value, webkit_dom_node_get_node_name(self));
        break;
    case PROP_NODE_VALUE:
        g_value_take_string(value, webkit_dom_node_get_node_value(self));
        break;
    case PROP_NODE_TYPE:
        g_value_set_uint(value, webkit_dom_node_get_node_type(self));
        break;
    case PROP_PARENT_NODE:
        g_value_set_object(value, webkit_dom_node_get_parent_node(self));
        break;
    case PROP_CHILD_NODES:
        g_value_set_object(value, webkit_dom_node_get_child_nodes(self));
        break;
    case PROP_FIRST_CHILD:
        g_value_set_object(value, webkit_dom_node_get_first_child(self));
        break;
    case PROP_LAST_CHILD:
        g_value_set_object(value, webkit_dom_node_get_last_child(self));
        break;
    case PROP_PREVIOUS_SIBLING:
        g_value_set_object(value, webkit_dom_node_get_previous_sibling(self));
        break;
    case PROP_NEXT_SIBLING:
        g_value_set_object(value, webkit_dom_node_get_next_sibling(self));
        break;
    case PROP_OWNER_DOCUMENT:
        g_value_set_object(value, webkit_dom_node_get_owner_document(self));
        break;
    case PROP_BASE_URI:
        g_value_take_string(value, webkit_dom_node_get_base_uri(self));
        break;
    case PROP_TEXT_CONTENT:
        g_value_take_string(value, webkit_dom_node_get_text_content(self));
        break;
    case PROP_PARENT_ELEMENT:
        g_value_set_object(value, webkit_dom_node_get_parent_element(self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static GObject* webkit_dom_node_constructor(GType type, guint constructPropertiesCount, GObjectConstructParam* constructProperties)
{
    GObject* object = G_OBJECT_CLASS(webkit_dom_node_parent_class)->constructor(type, constructPropertiesCount, constructProperties);

    WebKitDOMNodePrivate* priv = WEBKIT_DOM_NODE_GET_PRIVATE(object);
    priv->coreObject = static_cast<WebCore::Node*>(WEBKIT_DOM_OBJECT(object)->coreObject);
    WebKit::DOMObjectCache::put(priv->coreObject.get(), object);

    return object;
}

static void webkit_dom_node_class_init(WebKitDOMNodeClass* requestClass)
{
    GObjectClass* gobjectClass = G_OBJECT_CLASS(requestClass);
    g_type_class_add_private(gobjectClass, sizeof(WebKitDOMNodePrivate));
    gobjectClass->constructor = webkit_dom_node_constructor;
    gobjectClass->finalize = webkit_dom_node_finalize;
    gobjectClass->set_property = webkit_dom_node_set_property;
    gobjectClass->get_property = webkit_dom_node_get_property;

    g_object_class_install_property(
        gobjectClass,
        PROP_NODE_NAME,
        g_param_spec_string(
            "node-name",
            "Node:node-name",
            "read-only gchar* Node:node-name",
            "",
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NODE_VALUE,
        g_param_spec_string(
            "node-value",
            "Node:node-value",
            "read-write gchar* Node:node-value",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NODE_TYPE,
        g_param_spec_uint(
            "node-type",
            "Node:node-type",
            "read-only gushort Node:node-type",
            0, G_MAXUINT, 0,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PARENT_NODE,
        g_param_spec_object(
            "parent-node",
            "Node:parent-node",
            "read-only WebKitDOMNode* Node:parent-node",
            WEBKIT_DOM_TYPE_NODE,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_CHILD_NODES,
        g_param_spec_object(
            "child-nodes",
            "Node:child-nodes",
            "read-only WebKitDOMNodeList* Node:child-nodes",
            WEBKIT_DOM_TYPE_NODE_LIST,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_FIRST_CHILD,
        g_param_spec_object(
            "first-child",
            "Node:first-child",
            "read-only WebKitDOMNode* Node:first-child",
            WEBKIT_DOM_TYPE_NODE,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_LAST_CHILD,
        g_param_spec_object(
            "last-child",
            "Node:last-child",
            "read-only WebKitDOMNode* Node:last-child",
            WEBKIT_DOM_TYPE_NODE,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PREVIOUS_SIBLING,
        g_param_spec_object(
            "previous-sibling",
            "Node:previous-sibling",
            "read-only WebKitDOMNode* Node:previous-sibling",
            WEBKIT_DOM_TYPE_NODE,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NEXT_SIBLING,
        g_param_spec_object(
            "next-sibling",
            "Node:next-sibling",
            "read-only WebKitDOMNode* Node:next-sibling",
            WEBKIT_DOM_TYPE_NODE,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_OWNER_DOCUMENT,
        g_param_spec_object(
            "owner-document",
            "Node:owner-document",
            "read-only WebKitDOMDocument* Node:owner-document",
            WEBKIT_DOM_TYPE_DOCUMENT,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_BASE_URI,
        g_param_spec_string(
            "base-uri",
            "Node:base-uri",
            "read-only gchar* Node:base-uri",
            "",
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_TEXT_CONTENT,
        g_param_spec_string(
            "text-content",
            "Node:text-content",
            "read-write gchar* Node:text-content",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PARENT_ELEMENT,
        g_param_spec_object(
            "parent-element",
            "Node:parent-element",
            "read-only WebKitDOMElement* Node:parent-element",
            WEBKIT_DOM_TYPE_ELEMENT,
            WEBKIT_PARAM_READABLE));

}

static void webkit_dom_node_init(WebKitDOMNode* request)
{
    WebKitDOMNodePrivate* priv = WEBKIT_DOM_NODE_GET_PRIVATE(request);
    new (priv) WebKitDOMNodePrivate();
}

WebKitDOMNode* webkit_dom_node_insert_before(WebKitDOMNode* self, WebKitDOMNode* newChild, WebKitDOMNode* refChild, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(newChild), 0);
    g_return_val_if_fail(!refChild || WEBKIT_DOM_IS_NODE(refChild), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedNewChild = WebKit::core(newChild);
    WebCore::Node* convertedRefChild = WebKit::core(refChild);
    WebCore::ExceptionCode ec = 0;
    bool ok = item->insertBefore(*convertedNewChild, convertedRefChild, ec);
    if (ok)
        return WebKit::kit(convertedNewChild);

    WebCore::ExceptionCodeDescription ecdesc(ec);
    g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    return 0;
}

WebKitDOMNode* webkit_dom_node_replace_child(WebKitDOMNode* self, WebKitDOMNode* newChild, WebKitDOMNode* oldChild, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(newChild), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(oldChild), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedNewChild = WebKit::core(newChild);
    WebCore::Node* convertedOldChild = WebKit::core(oldChild);
    WebCore::ExceptionCode ec = 0;
    bool ok = item->replaceChild(*convertedNewChild, *convertedOldChild, ec);
    if (ok)
        return WebKit::kit(convertedOldChild);

    WebCore::ExceptionCodeDescription ecdesc(ec);
    g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    return 0;
}

WebKitDOMNode* webkit_dom_node_remove_child(WebKitDOMNode* self, WebKitDOMNode* oldChild, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(oldChild), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedOldChild = WebKit::core(oldChild);
    WebCore::ExceptionCode ec = 0;
    bool ok = item->removeChild(*convertedOldChild, ec);
    if (ok)
        return WebKit::kit(convertedOldChild);

    WebCore::ExceptionCodeDescription ecdesc(ec);
    g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    return 0;
}

WebKitDOMNode* webkit_dom_node_append_child(WebKitDOMNode* self, WebKitDOMNode* newChild, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(newChild), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedNewChild = WebKit::core(newChild);
    WebCore::ExceptionCode ec = 0;
    bool ok = item->appendChild(*convertedNewChild, ec);
    if (ok)
        return WebKit::kit(convertedNewChild);

    WebCore::ExceptionCodeDescription ecdesc(ec);
    g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    return 0;
}

gboolean webkit_dom_node_has_child_nodes(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    WebCore::Node* item = WebKit::core(self);
    gboolean result = item->hasChildNodes();
    return result;
}

WebKitDOMNode* webkit_dom_node_clone_node_with_error(WebKitDOMNode* self, gboolean deep, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::ExceptionCode ec = 0;
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->cloneNodeForBindings(deep, ec));
    if (ec) {
        WebCore::ExceptionCodeDescription ecdesc(ec);
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    }
    return WebKit::kit(gobjectResult.get());
}

void webkit_dom_node_normalize(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_NODE(self));
    WebCore::Node* item = WebKit::core(self);
    item->normalize();
}

gboolean webkit_dom_node_is_supported(WebKitDOMNode* self, const gchar* feature, const gchar* version)
{
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    g_return_val_if_fail(feature, FALSE);
    g_return_val_if_fail(version, FALSE);
    WTF::String convertedFeature = WTF::String::fromUTF8(feature);
    WTF::String convertedVersion = WTF::String::fromUTF8(version);
    return WebCore::SVGTests::hasFeatureForLegacyBindings(convertedFeature, convertedVersion);
}

gboolean webkit_dom_node_is_same_node(WebKitDOMNode* self, WebKitDOMNode* other)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(other), FALSE);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedOther = WebKit::core(other);
    gboolean result = item->isSameNode(convertedOther);
    return result;
}

gboolean webkit_dom_node_is_equal_node(WebKitDOMNode* self, WebKitDOMNode* other)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(other), FALSE);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedOther = WebKit::core(other);
    gboolean result = item->isEqualNode(convertedOther);
    return result;
}

gchar* webkit_dom_node_lookup_prefix(WebKitDOMNode* self, const gchar* namespaceURI)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(namespaceURI, 0);
    WebCore::Node* item = WebKit::core(self);
    WTF::String convertedNamespaceURI = WTF::String::fromUTF8(namespaceURI);
    gchar* result = convertToUTF8String(item->lookupPrefix(convertedNamespaceURI));
    return result;
}

gchar* webkit_dom_node_lookup_namespace_uri(WebKitDOMNode* self, const gchar* prefix)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(prefix, 0);
    WebCore::Node* item = WebKit::core(self);
    WTF::String convertedPrefix = WTF::String::fromUTF8(prefix);
    gchar* result = convertToUTF8String(item->lookupNamespaceURI(convertedPrefix));
    return result;
}

gboolean webkit_dom_node_is_default_namespace(WebKitDOMNode* self, const gchar* namespaceURI)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    g_return_val_if_fail(namespaceURI, FALSE);
    WebCore::Node* item = WebKit::core(self);
    WTF::String convertedNamespaceURI = WTF::String::fromUTF8(namespaceURI);
    gboolean result = item->isDefaultNamespace(convertedNamespaceURI);
    return result;
}

gushort webkit_dom_node_compare_document_position(WebKitDOMNode* self, WebKitDOMNode* other)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(other), 0);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedOther = WebKit::core(other);
    gushort result = item->compareDocumentPosition(*convertedOther);
    return result;
}

gboolean webkit_dom_node_contains(WebKitDOMNode* self, WebKitDOMNode* other)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), FALSE);
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(other), FALSE);
    WebCore::Node* item = WebKit::core(self);
    WebCore::Node* convertedOther = WebKit::core(other);
    gboolean result = item->contains(convertedOther);
    return result;
}

gchar* webkit_dom_node_get_node_name(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->nodeName());
    return result;
}

gchar* webkit_dom_node_get_node_value(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->nodeValue());
    return result;
}

void webkit_dom_node_set_node_value(WebKitDOMNode* self, const gchar* value, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_NODE(self));
    g_return_if_fail(value);
    g_return_if_fail(!error || !*error);
    WebCore::Node* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    WebCore::ExceptionCode ec = 0;
    item->setNodeValue(convertedValue, ec);
    if (ec) {
        WebCore::ExceptionCodeDescription ecdesc(ec);
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    }
}

gushort webkit_dom_node_get_node_type(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    gushort result = item->nodeType();
    return result;
}

WebKitDOMNode* webkit_dom_node_get_parent_node(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->parentNode());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMNodeList* webkit_dom_node_get_child_nodes(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::NodeList> gobjectResult = WTF::getPtr(item->childNodes());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMNode* webkit_dom_node_get_first_child(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->firstChild());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMNode* webkit_dom_node_get_last_child(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->lastChild());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMNode* webkit_dom_node_get_previous_sibling(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->previousSibling());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMNode* webkit_dom_node_get_next_sibling(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Node> gobjectResult = WTF::getPtr(item->nextSibling());
    return WebKit::kit(gobjectResult.get());
}

WebKitDOMDocument* webkit_dom_node_get_owner_document(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Document> gobjectResult = WTF::getPtr(item->ownerDocument());
    return WebKit::kit(gobjectResult.get());
}

gchar* webkit_dom_node_get_base_uri(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->baseURI());
    return result;
}

gchar* webkit_dom_node_get_text_content(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->textContent());
    return result;
}

void webkit_dom_node_set_text_content(WebKitDOMNode* self, const gchar* value, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_NODE(self));
    g_return_if_fail(value);
    g_return_if_fail(!error || !*error);
    WebCore::Node* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    WebCore::ExceptionCode ec = 0;
    item->setTextContent(convertedValue, ec);
    if (ec) {
        WebCore::ExceptionCodeDescription ecdesc(ec);
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    }
}

WebKitDOMElement* webkit_dom_node_get_parent_element(WebKitDOMNode* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_NODE(self), 0);
    WebCore::Node* item = WebKit::core(self);
    RefPtr<WebCore::Element> gobjectResult = WTF::getPtr(item->parentElement());
    return WebKit::kit(gobjectResult.get());
}


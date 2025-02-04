/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004-2008, 2014-2016 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include "CachedHTMLCollection.h"
#include <wtf/text/AtomicString.h>

namespace WebCore {

// HTMLCollection that limits to a particular tag.
class TagCollection final : public CachedHTMLCollection<TagCollection, CollectionTypeTraits<ByTag>::traversalType> {
public:
    static Ref<TagCollection> create(ContainerNode& rootNode, CollectionType type, const AtomicString& qualifiedName)
    {
        ASSERT_UNUSED(type, type == ByTag);
        return adoptRef(*new TagCollection(rootNode, qualifiedName));
    }

    virtual ~TagCollection();
    bool elementMatches(Element&) const;

private:
    TagCollection(ContainerNode& rootNode, const AtomicString& qualifiedName);

    AtomicString m_prefix;
    AtomicString m_localName;
};

class TagCollectionNS final : public CachedHTMLCollection<TagCollectionNS, CollectionTypeTraits<ByTag>::traversalType> {
public:
    static Ref<TagCollectionNS> create(ContainerNode& rootNode, const AtomicString& namespaceURI, const AtomicString& localName)
    {
        return adoptRef(*new TagCollectionNS(rootNode, namespaceURI, localName));
    }

    virtual ~TagCollectionNS();
    bool elementMatches(Element&) const;

private:
    TagCollectionNS(ContainerNode& rootNode, const AtomicString& namespaceURI, const AtomicString& localName);

    AtomicString m_namespaceURI;
    AtomicString m_localName;
};

class HTMLTagCollection final : public CachedHTMLCollection<HTMLTagCollection, CollectionTypeTraits<ByHTMLTag>::traversalType> {
public:
    static Ref<HTMLTagCollection> create(ContainerNode& rootNode, CollectionType type, const AtomicString& qualifiedName)
    {
        ASSERT_UNUSED(type, type == ByHTMLTag);
        return adoptRef(*new HTMLTagCollection(rootNode, qualifiedName));
    }

    virtual ~HTMLTagCollection();
    bool elementMatches(Element&) const;

private:
    HTMLTagCollection(ContainerNode& rootNode, const AtomicString& qualifiedName);

    AtomicString m_prefix;
    AtomicString m_loweredPrefix;
    AtomicString m_localName;
    AtomicString m_loweredLocalName;
};

inline bool TagCollection::elementMatches(Element& element) const
{
    return m_localName == element.localName() && m_prefix == element.prefix();
}

inline bool TagCollectionNS::elementMatches(Element& element) const
{
    if (m_localName != starAtom && m_localName != element.localName())
        return false;
    return m_namespaceURI == starAtom || m_namespaceURI == element.namespaceURI();
}

inline bool HTMLTagCollection::elementMatches(Element& element) const
{
    if (element.isHTMLElement())
        return m_loweredLocalName == element.localName() && m_loweredPrefix == element.prefix();
    return m_localName == element.localName() && m_prefix == element.prefix();
}

} // namespace WebCore

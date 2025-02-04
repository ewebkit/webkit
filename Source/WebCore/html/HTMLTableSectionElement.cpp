/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "HTMLTableSectionElement.h"

#include "ExceptionCode.h"
#include "GenericCachedHTMLCollection.h"
#include "HTMLCollection.h"
#include "HTMLNames.h"
#include "HTMLTableRowElement.h"
#include "HTMLTableElement.h"
#include "NodeList.h"
#include "NodeRareData.h"
#include "Text.h"

namespace WebCore {

using namespace HTMLNames;

inline HTMLTableSectionElement::HTMLTableSectionElement(const QualifiedName& tagName, Document& document)
    : HTMLTablePartElement(tagName, document)
{
}

Ref<HTMLTableSectionElement> HTMLTableSectionElement::create(const QualifiedName& tagName, Document& document)
{
    return adoptRef(*new HTMLTableSectionElement(tagName, document));
}

const StyleProperties* HTMLTableSectionElement::additionalPresentationAttributeStyle() const
{
    auto* table = findParentTable();
    if (!table)
        return nullptr;
    return table->additionalGroupStyle(true);
}

ExceptionOr<Ref<HTMLElement>> HTMLTableSectionElement::insertRow(int index)
{
    if (index < -1)
        return Exception { INDEX_SIZE_ERR };
    auto children = rows();
    int numRows = children->length();
    if (index > numRows)
        return Exception { INDEX_SIZE_ERR };
    auto row = HTMLTableRowElement::create(trTag, document());
    ExceptionCode ec = 0;
    if (numRows == index || index == -1)
        appendChild(row, ec);
    else {
        Node* n;
        if (index < 1)
            n = firstChild();
        else
            n = children->item(index);
        insertBefore(row, n, ec);
    }
    if (ec)
        return Exception { ec };
    return Ref<HTMLElement> { WTFMove(row) };
}

ExceptionOr<void> HTMLTableSectionElement::deleteRow(int index)
{
    auto children = rows();
    int numRows = children->length();
    if (index == -1) {
        if (!numRows)
            return { };
        index = numRows - 1;
    }
    if (index < 0 || index >= numRows)
        return Exception { INDEX_SIZE_ERR };
    ExceptionCode ec = 0;
    removeChild(*children->item(index), ec);
    if (ec)
        return Exception { ec };
    return { };
}

int HTMLTableSectionElement::numRows() const
{
    auto rows = childrenOfType<HTMLTableRowElement>(*this);
    return std::distance(rows.begin(), rows.end());
}

Ref<HTMLCollection> HTMLTableSectionElement::rows()
{
    return ensureRareData().ensureNodeLists().addCachedCollection<GenericCachedHTMLCollection<CollectionTypeTraits<TSectionRows>::traversalType>>(*this, TSectionRows);
}

}

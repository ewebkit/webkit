/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ewk_extension.h"

#include "Module.h"
#include "NotImplemented.h"
#include "WKBundle.h"
#include "WKBundleAPICast.h"
#include "ewk_extension_private.h"
#include "ewk_page_private.h"

using namespace WebKit;

static inline EwkExtension* toEwkExtendion(const void* clientInfo)
{
    return const_cast<EwkExtension*>(static_cast<const EwkExtension*>(clientInfo));
}

void EwkExtension::didCreatePage(WKBundleRef, WKBundlePageRef wkPage, const void* clientInfo)
{
    EwkExtension* self = toEwkExtendion(clientInfo);
    WebPage* page = toImpl(wkPage);

    EwkPage* ewkPage = new EwkPage(page);
    self->m_pageMap.add(page, std::unique_ptr<EwkPage>(ewkPage));

    for (auto& it : self->m_clients) {
        if (it->did_Create_Page)
            it->did_Create_Page(ewkPage, it->data);
    }
}

void EwkExtension::willDestroyPage(WKBundleRef, WKBundlePageRef wkPage, const void* clientInfo)
{
    EwkExtension* self = toEwkExtendion(clientInfo);
    WebPage* page = toImpl(wkPage);

    for (auto& it : self->m_clients) {
        if (it->will_Destroy_Page)
            it->will_Destroy_Page(self->m_pageMap.get(page), it->data);
    }
    self->m_pageMap.remove(page);
}

void EwkExtension::didReceiveMessage(WKBundleRef, WKStringRef, WKTypeRef, const void*)
{
    notImplemented();
}

void EwkExtension::didReceiveMessageToPage(WKBundleRef, WKBundlePageRef, WKStringRef, WKTypeRef, const void*)
{
    notImplemented();
}

EwkExtension::EwkExtension(InjectedBundle* bundle)
{
    WKBundleClientV1 wkBundleClient = {
        {
            1, // version
            this, // clientInfo
        },
        didCreatePage,
        willDestroyPage,
        0, // didInitializePageGroup
        didReceiveMessage,
        didReceiveMessageToPage
    };
    WKBundleSetClient(toAPI(bundle), &wkBundleClient.base);
}

EwkExtension::~EwkExtension()
{
}

void EwkExtension::append(Ewk_Extension_Client* client)
{
    m_clients.append(client);
}

void EwkExtension::remove(Ewk_Extension_Client* client)
{
    m_clients.remove(m_clients.find(client));
}

void ewk_extension_client_add(Ewk_Extension* extension, Ewk_Extension_Client* client)
{
    EINA_SAFETY_ON_NULL_RETURN(extension);
    EINA_SAFETY_ON_NULL_RETURN(client);

    extension->append(client);
}

void ewk_extension_client_del(Ewk_Extension* extension, Ewk_Extension_Client* client)
{
    EINA_SAFETY_ON_NULL_RETURN(extension);
    EINA_SAFETY_ON_NULL_RETURN(client);

    extension->remove(client);
}

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

/**
 * @file    ewk_page.h
 * @brief   Describes the Ewk_Page API.
 */

#ifndef ewk_page_h
#define ewk_page_h

#include <Eina.h>
#include <JavaScriptCore/JSBase.h>

typedef struct EwkPage Ewk_Page;

#ifdef __cplusplus
extern "C" {
#endif

struct EwkPageClient {
    int version;
    void *data;

    void (*load_finished)(Ewk_Page* page, void *data);
    void (*message_received)(Ewk_Page *page, const char *name, const Eina_Value* value, void *data);
};
typedef struct EwkPageClient Ewk_Page_Client;

EAPI JSGlobalContextRef ewk_page_js_global_context_get(Ewk_Page *page);

EAPI void ewk_page_client_add(Ewk_Page *page, Ewk_Page_Client *client);

#ifdef __cplusplus
}
#endif

#endif // ewk_extension_h

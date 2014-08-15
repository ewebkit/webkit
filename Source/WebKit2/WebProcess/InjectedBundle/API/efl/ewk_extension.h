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
 * @defgroup Ewk_Extension Ewk Extension
 * @ingroup EWebKit Extension
 *
 * ewebkit2 is based on multi process architecture.
 * So, user application can not access DOM Objects or JavaScript Objects directly.
 *
 * Instead, you can make your own extension by using EWebKit2 Extension.
 * Ewk Extension is used to comunicate with ewebkit.
 *
 * All extensions share one extension instance.
 *
 * In order to make your own extension,
 * you should implement @c Ewk_Extension_Initialize_Function in your shared object.
 */

/**
 * @file    ewk_extension.h
 * @brief   Describes the Ewk_Extension API.
 */

#ifndef ewk_extension_h
#define ewk_extension_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EwkPage Ewk_Page;

/**
 * Ewk_Extension is basic structure to comunicte with ewebkit.
 * All extensions share one extension instance.
 */
typedef struct EwkExtension Ewk_Extension;

/**
 * Declare Ewk_Extension_Initialize_Function.
 *
 * @brief  Type definition for the entry of the extension.
 *
 */
typedef void *(*Ewk_Extension_Initialize_Function)(Ewk_Extension *bundle, void *reserved);

struct EwkExtensionClient {
    int version;
    void *data;

    void (*did_Create_Page)(Ewk_Page* page, void *data);
    void (*will_Destroy_Page)(Ewk_Page* page, void *data);
};
typedef struct EwkExtensionClient Ewk_Extension_Client;

EAPI void ewk_extension_client_add(Ewk_Extension *extension, Ewk_Extension_Client *client);
EAPI void ewk_extension_client_del(Ewk_Extension *extension, Ewk_Extension_Client *client);

EAPI void ewk_extension_message_post(Ewk_Extension *extension, const char *name, const Eina_Value *body);

#ifdef __cplusplus
}
#endif

#endif // ewk_extension_h

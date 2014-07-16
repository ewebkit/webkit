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
 * Declare Ewk_Extension.
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
};
typedef struct EwkExtensionClient Ewk_Extension_Client;

EAPI void ewk_extension_client_set(Ewk_Extension *extension, Ewk_Extension_Client *client);

#ifdef __cplusplus
}
#endif

#endif // ewk_extension_h

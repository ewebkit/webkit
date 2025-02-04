/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 * Copyright (C) 2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UserMediaClient_h
#define UserMediaClient_h

#if ENABLE(MEDIA_STREAM)

#include <functional>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class MediaDevicesEnumerationRequest;
class MediaConstraints;
class Page;
class RealtimeMediaSource;
class UserMediaRequest;

class UserMediaClient {
public:
    virtual void pageDestroyed() = 0;

    virtual void requestUserMediaAccess(UserMediaRequest&) = 0;
    virtual void cancelUserMediaAccessRequest(UserMediaRequest&) = 0;

    virtual void enumerateMediaDevices(MediaDevicesEnumerationRequest&) = 0;
    virtual void cancelMediaDevicesEnumerationRequest(MediaDevicesEnumerationRequest&) = 0;

protected:
    virtual ~UserMediaClient() { }
};

WEBCORE_EXPORT void provideUserMediaTo(Page*, UserMediaClient*);

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)

#endif // UserMediaClient_h

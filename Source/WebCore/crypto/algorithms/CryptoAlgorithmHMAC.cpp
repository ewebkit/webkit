/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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
#include "CryptoAlgorithmHMAC.h"

#if ENABLE(SUBTLE_CRYPTO)

#include "CryptoAlgorithmHmacKeyParams.h"
#include "CryptoAlgorithmHmacKeyParamsDeprecated.h"
#include "CryptoAlgorithmHmacParamsDeprecated.h"
#include "CryptoKeyDataOctetSequence.h"
#include "CryptoKeyHMAC.h"
#include "ExceptionCode.h"

namespace WebCore {

const char* const CryptoAlgorithmHMAC::s_name = "HMAC";

static inline bool usagesAreInvalidForCryptoAlgorithmHMAC(CryptoKeyUsage usages)
{
    return usages & (CryptoKeyUsageEncrypt | CryptoKeyUsageDecrypt | CryptoKeyUsageDeriveKey | CryptoKeyUsageDeriveBits | CryptoKeyUsageWrapKey | CryptoKeyUsageUnwrapKey);
}

CryptoAlgorithmHMAC::CryptoAlgorithmHMAC()
{
}

CryptoAlgorithmHMAC::~CryptoAlgorithmHMAC()
{
}

Ref<CryptoAlgorithm> CryptoAlgorithmHMAC::create()
{
    return adoptRef(*new CryptoAlgorithmHMAC);
}

CryptoAlgorithmIdentifier CryptoAlgorithmHMAC::identifier() const
{
    return s_identifier;
}

bool CryptoAlgorithmHMAC::keyAlgorithmMatches(const CryptoAlgorithmHmacParamsDeprecated& parameters, const CryptoKey& key) const
{
    if (key.algorithmIdentifier() != s_identifier)
        return false;

    if (downcast<CryptoKeyHMAC>(key).hashAlgorithmIdentifier() != parameters.hash)
        return false;

    return true;
}

void CryptoAlgorithmHMAC::generateKey(const std::unique_ptr<CryptoAlgorithmParameters>&& parameters, bool extractable, CryptoKeyUsage usages, KeyOrKeyPairCallback&& callback, ExceptionCallback&& exceptionCallback, ScriptExecutionContext*)
{
    const auto& hmacParameters = downcast<CryptoAlgorithmHmacKeyParams>(*parameters);

    if (usagesAreInvalidForCryptoAlgorithmHMAC(usages)) {
        exceptionCallback(SYNTAX_ERR);
        return;
    }

    if (hmacParameters.length && !hmacParameters.length.value()) {
        exceptionCallback(OperationError);
        return;
    }

    auto result = CryptoKeyHMAC::generate(hmacParameters.length.valueOr(0), hmacParameters.hashIdentifier, extractable, usages);
    if (!result) {
        exceptionCallback(OperationError);
        return;
    }

    callback(result.get(), nullptr);
}

void CryptoAlgorithmHMAC::importKey(SubtleCrypto::KeyFormat format, KeyData&& data, const std::unique_ptr<CryptoAlgorithmParameters>&& parameters, bool extractable, CryptoKeyUsage usages, KeyCallback&& callback, ExceptionCallback&& exceptionCallback)
{
    const auto& hmacParameters = downcast<CryptoAlgorithmHmacKeyParams>(*parameters);

    if (usagesAreInvalidForCryptoAlgorithmHMAC(usages)) {
        exceptionCallback(SYNTAX_ERR);
        return;
    }

    RefPtr<CryptoKeyHMAC> result;
    switch (format) {
    case SubtleCrypto::KeyFormat::Raw:
        result = CryptoKeyHMAC::importRaw(hmacParameters.length.valueOr(0), hmacParameters.hashIdentifier, WTFMove(WTF::get<Vector<uint8_t>>(data)), extractable, usages);
        break;
    case SubtleCrypto::KeyFormat::Jwk: {
        auto checkAlgCallback = [](CryptoAlgorithmIdentifier hash, const Optional<String>& alg) -> bool {
            switch (hash) {
            case CryptoAlgorithmIdentifier::SHA_1:
                return !alg || alg.value() == "HS1";
            case CryptoAlgorithmIdentifier::SHA_224:
                return !alg || alg.value() == "HS224";
            case CryptoAlgorithmIdentifier::SHA_256:
                return !alg || alg.value() == "HS256";
            case CryptoAlgorithmIdentifier::SHA_384:
                return !alg || alg.value() == "HS384";
            case CryptoAlgorithmIdentifier::SHA_512:
                return !alg || alg.value() == "HS512";
            default:
                return false;
            }
            return false;
        };
        result = CryptoKeyHMAC::importJwk(hmacParameters.length.valueOr(0), hmacParameters.hashIdentifier, WTFMove(WTF::get<JsonWebKey>(data)), extractable, usages, WTFMove(checkAlgCallback));
        break;
    }
    default:
        exceptionCallback(NOT_SUPPORTED_ERR);
        return;
    }
    if (!result) {
        exceptionCallback(DataError);
        return;
    }

    callback(*result);
}

void CryptoAlgorithmHMAC::sign(const CryptoAlgorithmParametersDeprecated& parameters, const CryptoKey& key, const CryptoOperationData& data, VectorCallback&& callback, VoidCallback&& failureCallback, ExceptionCode& ec)
{
    const CryptoAlgorithmHmacParamsDeprecated& hmacParameters = downcast<CryptoAlgorithmHmacParamsDeprecated>(parameters);

    if (!keyAlgorithmMatches(hmacParameters, key)) {
        ec = NOT_SUPPORTED_ERR;
        return;
    }

    platformSign(hmacParameters, downcast<CryptoKeyHMAC>(key), data, WTFMove(callback), WTFMove(failureCallback), ec);
}

void CryptoAlgorithmHMAC::verify(const CryptoAlgorithmParametersDeprecated& parameters, const CryptoKey& key, const CryptoOperationData& expectedSignature, const CryptoOperationData& data, BoolCallback&& callback, VoidCallback&& failureCallback, ExceptionCode& ec)
{
    const CryptoAlgorithmHmacParamsDeprecated& hmacParameters = downcast<CryptoAlgorithmHmacParamsDeprecated>(parameters);

    if (!keyAlgorithmMatches(hmacParameters, key)) {
        ec = NOT_SUPPORTED_ERR;
        return;
    }

    platformVerify(hmacParameters, downcast<CryptoKeyHMAC>(key), expectedSignature, data, WTFMove(callback), WTFMove(failureCallback), ec);
}

void CryptoAlgorithmHMAC::generateKey(const CryptoAlgorithmParametersDeprecated& parameters, bool extractable, CryptoKeyUsage usages, KeyOrKeyPairCallback&& callback, VoidCallback&& failureCallback, ExceptionCode&, ScriptExecutionContext*)
{
    const CryptoAlgorithmHmacKeyParamsDeprecated& hmacParameters = downcast<CryptoAlgorithmHmacKeyParamsDeprecated>(parameters);

    RefPtr<CryptoKeyHMAC> result = CryptoKeyHMAC::generate(hmacParameters.hasLength ? hmacParameters.length : 0, hmacParameters.hash, extractable, usages);
    if (!result) {
        failureCallback();
        return;
    }

    callback(result.get(), nullptr);
}

void CryptoAlgorithmHMAC::importKey(const CryptoAlgorithmParametersDeprecated& parameters, const CryptoKeyData& keyData, bool extractable, CryptoKeyUsage usage, KeyCallback&& callback, VoidCallback&&, ExceptionCode& ec)
{
    if (!is<CryptoKeyDataOctetSequence>(keyData)) {
        ec = NOT_SUPPORTED_ERR;
        return;
    }
    const CryptoKeyDataOctetSequence& keyDataOctetSequence = downcast<CryptoKeyDataOctetSequence>(keyData);

    const CryptoAlgorithmHmacParamsDeprecated& hmacParameters = downcast<CryptoAlgorithmHmacParamsDeprecated>(parameters);

    RefPtr<CryptoKeyHMAC> result = CryptoKeyHMAC::create(keyDataOctetSequence.octetSequence(), hmacParameters.hash, extractable, usage);
    callback(*result);
}

}

#endif // ENABLE(SUBTLE_CRYPTO)

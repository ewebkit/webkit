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

#ifndef CryptoAlgorithmRSASSA_PKCS1_v1_5_h
#define CryptoAlgorithmRSASSA_PKCS1_v1_5_h

#include "CryptoAlgorithm.h"

#if ENABLE(SUBTLE_CRYPTO)

namespace WebCore {

class CryptoAlgorithmRsaSsaParamsDeprecated;
class CryptoKeyRSA;

class CryptoAlgorithmRSASSA_PKCS1_v1_5 final : public CryptoAlgorithm {
public:
    static const char* const s_name;
    static const CryptoAlgorithmIdentifier s_identifier = CryptoAlgorithmIdentifier::RSASSA_PKCS1_v1_5;

    static Ref<CryptoAlgorithm> create();

    CryptoAlgorithmIdentifier identifier() const override;

    void generateKey(const std::unique_ptr<CryptoAlgorithmParameters>&&, bool extractable, CryptoKeyUsage, KeyOrKeyPairCallback&&, ExceptionCallback&&, ScriptExecutionContext*) final;
    void importKey(SubtleCrypto::KeyFormat, KeyData&&, const std::unique_ptr<CryptoAlgorithmParameters>&&, bool extractable, CryptoKeyUsage, KeyCallback&&, ExceptionCallback&&) final;

    // The following will be deprecated.
    void sign(const CryptoAlgorithmParametersDeprecated&, const CryptoKey&, const CryptoOperationData&, VectorCallback&&, VoidCallback&& failureCallback, ExceptionCode&) override;
    void verify(const CryptoAlgorithmParametersDeprecated&, const CryptoKey&, const CryptoOperationData& signature, const CryptoOperationData&, BoolCallback&&, VoidCallback&& failureCallback, ExceptionCode&) override;
    void generateKey(const CryptoAlgorithmParametersDeprecated&, bool extractable, CryptoKeyUsage, KeyOrKeyPairCallback&&, VoidCallback&& failureCallback, ExceptionCode&, ScriptExecutionContext*) override;
    void importKey(const CryptoAlgorithmParametersDeprecated&, const CryptoKeyData&, bool extractable, CryptoKeyUsage, KeyCallback&&, VoidCallback&& failureCallback, ExceptionCode&) override;

private:
    CryptoAlgorithmRSASSA_PKCS1_v1_5();
    virtual ~CryptoAlgorithmRSASSA_PKCS1_v1_5();

    bool keyAlgorithmMatches(const CryptoAlgorithmRsaSsaParamsDeprecated& algorithmParameters, const CryptoKey&) const;
    void platformSign(const CryptoAlgorithmRsaSsaParamsDeprecated&, const CryptoKeyRSA&, const CryptoOperationData&, VectorCallback&&, VoidCallback&& failureCallback, ExceptionCode&);
    void platformVerify(const CryptoAlgorithmRsaSsaParamsDeprecated&, const CryptoKeyRSA&, const CryptoOperationData& signature, const CryptoOperationData&, BoolCallback&&, VoidCallback&& failureCallback, ExceptionCode&);
};

}

#endif // ENABLE(SUBTLE_CRYPTO)
#endif // CryptoAlgorithmRSASSA_PKCS1_v1_5_h

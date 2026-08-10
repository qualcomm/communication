/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "score/mw/com/impl/bindings/lola/proxy_instance_identifier.h"

#include "score/mw/com/impl/configuration/global_configuration.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"
#include "score/mw/log/slot_handle.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <limits>

namespace score::mw::com::impl::lola
{
namespace
{

constexpr GlobalConfiguration::ApplicationId kDummyProcessIdentifier{10U};
constexpr ProxyInstanceIdentifier::ProxyInstanceCounter kDummyProxyInstanceCounter{15U};

TEST(ProxyInstanceIdentifierHashTest, EqualObjectsReturnTheSameHash)
{
    // Given two ProxyInstanceIdentifier objects containing the same values
    const ProxyInstanceIdentifier unit_0{kDummyProcessIdentifier, kDummyProxyInstanceCounter};
    const ProxyInstanceIdentifier unit_1{kDummyProcessIdentifier, kDummyProxyInstanceCounter};

    // When hashing the two objects
    auto hash_result0 = std::hash<ProxyInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<ProxyInstanceIdentifier>{}(unit_1);

    // Then the hash results are the same
    EXPECT_EQ(hash_result0, hash_result1);
}

TEST(ProxyInstanceIdentifierHashTest, EqualObjectsWithMaxValuesReturnTheSameHash)
{
    // Given two ProxyInstanceIdentifier objects containing max values
    const ProxyInstanceIdentifier unit_0{std::numeric_limits<GlobalConfiguration::ApplicationId>::max(),
                                         std::numeric_limits<ProxyInstanceIdentifier::ProxyInstanceCounter>::max()};
    const ProxyInstanceIdentifier unit_1{std::numeric_limits<GlobalConfiguration::ApplicationId>::max(),
                                         std::numeric_limits<ProxyInstanceIdentifier::ProxyInstanceCounter>::max()};

    // When hashing the two objects
    auto hash_result0 = std::hash<ProxyInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<ProxyInstanceIdentifier>{}(unit_1);

    // Then the hash results are the same
    EXPECT_EQ(hash_result0, hash_result1);
}

TEST(ProxyInstanceIdentifierHashTest, ObjectsWithDifferentProcessIdentifierReturnsDifferentHash)
{
    // Given two ProxyInstanceIdentifier objects containing different process identifiers
    const ProxyInstanceIdentifier unit_0{kDummyProcessIdentifier, kDummyProxyInstanceCounter};
    const ProxyInstanceIdentifier unit_1{kDummyProcessIdentifier + 1U, kDummyProxyInstanceCounter};

    // When hashing the two objects
    auto hash_result0 = std::hash<ProxyInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<ProxyInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(ProxyInstanceIdentifierHashTest, ObjectsWithDifferentProxyInstanceCountersReturnsDifferentHash)
{
    // Given two ProxyInstanceIdentifier objects containing different proxy instance counters
    const ProxyInstanceIdentifier unit_0{kDummyProcessIdentifier, kDummyProxyInstanceCounter + 1U};
    const ProxyInstanceIdentifier unit_1{kDummyProcessIdentifier, kDummyProxyInstanceCounter};

    // When hashing the two objects
    auto hash_result0 = std::hash<ProxyInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<ProxyInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(ProxyInstanceIdentifierHashTest,
     ObjectsWithDifferentProcessIdentifiersAndProxyInstanceCountersReturnsDifferentHash)
{
    // Given two ProxyInstanceIdentifier objects containing different process identifiers and proxy instance counters
    const ProxyInstanceIdentifier unit_0{kDummyProcessIdentifier, kDummyProxyInstanceCounter};
    const ProxyInstanceIdentifier unit_1{kDummyProcessIdentifier + 1U, kDummyProxyInstanceCounter + 1U};

    // When hashing the two objects
    auto hash_result0 = std::hash<ProxyInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<ProxyInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(ProxyInstanceIdentifierTest, OperatorStreamOutputsExpectedString)
{
    // Given a ProxyInstanceIdentifier
    const ProxyInstanceIdentifier unit{kDummyProcessIdentifier, kDummyProxyInstanceCounter};

    // Fix for QNX: On QNX the mw::log framework cannot find a valid log mode configuration and falls back to
    // EmptyRecorder (error: "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead").
    // EmptyRecorder discards all output, so testing::internal::CaptureStdout() captures nothing and the
    // HasSubstr assertion always fails. The fix replaces CaptureStdout() with a NiceMock<RecorderMock> injected
    // via SetLogRecorder(), which intercepts LogStringView() calls directly — independent of the logging backend.
    // See also: service_element_type_test.cpp, generic_proxy_test.cpp.
    ::testing::NiceMock<score::mw::log::RecorderMock> recorder_mock{};
    const score::mw::log::SlotHandle handle{0U};

    ON_CALL(recorder_mock, IsLogEnabled(::testing::_, ::testing::_)).WillByDefault(::testing::Return(true));
    ON_CALL(recorder_mock, StartRecord(::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(score::cpp::optional<score::mw::log::SlotHandle>{handle}));

    // The operator<< implementation logs in 4 separate calls:
    //   LogStringView("Application ID:"), LogUint32(application_id),
    //   LogStringView(". Proxy Instance Counter:"), LogUint16(proxy_instance_counter)
    // So we verify each part individually.
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::HasSubstr("Application ID:")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogUint32(::testing::_, static_cast<std::uint32_t>(kDummyProcessIdentifier)))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::HasSubstr(". Proxy Instance Counter:")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogUint16(::testing::_, static_cast<std::uint16_t>(kDummyProxyInstanceCounter)))
        .Times(::testing::AtLeast(1));

    score::mw::log::SetLogRecorder(&recorder_mock);

    // When streaming the ProxyInstanceIdentifier to a log
    score::mw::log::LogFatal("test") << unit;

    // Restore the default recorder
    score::mw::log::SetLogRecorder(nullptr);
}

}  // namespace
}  // namespace score::mw::com::impl::lola

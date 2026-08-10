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
#include "score/mw/com/impl/bindings/lola/skeleton_instance_identifier.h"

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

constexpr LolaServiceId kDummyServiceId{10U};
constexpr LolaServiceInstanceId::InstanceId kDummyInstanceId{15U};

TEST(SkeletonInstanceIdentifierHashTest, EqualObjectsReturnTheSameHash)
{
    // Given two SkeletonInstanceIdentifier objects containing the same values
    const SkeletonInstanceIdentifier unit_0{kDummyServiceId, kDummyInstanceId};
    const SkeletonInstanceIdentifier unit_1{kDummyServiceId, kDummyInstanceId};

    // When hashing the two objects
    auto hash_result0 = std::hash<SkeletonInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<SkeletonInstanceIdentifier>{}(unit_1);

    // Then the hash results are the same
    EXPECT_EQ(hash_result0, hash_result1);
}

TEST(SkeletonInstanceIdentifierHashTest, EqualObjectsWithMaxValuesReturnTheSameHash)
{
    // Given two SkeletonInstanceIdentifier objects containing max values
    const SkeletonInstanceIdentifier unit_0{std::numeric_limits<LolaServiceId>::max(),
                                            std::numeric_limits<LolaServiceInstanceId::InstanceId>::max()};
    const SkeletonInstanceIdentifier unit_1{std::numeric_limits<LolaServiceId>::max(),
                                            std::numeric_limits<LolaServiceInstanceId::InstanceId>::max()};

    // When hashing the two objects
    auto hash_result0 = std::hash<SkeletonInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<SkeletonInstanceIdentifier>{}(unit_1);

    // Then the hash results are the same
    EXPECT_EQ(hash_result0, hash_result1);
}

TEST(SkeletonInstanceIdentifierHashTest, ObjectsWithDifferentServiceIdsReturnsDifferentHash)
{
    // Given two SkeletonInstanceIdentifier objects containing different service IDs
    const SkeletonInstanceIdentifier unit_0{kDummyServiceId, kDummyInstanceId};
    const SkeletonInstanceIdentifier unit_1{kDummyServiceId + 1U, kDummyInstanceId};

    // When hashing the two objects
    auto hash_result0 = std::hash<SkeletonInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<SkeletonInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(SkeletonInstanceIdentifierHashTest, ObjectsWithDifferentInstanceIdsReturnsDifferentHash)
{
    // Given two SkeletonInstanceIdentifier objects containing different instance IDs
    const SkeletonInstanceIdentifier unit_0{kDummyServiceId, kDummyInstanceId + 1U};
    const SkeletonInstanceIdentifier unit_1{kDummyServiceId, kDummyInstanceId};

    // When hashing the two objects
    auto hash_result0 = std::hash<SkeletonInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<SkeletonInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(SkeletonInstanceIdentifierHashTest, ObjectsWithDifferentApplicationIdAndUniqueIdentifierReturnsDifferentHash)
{
    // Given two SkeletonInstanceIdentifier objects containing different application Ids and unique
    // identifiers
    const SkeletonInstanceIdentifier unit_0{kDummyServiceId, kDummyInstanceId};
    const SkeletonInstanceIdentifier unit_1{kDummyServiceId + 1U, kDummyInstanceId + 1U};

    // When hashing the two objects
    auto hash_result0 = std::hash<SkeletonInstanceIdentifier>{}(unit_0);
    auto hash_result1 = std::hash<SkeletonInstanceIdentifier>{}(unit_1);

    // Then the hash results are different
    EXPECT_NE(hash_result0, hash_result1);
}

TEST(SkeletonInstanceIdentifierHashTest, OperatorStreamOutputsExpectedString)
{
    // Given a SkeletonInstanceIdentifier
    const SkeletonInstanceIdentifier unit{kDummyServiceId, kDummyInstanceId};

    // Fix for QNX: On QNX the mw::log framework cannot find a valid log mode configuration and falls back to
    // EmptyRecorder (error: "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead").
    // EmptyRecorder discards all output, so testing::internal::CaptureStdout() captures nothing and the
    // HasSubstr assertion always fails. The fix replaces CaptureStdout() with a NiceMock<RecorderMock> injected
    // via SetLogRecorder(), which intercepts Log*() calls directly — independent of the logging backend.
    // The operator<< logs in 4 separate calls: LogStringView("Service ID:"), LogUint16(service_id),
    // LogStringView(". Instance ID:"), LogUint16(instance_id).
    // See also: proxy_instance_identifier_test.cpp, service_element_type_test.cpp.
    ::testing::NiceMock<score::mw::log::RecorderMock> recorder_mock{};
    const score::mw::log::SlotHandle handle{0U};

    ON_CALL(recorder_mock, IsLogEnabled(::testing::_, ::testing::_)).WillByDefault(::testing::Return(true));
    ON_CALL(recorder_mock, StartRecord(::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(score::cpp::optional<score::mw::log::SlotHandle>{handle}));

    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::HasSubstr("Service ID:")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogUint16(::testing::_, static_cast<std::uint16_t>(kDummyServiceId)))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::HasSubstr(". Instance ID:")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogUint16(::testing::_, static_cast<std::uint16_t>(kDummyInstanceId)))
        .Times(::testing::AtLeast(1));

    score::mw::log::SetLogRecorder(&recorder_mock);

    // When streaming the SkeletonInstanceIdentifier to a log
    score::mw::log::LogFatal("test") << unit;

    // Restore the default recorder
    score::mw::log::SetLogRecorder(nullptr);
}

}  // namespace
}  // namespace score::mw::com::impl::lola

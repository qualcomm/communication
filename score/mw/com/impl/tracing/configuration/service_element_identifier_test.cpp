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
#include "score/mw/com/impl/tracing/configuration/service_element_identifier.h"

#include "score/mw/com/impl/service_element_type.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"
#include "score/mw/log/slot_handle.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unordered_map>

namespace score::mw::com::impl::tracing
{
namespace
{

const std::string kServiceTypeName{"type_name"};
const std::string kServiceElementName{"element_name"};
const ServiceElementType kServiceElementType{ServiceElementType::EVENT};

TEST(ServiceElementIdentifierHashTest, CanHash)
{
    // Given a ServiceElementIdentifier
    const ServiceElementIdentifier service_element_identifier{
        kServiceTypeName, kServiceElementName, kServiceElementType};

    // When calculating the hash of a ServiceElementIdentifier
    auto hash_value = std::hash<ServiceElementIdentifier>{}(service_element_identifier);

    // Then the hash value should be non-zero
    ASSERT_NE(hash_value, 0);
}

TEST(ServiceElementIdentifierHashTest, CanUseAsKeyInMap)
{
    // Given a ServiceElementIdentifier
    const ServiceElementIdentifier service_element_identifier{
        kServiceTypeName, kServiceElementName, kServiceElementType};

    // When using a ServiceElementIdentifier as a key in a map
    std::unordered_map<ServiceElementIdentifier, int> my_map{std::make_pair(service_element_identifier, 10)};

    // Then we compile and don't crash
}

TEST(ServiceElementIdentifierHashTest, HashesOfTheSameServiceElementIdentifiersAreEqual)
{
    // Given 2 ServiceElementIdentifiers with containing the same values
    const ServiceElementIdentifier service_element_identifier{
        "service_type_name", "service_element_name", kServiceElementType};
    const ServiceElementIdentifier service_element_identifier_2{
        "service_type_name", "service_element_name", kServiceElementType};

    // When calculating the hash of the ServiceElementIdentifiers
    auto hash_value = std::hash<ServiceElementIdentifier>{}(service_element_identifier);
    auto hash_value_2 = std::hash<ServiceElementIdentifier>{}(service_element_identifier_2);

    // Then the hash value should be equal
    ASSERT_EQ(hash_value, hash_value_2);
}

TEST(ServiceElementIdentifierHashDeathTest, HashingServiceElementIdentifierWithTooLongStringsTerminates)
{
    constexpr std::size_t max_buffer_size{1024U};

    std::string service_type_name(max_buffer_size, 'a');
    std::string service_element_name(max_buffer_size, 'b');

    // Given 2 ServiceElementIdentifiers with containing the same values
    const ServiceElementIdentifier service_element_identifier{
        service_type_name, service_element_name, kServiceElementType};

    // When calculating the hash of the ServiceElementIdentifier
    EXPECT_DEATH(std::hash<ServiceElementIdentifier>{}(service_element_identifier), ".*");
}

class ServiceElementIdentifierEqualityFixture
    : public ::testing::TestWithParam<std::pair<ServiceElementIdentifier, ServiceElementIdentifier>>
{
};

TEST_P(ServiceElementIdentifierEqualityFixture, HashesOfTheDifferentServiceElementIdentifiersAreNotEqual)
{
    const auto service_element_identifiers = GetParam();

    // Given 2 ServiceElementIdentifiers containing different values
    const auto service_element_identifier = service_element_identifiers.first;
    const auto service_element_identifier_2 = service_element_identifiers.second;

    // When calculating the hash of the ServiceElementIdentifiers
    auto hash_value = std::hash<ServiceElementIdentifier>{}(service_element_identifier);
    auto hash_value_2 = std::hash<ServiceElementIdentifier>{}(service_element_identifier_2);

    // Then the hash value should be different
    ASSERT_NE(hash_value, hash_value_2);
}

TEST_P(ServiceElementIdentifierEqualityFixture, DifferentServiceElementIdentifiersAreNotEqual)
{
    const auto service_element_identifiers = GetParam();

    // Given 2 ServiceElementIdentifiers containing different values
    const auto service_element_identifier = service_element_identifiers.first;
    const auto service_element_identifier_2 = service_element_identifiers.second;

    // Then the equality operator should return false
    ASSERT_FALSE(service_element_identifier == service_element_identifier_2);
}

INSTANTIATE_TEST_CASE_P(
    ServiceElementIdentifierEqualityFixture,
    ServiceElementIdentifierEqualityFixture,
    ::testing::Values(
        std::make_pair(ServiceElementIdentifier{"same_type_name", "same_element_name", ServiceElementType::EVENT},
                       ServiceElementIdentifier{"different_type_name", "same_element_name", ServiceElementType::EVENT}),
        std::make_pair(ServiceElementIdentifier{"same_type_name", "same_element_name", ServiceElementType::EVENT},
                       ServiceElementIdentifier{"same_type_name", "different_element_name", ServiceElementType::EVENT}),
        std::make_pair(ServiceElementIdentifier{"same_type_name", "same_element_name", ServiceElementType::EVENT},
                       ServiceElementIdentifier{"same_type_name", "same_element_name", ServiceElementType::FIELD})));

TEST(ServiceElementIdentifierComparisonTest, ComparingTheSameServiceElementIdentifierReturnsFalse)
{
    // Given an ServiceElementIdentifier
    ServiceElementIdentifier service_element_identifier_view{"a", "b", static_cast<ServiceElementType>(1U)};

    // Then the comparing the same ServiceElementIdentifier should return false
    ASSERT_FALSE(service_element_identifier_view < service_element_identifier_view);
}

class ServiceElementIdentifierComparisonFixture
    : public ::testing::TestWithParam<std::pair<ServiceElementIdentifier, ServiceElementIdentifier>>
{
};

TEST_P(ServiceElementIdentifierComparisonFixture, ServiceElementIdentifierComparisonReturnsCorrectResult)
{
    const auto service_element_identifiers = GetParam();

    // Given 2 ServiceElementIdentifiers where the first value is smaller than the second value
    const auto service_element_identifier_view = service_element_identifiers.first;
    const auto service_element_identifier_view_2 = service_element_identifiers.second;

    // Then the comparison operator should return true
    ASSERT_TRUE(service_element_identifier_view < service_element_identifier_view_2);
}

INSTANTIATE_TEST_CASE_P(
    ServiceElementIdentifierComparisonFixture,
    ServiceElementIdentifierComparisonFixture,
    ::testing::Values(std::make_pair(ServiceElementIdentifier{"a", "c", static_cast<ServiceElementType>(1U)},
                                     ServiceElementIdentifier{"b", "b", static_cast<ServiceElementType>(0U)}),
                      std::make_pair(ServiceElementIdentifier{"a", "b", static_cast<ServiceElementType>(1U)},
                                     ServiceElementIdentifier{"a", "c", static_cast<ServiceElementType>(0U)}),
                      std::make_pair(ServiceElementIdentifier{"a", "b", static_cast<ServiceElementType>(0U)},
                                     ServiceElementIdentifier{"a", "b", static_cast<ServiceElementType>(1U)})));

TEST(ServiceElementIdentifierStreamTest, OperatorStreamOutputsServiceElementData)
{
    // Given a ServiceElementIdentifier
    const ServiceElementIdentifier service_element_identifier{"TestType", "TestElement", ServiceElementType::EVENT};

    // Fix for QNX: On QNX the mw::log framework cannot find a valid log mode configuration and falls back to
    // EmptyRecorder (error: "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead").
    // EmptyRecorder discards all output, so testing::internal::CaptureStdout() captures nothing and the
    // HasSubstr assertions always fail. The fix replaces CaptureStdout() with a NiceMock<RecorderMock> injected
    // via SetLogRecorder(), which intercepts LogStringView() calls directly — independent of the logging backend.
    // The operator<< logs in 6 separate LogStringView calls; we use StrEq to avoid substring conflicts
    // (e.g. ", service element: " is a substring of ", service element type: ").
    // See also: service_element_type_test.cpp, proxy_instance_identifier_test.cpp.
    ::testing::NiceMock<score::mw::log::RecorderMock> recorder_mock{};
    const score::mw::log::SlotHandle handle{0U};

    ON_CALL(recorder_mock, IsLogEnabled(::testing::_, ::testing::_)).WillByDefault(::testing::Return(true));
    ON_CALL(recorder_mock, StartRecord(::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(score::cpp::optional<score::mw::log::SlotHandle>{handle}));

    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq("service type: ")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq("TestType")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq(", service element: ")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq("TestElement")))
        .Times(::testing::AtLeast(1));
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq(", service element type: ")))
        .Times(::testing::AtLeast(1));
    // ServiceElementType::EVENT is logged as the exact string "EVENT" by its operator<<
    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::StrEq("EVENT")))
        .Times(::testing::AtLeast(1));

    score::mw::log::SetLogRecorder(&recorder_mock);

    // When streaming the ServiceElementIdentifier to a log
    score::mw::log::LogFatal("test") << service_element_identifier;

    // Restore the default recorder
    score::mw::log::SetLogRecorder(nullptr);
}

}  // namespace
}  // namespace score::mw::com::impl::tracing

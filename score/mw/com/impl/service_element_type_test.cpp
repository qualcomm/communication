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
#include "score/mw/com/impl/service_element_type.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"
#include "score/mw/log/slot_handle.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score::mw::com::impl
{
namespace
{

/// \brief Helper: installs RecorderMock, logs the value, checks the expected substring, restores recorder.
///
/// \details On QNX the mw::log framework cannot find a valid log mode configuration and falls back to
/// EmptyRecorder (error: "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead").
/// EmptyRecorder discards all output, so testing::internal::CaptureStdout() captures nothing and the
/// HasSubstr assertions always fail. The fix replaces CaptureStdout() with a NiceMock<RecorderMock> injected
/// via SetLogRecorder(), which intercepts LogStringView() calls directly — independent of the logging backend.
/// This is the same pattern used in find_service_handle_test.cpp and generic_proxy_test.cpp.
void ExpectLogContains(const ServiceElementType& service_element_type, const std::string_view expected_substr)
{
    ::testing::NiceMock<score::mw::log::RecorderMock> recorder_mock{};
    const score::mw::log::SlotHandle handle{0U};

    ON_CALL(recorder_mock, IsLogEnabled(::testing::_, ::testing::_)).WillByDefault(::testing::Return(true));
    ON_CALL(recorder_mock, StartRecord(::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(score::cpp::optional<score::mw::log::SlotHandle>{handle}));

    EXPECT_CALL(recorder_mock, LogStringView(::testing::_, ::testing::HasSubstr(expected_substr)))
        .Times(::testing::AtLeast(1));

    score::mw::log::SetLogRecorder(&recorder_mock);
    score::mw::log::LogFatal("test") << service_element_type;
    score::mw::log::SetLogRecorder(nullptr);
}

TEST(ServiceElementTypeTest, DefaultConstructedEnumValueIsInvalid)
{
    // Given a default constructed ServiceElementType
    ServiceElementType service_element_type{};

    // Then the value of the enum should be invalid
    EXPECT_EQ(service_element_type, ServiceElementType::INVALID);
}

TEST(ServiceElementTypeTest, OperatorStreamOutputsInvalidWhenTypeIsInvalid)
{
    // Given a ServiceElementType set to INVALID
    // When streaming to a log, then the output should contain "INVALID"
    ExpectLogContains(ServiceElementType::INVALID, "INVALID");
}

TEST(ServiceElementTypeTest, OperatorStreamOutputsEventWhenTypeIsEvent)
{
    // Given a ServiceElementType set to EVENT
    // When streaming to a log, then the output should contain "EVENT"
    ExpectLogContains(ServiceElementType::EVENT, "EVENT");
}

TEST(ServiceElementTypeTest, OperatorStreamOutputsFieldWhenTypeIsField)
{
    // Given a ServiceElementType set to FIELD
    // When streaming to a log, then the output should contain "FIELD"
    ExpectLogContains(ServiceElementType::FIELD, "FIELD");
}

TEST(ServiceElementTypeTest, OperatorStreamOutputsMethodWhenTypeIsMethod)
{
    // Given a ServiceElementType set to METHOD
    // When streaming to a log, then the output should contain "METHOD"
    ExpectLogContains(ServiceElementType::METHOD, "METHOD");
}

TEST(ServiceElementTypeTest, OperatorStreamOutputsUnknownWhenTypeIsUnrecognized)
{
    // Given a ServiceElementType set to an unrecognized value
    // When streaming to a log, then the output should contain "UNKNOWN"
    ExpectLogContains(static_cast<ServiceElementType>(100U), "UNKNOWN");
}

}  // namespace
}  // namespace score::mw::com::impl

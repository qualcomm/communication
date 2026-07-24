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
#include "score/mw/com/impl/bindings/lola/methods/offered_state_machine.h"

#include <score/assert.hpp>

namespace score::mw::com::impl::lola
{

OfferedStateMachine::OfferedStateMachine()
    : current_state_{State::OFFERED},
      states_{std::make_unique<detail::OfferedState>(),
              std::make_unique<detail::StopOfferedState>(),
              std::make_unique<detail::ReOfferedState>()}
{
}

void OfferedStateMachine::Offer()
{
    current_state_ = states_.at(static_cast<std::uint8_t>(current_state_))->Offer();  // parasoft-suppress MISRACPP2023-7_0_6-a "D-021: deviation - widening uint8_t -> std::array::size_type, no narrowing"
}

void OfferedStateMachine::StopOffer()
{
    current_state_ = states_.at(static_cast<std::uint8_t>(current_state_))->StopOffer();  // parasoft-suppress MISRACPP2023-7_0_6-a "D-022: false positive - same rationale as D-021"
}

namespace detail
{

OfferedStateMachine::State OfferedState::Offer()
{
    return OfferedStateMachine::State::OFFERED;
}

OfferedStateMachine::State OfferedState::StopOffer()
{
    return OfferedStateMachine::State::STOP_OFFERED;
}

OfferedStateMachine::State StopOfferedState::Offer()
{
    return OfferedStateMachine::State::RE_OFFERED;
}

OfferedStateMachine::State StopOfferedState::StopOffer()
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(0);  // parasoft-suppress MISRACPP2023-7_0_2-a "D-023: deviation - SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD literal 0 coerced to bool inside the assertion macro is the project assertion idiom"
}

OfferedStateMachine::State ReOfferedState::Offer()
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(0);  // parasoft-suppress MISRACPP2023-7_0_2-a "D-024: same rationale as D-023"
}

OfferedStateMachine::State ReOfferedState::StopOffer()
{
    return OfferedStateMachine::State::STOP_OFFERED;
}

}  // namespace detail
}  // namespace score::mw::com::impl::lola

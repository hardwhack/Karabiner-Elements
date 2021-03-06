#define CATCH_CONFIG_MAIN
#include "../../vendor/catch/catch.hpp"

#include "grabbable_state_queue.hpp"
#include "thread_utility.hpp"

namespace {
auto registry_entry_id1 = krbn::registry_entry_id(1);
} // namespace

TEST_CASE("initialize") {
  krbn::thread_utility::register_main_thread();
}

TEST_CASE("grabbable_state_queue") {
  {
    krbn::grabbable_state_queue queue;

    REQUIRE(!queue.find_current_grabbable_state());
    REQUIRE(!queue.get_first_grabbed_event_time_stamp());

    for (auto time_stamp = 1000ULL; time_stamp < 10000ULL; time_stamp += 1000ULL) {
      auto state = std::make_shared<krbn::grabbable_state>(registry_entry_id1,
                                                           krbn::grabbable_state::state::grabbable,
                                                           krbn::grabbable_state::ungrabbable_temporarily_reason::none,
                                                           time_stamp);
      queue.push_back_grabbable_state(state);

      REQUIRE(*(queue.find_current_grabbable_state()) == *state);
    }

    // Remove states after first_grabbed_event_time_stamp_.
    queue.update_first_grabbed_event_time_stamp(5000ULL);
    REQUIRE(*(queue.get_first_grabbed_event_time_stamp()) == 5000ULL);
    REQUIRE(queue.find_current_grabbable_state()->get_time_stamp() == 4000ULL);

    // Ignore events after first_grabbed_event_time_stamp_.
    {
      auto state = std::make_shared<krbn::grabbable_state>(registry_entry_id1,
                                                           krbn::grabbable_state::state::grabbable,
                                                           krbn::grabbable_state::ungrabbable_temporarily_reason::none,
                                                           6000ULL);
      queue.push_back_grabbable_state(state);
      REQUIRE(queue.find_current_grabbable_state()->get_time_stamp() == 4000ULL);
    }
    {
      auto state = std::make_shared<krbn::grabbable_state>(registry_entry_id1,
                                                           krbn::grabbable_state::state::grabbable,
                                                           krbn::grabbable_state::ungrabbable_temporarily_reason::none,
                                                           4500ULL);
      queue.push_back_grabbable_state(state);
      REQUIRE(queue.find_current_grabbable_state()->get_time_stamp() == 4500ULL);
    }
  }
}

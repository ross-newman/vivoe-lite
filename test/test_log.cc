#define CATCH_CONFIG_MAIN
#include <unistd.h>

#include <catch/catch.hpp>
#include <iostream>
#include <string>

#include "events_gva.h"
#include "gva.h"
#include "log_gva.h"

using namespace gva;

static EventsGva *events;

TEST_CASE("Log", "Logging tests") {
  std::string test1 = "This is a string test";
  char test2[] = "This is a char* test";
  //  logGva::log(test1, LOG_INFO);
  logGva::log(test2, LOG_INFO);
  logGva::log(test2, LOG_DEBUG);
  logGva::log(test2, LOG_ERROR);
  logGva::log(test2, LOG_WARNING);
  logGva::finish();

  REQUIRE(0 == 0);
}

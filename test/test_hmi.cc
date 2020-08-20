#define CATCH_CONFIG_MAIN
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include <catch/catch.hpp>
#include <iostream>

#include "eventsGva.hpp"
#include "gva.hpp"
#include "hmiGva.hpp"

using namespace gva;

static eventsGva *events;

TEST_CASE("HMI Test", "Constructor test") {
  int i = 0;
  REQUIRE(i == 0);
}

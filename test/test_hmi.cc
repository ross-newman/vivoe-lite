#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gva.hpp"
#include "eventsGva.hpp"
#include "hmiGva.hpp"

using namespace gva;

static eventsGva *events;

TEST_CASE( "HMI Test", "Constructor test" ){
  int i = 0;
  REQUIRE( i==0 );
}



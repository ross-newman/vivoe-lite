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

static eventsGva *events = 0;

TEST_CASE( "eventsGva Constructor" ){  
  events = new eventsGva();

  REQUIRE( events !=0 );
}

TEST_CASE( "eventsGva flush" ){
  events->flush();
  
  REQUIRE( events !=0 );
  free(events);
}

TEST_CASE( "eventsGva Constructor2" ){
  // instantiate events
  KeyPowerOn on;

  hmi::start();
  hmi::dispatch(on);    

  eventsGva io(hmi::getRendrer()->getDisplay(), hmi::getRendrer()->getWindow());
  
  REQUIRE( hmi::getRendrer()->getDisplay() !=0 );
  REQUIRE( hmi::getRendrer()->getWindow() !=0 );
  REQUIRE( events !=0 );
}

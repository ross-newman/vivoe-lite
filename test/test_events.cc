#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gva.h"
#include "events_gva.h"
#include "hmi_gva.h"

using namespace gva;

static eventsGva *events = 0;

TEST_CASE( "eventsGva Constructor1", "Constructor test" ){  
  events = new eventsGva();

  REQUIRE( events !=0 );
}

TEST_CASE( "eventsGva", "flush" ){
  events->flush();
  
  REQUIRE( events !=0 );
  free(events);
}

TEST_CASE( "eventsGva Constructor2", "Constructor test2" ){
  // instantiate events
  KeyPowerOn on;

  hmi::start();
  hmi::dispatch(on);    

  eventsGva io(hmi::getRendrer()->getDisplay(), hmi::getRendrer()->getWindow(), hmi::getRendrer()->getTouch());
  
  REQUIRE( hmi::getRendrer()->getDisplay() !=0 );
  REQUIRE( hmi::getRendrer()->getWindow() !=0 );
  REQUIRE( events !=0 );
}


#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <iostream>
#include <unistd.h>
#include "gva.h"
#include "hmi_gva.h"
#include "events_gva.h"

using namespace gva;

static EventsGva *events = 0;

TEST_CASE( "eventsGva Constructor1", "Constructor test" ){  
  events = new EventsGva(hmi::GetRendrer()->getWindow(), hmi::GetRendrer()->getTouch());

  REQUIRE( events !=0 );
}

TEST_CASE( "eventsGva", "flush" ){
//  events->flush();
  
  REQUIRE( events !=0 );
  free(events);
}

TEST_CASE( "eventsGva Constructor2", "Constructor test2" ){
  // instantiate events
  EventKeyPowerOn on;

  hmi::start();
  hmi::dispatch(on);    

  EventsGva io(hmi::GetRendrer()->getWindow(), hmi::GetRendrer()->getTouch());
  
  REQUIRE( hmi::GetRendrer()->getWindow() !=0 );
  REQUIRE( events !=0 );
}


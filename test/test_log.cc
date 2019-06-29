#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gva.h"
#include "events_gva.h"
#include "log_gva.h"

using namespace gva;

static eventsGva *events;

TEST_CASE( "Log", "Logging tests" ){
  std::string test1 = "This is a string test";
  char test2[] = "This is a char* test";
//  logGva::log(test1, LOG_INFO); 
  logGva::log(test2, LOG_INFO); 
  logGva::log(test2, LOG_DEBUG); 
  logGva::log(test2, LOG_ERROR); 
  logGva::log(test2, LOG_WARNING); 
  logGva::finish(); 

  REQUIRE( 0==0 );
}



#!/bin/bash

SENSOR=$1

case "$SENSOR" in
  # Sights
  1)
    IPADDR=239.192.3.103 #PrimDay
    ;;
  2)
    IPADDR=239.192.3.104 #PrimTi
    ;;
  3)
    IPADDR=239.192.2.202 #PrimEng
    ;;
  4)
    IPADDR=239.192.1.202 #PrimEng
    ;;
  5)
    IPADDR=239.192.1.119 #JavlinSight 
    ;;

  # Cameras
  6)
    IPADDR=239.192.1.44
    ;;
  7)
    IPADDR=239.192.1.45
    ;;
  8)
    IPADDR=239.192.1.41
    ;;
  9)
    IPADDR=239.192.1.42
    ;;
  10)
    IPADDR=239.192.1.43
    ;;
  11)
    IPADDR=239.192.1.46
    ;;
  12)
    IPADDR=239.192.6.110
    ;;
  ## SCOUT FCC Manager
  13)
    IPADDR=239.192.3.105
    ;;
  14)
    IPADDR=239.192.3.106
    ;;
  ## WSCP FCC Manager
  15)
    IPADDR=239.192.3.107
    ;;
  *)
    echo "ERROR: Invalid sensor selected $SENSOR."
    exit 1
    ;;
esac


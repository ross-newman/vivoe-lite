# Developer Notes
Below are a few developer notes.

# Version and Release numbering
Version numbering follows the semver standard fro MAJOR, MINOR and PATCH See https://semver.org/

# Coding Standard
This project will follow the Google C++ coding standard https://google.github.io/styleguide/cppguide.html

# Enhancements
New features may be added from time to time. Raise a bug report to request a feature. If adopted then the freature must be covered by the automated tests and executed on travis-ci.org. 

# Vunrabilities
If you find an issue or problem with the code you can raise a bug detailing the issue and steps required to reproduce it. This will then get addressed. https://github.com/ross-newman/vivoe-lite/issues

# Notes
## Features under development
Current features being worked on.
- [x] Live GPS (USB) source support
- [x] Offline maps using libosmscout
- [ ] Route planning libosmscout
- [ ] Display incomming live uncompressed RTP(YUV) video streams
- [ ] Display outgoing live uncompressed RTP(YUV) video streams
- [ ] DDS control via gva-tool
- [ ] GVA Display emulation via gva-tool
- [ ] XML config files need implementing
- [ ] Alarms class
- [ ] Keyboard input working with touch input (mouse click)

## Known Issues
* Not all labels are implemented and touch (mouse click enabled)
* Maps BMS currently mocked up
* Live video input macro defined out (Issue with Gstreamer1.0)

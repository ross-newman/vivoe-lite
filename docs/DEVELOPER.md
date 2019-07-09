# Developer Notes
Below are a few developer notes.

# Version and Release numbering
Version numbering follows the semver standard fro MAJOR, MINOR and PATCH See https://semver.org/

# Coding Standard
This project will follow the Google C++ coding standard https://google.github.io/styleguide/cppguide.html

# Enhancements
New features may be added from time to time. Raise a bug report to request a feature. If adopted then the freature must be covered by the automated tests and executed on travis-ci.org. 

## Reporting problems, asking questions
We appreciate feedback, questions or bug reports. When you need help with the code, follow the process outlined in the Stack Overflow https://stackoverflow.com/help/mcve document. Ensure that the posted examples are:

- **minimal**: Use as little code as possible that still produces the same problem.
- **complete**: Provide all parts needed to reproduce the problem. Check if you can strip external dependency and still show the problem. The less time we spend on reproducing the problems, the more time we can dedicate to the fixes.
- **verifiable**: Test the code you are about to provide, to make sure that it reproduces the problem. Remove all other problems that are not related to your request.

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

For a complete insite to current development then why not contribute and pick a task for our [Zenhub](http://zenhub.com) backlog to help out.

## Known Issues
* Not all labels are implemented and touch (mouse click enabled)
* Maps BMS currently mocked up
* Live video input macro defined out (Issue with Gstreamer1.0)

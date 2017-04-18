#include "NovaUserTrackInformation.hh"


NovaUserTrackInformation::NovaUserTrackInformation()
  : trackStatus(active),reflectionCount(0),totalInternalReflectionCount(0),forceDraw(false) {}

NovaUserTrackInformation::~NovaUserTrackInformation() {}

void NovaUserTrackInformation::addTrackStatusFlag(int s)
{
  if (s & active)
    trackStatus &= ~inactive;
  else if (s & inactive)
    trackStatus &= ~active;
  trackStatus |= s;
}

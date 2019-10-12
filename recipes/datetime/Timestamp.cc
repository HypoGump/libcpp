#include "Timestamp.h"

using namespace libcpp;

Timestamp::Timestamp()
 : tpoint_()
{
}

~Timestamp::Timestamp(TimePoint t)
 : tpoint_(t)
{
}

TimePoint Timestamp::now() const
{
  return SteadyClock::now();
}

std::string Timestamp::toString()
{
  return ;
}

std::string Timestamp::toFormattedString()
{
  return ;
}
#include "eventmaskset.h"

using namespace soma::recorder; 

EventMaskSet::EventMaskSet() :
  pMaskLUT_(new char[EVENT_SRC_MAX * EVENT_CMD_MAX])
{
  allset(false);  // unset all
}



EventMaskSet::~EventMaskSet()
{
  delete[] pMaskLUT_; 


}

void EventMaskSet::allset(bool state)
{
  for (int i = 0; i < EVENT_SRC_MAX; i++) {
    for (int j = 0; j < EVENT_CMD_MAX; j++) { 
      set(i, j, state); 
    }
  }


}

bool EventMaskSet::isSet(eventsource_t src, eventcmd_t cmd) const
{
  if (pMaskLUT_[src * EVENT_SRC_MAX + cmd] == 0) {
    return false; 
  } else {
    return true; 
  }
  

}

void EventMaskSet::set(eventsource_t src, eventcmd_t cmd, bool state)
{
  if (state) {
    pMaskLUT_[src * EVENT_SRC_MAX + cmd] = 1; 
  } else {
    pMaskLUT_[src * EVENT_SRC_MAX + cmd] =0 ; 

  } 

}

void EventMaskSet::set(eventsource_t src, const std::vector<eventcmd_t> & cmds, 
		       bool state) {
  
  for(std::vector<eventcmd_t>::const_iterator i = cmds.begin(); 
      i != cmds.end(); ++i) {
    set(src, *i, state); 
  }
  
  
}

#ifndef big_event_h
#define big_event_h

#include "SmallEvent.h"

class BigEvent {

  public:
    inline BigEvent(SmallEvent a1, SmallEvent a2){
      deex = a1;
      decay = a2;
    }

  private:
    SmallEvent deex;
    SmallEvent decay;
};

#endif

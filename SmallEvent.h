#ifndef small_event_h
#define small_event_h

class SmallEvent {

  public:
    inline SmallEvent(int a1, double a2, SmallEvent::type a3){
      index = a1;
      event_time = a2;
      type = a3;
    }
    enum type {
        deexcitation,
        decay_pPs,
        decay_oPs
    };

  private:
    int index;
    double event_time;
    SmallEvent::type type;
};

#endif

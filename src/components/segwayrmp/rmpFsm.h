#ifndef _H_RmpFsm
#define _H_RmpFsm
#include <stddef.h>
#include "rmpcontext.h"
class RmpFsm;

class RmpFsmState {
public:

  virtual const char* StateName() const = 0;
  virtual void repair(RmpFsm& s);
  virtual void deactivate(RmpFsm& s);
  virtual void activate(RmpFsm& s);
};

class RmpFsmFaultHardwareState : public RmpFsmState {
public:
  virtual const char* StateName() const
  {return("FaultHardware");};
  virtual void repair(RmpFsm&);
};

class RmpFsmFaultRemoteObjectState : public RmpFsmState {
public:
  virtual const char* StateName() const
  {return("FaultRemoteObject");};
  virtual void repair(RmpFsm&);
};

class RmpFsmFaultNamingServiceState : public RmpFsmState {
public:
  virtual const char* StateName() const
  {return("FaultNamingService");};
  virtual void repair(RmpFsm&);
};

class RmpFsmActiveState : public RmpFsmState {
public:
  virtual const char* StateName() const
  {return("Active");};
  virtual void deactivate(RmpFsm&);
};

class RmpFsmIdleState : public RmpFsmState {
public:
  virtual const char* StateName() const
  {return("Idle");};
  virtual void activate(RmpFsm&);
};
class RmpFsm : public RmpContext {
  public:
  static RmpFsmFaultHardwareState FaultHardwareState;
  static RmpFsmFaultRemoteObjectState FaultRemoteObjectState;
  static RmpFsmFaultNamingServiceState FaultNamingServiceState;
  static RmpFsmActiveState ActiveState;
  static RmpFsmIdleState IdleState;
  RmpFsm();// default constructor
  void repair() {itsState->repair(*this);}
  void deactivate() {itsState->deactivate(*this);}
  void activate() {itsState->activate(*this);}
  void SetState(RmpFsmState& theState) {itsState=&theState;}
  RmpFsmState& GetState() const {return *itsState;};
  private:
    RmpFsmState* itsState;
};
#endif

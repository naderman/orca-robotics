#ifndef _H_TeleopFsm
#define _H_TeleopFsm
#include <stddef.h>
class TeleopFsm;

class TeleopFsmState {
public:

  virtual const char* StateName() const = 0;
  virtual void humanShutdown(TeleopFsm& s);
  virtual void activate(TeleopFsm& s);
};

class TeleopFsmActiveState : public TeleopFsmState {
public:
  virtual const char* StateName() const
  {return("Active");};
  virtual void humanShutdown(TeleopFsm&);
};

class TeleopFsmIdleState : public TeleopFsmState {
public:
  virtual const char* StateName() const
  {return("Idle");};
  virtual void activate(TeleopFsm&);
};


class TeleopFsm {
  public:
  static TeleopFsmActiveState ActiveState;
  static TeleopFsmIdleState IdleState;
  TeleopFsm();// default constructor
  void humanShutdown() {itsState->humanShutdown(*this);}
  void activate() {itsState->activate(*this);}
  void SetState(TeleopFsmState& theState) {itsState=&theState;}
  TeleopFsmState& GetState() const {return *itsState;};

  // to be implemented by the derived class
  virtual void startup() = 0;
  virtual void shutdown() = 0;
  virtual void FSMError(const char* t, const char* s) = 0;

  private:
    TeleopFsmState* itsState;
};
#endif

#include "teleopfsm.h"
//static char _versID[] = "Version $Id$";
TeleopFsmActiveState TeleopFsm::ActiveState;
TeleopFsmIdleState TeleopFsm::IdleState;
void TeleopFsmState::deactivate(TeleopFsm& s)
  {s.FSMError("deactivate", s.GetState().StateName());}
void TeleopFsmState::activate(TeleopFsm& s)
  {s.FSMError("activate", s.GetState().StateName());}
void TeleopFsmActiveState::deactivate(TeleopFsm& s) {
  s.SetState(TeleopFsm::IdleState);
  s.shutdown();
}
void TeleopFsmIdleState::activate(TeleopFsm& s) {
  s.SetState(TeleopFsm::ActiveState);
  s.startup();
}
TeleopFsm::TeleopFsm() : itsState(&IdleState) {}

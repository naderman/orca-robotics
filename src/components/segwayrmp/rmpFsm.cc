#include "rmpFsm.h"
static char _versID[] = "No Version.";
RmpFsmFaultHardwareState RmpFsm::FaultHardwareState;
RmpFsmFaultRemoteObjectState RmpFsm::FaultRemoteObjectState;
RmpFsmFaultNamingServiceState RmpFsm::FaultNamingServiceState;
RmpFsmActiveState RmpFsm::ActiveState;
RmpFsmIdleState RmpFsm::IdleState;
void RmpFsmState::repair(RmpFsm& s)
  {s.FSMError("repair", s.GetState().StateName());}
void RmpFsmState::deactivate(RmpFsm& s)
  {s.FSMError("deactivate", s.GetState().StateName());}
void RmpFsmState::activate(RmpFsm& s)
  {s.FSMError("activate", s.GetState().StateName());}
void RmpFsmFaultHardwareState::repair(RmpFsm& s) {
  s.SetState(RmpFsm::ActiveState);
  s.repairHardware();
}
void RmpFsmFaultRemoteObjectState::repair(RmpFsm& s) {
  s.SetState(RmpFsm::ActiveState);
  s.repairRemoteObject();
}
void RmpFsmFaultNamingServiceState::repair(RmpFsm& s) {
  s.SetState(RmpFsm::ActiveState);
  s.repairNamingService();
}
void RmpFsmActiveState::deactivate(RmpFsm& s) {
  s.SetState(RmpFsm::IdleState);
  s.deactivate();
}
void RmpFsmIdleState::activate(RmpFsm& s) {
  s.SetState(RmpFsm::ActiveState);
  s.activate();
}
RmpFsm::RmpFsm() : itsState(&IdleState) {}

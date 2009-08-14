#include "../message.h"
#include <iostream>

using namespace mhipa10arc;
using namespace std;

int main (void)
{
	Message mymsg(45);
	cout << "mymsg:" << endl << mymsg.toString() << endl;

	hydrointerfaces::MitsubishiPA10::ManipulatorCommand cmd;
	for (unsigned int n=0; n<7; ++n)
	{
		hydrointerfaces::MitsubishiPA10::ServoCmd servo;
		servo.brakeON = true;
		servo.isTorqueCmd = true;
		servo.servoON = false;
		servo.cmdValue = 1.0;

		cmd.servoCmds.push_back(servo);
	}
	MotionCommandFactory fact(cmd);
	MessagePtr cmdmsg = fact.createMessage();

	cout << "cmdmsg:" << endl << cmdmsg->toString() << endl;
}


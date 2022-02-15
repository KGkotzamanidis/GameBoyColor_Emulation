#include "motherboard.h"
#include "bus.h"

void MOTHERBOARD::Initialize(class BUS* _sendSignal) {
	sendSignal = _sendSignal;
}

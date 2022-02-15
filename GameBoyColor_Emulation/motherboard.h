#ifndef  _MOTHERBOARD_H
class MOTHERBOARD {
	public:
		void Initialize(class BUS* _sendSignal);
	protected:
		class BUS* sendSignal;
};
#endif
class Notification {
	public:
		virtual void sendMessage(const char* message) = 0;
		virtual char* receiveMessage() = 0;
};

class Notification {
	public:
		virtual void sendMessage(const char* message) = 0;
		virtual std::string receiveMessage() = 0;
};

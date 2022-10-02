class Notification {
	public:
		virtual void sendMessage(std::string message) = 0;
		virtual std::string receiveMessage() = 0;
};

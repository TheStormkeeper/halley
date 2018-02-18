#pragma once
#include <memory>
#include "halley/text/halleystring.h"
#include "halley/net/connection/reliable_connection.h"
#include "halley/net/connection/message_queue.h"
#include "halley/support/logger.h"
#include "devcon_messages.h"

namespace Halley
{
	class NetworkService;

	class DevConClient : private ILoggerSink
	{
	public:
		DevConClient(std::unique_ptr<NetworkService> service, const String& address, int port = devConPort);
		~DevConClient();

		void update();

	private:
		std::unique_ptr<NetworkService> service;
		String address;
		int port;

		std::shared_ptr<ReliableConnection> connection;
		std::shared_ptr<MessageQueue> queue;

		void connect();
		void log(LoggerLevel level, const String& msg) override;
	};
}
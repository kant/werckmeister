#ifndef FMAPP_NET_UDPSEND_HPP
#define FMAPP_NET_UDPSEND_HPP

#include <tuple>
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <fm/common.hpp>
#include <boost/core/noncopyable.hpp>

namespace funk {
	/**
	 * test the connection using: socat UDP-RECV:$port STDOUT
	 */ 
	class UdpSender : boost::noncopyable {
		public:
			typedef std::vector<fm::Byte> Bytes;
			void start();
			void stop();
			void send(const Bytes &bytes);
		private:
			typedef std::string Host;
			typedef std::string Port;
			std::tuple<Host, Port> getHostInfo(const std::string &str) const;
			typedef boost::asio::ip::udp::socket Socket;
			typedef std::shared_ptr<Bytes> BytesPtr;
			typedef std::unique_ptr<Socket> SocketPtr;
			typedef boost::asio::ip::udp::endpoint Endpoint;
			typedef std::unique_ptr<Endpoint> EndpointPtr;
			typedef boost::system::error_code ErrorCode;
			typedef boost::asio::io_service Service;
			Service _service;
			SocketPtr _socket;
			Endpoint _endpoint;
		protected:
			void onSend(const ErrorCode &error, BytesPtr bytes, std::size_t numBytes);
		public:
			UdpSender(const std::string &host);
	};
}

#endif

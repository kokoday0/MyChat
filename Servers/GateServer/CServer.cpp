#include "CServer.h"
#include"HttpConnection.h"
CServer::CServer(io_server& io,  short port)
	:_acceptor(io,tcp::endpoint(tcp::v4(),port)),_io(io),_socket(io),_port(port)
{
	
}

void CServer::Start()
{
	auto self = shared_from_this();
	std::cout << "Http Server Start Listening..." << std::endl;
	_acceptor.async_accept(_socket, [self](beast::error_code err) {
		try
		{
			if (err)
			{
				self->Start();
				return;
			}
			//处理新连接
			std::cout << "Get Http Connection Success" << std::endl;
			std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
			self->Start();
		}
		catch (std::exception& exp)
		{
			std::cout << "Server Accept Fail, Exp is : " << exp.what() << std::endl;
			self->Start();
		}
		});
}

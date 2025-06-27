#include "VarifyImplService.h"
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/NetException.h>
#include <random>

VarifyImplService::VarifyImplService()
{
}
grpc::Status VarifyImplService::GetVarifyCode(grpc::ServerContext* context, const message::GetVarifyReq* req, message::GetVarifyRsp* rsp)
{
	//读取目标email
	auto email = req->email();
	bool success = SendEmail(email);
	if (!success) return grpc::Status::CANCELLED;
	return grpc::Status::OK;
}

bool VarifyImplService::SendEmail(std::string email)
{
	try
	{
		//连接SMTP服务器
		Poco::Net::SecureSMTPClientSession smtp("smtp.163.com", 25);
		smtp.login(Poco::Net::SecureSMTPClientSession::AUTH_LOGIN, "18090074638@163.com"
			, "GKtEPkhaLbh2HCjU");
		
		//构建邮件

		Poco::Net::MailMessage message;
		message.setSender("18090074638@163.com");
		message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
		message.setSubject("VarifyCode");
		message.setContent("请查收你的验证码 ： " + GetRandomCode(6));
		//发送
		smtp.sendMessage(message);
		smtp.close();
		std::cout << "SendEmail Success, email is : " << email << std::endl;
	}
	catch (Poco::Net::SMTPException& e)
	{
		std::cerr << "SMTP Error: " << e.message() << std::endl;
		std::cout << "SendEmail Fail to "<< std::endl;
		return false;
	}
	return true;
}

std::string VarifyImplService::GetRandomCode(int num)
{
	// 1. 随机引擎（选择其一）
	std::random_device rd;  // 真随机种子
	std::mt19937 gen(rd()); // Mersenne Twister 引擎（常用伪随机）

	// 2. 定义分布（如均匀整数分布）
	std::uniform_int_distribution<int> dist_num(0, 9); 
	std::uniform_int_distribution<int> dist_choose(0, 2);//0-数字，1-小写字母，2-大写字母
	std::uniform_int_distribution<int> dist_letter(0,25);

	// 3. 生成随机数
	std::string res = "";
	for (int i = 0; i < num; ++i)
	{
		int random_class = dist_choose(gen);
		switch (random_class)
		{
		case 0: {
			res = res + char(dist_num(gen) + 48);
			break;
		}
		case 1: {
			res = res + char(dist_letter(gen) + 97);
			break;
		}
		case 2: {
			res = res + char(dist_letter(gen) + 65);
			break;
		}
		default:
			break;
		}
	}
	return res;
}

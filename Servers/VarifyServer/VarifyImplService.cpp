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
	//��ȡĿ��email
	auto email = req->email();
	bool success = SendEmail(email);
	if (!success) return grpc::Status::CANCELLED;
	return grpc::Status::OK;
}

bool VarifyImplService::SendEmail(std::string email)
{
	try
	{
		//����SMTP������
		Poco::Net::SecureSMTPClientSession smtp("smtp.163.com", 25);
		smtp.login(Poco::Net::SecureSMTPClientSession::AUTH_LOGIN, "18090074638@163.com"
			, "GKtEPkhaLbh2HCjU");
		
		//�����ʼ�

		Poco::Net::MailMessage message;
		message.setSender("18090074638@163.com");
		message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
		message.setSubject("VarifyCode");
		message.setContent("����������֤�� �� " + GetRandomCode(6));
		//����
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
	// 1. ������棨ѡ����һ��
	std::random_device rd;  // ���������
	std::mt19937 gen(rd()); // Mersenne Twister ���棨����α�����

	// 2. ����ֲ�������������ֲ���
	std::uniform_int_distribution<int> dist_num(0, 9); 
	std::uniform_int_distribution<int> dist_choose(0, 2);//0-���֣�1-Сд��ĸ��2-��д��ĸ
	std::uniform_int_distribution<int> dist_letter(0,25);

	// 3. ���������
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

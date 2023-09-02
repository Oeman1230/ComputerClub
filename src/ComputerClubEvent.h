
#ifndef __COMPUTER_CLUB_EVENT_H__
#define __COMPUTER_CLUB_EVENT_H__

#include "WorkTime.h"


/*
	���� �������� ������ �������/�������� �������
	�������� ������� ������ � ������� ������������� ������� "�������"
*/


enum class ClubInputEventIDs
{
	UNKNOWN = 0,
	CLIENT_CAME,
	CLIENT_SIT,
	CLIENT_WAIT,
	CLIENT_OUT
};

enum class ClubOutputEventIDs
{
	UNKNOWN = 0,
	CLIENT_OUT = 11,
	CLIENT_SIT,
	GOT_ERROR
};

namespace ClubErrors
{
	struct BaseError
	{
		const std::string desc;
		BaseError(std::string errText) : desc(errText){}
	};
	//���������� ��� ������� ����� �������, ����� �� ��� ������
	struct YouShallNotPass : BaseError { YouShallNotPass() : BaseError("YouShallNotPass") {} };
	//���������� ��� ������� ����� � ��������� �����
	struct NotOpenYet : BaseError { NotOpenYet() : BaseError("NotOpenYet") {}};
	//���������� ��� ������� ���������� �� ������� �����������
	struct PlaceIsBusy : BaseError { PlaceIsBusy() : BaseError("PlaceIsBusy") {}};
	//���������� ��� ������ � �������� ��� �����
	struct ClientUnknown : BaseError { ClientUnknown() : BaseError("ClientUnknown") {}};
	//���������� ��� �������� ������� �� ����� ������� ��������� ������
	struct ICanWaitNoLonger : BaseError { ICanWaitNoLonger() : BaseError("ICanWaitNoLonger") {} };
	
	//���������� ��� ��������� ������� �����
	struct ClubIsBusy : BaseError { ClubIsBusy() : BaseError("ClubIsBusy") {} };
	
};

struct ClubEvent
{

	WorkTime eventTime;
	int eventId;
	std::string eventBody;

};

#endif // !__COMPUTER_CLUB_EVENT_H__



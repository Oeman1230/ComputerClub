
#ifndef __COMPUTER_CLUB_EVENT_H__
#define __COMPUTER_CLUB_EVENT_H__

#include "WorkTime.h"


/*
	Файл содержит номера входных/выходных событий
	Содержит события ошибки и базовое представление объекта "событие"
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
	//Исключение при попытки входа клиента, когда он уже внутри
	struct YouShallNotPass : BaseError { YouShallNotPass() : BaseError("YouShallNotPass") {} };
	//Исключение при попытки входа в нерабочее время
	struct NotOpenYet : BaseError { NotOpenYet() : BaseError("NotOpenYet") {}};
	//Исключение при попытки размещения за занятым компьютером
	struct PlaceIsBusy : BaseError { PlaceIsBusy() : BaseError("PlaceIsBusy") {}};
	//Исключение при работе с клиентом вне клуба
	struct ClientUnknown : BaseError { ClientUnknown() : BaseError("ClientUnknown") {}};
	//Исключение при ожидании клиента во время наличия свободных столов
	struct ICanWaitNoLonger : BaseError { ICanWaitNoLonger() : BaseError("ICanWaitNoLonger") {} };
	
	//Исключение при полностью занятом клубе
	struct ClubIsBusy : BaseError { ClubIsBusy() : BaseError("ClubIsBusy") {} };
	
};

struct ClubEvent
{

	WorkTime eventTime;
	int eventId;
	std::string eventBody;

};

#endif // !__COMPUTER_CLUB_EVENT_H__



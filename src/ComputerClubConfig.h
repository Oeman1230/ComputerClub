
#ifndef __COMPUTER_CLUB_CONFIG_H__
#define __COMPUTER_CLUB_CONFIG_H__

#include "WorkTime.h"

/*
	Стуктура конфигурации компьютерного клуба
*/
struct ComputerClubConfig
{
	int computersCount;
	WorkTime openTime;
	WorkTime closeTime;
	int hourCost;

	

	ComputerClubConfig(int count, std::string open, std::string close, int cost)
	{
		computersCount = count;
		openTime = WorkTime(open, TIME_SEPARATOR);
		closeTime = WorkTime(close, TIME_SEPARATOR);
		hourCost = cost;
	}
	ComputerClubConfig() : computersCount(0), hourCost(0){}

	ComputerClubConfig& operator=(const ComputerClubConfig& newConfig)
	{
		this->computersCount = newConfig.computersCount;
		this->openTime = newConfig.openTime;
		this->closeTime = newConfig.closeTime;
		this->hourCost = newConfig.hourCost;

		return *this;
	}

	static constexpr int _FIELDS_COUNT = 4;

};


#endif // !__COMPUTER_CLUB_CONFIG_H__




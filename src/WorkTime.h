
#ifndef __WORK_TIME_H__
#define __WORK_TIME_H__


#include <string>
#include <exception>

#include "Utilities.h"


static const std::string TIME_SEPARATOR = ":";
/*
	Класс оболочка для работы со временем
*/
class WorkTime
{
public:

	WorkTime() : hours(0), minutes(0) {}
	WorkTime(std::string textWorkTime, std::string sep)
	{
		
		StrVectorPtr tempStrTime;
		//разделяем строку на часы и минуты
		tempStrTime = split(textWorkTime, sep);

		if (tempStrTime == nullptr)
		{
			throw std::exception();
		}

		try
		{
			
			if (!isStrDigit(tempStrTime->at(0)))
			{
				throw std::exception();
			}
			hours = std::atoi(tempStrTime->at(0).c_str());

			if (!isStrDigit(tempStrTime->at(1)))
			{
				throw std::exception();
			}
			minutes = atoi(tempStrTime->at(1).c_str());

		}
		catch (std::exception&)
		{
			throw std::exception();
		}

		inputStr = textWorkTime;
	}

	bool operator>(const WorkTime& compTime)
	{

		int thisAllMinutes = this->hours * MINUTE_IN_HOUR + this->minutes;
		int compTimeAllMinutes = compTime.getHour() * MINUTE_IN_HOUR + compTime.getMinute();

		if (thisAllMinutes > compTimeAllMinutes)
			return true;
		return false;
	}
	bool operator<(const WorkTime& compTime)
	{
		int thisAllMinutes = this->hours * MINUTE_IN_HOUR + this->minutes;
		int compTimeAllMinutes = compTime.getHour() * MINUTE_IN_HOUR + compTime.getMinute();

		if (thisAllMinutes < compTimeAllMinutes)
			return true;
		return false;

	}
	bool operator==(const WorkTime& compTime)
	{
		if (this->hours != compTime.getHour())
			return false;
		if (this->minutes != compTime.getMinute())
			return false;
		return true;
	}

public:

	static const int MINUTE_IN_HOUR = 60;
	WorkTime& operator+=(const WorkTime& time)
	{
		this->hours += time.getHour();
		
		int tempMinutes = this->getMinute();
		tempMinutes += time.getMinute();

		if (tempMinutes >= MINUTE_IN_HOUR)
		{
			this->hours++;
			tempMinutes -= MINUTE_IN_HOUR;
		}
		this->minutes = tempMinutes;

		return *this;
	}


	/*
		Функция вычисления разницы между вторым и первым временем
		Вычитает из sTime время fTime 
		Возвращает время в формате WorkTime
 	*/
	static WorkTime getDifference(const WorkTime& fTime, const WorkTime& sTime)
	{
		WorkTime ret;
		

		int fTimeAllMinutes = fTime.getHour() * MINUTE_IN_HOUR + fTime.getMinute();
		int sTimeAllMinutes = sTime.getHour() * MINUTE_IN_HOUR + sTime.getMinute();

		int minuteDifference = sTimeAllMinutes - fTimeAllMinutes;

		int hours = (int)(minuteDifference/MINUTE_IN_HOUR);
		int minutes = (int)(minuteDifference % MINUTE_IN_HOUR);

		ret.setHour(hours);
		ret.setMinute(minutes);

		return ret;
	}

public:

	void setHour(int hour) { hours = hour; }
	void setMinute(int minute) { minutes = minute; }
	int getHour() const { return hours; }
	int getMinute() const { return minutes; }
	void setStrTime(const std::string& newStr) {inputStr = newStr;
	}
	
	/*
		Функция выдачи времени в формате ХХ:ХХ
	*/
	std::string getTimeStr() const
	{ 
		if (inputStr.empty())
		{

			std::string tempStr;

			if (hours < 10)
			{
				tempStr.append("0");
			}
			tempStr.append(std::to_string(hours).c_str());
			tempStr.append(TIME_SEPARATOR);

			if (minutes < 10)
			{
				tempStr.append("0");
			}

			tempStr.append(std::to_string(minutes).c_str());

			return tempStr;
		}
		
		return inputStr; 
	}

private:

	std::string inputStr;
	int hours;
	int minutes;

};



#endif // !1


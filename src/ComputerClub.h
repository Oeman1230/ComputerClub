
#ifndef __COMPTER_CLUB_H__
#define __COMPTER_CLUB_H__

#include <string>
#include <vector>
#include <memory>
#include <list>

#include "ComputerClubConfig.h"
#include "ComputerClubEvent.h"

/*
	Главный класс компьютерного клуба.
*/
class ComputerCub
{
public:

	ComputerCub(std::string fileToWorkName);
	~ComputerCub() { free(); }

	//функция запуска обработки событий компьютерного клуба
	int start();

private:
	void initComputers();
	
	//Инициализация конфигурации компьютерного клуба из считанного файла
	int readConfigFromFile();
	/*
		Функция выдачи сформированного события из списка текстовых событий
		Принимает индекс в массиве строк из рабочего файла 
	*/
	ClubEvent readEvent(size_t index);
	//Действия после выполнения всех процедур обработки
	void actionAfterClosing();
	/*
		Обработка события по заданным шаблонам
		Принимает объект события
	*/
	void handleEvent(const ClubEvent& e);
	/*
		Добавления объекта события в список сформованных событий
		Принимает объект события
	*/
	void insertEventToList(const ClubEvent& e);

	/*
		Функция выдачи строки с ошибочным событием из файла
		Принимает индекс в массиве строк из файла
	*/
	void printErrorLine(size_t index);
	/*
		Фукнция вычисления прибыли по конфигурации
		Принимает время за компьютером
	*/
	int calcIncome(WorkTime timeUsed);

public:

	//Функция выдачи стартовой информации клуба
	void printClubOpenData();
	//Функция выдачи всех входных/выходных событий
	void printClubEvents();
	//Функция выдачи информации после закрытия клуба
	void printClubCloseData();
	

private:

	std::string fileName;
	//Массив строк из рабочего файла с событиями
	std::shared_ptr<std::vector<std::string>> strLines;
	size_t startStrLine;

private:

	struct Client
	{
		Client(){}
		Client(std::string newName) : name(newName){}
		std::string name;
		WorkTime atComputerStartTime; //Время начала пользования комьютером
		WorkTime startUsingComputerTime; //Время начала пользования оплачиваемого времени за компьютером
	};

	struct ComputerSeat
	{
		std::weak_ptr<Client> owner; //Клиент за компьютером
		WorkTime timeUsed; //Время пользования комьютером
		int seatIncome = 0; //Прибыль места
	};

private:
	/*
		Фукнция проверки открытия клуба
		Принимает время вхождения в клуб
		Возвращает false в нерабочее время
	*/
	bool isClubOpen(const WorkTime& time);
	/*
		Функция проверки места на занятость
		Принимает индекс в списке компьютеров
		Возвращает true если комьютер занят
	*/
	bool isPlaceBusy(size_t seatIndex);
	//Функция выдачи индекса первого свобного места
	size_t findFirstFreePos();
	//Функция исключения клиента из списка людей внутри клуба
	void removePersonFromPeopleInside(const Client& person);
	//Функция исключения клиента из списка ожидания
	void removePersonFromAwaitingList(const Client& person);
	//функция проверки нахождения клиента внутри клуба
	bool isClientInside(const Client& person);

private:
	
	/*
		Блок функций обработчиков событий
	*/
	//Id 1
	void clientCameAction(const ClubEvent& e);
	//Id 2
	void clientSitAction(const ClubEvent& e);
	//Id 3
	void clientAwaitAction(const ClubEvent& e);
	//Id 4
	void clientOutAction(const ClubEvent& e);

private:

	ComputerClubConfig config;
	bool canStart;
	std::list<ClubEvent> clubEvents;


	std::vector<ComputerSeat> computers;
	std::list<std::shared_ptr<Client>> peopleInside;
	std::list<std::weak_ptr<Client>> awaitingPeople;

	//Функция очистки всей памяти
	void free();

};

#endif // !__COMPTER_CLUB_H__




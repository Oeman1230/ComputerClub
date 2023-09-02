
#include <iostream>
#include <algorithm>

#include "ComputerClub.h"
#include "Utilities.h"

ComputerCub::ComputerCub(std::string inputFileName)
	: fileName(inputFileName), canStart(true), startStrLine(0)
{
	
	try
	{
		strLines = readLinesFromFile(fileName);

		//убираем лишние пустые символы при наличии для удобства
		compressStrArr(strLines); 
	}
	catch (...)
	{
		free();
		canStart = false;
		return;
	}
	int problemConfigLine = readConfigFromFile();

	if (problemConfigLine != -1)
	{
		printErrorLine(problemConfigLine);
		canStart = false;
	}

}

void ComputerCub::initComputers()
{
	computers.resize(config.computersCount);
}

int ComputerCub::readConfigFromFile()
{
	const int COMPUTER_COUNT_POS = 0;
	const int COST_POS = 2;
	const int TIME_OPEN_TIME_CLOSE_POS = 1;
	const int OPEN_DATA_END_LINE = 3;

	this->startStrLine = OPEN_DATA_END_LINE;

	int tempComputersCount = 0;
	int tempCost = 0;

	try
	{

		if (!isStrDigit(strLines->at(COMPUTER_COUNT_POS)))
		{
			throw std::exception();
		}

		tempComputersCount = std::atoi(strLines->at(COMPUTER_COUNT_POS).c_str());

		if (tempComputersCount <= 0)
		{
			throw std::exception();
		}

	}
	catch (const std::exception&)
	{
		return COMPUTER_COUNT_POS;
	}

	try
	{
		if (!isStrDigit(strLines->at(COST_POS)))
		{
			throw std::exception();
		}

		tempCost = std::atoi(strLines->at(COST_POS).c_str());
	}
	catch (const std::exception&)
	{
		return COST_POS;
	}

	try
	{
		//Время открытия и закрытия приходит в формате "ХХ:ХХ ХХ:ХХ" 
		//Разделяем на две строки
		StrVectorPtr tempStrTime = split(strLines->at(TIME_OPEN_TIME_CLOSE_POS), " ");
		if (tempStrTime == nullptr)
		{
			throw std::exception();
		}

		std::string tempOpen = tempStrTime->at(0);
		std::string tempClose = tempStrTime->at(1);

		config = ComputerClubConfig(tempComputersCount, tempOpen, tempClose, tempCost);

	}
	catch (const std::exception&)
	{
		return TIME_OPEN_TIME_CLOSE_POS;
	}


	

	return -1;
}

void ComputerCub::insertEventToList(const ClubEvent& e)
{
	clubEvents.push_back(e);
}

void ComputerCub::printErrorLine(size_t index)
{
	std::cout << strLines->at(index) << "\n";
}

void ComputerCub::printClubOpenData()
{
	std::cout << config.openTime.getTimeStr() << "\n";
}

int ComputerCub::calcIncome(WorkTime timeUsed)
{
	int ret = 0;

	int hoursUsed = timeUsed.getHour();

	if (timeUsed.getMinute() > 0)
	{
		hoursUsed++;
	}

	ret = hoursUsed * config.hourCost;

	return ret;
}

void ComputerCub::printClubCloseData()
{

	std::cout << config.closeTime.getTimeStr() << "\n";

	for (size_t i = 0; i < computers.size(); i++)
	{
		size_t realPos = i + 1;
		std::cout << realPos << " " << computers.at(i).seatIncome << " ";
		std::cout << computers.at(i).timeUsed.getTimeStr() << "\n";
	}
}

void ComputerCub::printClubEvents()
{

	for (auto it : clubEvents)
	{
		std::cout << it.eventTime.getTimeStr() << " ";
		std::cout << it.eventId << " ";
		std::cout << it.eventBody << "\n";
	}
}

int ComputerCub::start()
{
	//Проверка успешности инициализации класса
	if (canStart != true)
	{
		return -1;
	}

	try
	{
		initComputers();
	}
	catch (const std::exception&)
	{
		return -1;
	}
	
	for (size_t i = startStrLine; i < strLines->size(); i++)
	{
		
		ClubEvent itEvent;
		try {
			itEvent = readEvent(i);
			handleEvent(itEvent);	
		}
		catch (const std::exception&)
		{
			printErrorLine(i);
			return -1;
		}

	}

	try
	{
		actionAfterClosing();
	}
	catch (const std::exception&)
	{

	}

	//Выдача необходимой информации
	printClubOpenData();
	printClubEvents();
	printClubCloseData();

	return 0;
}

void ComputerCub::actionAfterClosing()
{
	std::list<std::string> lastClients;
	
	//Заполняем список последних клиентов людьми за компьютерами
	for (size_t i = 0; i < computers.size(); i++)
	{
		if (computers.at(i).owner.expired())
		{
			continue;
		}
		std::shared_ptr<Client> itClient = computers.at(i).owner.lock();

		lastClients.push_back(itClient->name);
		WorkTime timeSpent = WorkTime::getDifference(itClient->atComputerStartTime, config.closeTime);
		//обновляем состояния компьютеров
		computers.at(i).timeUsed += timeSpent;
		computers.at(i).seatIncome += calcIncome(timeSpent);

		removePersonFromPeopleInside(*itClient);

		itClient.reset();
		computers.at(i).owner.reset();
	}

	//Заполняем список последних клиентов людьми в очереди ожидания
	auto awaitingIt = awaitingPeople.begin();
	while (awaitingIt != awaitingPeople.end())
	{
		if (awaitingIt->expired())
		{
			awaitingIt++;
			continue;
		}

		lastClients.push_back(awaitingIt->lock()->name);
		removePersonFromPeopleInside(*awaitingIt->lock().get());
		awaitingIt++;
	}
	awaitingPeople.clear();

	//Добавляем оставшихся людей
	for (auto it : peopleInside)
	{
		lastClients.push_back(it->name);
	}

	lastClients.sort();

	//Генерация выходного события для каждого уходящего клиента 
	auto lastOutClientIt = lastClients.begin();
	while (lastOutClientIt != lastClients.end())
	{
		ClubEvent lastE;
		lastE.eventTime = config.closeTime;
		lastE.eventId = (int)ClubOutputEventIDs::CLIENT_OUT;
		lastE.eventBody = *lastOutClientIt;

		clubEvents.push_back(lastE);

		lastOutClientIt++;
	}


}

ClubEvent ComputerCub::readEvent(size_t index)
{
	ClubEvent ret;

	WorkTime eTime;
	int eId;
	std::string eBody;


	StrVectorPtr eLine = split(strLines->at(index), " ");

	if (eLine == nullptr)
	{
		throw std::exception(); 
	}
	//Минимальное количество параметров события
	const int SHORT_INPUT_EVENT_SIZE = 2;
	//Максимальное количество параметров события
	const int LONG_INPUT_EVENT_SIZE = 3;
	//Проверка на корректность количества параметров в строке события
	if (eLine->size() - 1 > LONG_INPUT_EVENT_SIZE || eLine->size() - 1 < SHORT_INPUT_EVENT_SIZE)
	{
		throw std::exception();
	}
	
	//По шаблону события первым идет время
	eTime = WorkTime(eLine->at(0), TIME_SEPARATOR);
	//По шаблону события вторым идет id
	if (!isStrDigit(eLine->at(1)))
	{
		throw std::exception();
	}
	eId = std::atoi(eLine->at(1).c_str());

	//По шаблону третьим параметром должно идти имя клиента 
	eBody.append(eLine->at(SHORT_INPUT_EVENT_SIZE));

	
	//В случае наличия доп параметров добавляем в тело события
	if (eLine->size() - 1 > SHORT_INPUT_EVENT_SIZE)
	{
		eBody.append(" ");
		eBody.append(eLine->at(LONG_INPUT_EVENT_SIZE));
	}

	ret.eventTime = eTime;
	ret.eventId = eId;
	ret.eventBody = eBody;
	return ret;
}

void ComputerCub::handleEvent(const ClubEvent& e)
{
	//Добавляем входное событие в список
	insertEventToList(e);

	try
	{
		switch ((ClubInputEventIDs)e.eventId)
		{
		case ClubInputEventIDs::CLIENT_CAME:
			clientCameAction(e);
			break;
		case ClubInputEventIDs::CLIENT_SIT:
			clientSitAction(e);
			break;
		case ClubInputEventIDs::CLIENT_WAIT:
			clientAwaitAction(e);
			break;
		case ClubInputEventIDs::CLIENT_OUT:
			clientOutAction(e);
			break;

		default:
			throw std::exception();
			break;
		}
	}
	//Обработка известных событий-ошибок
	catch (const ClubErrors::BaseError& err)
	{
		ClubEvent errorEvent;
		errorEvent.eventTime = e.eventTime;
		errorEvent.eventId = (int)ClubOutputEventIDs::GOT_ERROR;
		errorEvent.eventBody = err.desc;
		//Программа не должна прекращать работу
		insertEventToList(errorEvent);
	}

}

bool ComputerCub::isClubOpen(const WorkTime& time)
{
	if (config.openTime > time)
	{
		return false;
	}
	if (config.closeTime < time)
	{
		return false;
	}
	return true;


}

void ComputerCub::clientCameAction(const ClubEvent& e)
{
	if (!isClubOpen(e.eventTime))
	{
		throw ClubErrors::NotOpenYet();
	}

	if (isClientInside(Client(e.eventBody)))
	{
		throw ClubErrors::YouShallNotPass();
	}

	//Добавляем клиента в список вошедших людей

	std::shared_ptr<Client> tempPerson = std::make_shared<Client>();
	tempPerson->name = e.eventBody;
	peopleInside.push_back(tempPerson);
}

bool ComputerCub::isPlaceBusy(size_t seatIndex)
{
	if (seatIndex < 0 || seatIndex >= computers.size())
	{
		throw std::exception();
	}

	ComputerSeat seat = computers.at(seatIndex);

	if (seat.owner.expired())
	{
		return false;
	}
	return true;
}

bool ComputerCub::isClientInside(const Client& person)
{
	//Ищем клиента по имени
	auto personIt = std::find_if(peopleInside.begin(), peopleInside.end(),
		[&](const std::shared_ptr<Client> it)
		{
			if (it->name == person.name)
			{
				return true;
			}
			return false;
		}
	);

	//Случай, когда человека нет внутри
	if (personIt == peopleInside.end()) 
	{
		return false;
	}
	return true;

}

size_t ComputerCub::findFirstFreePos()
{
	
	size_t i = 0;

	for (; i < computers.size(); i++)
	{
		if (computers.at(i).owner.expired())
		{
			break;
		}
	}

	if (i >= computers.size()) //Случай, когда весь клуб занят
	{
		throw ClubErrors::ClubIsBusy();
	}

	return i;

}

void ComputerCub::clientSitAction(const ClubEvent& e)
{
	if (!isClubOpen(e.eventTime))
	{
		throw ClubErrors::NotOpenYet();
	}

	//Известно, что событие посадки клиента состоит из 2 частей - разделяем
	StrVectorPtr eBody = split(e.eventBody, " ");
	
	if (eBody == nullptr)
	{
		throw std::exception();
	}

	if (isClientInside(Client(eBody->at(0))) == false)
	{
		throw ClubErrors::ClientUnknown();
	}

	//Конвертируем реальный индекс позиции в программный 
	size_t realPos = std::atoi(eBody->at(1).c_str());
	size_t pos = realPos - 1;

	//Находим указатель на человека в списке людей внутри. Предыдущая проверка гарантирует наличие
	auto currentPerson = std::find_if(peopleInside.begin(), peopleInside.end(),
		[&](std::shared_ptr<Client> it)
		{
			if (it->name == eBody->at(0))
			{
				return true;
			}
			return false;

		});

	//Проверка сидит ли человек за компьютером
	auto isPersonSittingIt = computers.begin();
	while (isPersonSittingIt != computers.end())
	{
		if (isPersonSittingIt->owner.expired())
		{
			isPersonSittingIt++;
			continue;
		}
		//Случай, если человек уже сидит за столом
		if (isPersonSittingIt->owner.lock()->name == currentPerson->get()->name)
		{
			break;
		}

		isPersonSittingIt++;
	}

	//Проверка места на доступность
	if (isPlaceBusy(pos))
	{
		//В случае, если человек не сидит за столом, то он попадает в список ожидания
		if (isPersonSittingIt == computers.end())
		{
			awaitingPeople.push_back(*currentPerson);
		}

		throw ClubErrors::PlaceIsBusy();
	}

	//Случай пересадки клиента на другое место
	if (isPersonSittingIt != computers.end())
	{
		//Запоминаем время за компьютером
		WorkTime timeUsed = WorkTime::getDifference(currentPerson->get()->atComputerStartTime, e.eventTime);
		isPersonSittingIt->timeUsed += timeUsed;

		//Перемещаем клиента за другой стол
		computers.at(pos).owner = isPersonSittingIt->owner;
		isPersonSittingIt->owner.reset();
		computers.at(pos).owner.lock()->atComputerStartTime = e.eventTime;
		return;
	}
	
	//данный случай, когда клиент впервые сел за стол
	currentPerson->get()->atComputerStartTime = e.eventTime;
	currentPerson->get()->startUsingComputerTime = e.eventTime;
	computers.at(pos).owner = *currentPerson;

}


void ComputerCub::clientAwaitAction(const ClubEvent& e)
{
	if (!isClientInside(Client(e.eventBody)))
	{
		throw ClubErrors::ClientUnknown();
	}

	try
	{
		//Проверка исключением на занятость столов
		findFirstFreePos(); //Номер свободного стола не важен

		throw ClubErrors::ICanWaitNoLonger();
	}
	catch (const ClubErrors::ClubIsBusy&)
	{
		
	}

	//Проверяем есть ли клиент в списке ожидания
	auto personInside = peopleInside.begin();
	while (personInside != peopleInside.end())
	{
		if (personInside->get()->name == e.eventBody)
		{
			break;
		}

		personInside++;
	}
	//Если клиент не в списке ожидания
	if (personInside == peopleInside.end())
	{
		awaitingPeople.push_back(*personInside);
	}

	//Если в списке ожидания клиентов больше, чем столов, генерируем событие выхода человека
	if (awaitingPeople.size() > computers.size())
	{
		ClubEvent tooManyPeopleE = e;
		tooManyPeopleE.eventId = (int)ClubOutputEventIDs::CLIENT_OUT;

		insertEventToList(tooManyPeopleE);
		
		removePersonFromPeopleInside(Client(e.eventBody));
		removePersonFromAwaitingList(Client(e.eventBody));
	}

	
}

void ComputerCub::removePersonFromAwaitingList(const Client& person)
{
	
	auto it = awaitingPeople.begin();
	
	while (it != awaitingPeople.end())
	{
		std::weak_ptr<Client> curPerson = *it;

		if (curPerson.expired())
		{
			awaitingPeople.erase(it);
			return;
		}

		if (curPerson.lock()->name == person.name)
		{
			awaitingPeople.erase(it);
			return;
		}
		it++;
	}


}

void ComputerCub::removePersonFromPeopleInside(const Client& person)
{
	auto it = peopleInside.begin();

	while (it != peopleInside.end())
	{

		if (it->get()->name == person.name)
		{
			peopleInside.remove(*it);
			return;

		}

		it++;
	}

	if (it == peopleInside.end())
	{
		throw ClubErrors::ClientUnknown();
	}


}

void ComputerCub::clientOutAction(const ClubEvent& e)
{

	if (!isClientInside(Client(e.eventBody)))
	{
		throw ClubErrors::ClientUnknown();
	}
	//Поиск человека за столом
	size_t i = 0;
	for (; i < computers.size(); i++)
	{
		if (computers.at(i).owner.expired())
		{
			continue;
		}

		if (computers.at(i).owner.lock()->name == e.eventBody)
		{
			break;
		}


	}

	//Случай, если человек не сидит
	if (i >= computers.size())
	{
		removePersonFromPeopleInside(Client(e.eventBody));
		return;
	}

	//Не выбрасывающее исключение удаление из списка
	removePersonFromAwaitingList(Client(e.eventBody));

	//Обновляем состояния стола
	std::shared_ptr<Client> curClient = computers.at(i).owner.lock();

	WorkTime timeSpent = WorkTime::getDifference(curClient->atComputerStartTime, e.eventTime);
	computers.at(i).timeUsed += timeSpent;

	//считаем долг человека
	WorkTime computerUsageTime = WorkTime::getDifference(curClient->startUsingComputerTime, e.eventTime);
	computers.at(i).seatIncome += calcIncome(computerUsageTime);

	curClient.reset();
	computers.at(i).owner.reset();
	

	removePersonFromPeopleInside(Client(e.eventBody));

	//Случай, если есть люди в списке ожидания
	if (awaitingPeople.size() != 0)
	{
		//Сажаем первого ожидающего клиента за стол
		std::weak_ptr<Client> lastWaitingPerson;
		lastWaitingPerson = awaitingPeople.front();
		awaitingPeople.pop_front();

		lastWaitingPerson.lock()->atComputerStartTime = e.eventTime;
		lastWaitingPerson.lock()->startUsingComputerTime = e.eventTime;

		computers.at(i).owner = lastWaitingPerson;

		//Генерируем выходное событие посадки клиента
		ClubEvent clientOutE;

		clientOutE.eventTime = e.eventTime;
		clientOutE.eventId = (int)ClubOutputEventIDs::CLIENT_SIT;

		clientOutE.eventBody.append(lastWaitingPerson.lock()->name);
		clientOutE.eventBody.append(" ");
		clientOutE.eventBody.append(std::to_string(i + 1));

		
		insertEventToList(clientOutE);
	}

}

void ComputerCub::free()
{

	for (size_t i = 0; i < computers.size(); i++)
	{
		computers.at(i).owner.reset();
	}
	computers.clear();

	for (auto it : awaitingPeople)
	{
		it.reset();
	}
	awaitingPeople.clear();

	for (auto it : peopleInside)
	{
		it.reset();
	}
	peopleInside.clear();

	clubEvents.clear();

	strLines->clear();
	strLines.reset();
}


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

		//������� ������ ������ ������� ��� ������� ��� ��������
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
		//����� �������� � �������� �������� � ������� "��:�� ��:��" 
		//��������� �� ��� ������
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
	//�������� ���������� ������������� ������
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

	//������ ����������� ����������
	printClubOpenData();
	printClubEvents();
	printClubCloseData();

	return 0;
}

void ComputerCub::actionAfterClosing()
{
	std::list<std::string> lastClients;
	
	//��������� ������ ��������� �������� ������ �� ������������
	for (size_t i = 0; i < computers.size(); i++)
	{
		if (computers.at(i).owner.expired())
		{
			continue;
		}
		std::shared_ptr<Client> itClient = computers.at(i).owner.lock();

		lastClients.push_back(itClient->name);
		WorkTime timeSpent = WorkTime::getDifference(itClient->atComputerStartTime, config.closeTime);
		//��������� ��������� �����������
		computers.at(i).timeUsed += timeSpent;
		computers.at(i).seatIncome += calcIncome(timeSpent);

		removePersonFromPeopleInside(*itClient);

		itClient.reset();
		computers.at(i).owner.reset();
	}

	//��������� ������ ��������� �������� ������ � ������� ��������
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

	//��������� ���������� �����
	for (auto it : peopleInside)
	{
		lastClients.push_back(it->name);
	}

	lastClients.sort();

	//��������� ��������� ������� ��� ������� ��������� ������� 
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
	//����������� ���������� ���������� �������
	const int SHORT_INPUT_EVENT_SIZE = 2;
	//������������ ���������� ���������� �������
	const int LONG_INPUT_EVENT_SIZE = 3;
	//�������� �� ������������ ���������� ���������� � ������ �������
	if (eLine->size() - 1 > LONG_INPUT_EVENT_SIZE || eLine->size() - 1 < SHORT_INPUT_EVENT_SIZE)
	{
		throw std::exception();
	}
	
	//�� ������� ������� ������ ���� �����
	eTime = WorkTime(eLine->at(0), TIME_SEPARATOR);
	//�� ������� ������� ������ ���� id
	if (!isStrDigit(eLine->at(1)))
	{
		throw std::exception();
	}
	eId = std::atoi(eLine->at(1).c_str());

	//�� ������� ������� ���������� ������ ���� ��� ������� 
	eBody.append(eLine->at(SHORT_INPUT_EVENT_SIZE));

	
	//� ������ ������� ��� ���������� ��������� � ���� �������
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
	//��������� ������� ������� � ������
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
	//��������� ��������� �������-������
	catch (const ClubErrors::BaseError& err)
	{
		ClubEvent errorEvent;
		errorEvent.eventTime = e.eventTime;
		errorEvent.eventId = (int)ClubOutputEventIDs::GOT_ERROR;
		errorEvent.eventBody = err.desc;
		//��������� �� ������ ���������� ������
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

	//��������� ������� � ������ �������� �����

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
	//���� ������� �� �����
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

	//������, ����� �������� ��� ������
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

	if (i >= computers.size()) //������, ����� ���� ���� �����
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

	//��������, ��� ������� ������� ������� ������� �� 2 ������ - ���������
	StrVectorPtr eBody = split(e.eventBody, " ");
	
	if (eBody == nullptr)
	{
		throw std::exception();
	}

	if (isClientInside(Client(eBody->at(0))) == false)
	{
		throw ClubErrors::ClientUnknown();
	}

	//������������ �������� ������ ������� � ����������� 
	size_t realPos = std::atoi(eBody->at(1).c_str());
	size_t pos = realPos - 1;

	//������� ��������� �� �������� � ������ ����� ������. ���������� �������� ����������� �������
	auto currentPerson = std::find_if(peopleInside.begin(), peopleInside.end(),
		[&](std::shared_ptr<Client> it)
		{
			if (it->name == eBody->at(0))
			{
				return true;
			}
			return false;

		});

	//�������� ����� �� ������� �� �����������
	auto isPersonSittingIt = computers.begin();
	while (isPersonSittingIt != computers.end())
	{
		if (isPersonSittingIt->owner.expired())
		{
			isPersonSittingIt++;
			continue;
		}
		//������, ���� ������� ��� ����� �� ������
		if (isPersonSittingIt->owner.lock()->name == currentPerson->get()->name)
		{
			break;
		}

		isPersonSittingIt++;
	}

	//�������� ����� �� �����������
	if (isPlaceBusy(pos))
	{
		//� ������, ���� ������� �� ����� �� ������, �� �� �������� � ������ ��������
		if (isPersonSittingIt == computers.end())
		{
			awaitingPeople.push_back(*currentPerson);
		}

		throw ClubErrors::PlaceIsBusy();
	}

	//������ ��������� ������� �� ������ �����
	if (isPersonSittingIt != computers.end())
	{
		//���������� ����� �� �����������
		WorkTime timeUsed = WorkTime::getDifference(currentPerson->get()->atComputerStartTime, e.eventTime);
		isPersonSittingIt->timeUsed += timeUsed;

		//���������� ������� �� ������ ����
		computers.at(pos).owner = isPersonSittingIt->owner;
		isPersonSittingIt->owner.reset();
		computers.at(pos).owner.lock()->atComputerStartTime = e.eventTime;
		return;
	}
	
	//������ ������, ����� ������ ������� ��� �� ����
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
		//�������� ����������� �� ��������� ������
		findFirstFreePos(); //����� ���������� ����� �� �����

		throw ClubErrors::ICanWaitNoLonger();
	}
	catch (const ClubErrors::ClubIsBusy&)
	{
		
	}

	//��������� ���� �� ������ � ������ ��������
	auto personInside = peopleInside.begin();
	while (personInside != peopleInside.end())
	{
		if (personInside->get()->name == e.eventBody)
		{
			break;
		}

		personInside++;
	}
	//���� ������ �� � ������ ��������
	if (personInside == peopleInside.end())
	{
		awaitingPeople.push_back(*personInside);
	}

	//���� � ������ �������� �������� ������, ��� ������, ���������� ������� ������ ��������
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
	//����� �������� �� ������
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

	//������, ���� ������� �� �����
	if (i >= computers.size())
	{
		removePersonFromPeopleInside(Client(e.eventBody));
		return;
	}

	//�� ������������� ���������� �������� �� ������
	removePersonFromAwaitingList(Client(e.eventBody));

	//��������� ��������� �����
	std::shared_ptr<Client> curClient = computers.at(i).owner.lock();

	WorkTime timeSpent = WorkTime::getDifference(curClient->atComputerStartTime, e.eventTime);
	computers.at(i).timeUsed += timeSpent;

	//������� ���� ��������
	WorkTime computerUsageTime = WorkTime::getDifference(curClient->startUsingComputerTime, e.eventTime);
	computers.at(i).seatIncome += calcIncome(computerUsageTime);

	curClient.reset();
	computers.at(i).owner.reset();
	

	removePersonFromPeopleInside(Client(e.eventBody));

	//������, ���� ���� ���� � ������ ��������
	if (awaitingPeople.size() != 0)
	{
		//������ ������� ���������� ������� �� ����
		std::weak_ptr<Client> lastWaitingPerson;
		lastWaitingPerson = awaitingPeople.front();
		awaitingPeople.pop_front();

		lastWaitingPerson.lock()->atComputerStartTime = e.eventTime;
		lastWaitingPerson.lock()->startUsingComputerTime = e.eventTime;

		computers.at(i).owner = lastWaitingPerson;

		//���������� �������� ������� ������� �������
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

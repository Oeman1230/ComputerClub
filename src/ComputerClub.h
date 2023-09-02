
#ifndef __COMPTER_CLUB_H__
#define __COMPTER_CLUB_H__

#include <string>
#include <vector>
#include <memory>
#include <list>

#include "ComputerClubConfig.h"
#include "ComputerClubEvent.h"

/*
	������� ����� ������������� �����.
*/
class ComputerCub
{
public:

	ComputerCub(std::string fileToWorkName);
	~ComputerCub() { free(); }

	//������� ������� ��������� ������� ������������� �����
	int start();

private:
	void initComputers();
	
	//������������� ������������ ������������� ����� �� ���������� �����
	int readConfigFromFile();
	/*
		������� ������ ��������������� ������� �� ������ ��������� �������
		��������� ������ � ������� ����� �� �������� ����� 
	*/
	ClubEvent readEvent(size_t index);
	//�������� ����� ���������� ���� �������� ���������
	void actionAfterClosing();
	/*
		��������� ������� �� �������� ��������
		��������� ������ �������
	*/
	void handleEvent(const ClubEvent& e);
	/*
		���������� ������� ������� � ������ ������������ �������
		��������� ������ �������
	*/
	void insertEventToList(const ClubEvent& e);

	/*
		������� ������ ������ � ��������� �������� �� �����
		��������� ������ � ������� ����� �� �����
	*/
	void printErrorLine(size_t index);
	/*
		������� ���������� ������� �� ������������
		��������� ����� �� �����������
	*/
	int calcIncome(WorkTime timeUsed);

public:

	//������� ������ ��������� ���������� �����
	void printClubOpenData();
	//������� ������ ���� �������/�������� �������
	void printClubEvents();
	//������� ������ ���������� ����� �������� �����
	void printClubCloseData();
	

private:

	std::string fileName;
	//������ ����� �� �������� ����� � ���������
	std::shared_ptr<std::vector<std::string>> strLines;
	size_t startStrLine;

private:

	struct Client
	{
		Client(){}
		Client(std::string newName) : name(newName){}
		std::string name;
		WorkTime atComputerStartTime; //����� ������ ����������� ����������
		WorkTime startUsingComputerTime; //����� ������ ����������� ������������� ������� �� �����������
	};

	struct ComputerSeat
	{
		std::weak_ptr<Client> owner; //������ �� �����������
		WorkTime timeUsed; //����� ����������� ����������
		int seatIncome = 0; //������� �����
	};

private:
	/*
		������� �������� �������� �����
		��������� ����� ��������� � ����
		���������� false � ��������� �����
	*/
	bool isClubOpen(const WorkTime& time);
	/*
		������� �������� ����� �� ���������
		��������� ������ � ������ �����������
		���������� true ���� �������� �����
	*/
	bool isPlaceBusy(size_t seatIndex);
	//������� ������ ������� ������� �������� �����
	size_t findFirstFreePos();
	//������� ���������� ������� �� ������ ����� ������ �����
	void removePersonFromPeopleInside(const Client& person);
	//������� ���������� ������� �� ������ ��������
	void removePersonFromAwaitingList(const Client& person);
	//������� �������� ���������� ������� ������ �����
	bool isClientInside(const Client& person);

private:
	
	/*
		���� ������� ������������ �������
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

	//������� ������� ���� ������
	void free();

};

#endif // !__COMPTER_CLUB_H__




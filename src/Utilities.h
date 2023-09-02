
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>

/*
	Файл с полезными функциями, не относящимися к компьютерному клубу
*/

static void fileInput(std::string fileName, std::string inputText)
{
	std::ofstream fileOut;
	fileOut.open(fileName, std::ios::app);

	if (fileOut.is_open())
	{
		fileOut << inputText << std::endl;
	}


	fileOut.close();
}

typedef std::shared_ptr<std::vector<std::string>> StrVectorPtr;

static StrVectorPtr readLinesFromFile(std::string fileName)
{
	StrVectorPtr ret = std::make_shared<std::vector<std::string>>();

	std::ifstream fromFile(fileName, std::ifstream::in);

	if (fromFile.is_open())
	{
		
		

		std::string tempStr;
		while (std::getline(fromFile, tempStr))
		{
			if (tempStr.empty())
			{
				continue;
			}
			if (tempStr.at(tempStr.size() - 1) == (char)(13))
			{
				tempStr.pop_back();
			}

			ret->push_back(tempStr.c_str());
		}

	}
	else
	{
		std::cout << "Cant open file:" << fileName << "\n";
		throw std::exception();
	}
	fromFile.close();


	return ret;
}

static StrVectorPtr divideStrByIndex(const std::string& text, size_t index)
{
	StrVectorPtr ret = std::make_shared<std::vector<std::string>>();

	if (text.size() < 2)
	{
		ret.reset();
		return nullptr;
	}

	auto it = text.begin();
	std::string temp;
	temp.resize(index);

	std::advance(it, index);
	std::copy(text.begin(), it, temp.begin());
	ret->push_back(temp);

	temp.clear();
	temp.resize(text.size() - index);
	std::copy(it, text.end(), temp.begin());
	ret->push_back(temp);

	return ret;
}

static StrVectorPtr split(const std::string& text, const std::string& sep)
{
	if (text.size() < 1)
	{
		return nullptr;
	}


	StrVectorPtr ret = std::make_shared<std::vector<std::string>>();

	if (text.size() < 2)
	{
		ret->push_back(text);
		return ret;
	}


	size_t i = 0;
	for (; i < text.size(); i++)
	{
		if (text[i] == sep[0])
		{
			break;
		}
	}

	if (i == text.size())
	{
		ret.reset();
		return nullptr;
	}

	auto it = text.begin();

	std::string temp;
	temp.resize(i);
	std::advance(it, i);
	std::copy(text.begin(), it, temp.begin());
	ret->push_back(temp);

	StrVectorPtr tempVec = nullptr;
	
	temp.clear();
	temp.resize(text.size() - i - 1);
	it++;
	std::copy(it, text.end(), temp.begin());

	tempVec = split(temp, sep);

	if (tempVec != nullptr)
	{
		for (size_t j = 0; j < tempVec->size(); j++)
		{
			ret->push_back(tempVec->at(j));
		}
	}
	else
	{
		ret->push_back(temp);
	}

	if (ret->empty() == true)
	{
		ret.reset();
		ret = nullptr;
	}

	return ret;
}

static bool isStrDigit(std::string textNum)
{
	bool ret = true;

	for (size_t i = 0; i < textNum.size(); i++)
	{

		if (!std::isdigit(static_cast<unsigned char>(textNum.at(i))))
		{
			ret = false;
			break;
		}


	}
	return ret;
}

static void compressStrArr(const StrVectorPtr& vec)
{
	
	for (size_t i = vec->size() - 1; i != 0; i--)
	{
		std::string it = vec->at(i);

		if (it == "" || it == "\n" || it == " ")
		{
			auto eraseIt = vec->begin();
			std::advance(eraseIt, i);
			vec->erase(eraseIt);
		}
		else
		{
			return;
		}


	}



}

#endif // !__UTILITIES_H__



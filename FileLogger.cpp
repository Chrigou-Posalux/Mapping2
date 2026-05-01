/******************************************************************************
* This file is subject to the terms and conditions defined in
* file 'LICENSE.txt', which is part of this source code package.
*
*
* $Author: orion $
* $Date: 2023-03-21 18:04:16 +0100 (mar., 21 mars 2023) $
* $Revision: 3597 $
*
******************************************************************************/

#include "FileLogger.h"

FileLogger::FileLogger()
{
}

FileLogger::FileLogger(std::string pathFile, bool bAppend)
{
	openFile(pathFile, bAppend);
}

FileLogger::~FileLogger()
{
	closeFile();
}

bool FileLogger::openFile(std::string pathFile, bool bAppend)
{
	m_isOpenForWrite = false;
	const char* fileName = pathFile.c_str();

	if (bAppend)
	{
		m_fileStreamWrite.open(fileName, std::ios_base::app);
	}
	else
	{
		m_fileStreamWrite.open(fileName);
	}

	if (m_fileStreamWrite.is_open())
	{
		m_isOpenForWrite = true;
	}

	return m_isOpenForWrite;
}

bool FileLogger::openFileForRead(std::string pathFile)
{
	m_isOpenForRead = false;
	const char* fileName = pathFile.c_str();

	m_fileStreamRead.open(fileName, std::ios_base::in);

	if (m_fileStreamRead.is_open())
	{
		m_isOpenForRead = true;
	}

	return m_isOpenForRead;
}

void FileLogger::writeString(std::string text)
{
	if (m_isOpenForWrite)
	{
		m_fileStreamWrite << text << std::endl;
	}
}

void FileLogger::writeACIJ(int area, int canvas, int imgF, int job)
{
	if (m_isOpenForWrite)
	{
		m_fileStreamWrite << "a" << std::to_string(area) << std::endl;
		m_fileStreamWrite << "c" << std::to_string(canvas) << std::endl;
		m_fileStreamWrite << "i" << std::to_string(imgF) << std::endl;
		m_fileStreamWrite << "j" << std::to_string(job) << std::endl;
	}
}

bool FileLogger::readACIJ(int* area, int* canvas, int* imgF, int* job)
{
	bool ret = true;

	if (m_isOpenForRead)
	{
		std::string line;
		m_fileStreamRead >> line;
		if (line[0] == 'a')
		{
			*area = atoi(&line[1]);
		}
		else
		{
			ret = false;
		}

		m_fileStreamRead >> line;
		if (line[0] == 'c')
		{
			*canvas = atoi(&line[1]);
		}
		else
		{
			ret = false;
		}

		m_fileStreamRead >> line;
		if (line[0] == 'i')
		{
			*imgF = atoi(&line[1]);
		}
		else
		{
			ret = false;
		}

		m_fileStreamRead >> line;
		if (line[0] == 'j')
		{
			*job = atoi(&line[1]);
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		ret = false;
	}
	return ret;
}

int FileLogger::readInteger()
{
	int retour = 0;
	if (m_isOpenForRead)
	{
		std::string line;
		m_fileStreamRead >> line;
		retour = atoi(&line[0]);
	}
	return retour;
}

std::string FileLogger::read()
{
	std::string retour = "";
	if (m_isOpenForRead)
	{
		std::string line;
		m_fileStreamRead >> line;
		retour = line;
	}
	return retour;
}

void FileLogger::closeFile()
{
	if (m_isOpenForWrite)
	{
		m_fileStreamWrite.flush();
		m_fileStreamWrite.close();
		m_isOpenForWrite = false;
	}
	if (m_isOpenForRead)
	{
		m_fileStreamRead.close();
		m_isOpenForRead = false;
	}
}
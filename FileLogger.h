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

#pragma once

#include <iostream>
#include <fstream>
#include <string>


/**
 * FileLogger is a Singleton class which defines GetInstance to serves
 * clients with the same instance over and over.
 */
class FileLogger
{
private:
    std::ofstream m_fileStreamWrite;
    std::ifstream m_fileStreamRead;

	bool m_isOpenForRead{ false };
    bool m_isOpenForWrite{ false };

public:
    /**************************************************
    * Constructor
    *************************************************/
    FileLogger();

    /**************************************************
    * Constructor
    * Open a file for append write
    * @param string fullpath and name of the file
    * @param bool bAppend = true for appending else file will be newly created
    * \return bool true if file is successfully opened
    *************************************************/
    FileLogger(std::string pathFile, bool bAppend);

    /**************************************************
    * Destructor
    *************************************************/
    ~FileLogger();
    
    /**************************************************
    * Open a file for append write
    * @param string fullpath and name of the file
    * @param bool bAppend = true for appending else file will be newly created
    * \return bool true if file is successfully opened
    *************************************************/
    bool openFile(std::string pathFile, bool bAppend);

    /**************************************************
    * Open a file for read
    * @param string fullpath and name of the file
    * \return bool true if file is successfully opened
    *************************************************/
    bool openFileForRead(std::string pathFile);

    /**************************************************
    * Write a text into the opened file
    * @param string text to write
    * \return none
    *************************************************/
    void writeString(std::string text);

    /**************************************************
    * Write recover information into the opened file
    * @param area
    * @param canvas
    * @param imgF
    * @param job
    * \return none
    *************************************************/
    void writeACIJ(int area,
        int canvas,
        int imgF,
        int job);

    /**************************************************
    * Read recover information from the opened file
    * @param area
    * @param canvas
    * @param imgF
    * @param job
    * \return none
    *************************************************/
    bool readACIJ(int* area,
        int* canvas,
        int* imgF,
        int* job);

    /**************************************************
    * Read integer
    * \return integer
    *************************************************/
    int readInteger();

    std::string read();

    /**************************************************
    * Close the opened file
    * \return none
    *************************************************/
    void closeFile();
};
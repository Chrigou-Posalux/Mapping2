#pragma once

#include <iostream>
#include <cstdarg>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <codecvt>

#include "FileLogger.h"

#define MAX_PATH_LENGTH     512

const std::string MappingCorrections = "mapping_corrections.csv";
const std::string MappingCorrections2 = "mapping_corrections-2.csv";

struct MappingTopology {
    double x = 0.0;
    double y = 0.0;
    double dX = 0.0;
    double dY = 0.0;
};

struct MappingTopologyInterpolation {
    double distance = 0.0;
    MappingTopology point;
};

struct Position
{
	double x{ 0.0 };	/**< x position */
	double y{ 0.0 };	/**< y position */

	/**************************************************
	* Constructor.
	*************************************************/
	Position() {}

	/**************************************************
	* Parameterized constructor.
	*
	* @param x X value
	* @param y Y value
	*************************************************/
	Position(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	/**************************************************
	* Copy constructor.
	*************************************************/
	Position(Position const& other)
	{
		this->x = other.x;
		this->y = other.y;
	}

	/**************************************************
	* Move constructor.
	*************************************************/
	Position(Position&& other) noexcept
	{
		this->x = other.x;
		this->y = other.y;
	}

	/**************************************************
	* Copy assignment.
	*************************************************/
	Position& operator=(const Position& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	/**************************************************
	* Move assignment.
	*************************************************/
	Position& operator=(Position&& other) noexcept
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	/**************************************************
	* Destructor.
	*************************************************/
	~Position() {}
};

bool m_bDrillMappingFile = false;

int nbOfDrillMappingPoints;

std::vector<MappingTopology> mapDrillPoints;

const std::string strPrecision(double value, int decimals);

double_t computeDistanceBetween2Points(double_t value1, double_t value2);

double_t computeZInterpolatedFrom2Z(double_t xValue, double_t xMin, double_t xPercent,
    double_t z1, double_t z2);

double_t computeZInterpolatedFrom4Z(double_t xValue, double_t xMin, double_t xPercent,
    double_t yValue, double_t yMin, double_t yPercent,
    double_t z1, double_t z2, double_t z3, double_t z4);

bool getTableXYCorrection(bool interpolate, int nbPoints,
    const std::vector<MappingTopology>& mapPoints, MappingTopology* point);

void LOG_notice(const char* format, ...);

std::vector<MappingTopology> checkMappingFile(bool* flag, int* nbPoints, const std::string fileName);

void createMapping2();

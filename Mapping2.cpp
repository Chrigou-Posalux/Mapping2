#include "Mapping2.h"

int main()
{
    std::cout << "Mapping2 utility:\n";

	mapDrillPoints = checkMappingFile(&m_bDrillMappingFile, &nbOfDrillMappingPoints, MappingCorrections);
	if (mapDrillPoints.empty())
	{
		LOG_notice("Missing or empty mapping_corrections.csv file in the same directory as this executable!");
		return -1;
	}
	createMapping2();
	return 0;
}

void LOG_notice(const char* format, ...)
{
	// This is a placeholder for the actual logging implementation.
	// In a real application, this would log the message to a file or console.
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	printf("\n");
	va_end(args);
}

const std::string strPrecision(double value, int decimals)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(decimals) << value;
	return stream.str();
}

bool bFileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

double_t computeZInterpolatedFrom2Z(double_t xValue, double_t xMin, double_t xPercent,
	double_t z1, double_t z2)
{
	double_t zValue = 0;
	double_t xpc = 0.0;

	if (xPercent != 0.0)
	{
		xpc = (xValue - xMin) / xPercent;
	}
	zValue = z1 * (1.0 - xpc) + z2 * xpc;

	return zValue;
}

double_t computeZInterpolatedFrom4Z(double_t xValue, double_t xMin, double_t xPercent,
	double_t yValue, double_t yMin, double_t yPercent,
	double_t z1, double_t z2, double_t z3, double_t z4)
{
	double_t zValue = 0;
	double_t xpc = 0.0;
	double_t ypc = 0.0;

	if (xPercent != 0.0)
	{
		xpc = (xValue - xMin) / xPercent;
	}
	if (yPercent != 0.0)
	{
		ypc = (yValue - yMin) / yPercent;
	}
	zValue = z1 * (1.0 - xpc) * (1.0 - ypc) +
		z2 * xpc * (1.0 - ypc) +
		z3 * (1.0 - xpc) * ypc +
		z4 * xpc * ypc;

	return zValue;
}

double_t computeDistanceBetween2Points(double_t value1, double_t value2)
{
	return abs(value1 - value2);
}

bool getTableXYCorrection(bool interpolate, int nbPoints,
	const std::vector<MappingTopology>& mapPoints, MappingTopology* point)
{
	const int FOUR = 4;
	double lastDistance = 999.9;
	MappingTopologyInterpolation nearestPoint;
	std::vector<MappingTopologyInterpolation> fourNearestPoints;

	if (nbPoints == 0)
	{
		return false;
	}

	nearestPoint.distance = 999.0;
	nearestPoint.point.x = 999.0;
	nearestPoint.point.y = 999.0;
	nearestPoint.point.dX = 0.0;
	nearestPoint.point.dY = 0.0;

	if (interpolate)
	{
		if (nbPoints < FOUR)
		{
			// We cannot interpolate if we don't have at least 4 points!
			return false;
		}

		for (int i = 0; i < FOUR; i++)
		{
			fourNearestPoints.push_back(nearestPoint);
		}
	}

	for (int i = 0; i < nbPoints; ++i)
	{
		double distance = sqrt(pow(mapPoints.at(i).x - point->x, 2.0) + pow(mapPoints.at(i).y - point->y, 2.0));
		if (distance < lastDistance)
		{
			lastDistance = distance;
			nearestPoint.distance = distance;
			nearestPoint.point.x = mapPoints.at(i).x;
			nearestPoint.point.y = mapPoints.at(i).y;
			nearestPoint.point.dX = mapPoints.at(i).dX;
			nearestPoint.point.dY = mapPoints.at(i).dY;

			if (interpolate)
			{
				// Add the result to the top of the vector
				auto it = fourNearestPoints.begin();
				it = fourNearestPoints.insert(it, nearestPoint);

				if (fourNearestPoints.size() > FOUR)
				{
					// Keep only the 4 first coordinates
					fourNearestPoints.pop_back();
				}
			}
		}
		else if (interpolate)
		{
			// Search in the other points if we are near
			int size = fourNearestPoints.size();
			auto it = fourNearestPoints.begin();
			for (int j = 1; j < size; j++)
			{
				it++;
				if (distance < fourNearestPoints.at(j).distance)
				{
					// Add the result to the right place of the vector
					MappingTopologyInterpolation mtiPoint;
					mtiPoint.distance = distance;
					mtiPoint.point.x = mapPoints.at(i).x;
					mtiPoint.point.y = mapPoints.at(i).y;
					mtiPoint.point.dX = mapPoints.at(i).dX;
					mtiPoint.point.dY = mapPoints.at(i).dY;
					it = fourNearestPoints.insert(it, mtiPoint);

					if (fourNearestPoints.size() > FOUR)
					{
						// Keep only the 4 first coordinates
						fourNearestPoints.pop_back();
					}

					break;	// for
				}
			}
		}
	}

	if (interpolate)
	{
		double x100 = 0.0;
		double y100 = 0.0;
		// The fourNearestPoints are ordered from nearest to furthest away
		if (computeDistanceBetween2Points(point->x, fourNearestPoints.at(0).point.x) < 0.01)
		{
			// The point is on the Y axis, interpolate between 2 nearest points
			y100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.y, fourNearestPoints.at(1).point.y);
			nearestPoint.point.dX = computeZInterpolatedFrom2Z(point->y, fourNearestPoints.at(0).point.y, y100,
				fourNearestPoints.at(0).point.dX, fourNearestPoints.at(1).point.dX);
			nearestPoint.point.dY = computeZInterpolatedFrom2Z(point->y, fourNearestPoints.at(0).point.y, y100,
				fourNearestPoints.at(0).point.dY, fourNearestPoints.at(1).point.dY);
		}
		else if (computeDistanceBetween2Points(point->y, fourNearestPoints.at(0).point.y) < 0.01)
		{
			// The point is on the X axis, interpolate between 2 nearest points
			x100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.x, fourNearestPoints.at(1).point.x);
			nearestPoint.point.dX = computeZInterpolatedFrom2Z(point->x, fourNearestPoints.at(0).point.x, x100,
				fourNearestPoints.at(0).point.dX, fourNearestPoints.at(1).point.dX);
			nearestPoint.point.dY = computeZInterpolatedFrom2Z(point->x, fourNearestPoints.at(0).point.x, x100,
				fourNearestPoints.at(0).point.dY, fourNearestPoints.at(1).point.dY);
		}
		else
		{
			// The point is inside the area, interpolate between 4 nearest points
			// First, find the 100% steps for both axes
			double xtmp = computeDistanceBetween2Points(fourNearestPoints.at(0).point.x, fourNearestPoints.at(1).point.x);
			x100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.x, fourNearestPoints.at(2).point.x);
			x100 = std::max(xtmp, x100);
			x100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.x, fourNearestPoints.at(3).point.x);
			x100 = std::max(xtmp, x100);
			double ytmp = computeDistanceBetween2Points(fourNearestPoints.at(0).point.y, fourNearestPoints.at(1).point.y);
			y100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.y, fourNearestPoints.at(2).point.y);
			y100 = std::max(ytmp, y100);
			y100 = computeDistanceBetween2Points(fourNearestPoints.at(0).point.y, fourNearestPoints.at(3).point.y);
			y100 = std::max(ytmp, y100);
			nearestPoint.point.dX = computeZInterpolatedFrom4Z(point->x, fourNearestPoints.at(0).point.x, x100,
				point->y, fourNearestPoints.at(0).point.y, y100,
				fourNearestPoints.at(0).point.dX, fourNearestPoints.at(1).point.dX, fourNearestPoints.at(2).point.dX, fourNearestPoints.at(3).point.dX);
			nearestPoint.point.dY = computeZInterpolatedFrom4Z(point->y, fourNearestPoints.at(0).point.y, y100,
				point->x, fourNearestPoints.at(0).point.x, x100,
				fourNearestPoints.at(0).point.dY, fourNearestPoints.at(1).point.dY, fourNearestPoints.at(2).point.dY, fourNearestPoints.at(3).point.dY);
		}
		//LOG_notice("x100=%s y100=%s nearest points: (%s, %s), (%s, %s), (%s, %s), (%s, %s)",
		//	strPrecision(x100, 2).c_str(), strPrecision(y100, 2).c_str(),
		//	strPrecision(fourNearestPoints.at(0).point.x, 2).c_str(), strPrecision(fourNearestPoints.at(0).point.y, 2).c_str(),
		//	strPrecision(fourNearestPoints.at(1).point.x, 2).c_str(), strPrecision(fourNearestPoints.at(1).point.y, 2).c_str(),
		//	strPrecision(fourNearestPoints.at(2).point.x, 2).c_str(), strPrecision(fourNearestPoints.at(2).point.y, 2).c_str(),
		//	strPrecision(fourNearestPoints.at(3).point.x, 2).c_str(), strPrecision(fourNearestPoints.at(3).point.y, 2).c_str());
	}

	point->dX = nearestPoint.point.dX;
	point->dY = nearestPoint.point.dY;
	return true;
}

double parseFloat(const std::string& s)
{
	std::size_t pos = 0;
	double v = std::stof(s, &pos);
	if (pos != s.size())
		throw std::invalid_argument("Invalid double");
	return v;
}

std::vector<MappingTopology> checkMappingFile(bool* flag, int* nbPoints, const std::string fileName)
{
	std::string line;
	double Xmin = 0.0;
	double Ymin = 0.0;
	double Xmax = 0.0;
	double Ymax = 0.0;
	std::vector<MappingTopology> mapPoints;
	char bufTmp[MAX_PATH_LENGTH + 1];
	*flag = false;
	*nbPoints = 0;
	strncpy_s(bufTmp, fileName.c_str(), MAX_PATH_LENGTH);
	bufTmp[MAX_PATH_LENGTH] = '\0';
	if (!bFileExists(bufTmp))
	{
		return mapPoints;
	}
	LOG_notice("%s file exists, trying to read it...", bufTmp);

	std::ifstream file(bufTmp, std::ios::binary);
	if (!file.is_open())
	{
		file.close();
		LOG_notice("%s file cannot be opened!", bufTmp);
		return mapPoints;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string decryptedtext = buffer.str(); // Contenu encodé en Base64

	if (decryptedtext.empty()) {
		LOG_notice("%s file does not contains values!", bufTmp);
		return mapPoints;
	}

	file.close();

	std::vector<std::string> lines;
	std::istringstream stream(decryptedtext);
	//std::string line;
	while (std::getline(stream, line)) {
		//lines.push_back(line);
		(*nbPoints)++;
	}

	if (*nbPoints)
	{
		stream.clear();
		stream.seekg(0, stream.beg);
		int incr = 0;
		while (getline(stream, line))
		{
			if ((line.at(0) == '#') || (line.at(0) == '\r'))
			{
				continue;	// to next line
			}
			// decode the line
			bool bErrorInLine = false;
			int iMax = 4;	// max 4 values
			double fValues[4];
			std::stringstream comm;
			comm.str(line.substr(0));
			std::string sstr;
			char separator = ';';	// .CSV separator
			while (getline(comm, sstr, separator))
			{
				if (iMax == 0)
				{
					break;
				}
				try {
					fValues[iMax - 1] = parseFloat(sstr);
				}
				catch (std::invalid_argument& ex) {
					LOG_notice("===> Error at line %d, please check your file!", incr + 1);
					(*nbPoints)--;
					break;
				}
				iMax--;
				if (iMax < 0)
				{
					bErrorInLine = true;	// more than 4 values in the line
					LOG_notice("===> Error at line %d, please check your file!", incr + 1);
					(*nbPoints)--;
					break;	// while (Security)
				}
			}
			line.clear();
			if (iMax != 0)
			{
				bErrorInLine = true;	// less than 4 values in the line
			}
			if (bErrorInLine == false)
			{
				Xmin = std::min(Xmin, fValues[1]);
				Ymin = std::min(Ymin, fValues[0]);
				Xmax = std::max(Xmax, fValues[1]);
				Ymax = std::max(Ymax, fValues[0]);
				MappingTopology mapPoint{ fValues[3], fValues[2], fValues[1], fValues[0] };
				mapPoints.push_back(mapPoint);
				incr++;
			}
		}
		if (incr == *nbPoints)
		{
			LOG_notice("%s: successfully read %d points, Xrange=%s  Yrange=%s", bufTmp, *nbPoints,
				strPrecision(Xmax - Xmin, 4).c_str(), strPrecision(Ymax - Ymin, 4).c_str());
			*flag = true;

			if ((abs(Xmax - Xmin) < 0.0001) || (abs(Ymax - Ymin) < 0.0001))
			{
				LOG_notice("The range of mapping results should not be zero, please check your file!");
			}
		}
		else if (incr == 0)
		{
			LOG_notice("%s file does not contains valid lines!", bufTmp);
		}
		else
		{
			LOG_notice("%s file contains some invalid lines!", bufTmp);
			*flag = true;
		}
	}
	else
	{
		LOG_notice("%s empty file!", bufTmp);
	}
	return mapPoints;
}

bool copyFile(const char* source, const char* destination)
{
	std::ifstream  src(source, std::ios::binary);
	std::ofstream  dst(destination, std::ios::binary);

	dst << src.rdbuf();
	return true;
}

void createMapping2()
{
	LOG_notice("Creating mapping file, please wait...");

	char bufTmp[MAX_PATH_LENGTH + 1];
	MappingTopology position;
	double Xmin = 999.0;
	double Xmax = -999.0;
	double Ymin = 999.0;
	double Ymax = -999.0;
	double xStep = 999.0;
	double yStep = 999.0;
	std::vector<Position> interpolationList;
	if (nbOfDrillMappingPoints)
	{
		sprintf_s(bufTmp, "%s", MappingCorrections.c_str());
		std::string srcFile = bufTmp;
		sprintf_s(bufTmp, "%s", MappingCorrections2.c_str());
		std::string dstFile = bufTmp;
		copyFile(srcFile.c_str(), dstFile.c_str());
		FileLogger sflMappingXY(MappingCorrections2, true);
		// get min/max of coordinates in file
		for (int i = 0; i < nbOfDrillMappingPoints; ++i)
		{
			if (i > 0)
			{
				double xTemp = abs(mapDrillPoints.at(i).x - mapDrillPoints.at(0).x);
				xTemp = std::min(xStep, xTemp);
				if (xTemp > 0.00001)
				{
					xStep = xTemp;
				}
				double yTemp = abs(mapDrillPoints.at(i).y - mapDrillPoints.at(0).y);
				yTemp = std::min(yStep, yTemp);
				if (yTemp > 0.00001)
				{
					yStep = yTemp;
				}
			}
			Xmin = std::min(mapDrillPoints.at(i).x, Xmin);
			Ymin = std::min(mapDrillPoints.at(i).y, Ymin);
			Xmax = std::max(mapDrillPoints.at(i).x, Xmax);
			Ymax = std::max(mapDrillPoints.at(i).y, Ymax);
		}

		// create now a mapping every middle point
		//
		// 0 2 0 2 0
		// 3 1 3 1 3
		// 0 2 0 2 0
		// 3 1 3 1 3
		// 0 2 0 2 0
		//
		// 0 = original measure
		// 1 = first interpolation
		// 2 = second interpolation
		// 3 = third interpolation
		xStep = xStep / 2.0;
		yStep = yStep / 2.0;
		int i = 0;
		int j = 0;
		Position interpolPoint;
		// first interpolation
		for (double y = Ymin + yStep - 0.0001, j = 0; y <= Ymax; y += (2 * yStep), j++)
		{
			for (double x = Xmin + xStep - 0.0001, i = 0; x <= Xmax; x += (2 * xStep), i++)
			{
				interpolPoint.x = x;
				interpolPoint.y = y;
				interpolationList.push_back(interpolPoint);
			}
		}

		// second interpolation
		for (double y = Ymin - 0.0001, j = 0; y <= Ymax; y += (2 * yStep), j++)
		{
			for (double x = Xmin + xStep - 0.0001, i = 0; x <= Xmax; x += (2 * xStep), i++)
			{
				interpolPoint.x = x;
				interpolPoint.y = y;
				interpolationList.push_back(interpolPoint);
			}
		}

		// third interpolation
		for (double y = Ymin + yStep - 0.0001, j = 0; y <= Ymax; y += (2 * yStep), j++)
		{
			for (double x = Xmin - 0.0001, i = 0; x <= Xmax; x += (2 * xStep), i++)
			{
				interpolPoint.x = x;
				interpolPoint.y = y;
				interpolationList.push_back(interpolPoint);
			}
		}

		int idx = 0;
		int progress = 0;
		int last_progress = 0;
		int total = interpolationList.size();

		for (auto iPt : interpolationList)
		{
			position.x = iPt.x;
			position.y = iPt.y;
			position.dX = 0.0;
			position.dY = 0.0;
			getTableXYCorrection(true, nbOfDrillMappingPoints, mapDrillPoints, &position);
			sflMappingXY.writeString(
				strPrecision(position.x, 2) + ";" +
				strPrecision(position.y, 2) + ";" +
				strPrecision(position.dX, 4) + ";" +
				strPrecision(position.dY, 4) + ";");
			idx++;
			progress = (idx * 100) / total;
			if (progress != last_progress)
			{
				std::cout << "\r" << progress << "% ";
				last_progress = progress;
			}
		}
		sflMappingXY.closeFile();
	}
	LOG_notice("\nmapping_corrections-2.csv file successfully created!");
}

#include "Application.h"
#include "UtilityStats.h"
#include <iostream>
#include <fstream>
#include "BST.h"
#include <cmath>


double CalculateMAD(const WeatherLog& log, int year, int month, int type, double mean)
{
    double sum = 0;
    int count = 0;

    for (int i = 0; i < log.GetSize(); i++)
    {
        const WeatherRec& rec = log.GetRecord(i);

        if (rec.GetDate().GetYear() == year &&
            rec.GetDate().GetMonth() == month)
        {
            double value = 0;
            bool valid = false;

            if (type == 1 && rec.HasSpeed())
            {
                value = rec.GetSpeed();
                valid = true;
            }
            else if (type == 2 && rec.HasTemp())
            {
                value = rec.GetTemperature();
                valid = true;
            }
            else if (type == 3 && rec.HasSolar())
            {
                value = rec.GetSolarRadiation();
                valid = true;
            }

            if (valid)
            {
                sum += std::abs(value - mean);
                count++;
            }
        }
    }

    if (count == 0) return 0;
    return sum / count;
}

// Constructor for reference to WeatherLog.
Application::Application(WeatherLog& log)
    : m_log(log)
{
    for (int i = 0; i < m_log.GetSize(); i++)
    {
        const WeatherRec& rec = m_log.GetRecord(i);
        int month = rec.GetDate().GetMonth();

        m_dataMap.insert({month, rec});

        if (!m_tree.Search(month))
        {
            m_tree.Insert(month);
        }
    }
}
// Runs the main menu loop.
void Application::Run()
{
    int choice = 0;

    do
    {
        DisplayMenu();
        choice = ReadInt();

        switch (choice)
        {
        case 1:
            DoOption1();
            break;
        case 2:
            DoOption2();
            break;
        case 3:
            DoOption3();
            break;
        case 4:
            DoOption4();
            break;
        case 5:
            DoBST();
            break;

        case 6:
            std::cout << "Exiting...\n";
            break;
        }
    }
    while (choice != 6);
}

// Displays the menu options.
void Application::DisplayMenu() const
{
    std::cout << "1. Wind speed stats (month/year)\n";
    std::cout << "2. Temperature stats (year)\n";
    std::cout << "3. sPCC calculation (month)\n";    
    std::cout << "4. Generate WindTempSolar.csv\n";
    std::cout << "5. BST Traversal\n";
    std::cout << "6. Exit\n";

}

// Reads an integer from the user.
int Application::ReadInt() const
{
    int value;
    std::cin >> value;
    return value;
}

// Returns the English month name.
const char* Application::GetMonthName(int month) const
{
    static const char* names[] =
    {
        "January","February","March","April","May","June",
        "July","August","September","October","November","December"
    };

    if (month >= 1 && month <= 12)
    {
        return names[month - 1];
    }
    else
    {
        return "";
    }
}

// Menu option 1 for wind stats.
void Application::DoOption1()
{
    std::cout << "Enter month (1-12): ";
    int month = ReadInt();

    std::cout << "Enter year: ";
    int year = ReadInt();

    if (month < 1 || month > 12)
    {
        std::cout << "Invalid month.\n";
        return;
    }

    double sum = 0;
int count = 0;

auto range = m_dataMap.equal_range(month);

for (auto it = range.first; it != range.second; ++it)
{
    const WeatherRec& rec = it->second;

    if (rec.GetDate().GetYear() == year && rec.HasSpeed())
    {
        sum += rec.GetSpeed();
        count++;
    }
}

if (count == 0)
{
    std::cout << GetMonthName(month) << " " << year << ": No Data\n";
    return;
}

double mean = sum / count;

// standard deviation
double variance = 0;

for (auto it = range.first; it != range.second; ++it)
{
    const WeatherRec& rec = it->second;

    if (rec.GetDate().GetYear() == year && rec.HasSpeed())
    {
        variance += (rec.GetSpeed() - mean) * (rec.GetSpeed() - mean);
    }
}

double sd = std::sqrt(variance / count);

std::cout << GetMonthName(month) << " " << year << ":\n";
std::cout << "Average speed: " << mean << " km/h\n";
std::cout << "Sample stdev: " << sd << "\n";
}

// Menu option 2 for temperature stats.
void Application::DoOption2()
{
    std::cout << "Enter year: ";
    int year = ReadInt();

    std::cout << year << "\n";

    for (int month = 1; month <= 12; ++month)
    {
        double mean = UtilityStats::MeanTemp(m_log, year, month);

        if (mean == 0.0)
        {
            std::cout << GetMonthName(month) << ": No Data\n";
        }
        else
        {
            double sd = UtilityStats::SDTemp(m_log, year, month, mean);

            std::cout << GetMonthName(month)
                      << ": average: " << mean
                      << " degrees C, stdev: " << sd
                      << "\n";
        }
    }
}

//Option 3
void Application::DoOption3()
{
    int month, year;

    std::cout << "Enter month (1-12): ";
    month = ReadInt();

    std::cout << "Enter year: ";
    year = ReadInt();

    int choice1, choice2;

    std::cout << "\nSelect first variable:\n";
    std::cout << "1. Temperature\n2. Wind Speed\n3. Solar Radiation\n";
    choice1 = ReadInt();

    std::cout << "\nSelect second variable:\n";
    std::cout << "1. Temperature\n2. Wind Speed\n3. Solar Radiation\n";
    choice2 = ReadInt();

    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0, sumY2 = 0;
    int n = 0;

    for (int i = 0; i < m_log.GetSize(); i++)
    {
        const WeatherRec& rec = m_log.GetRecord(i);

        if (rec.GetDate().GetMonth() == month &&
            rec.GetDate().GetYear() == year)
        {
            double x = 0, y = 0;
            bool validX = false, validY = false;

            // X variable
            if (choice1 == 1 && rec.HasTemp())
            {
                x = rec.GetTemperature();
                validX = true;
            }
            else if (choice1 == 2 && rec.HasSpeed())
            {
                x = rec.GetSpeed();
                validX = true;
            }
            else if (choice1 == 3 && rec.HasSolar())
            {
                x = rec.GetSolarRadiation();
                validX = true;
            }

            // Y variable
            if (choice2 == 1 && rec.HasTemp())
            {
                y = rec.GetTemperature();
                validY = true;
            }
            else if (choice2 == 2 && rec.HasSpeed())
            {
                y = rec.GetSpeed();
                validY = true;
            }
            else if (choice2 == 3 && rec.HasSolar())
            {
                y = rec.GetSolarRadiation();
                validY = true;
            }

            if (validX && validY)
            {
                sumX += x;
                sumY += y;
                sumXY += x * y;
                sumX2 += x * x;
                sumY2 += y * y;
                n++;
            }
        }
    }

    if (n < 2)
    {
        std::cout << "Not enough data\n";
        return;
    }

    double numerator = n * sumXY - sumX * sumY;
    double denominator = std::sqrt((n * sumX2 - sumX * sumX) *
                              (n * sumY2 - sumY * sumY));

    if (denominator == 0)
    {
        std::cout << "Division by zero error\n";
        return;
    }

    double r = numerator / denominator;

    std::cout << "\n📊 sPCC Result: " << r << std::endl;
}


// Menu option 4 and writes to output CSV file.
void Application::DoOption4()
{
    std::cout << "Enter year: ";
    int year = ReadInt();

    std::ofstream out("WindTempSolar.csv");
    if (!out)
    {
        std::cout << "Error creating WindTempSolar.csv\n";
        return;
    }

    out << year << "\n";

    bool yearHasData = false;

    for (int month = 1; month <= 12; ++month)
    {
        if (UtilityStats::HasAnyDataForMonth(m_log, year, month))
        {
            yearHasData = true;

            double meanWind = UtilityStats::MeanWind(m_log, year, month);
            double meanTemp = UtilityStats::MeanTemp(m_log, year, month);
            double totalSolar = UtilityStats::SolarTotal(m_log, year, month);

            out << GetMonthName(month) << ",";

            if (meanWind != 0.0)
            {
                double sdWind = UtilityStats::SDWind(m_log, year, month, meanWind);
                double madWind = CalculateMAD(m_log, year, month, 1, meanWind);

                out << meanWind << "(" << sdWind << "," << madWind << ")";
            }
            out << ",";

            if (meanTemp != 0.0)
            {
                double sdTemp = UtilityStats::SDTemp(m_log, year, month, meanTemp);
                double madTemp = CalculateMAD(m_log, year, month, 2, meanTemp);

                out << meanTemp << "(" << sdTemp << "," << madTemp << ")";
}
            out << ",";

            if (totalSolar != 0.0)
            {
                double madSolar = CalculateMAD(m_log, year, month, 3, totalSolar);
                out << totalSolar << "(0," << madSolar << ")";
            }

            out << "\n";
        }
    }

    if (!yearHasData)
    {
        out << "No Data\n";
    }

    out.close();
    std::cout << "WindTempSolar.csv generated successfully.\n";
}


void PrintMonth(const int& month)
{
    std::cout << "Month: " << month << std::endl;
}

void Application::DoBST()
{
    std::cout << "\n--- BST Months ---\n";
    m_tree.Inorder(PrintMonth);
}
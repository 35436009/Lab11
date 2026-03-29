#ifndef APPLICATION_H
#define APPLICATION_H

#include "WeatherLog.h"
#include <map>        // ✅ ADD
#include "BST.h"      // ✅ ADD

class Application
{
public:
    Application(WeatherLog& log);
    void Run();

private:
    WeatherLog& m_log;

    std::multimap<int, WeatherRec> m_dataMap;  // ✅ ADD
    BST<int> m_tree;                           // ✅ ADD

    void DisplayMenu() const;
    int ReadInt() const;
    const char* GetMonthName(int month) const;

    void DoOption1();
    void DoOption2();
    void DoOption3();
    void DoOption4();
    void DoBST();
};

#endif
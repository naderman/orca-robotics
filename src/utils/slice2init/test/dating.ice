#ifndef TEST_DATING_ICE
#define TEST_DATING_ICE

module test
{

enum Desirability
{
    Low,
    Medium,
    High
};

struct Time
{
    int seconds;
    int useconds;
};

struct Date
{   
    // 'time' would clash with 'Time'
    Time tiempo;
    string name;
};

sequence<Date> DateSeq;

dictionary<string,Desirability> NameDesirabilityDict;

class Place
{
    double x;
};

class Restaurant extends Place
{
    string name;
};

interface Calendar
{
    Date getNextDate();
//         throws DataNotExistException, HardwareFailedException;
};

struct Information
{
    Calendar* cal;
};

}; // module

#endif

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

interface Calendar
{
    Date getNextDate();
//         throws DataNotExistException, HardwareFailedException;
};

}; // module

#endif

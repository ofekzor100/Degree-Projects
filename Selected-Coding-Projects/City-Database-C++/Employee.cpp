#include "Employee.h"
#include <iostream>
#include <string>
using std::endl;
using namespace std;
using std::string;

namespace mtm {

    using namespace mtm;
    Employee::Employee(int id, string firstName, string lastName, int birthYear)
: Citizen(id, firstName, lastName, birthYear), salary(0),score(0),skills{}{}

int Employee::getSalary() const {
    return salary;
}

int Employee::getScore() const {
    return score;
}

void Employee::learnSkill(const Skill& skill)
{
    if (skills.find(skill)!=skills.end())
    {
        throw SkillAlreadyLearned();
    }
    if(score<skill.getRequiredPoints())
    {
        throw CanNotLearnSkill();
    }
    skills.insert(skill);
}

void Employee::forgetSkill(const int& id)
{
    for (set<Skill>::iterator skill=skills.begin(); skill!=skills.end(); skill++)
    {
        if (skill->getId() == id)
        {
            skills.erase(skill);
            return;
        }
    }
    throw DidNotLearnSkill();
}

bool Employee::hasSkill(const int& id) const
{
    for (set<Skill>::iterator skill=skills.begin(); skill!=skills.end(); skill++)
    {
        if (skill->getId() == id)
        {
            return true;
        }
    }
    return false;
}

void Employee::setSalary(const int& salaryToSet)
{
    if(salary + salaryToSet < 0)
    {
        salary = 0;
        return;
    }
    salary+=salaryToSet;
}

void Employee::setScore(const int& scoreToSet)
{
    if (score + scoreToSet < 0)
    {
        score=0;
        return;
    }
    score+=scoreToSet;
}

std::ostream& Employee::printShort(std::ostream& stream) const
{
    citizenPrintShortAndLong(stream,false);
    return stream << getSalary() << " Score: " << getScore() << endl;
}

std::ostream& Employee::printLong(std::ostream& stream) const
{
    citizenPrintShortAndLong(stream,true);
    stream << getSalary() << " Score: " << getScore();
    if(!(skills.empty()))
    {
        stream << " Skills: " << endl;
    }
    else
    {
        stream<<endl;
    }
    for (set<Skill>::iterator skill=skills.begin(); skill!=skills.end(); skill++)
    {
        stream << *skill;
    }
    return stream;
}

Employee* Employee::clone() const
{
    return new Employee(*this);
}

}
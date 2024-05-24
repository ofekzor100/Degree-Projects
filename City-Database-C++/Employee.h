#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include <iostream>
#include <set>
#include "Skill.h"
#include "Citizen.h"
using std::string;
using std::ostream;
using std::set;
using std::iterator;

namespace mtm {
class Employee : public Citizen {
    int salary;
    int score;
    set<Skill> skills;

public:
    Employee(int id, string firstName, string lastName, int birthYear);
    ~Employee() override = default;
    Employee& operator=(const Employee&)=default;
    Employee(const Employee&)=default;
    int getSalary() const;
    int getScore() const;
    void learnSkill(const Skill& skill);
    void forgetSkill(const int& id);
    bool hasSkill(const int& id) const;
    void setSalary(const int& salaryToSet);
    void setScore(const int& scoreToSet);
    std::ostream& printShort(std::ostream& stream) const override;
    std::ostream& printLong(std::ostream& stream) const override;
    Employee* clone() const override;
};

}
#endif //EMPLOYEE_H
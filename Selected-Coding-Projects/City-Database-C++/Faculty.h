//
// Created by ofekz on 12/30/2021.
//

#ifndef CLASSES_FACULTY_H
#define CLASSES_FACULTY_H

#include <iostream>
#include "Skill.h"
#include "Employee.h"
#include <memory>
using std::string;
using std::ostream;
using namespace std;

namespace mtm{

    using namespace mtm;

class Condition{
public:
    virtual bool operator()(Employee* employee)=0;
};

template<class T>
class Faculty{
    int Id;
    Skill skill;
    int AddedPoints;
    T* condition;

public:
    explicit Faculty(int Id,const Skill& skill,int AddedPoints,T* condition);
    ~Faculty()=default;
    Faculty& operator=(const Faculty&)=default;
    Faculty(const Faculty&)=default;
    int getId() const;
    int getAddedPoints() const;
    Skill getSkill() const ;
    void teach(Employee* employee);

};

template<class T>
Faculty<T>::Faculty(int Id,const Skill& skill,int AddedPoints,T* condition) :
        Id(Id), skill(skill) , AddedPoints(AddedPoints), condition(condition)
{
}

template<class T>
int Faculty<T>::getId() const
{
    return Id;
}

template<class T>
int Faculty<T>::getAddedPoints() const
{
    return AddedPoints;
}

template<class T>
Skill Faculty<T>::getSkill() const
{
    return skill;
}

template<class T>
void Faculty<T>:: teach(Employee* employee)
{
    if((*condition)(employee))
    {
        employee->learnSkill(getSkill()) ;
        employee->setScore(AddedPoints);
    }
    else
    {
        throw EmployeeNotAccepted();
    }
}
}

#endif //CLASSES_FACULTY_H

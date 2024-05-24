//
// Created by illus on 28/12/2021.
//

#include "Manager.h"
#include "Citizen.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using std::string;
using std::set;
using std::vector;

namespace mtm {

    using namespace mtm;

Manager::Manager(int id, string firstName, string lastName, int birthYear) : Citizen(id, firstName, lastName, birthYear), salary(0), employees{}
{}

Manager::Manager(const Manager& manager) : Citizen(manager), salary(manager.getSalary()), employees{manager.employees}
{
}

int Manager::getSalary() const
{
    return salary;
}

void Manager::addEmployee(Employee* employee)
{
    if (find(employees.begin(),employees.end(), employee)!=employees.end())
    {
        throw EmployeeAlreadyHired();
    }
    employees.push_back(employee);
    sort(employees.begin(), employees.end(), compareCitizensByPointer);
}

void Manager::removeEmployee(int id)
{
    for (unsigned int i=0; i<employees.size(); i++)
    {
        if (employees[i]->getId() == id)
        {
            employees.erase(employees.begin()+i);
            return;
        }
    }
    throw EmployeeIsNotHired();
}

void Manager::setSalary(const int& salaryToSet)
{
    if(this->salary + salaryToSet < 0)
        return;
    salary+=salaryToSet;
}

std::ostream& Manager::printShort(std::ostream& stream) const
{
    citizenPrintShortAndLong(stream,false);
    return stream << getSalary() << endl;
}

std::ostream& Manager::printLong(std::ostream& stream) const
{
    citizenPrintShortAndLong(stream, true);
    stream << getSalary() << endl;
    if(!(employees.empty()))
    {
        stream << "Employees: " << endl;
    }
    for (unsigned int i=0; i<employees.size(); i++)
    {
        employees[i]->printShort(stream);
    }
    return stream;
}

Manager* Manager::clone() const
{
    return new Manager(*this);
}

vector<Employee*> Manager::getEmployees() const
{
    return employees;
}

void Manager::removeAllEmployees(int groupSalaryToReduce)
{
    setEmployeesSalaries(-groupSalaryToReduce);
    employees.clear();
}

void Manager::setEmployeesSalaries(int SalaryToSet) const
{
    for (unsigned int i=0; i<employees.size(); i++)
    {
        employees[i]->setSalary(SalaryToSet);
    }
}

}
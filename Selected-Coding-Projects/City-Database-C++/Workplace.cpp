//
// Created by illus on 29/12/2021.
//

#include "Workplace.h"
#include <algorithm>

using std::endl;

namespace mtm{

using namespace mtm;

Workplace::Workplace(const int& id, const string& workplaceName, const int& workersSalary, const int& managersSalary)
: id(id), workplaceName(workplaceName), workersSalary(workersSalary), managersSalary(managersSalary), managers{}{}

int Workplace::getId() const
{
    return id;
}

string Workplace::getName() const
{
    return workplaceName;
}

int Workplace::getWorkersSalary() const
{
    return workersSalary;
}

int Workplace::getManagersSalary() const
{
    return managersSalary;
}

void Workplace::hireManager(Manager* manager)
{
    if (find(managers.begin(), managers.end(), manager)!=managers.end())
    {
        throw ManagerAlreadyHired();
    }
    if (manager->getSalary()>0)
    {
        throw CanNotHireManager();
    }
    manager->setSalary(managersSalary);
    manager->setEmployeesSalaries(workersSalary);
    managers.push_back(manager);
    sort(managers.begin(), managers.end(), compareCitizensByPointer);
}

void Workplace::fireEmployee(const int& employeeId, const int& managerId)
{
    for (unsigned int i=0; i<managers.size(); i++)
    {
        if (managers[i]->getId() == managerId)
        {
            vector<Employee*> employees=managers[i]->getEmployees();
            for (unsigned int i=0; i<employees.size(); i++)
            {
                if (employees[i]->getId() == employeeId)
                {
                    employees[i]->setSalary(-workersSalary);
                }
            }
            managers[i]->removeEmployee(employeeId);
            return;
        }
    }
    throw ManagerIsNotHired();
}

void Workplace::fireManager(const int& managerId)
{
    for (unsigned int i=0; i<managers.size(); i++)
    {
        if (managers[i]->getId() == managerId)
        {
            managers[i]->setEmployeesSalaries(-workersSalary);
            managers[i]->setSalary(-managersSalary);
            managers.erase(managers.begin()+i);
            return;
        }
    }
    throw ManagerIsNotHired();
}

ostream& operator<<(ostream& stream, const Workplace& workplace)
{
    stream << "Workplace name - " << workplace.workplaceName;
    if (workplace.managers.begin()!=workplace.managers.end())
    {
        stream << " Groups:" << endl;
    }
    else{
        stream<< endl;
    }
    for (unsigned int i=0; i<workplace.managers.size(); i++)
    {
        stream  << "Manager ";
        workplace.managers[i]->printLong(stream);
    }
    return stream;
}

    vector<Manager*> Workplace::getManagers(){
    return managers;
}

bool Workplace::isEmployeeInWorkplace(int employeeId) const
{
    for (unsigned int i=0; i<managers.size(); i++)
    {
        vector<Employee*> employees=managers[i]->getEmployees();
        for (unsigned int i=0; i<employees.size(); i++)
        {
            if (employees[i]->getId() == employeeId)
            {
                return true;
            }
        }
    }
    return false;
}

}
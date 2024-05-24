//
// Created by illus on 06/01/2022.
//

#ifndef CITY_H
#define CITY_H
#include <string>
#include <set>
#include "Workplace.h"
#include "Faculty.h"
#include "Manager.h"
#include "Employee.h"
#include <typeinfo>
#include <memory>
#include <vector>
using std::vector;
using std::string;
using std::set;
using namespace std;

namespace mtm {
class City
{
    string cityName;
    vector<Citizen*> citizens;
    set<Workplace*> workplaces;
    set<Faculty<Condition>*> faculties;
public:
    explicit City(string cityName);
    City(const City& city)=default;
    ~City();
    void addEmployee(int id, string firstName, string lastName, int birthYear);
    void addManager(int id, string firstName, string lastName, int birthYear);
    void addFaculty(int id,const Skill& skill,int AddedPoints,Condition* condition);
    void createWorkplace(const int& id, const string& workplaceName, const int& workersSalary, const int& managersSalary);
    void teachAtFaculty(int employeeId, int facultyId);
    bool isManagerInCity(int managerId) const;
    bool isEmployeeInCity(int employeeId) const;
    void hireManagerAtWorkplace(int managerId, int workplaceId);
    void fireEmployeeAtWorkplace (int employeeId, int managerId, int workplaceId);
    void fireManagerAtWorkplace (int managerId, int workplaceId);
    int getAllAboveSalary(std::ostream& ostream, int salary) const;
    bool isWorkingInTheSameWorkplace(int citizenId1, int citizenId2) const;
    void printAllEmployeesWithSkill(std::ostream& stream, const int& skillId) const;
    template<class T>
    void hireEmployeeAtWorkplace(T condition, const int& employeeId,const int& managerId, const int& workplaceId);
};

template<class T>
void City::hireEmployeeAtWorkplace(T condition, const int& employeeId,const int& managerId, const int& workplaceId)
{
    if (!isManagerInCity(managerId))
    {
        throw ManagerDoesNotExist();
    }
    for (set<Workplace*>::iterator workplace=workplaces.begin(); workplace!=workplaces.end(); workplace++)
    {
        if ((*workplace)->getId() == workplaceId)
        {
            for (unsigned int i=0; i<citizens.size(); i++)
            {
                if (citizens[i]->getId() == employeeId && typeid(*citizens[i]) == typeid(Employee))
                {
                    (*workplace)->hireEmployee(condition, dynamic_cast<Employee*>(citizens[i]),managerId);
                    return;
                }
            }
            throw EmployeeDoesNotExist();
        }
    }
    throw WorkplaceDoesNotExist();
}

}

#endif //CITY_H

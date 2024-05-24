//
// Created by illus on 29/12/2021.
//

#ifndef WORKPLACE_H
#define WORKPLACE_H
#include "Manager.h"
#include "Employee.h"
#include <string>
#include <vector>
#include <memory>
using std::string;
using std::vector;

namespace mtm {

using namespace mtm;
class Workplace
{
    int id;
    string workplaceName;
    int workersSalary;
    int managersSalary;
    vector<Manager*> managers;

public:
    Workplace(const int& id, const string& workplaceName, const int& workersSalary, const int& managersSalary);
    Workplace(const Workplace& workplace)=default;
    Workplace& operator=(const Workplace&)=default;
    ~Workplace()=default;
    int getId() const;
    vector<Manager*> getManagers();
    string getName() const;
    int getWorkersSalary() const;
    int getManagersSalary() const;
    void hireManager(Manager* manager);
    void fireEmployee(const int& employeeId, const int& managerId);
    void fireManager(const int& managerId);
    friend ostream& operator<<(ostream& stream, const Workplace& workplace);
    bool isEmployeeInWorkplace(int employeeId) const;
    template<class Condition>
    void hireEmployee(Condition condition, Employee* employeeToHire,const int& managerId){
        for (unsigned int i=0; i<managers.size(); i++)
        {
            if (managers[i]->getId() == managerId)
            {
                if(condition(employeeToHire))
                {
                    managers[i]->addEmployee(employeeToHire);
                    employeeToHire->setSalary(workersSalary);
                    return;
                }
                else
                {
                    throw EmployeeNotSelected();
                }
            }
        }
        throw ManagerIsNotHired();
    }
};

}

#endif //WORKPLACE_H

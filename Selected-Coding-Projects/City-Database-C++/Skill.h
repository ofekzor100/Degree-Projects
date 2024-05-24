//
// Created by illus on 29/12/2021.
//

#ifndef SKILL_H
#define SKILL_H

#include <iostream>
#include "exceptions.h"
using std::string;
using std::ostream;

namespace mtm {
class Skill {

    int id;
    string name;
    int requiredPoints;

public:
    Skill(int id, string name, int RequiredPoints);
    ~Skill()=default;
    Skill& operator=(const Skill&)=default;
    Skill(const Skill&)=default;
    int getId() const;
    string getName() const;
    int getRequiredPoints() const;
    friend ostream& operator<<(ostream& os,const Skill& skill_to_print);
    friend bool operator==(const Skill& skill1,const Skill& skill2);
    friend bool operator<(const Skill& skill1,const Skill& skill2);
    Skill operator++(int);
    Skill& operator+=(const int& to_add);

};

bool operator!=(const Skill& skill1, const Skill& skill2);
bool operator>(const Skill& skill1, const Skill& skill2);
bool operator>=(const Skill& skill1, const Skill& skill2);
bool operator<=(const Skill& skill1, const Skill& skill2);
Skill operator+(const Skill& skill1, const int& points);
Skill operator+(const int& points,const Skill& skill1);

}
#endif //SKILL_H

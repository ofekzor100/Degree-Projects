#include "Skill.h"

#include <utility>
using std::endl;
namespace mtm {

using namespace mtm;

Skill::Skill(int id, string name, int RequiredPoints) : id(id), name(name), requiredPoints(RequiredPoints)
{
}

int Skill::getId () const{
    return this->id;
}

string Skill::getName() const{
    return this->name;
}

int Skill::getRequiredPoints() const {
    return this->requiredPoints;
}

ostream& operator<<(ostream& os,const Skill& skill_to_print){

    return os<<skill_to_print.name << endl;
}


bool operator==(const Skill& skill1,const Skill& skill2){
    return skill1.id==skill2.id;
}

bool operator<(const Skill& skill1,const Skill& skill2){
    return skill1.id<skill2.id;
}

bool operator!=(const Skill& skill1,const Skill& skill2){
    return !(skill1==skill2);
}

bool operator>=(const Skill& skill1,const Skill& skill2){
    return !(skill1<skill2);
}

bool operator<=(const Skill& skill1,const Skill& skill2){
    return ((skill1<skill2)||(skill1==skill2));
}
bool operator>(const Skill& skill1,const Skill& skill2){
    return !(skill1<=skill2);
}

Skill Skill::operator++(int) {
    Skill s1=*this;
    s1.requiredPoints=requiredPoints++;
    return s1;
}

Skill& Skill::operator+=(const int& to_add){
    if(to_add<0)
    {
        throw NegativePoints();
    }
    requiredPoints+=to_add;
    return *this;
}

Skill operator+(const Skill& skill1,const int& points) {
    if(points<0)
    {
        throw NegativePoints();
    }
    return Skill(skill1)+=points;
}

Skill operator+(const int& points,const Skill& skill1) {
        return skill1 + points;
    }



}

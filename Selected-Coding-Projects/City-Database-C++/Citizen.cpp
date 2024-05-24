//
// Created by illus on 28/12/2021.
//

#include "Citizen.h"
using std::endl;

Citizen::Citizen(int id, string firstName, string lastName, int birthYear): id(id), firstName(firstName), lastName(lastName), birthYear(birthYear)
{}

int Citizen::getId() const
{
    return id;
}

string Citizen::getFirstName() const
{
    return firstName;
}

string Citizen::getLastName() const
{
    return lastName;
}

int Citizen::getBirthYear() const
{
    return birthYear;
}

bool operator==(const Citizen& citizen1, const Citizen& citizen2)
{
    return citizen1.id == citizen2.id;
}

bool operator>(const Citizen& citizen1, const Citizen& citizen2)
{
    return citizen1.id > citizen2.id;
}

bool operator!=(const Citizen& citizen1, const Citizen& citizen2)
{
    return !(citizen1==citizen2);
}

bool operator<=(const Citizen& citizen1, const Citizen& citizen2)
{
    return !(citizen1>citizen2);
}

bool operator<(const Citizen& citizen1, const Citizen& citizen2)
{
    return (citizen1<=citizen2) && (citizen1!=citizen2);
}

bool operator>=(const Citizen& citizen1, const Citizen& citizen2)
{
    return !(citizen1<citizen2);
}

void Citizen::citizenPrintShortAndLong (std::ostream& stream, bool isPrintLong) const
{
    stream << getFirstName() << " " << getLastName() << endl;
    if (isPrintLong)
    {
        stream << "id - " << getId() << " birth_year - " << getBirthYear() << endl;
    }
    stream << "Salary: ";
}

bool compareCitizensByPointer(Citizen* citizen1, Citizen* citizen2)
{
    return (*citizen1)<(*citizen2);
}
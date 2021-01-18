#include "Course.h"
#include "../GUI/GUI.h"
Course::Course(Course_Code r_code, string r_title, int crd):code(r_code),Title(r_title)
{
	credits = crd;
}

Course::~Course()
{
}

Course_Code Course::getCode() const
{
	return code;
}

string Course::getTitle() const
{
	return Title;
}

//return course credits
int Course::getCredits() const
{
	return credits;
}

void Course::set_Elective(bool b)
{
	Elective = b;
}

bool Course::is_ELective() const
{
	return Elective;
}

Course_type Course::getCourseType() const
{
	return c_type;
}

void Course::setCourseType(Course_type s)
{
	c_type = s;
}

void Course::setPreReq(list<Course_Code> p)
{
	PreReq = p;
}

void Course::setCoReq(list<Course_Code> c)
{
	CoReq = c ;
}

list<Course_Code> Course::retPreReq() const
{
	return PreReq;
}

list<Course_Code> Course::retCoReq() const
{
	return CoReq;
}

void Course::setError(bool e)
{
	Has_Error = e;
}

bool Course::retError() const
{
	return Has_Error;
}

void Course::DrawMe(GUI* pG) 
{
	pG->DrawCourse(this);
}
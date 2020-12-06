#include "ActionAddNote.h"
#include "..\Registrar.h"
#include "../Courses/UnivCourse.h"

ActionAddNote::ActionAddNote(Registrar* p) :Action(p)
{
}

bool ActionAddNote::Execute()
{
	GUI* pGUI = pReg->getGUI();
	StudyPlan* Ps = pReg->getStudyPlan();
	pGUI->PrintMsg("Add Note:");
	string note = pGUI->GetSrting();
	Ps->AddPlanNote(note);
	return true;
}


ActionAddNote::~ActionAddNote()
{
}
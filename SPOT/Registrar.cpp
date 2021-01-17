#include "Registrar.h"
#include "Actions/ActionAddCourse.h"
#include "Actions/ActionAddNote.h"
#include "Actions/ActionDeleteCourse.h"
#include "Actions/ActionMove.h"
#include "Actions/ActionMove.h"
#include "Actions/ActionEditCourseCode.h"
#include"Actions/ActionSave.h"
#include"Actions/ActionSave.h"
#include "Actions/ActionImportSplan.h"
#include <string>
#include <fstream>
#include <sstream>
//#include <iostream> //debug
using namespace std;

void Registrar::CheckRules()
{
	CurrentReqs.TotalCredsAchieved = (pSPlan->GetTotalcrds() >= RegRules.TotalCredit);
	CurrentReqs.MajorCredsAchieved = (pSPlan->GetMajorcrds() >= RegRules.ReqMajorCompulsoryCredits);
	CurrentReqs.UniversityCoursesAchieved = (pSPlan->GetUnivcrds() >= RegRules.ReqUnivCompulsoryCredits);
	CurrentReqs.TrackCredsAchieved = (pSPlan->GetTrackcrds() >= RegRules.ReqTrackCredits);
	CurrentReqs.MajorCoursesAchieved = true, CurrentReqs.UniversityCoursesAchieved = true, CurrentReqs.TrackCoursesAchieved = true;
	/// Check if one of the university courses is missing
	for (auto crs : RegRules.UnivCompulsory)
	{
		if (pSPlan->ReturnCoursePointer(crs) == nullptr)
		{
			CurrentReqs.UniversityCoursesAchieved = false;
			break;
		}
	}
	/// Check if one of the Track courses is missing
	for (auto crs : RegRules.TrackCompulsory)
	{
		if (pSPlan->ReturnCoursePointer(crs) == nullptr)
		{
			CurrentReqs.TrackCoursesAchieved = false;
			break;
		}
	}
	/// Check if one of the Major courses is missing
	for (auto crs : RegRules.MajorCompulsory)
	{
		if (pSPlan->ReturnCoursePointer(crs) == nullptr)
		{
			CurrentReqs.MajorCoursesAchieved = false;
			break;
		}
	}
}

void Registrar::SetCurrentIssue()
{
	if (!(CurrentReqs.TotalCredsAchieved && CurrentReqs.UniversityCredsAchieved && CurrentReqs.TrackCredsAchieved,
		CurrentReqs.MajorCredsAchieved &&
		CurrentReqs.UniversityCoursesAchieved && CurrentReqs.MajorCoursesAchieved && CurrentReqs.TrackCoursesAchieved))
		CurrentIssue = Critical;

}

Registrar::Registrar()
{ 
	ImportRules();
	GetCourseCatalog();                     
	pGUI = new GUI;       	                     //create interface object
	pSPlan = new StudyPlan;	                    //create a study plan.
	Push2Stack();

}

//returns a pointer to GUI
GUI* Registrar::getGUI() const
{
	return pGUI;
}

//returns the study plan
StudyPlan* Registrar::getStudyPlan() const
{
	return pSPlan;
}

Rules Registrar::ReturnRules() const
{
	return RegRules;
}

Action* Registrar::CreateRequiredAction() 
{	
	ActionData actData = pGUI->GetUserAction("Pick an action...");
	Action* RequiredAction = nullptr;

	switch (actData.actType)
	{
	case ADD_CRS:	//add_course action
		RequiredAction = new ActionAddCourse(this);
		
		break;
	case DRAW_AREA :
	{
		
		{
			pGUI->ClearStatusBar();
			int x, y;
			x = actData.x;
			y = actData.y;
			graphicsInfo gInfo{ x,y };
			int year;
			SEMESTER sem;
			StudyPlan* pp = getStudyPlan();
			pp->DetYearSem(gInfo, year, sem);
			Course* pc = pp->ReturnCoursePointer(gInfo);
			if (pc == nullptr)
			{
				break;
			}
			else
			{
				pc->setSelected(true);
				pc->DrawMe(pGUI);
				pGUI->DrawCourseDeps(pSPlan, pc);
				string pc2 = to_string(pc->getCredits());
				string title = pc->getTitle();
				string code = pc->getCode();
				string courseinfo ="Title: " +  title + ", " + "Code: " + code + ", " + "Credits: " + pc2;
				ActionData actData = pGUI->GetUserAction(courseinfo);
				pc->setSelected(0);
				break;
			}
		}
	}
	case SAVE:
		RequiredAction = new ActionSave(this);
		break;

	case LOAD:
		RequiredAction = new ActionImportSplan(this);
		break;

	case UNDO:
		UndoF();
		break;

	case REDO:
		RedoF();
		break;
	case NOTES_AREA :
		RequiredAction = new ActionAddNote(this);
		break;
	case MOVE :
		RequiredAction = new ActionMove(this);
		break;

	//TODO: Add case for each action
	case EXIT:
		running = false;
		break;
	case DEL_CRS:    //delete course action
		RequiredAction = new ActionDeleteCourse(this);

		break;
	case EDIT:  // Edit course in the study plan
		RequiredAction = new ActionEditCourseCode(this);
		
	}
	return RequiredAction;
}

//Executes the action, Releases its memory, and return true if done, false if cancelled
bool Registrar::ExecuteAction(Action* pAct)
{
	bool done = pAct->Execute();
	delete pAct;	//free memory of that action object (either action is exec or cancelled)
	return done;
}

void Registrar::Run()
{

	running = true;
	while (running)
	{
		//update interface here as CMU Lib doesn't refresh itself
		//when window is minimized then restored
		UpdateInterface();

		Action* pAct = CreateRequiredAction();

		if (pAct)	//if user doesn't cancel
		{
			if (ExecuteAction(pAct))   //if action is not cancelled
			{
				Push2Stack();
				//UpdateInterface();   //useless i think !
			}
		}	
	}
}

void Registrar::Push2Stack()
{
	StudyPlan temp;
	temp = *(pSPlan);
	temp.StaticCopyit(pSPlan);
	UndoS.push(temp);
}

void Registrar::UndoF()
{
	if (UndoS.size() >1 )
	{
		RedoS.push(UndoS.top());
		UndoS.pop();
		PlanTemp = UndoS.top();
		pSPlan =& PlanTemp;
	}
}

void Registrar::RedoF()
{
	if (RedoS.size()>0)
	{
		UndoS.push(RedoS.top());
		PlanTemp = UndoS.top();
		pSPlan = &PlanTemp;
		RedoS.pop();
	}
}



void Registrar::UpdateInterface()
{
	pGUI->UpdateInterface();	//update interface items      //test
	pSPlan->DrawMe(pGUI);		//make study plan draw itself
}

void Registrar::GetCourseCatalog()
{
	string file_name = "Catalog - 2020 12 19 .txt";
	vector<vector<string>> Words;
	string Line;
	ifstream Myfile("Rules\\" + file_name);
	if (Myfile.is_open())
	{
		while (getline(Myfile, Line))
		{
			stringstream ssLine(Line);
			string Word;
			vector<string> linewrds;
			while (getline(ssLine, Word, ','))
				linewrds.push_back(Word);
			Words.push_back(linewrds);
		}
		for (auto w :Words )
		{	
		CourseInfo c;
		c.Title = w[1];
		c.Code = w[0];
		if ( w[3] != "")
		{
             string sup= w[3].substr(7, w[3].size());
			 stringstream req(sup);
			 string halfcode;
			 string xcode="";

			 //cout << "crc : ";
			 while (getline(req, halfcode, ' '))
			 {

				 if (halfcode == "And")
				 {
					 xcode = xcode.substr(0, xcode.size() - 1);
					 //cout << xcode << "//";
					 c.CoReqList.push_back(xcode);
					 xcode = "";
				 }
				 else
				 {
					 xcode += halfcode+' ';
				 }
			 }
			 xcode = xcode.substr(0, xcode.size() - 1);
			 //cout << xcode << "//";
			 c.CoReqList.push_back(xcode); 
			 //cout << "End"<<endl;
		}
		if (w.size()==5)
		{
			//cout << "prc : ";
			string sup = w[4].substr(8, w[4].size());
			stringstream req(sup);
			string halfcode;
			string xcode = "";
			while (getline(req, halfcode, ' '))
			{
				
				if (halfcode == "And")
				{
					xcode = xcode.substr(0, xcode.size() - 1);
					//cout << xcode <<"//";
					c.PreReqList.push_back(xcode);
					xcode = "";
				}
				else
				{
					xcode += halfcode + ' ';
				}
			}
			xcode = xcode.substr(0, xcode.size() - 1);
			//cout << xcode << "//";
			c.PreReqList.push_back(xcode);
			//cout << "End" << endl;
		}
		/*for (int i = 2; i < 5; i++)    
			c.PreReqList.push_back(w[i]);
		for (int i = 5; i < 8; i++)
			c.CoReqList.push_back(w[i]);*/
		char s = w[2][0];
		c.Credits = s - '0';
		//c.type = w[9];
		RegRules.CourseCatalog.push_back(c);
		}


	}
}

void Registrar::ImportRules()
{
	string file_name = "ENV-Requirements.txt";
	vector<vector<string>> Words;
	string Line;
	ifstream Myfile("Rules\\" + file_name);
	if (Myfile.is_open())
	{
		while (getline(Myfile, Line))
		{
			stringstream ssLine(Line);
			string Word;
			vector<string> linewrds;
			while (getline(ssLine, Word, ','))
				linewrds.push_back(Word);
			Words.push_back(linewrds);
		}
		RegRules.TotalCredit = stoi(Words[0][0]);
		RegRules.ReqUnivCompulsoryCredits = stoi(Words[1][0]);
		RegRules.ReqUnivElectiveCredits = stoi(Words[1][1]);
		RegRules.ReqTrackCredits = stoi(Words[2][0]);
		RegRules.ReqMajorCompulsoryCredits = stoi(Words[3][0]);
		RegRules.ReqMajorElectiveCredits = stoi(Words[3][1]);
		for (auto var : Words[6])
		{
			Course_Code x = var;
			RegRules.UnivCompulsory.push_back(x);
		}
		for (auto var : Words[7])
		{
			Course_Code x = var;
			RegRules.UnivElective.push_back(x);
		}
		for (auto var : Words[8])
		{
			Course_Code x = var;
			RegRules.TrackCompulsory.push_back(x);
		}
		for (auto var : Words[9])
		{
			Course_Code x = var;
			RegRules.MajorCompulsory.push_back(x);
		}
		for (auto var : Words[10])
		{
			Course_Code x = var;
			RegRules.MajorElective.push_back(x);
		}
	}
}


Course * Registrar::CreateCourseP(Course_Code code)
{
	bool state = true;
	for (auto i : RegRules.CourseCatalog)
	{
		if (code == i.Code)
		{
			string title = i.Title;
			int credits = i.Credits;
			Course* pC = new Course(code, title, credits);
			list<Course_Code> coreq(i.CoReqList.begin(), i.CoReqList.end());            //converting the vector into a list
			list<Course_Code> preq(i.PreReqList.begin(), i.PreReqList.end());
			pC->setCoReq(coreq);
			pC->setPreReq(preq);
			return pC;
			state = false;
			break;
		}
	}
	if(state)
	return nullptr;
}
Registrar::~Registrar()
{
	delete pGUI;
	//delete pSPlan;
}

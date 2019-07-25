#include "Morphology.h"
#include <iostream>

Morphology::~Morphology(){
//	cout << endl << "DELETED MORPHOLOGY" << endl << endl; 
	if (control) {
		control->~Control();
	}
};

vector<shared_ptr<ER_Module>> Morphology::getCreatedModules() {
	vector<shared_ptr<ER_Module>> nullVec;
	return nullVec;
}



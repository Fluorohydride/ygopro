#include "CXMLRegistry.h"

CXMLRegistry::CXMLRegistry(io::IFileSystem *fsys) {
	fileSystem = fsys;
	fileSystem->grab();
	registry = NULL;
}
CXMLRegistry::~CXMLRegistry() {
	if(registry != NULL) registry->drop();
	fileSystem->drop();
}
bool CXMLRegistry::isTopLevelNode(const wchar_t *node) {
	if(registry->findChildByName(node)) return true;
	else return false;
}
CXMLNode *CXMLRegistry::resolveContext(const wchar_t* context) {
	core::stringw sContext;
	CXMLNode *currentNode;
	u16 start;
	s16 end;
	if(!context) return currentContext;
	sContext = context;
	if(sContext.size() == 0) return 0;
	// Make sure the context ends in a /
	if(sContext.lastChar() != L'/') 
		sContext += L'/';	
	start = 0;
	end = sContext.findFirst('/');
	// Theres no values in the top level nodes
	if(end == -1) return 0;
	currentNode = registry;
	while(end != -1) {		
		currentNode = currentNode->findChildByName(sContext.subString(start,end-start).c_str());
		if(currentNode == NULL) return NULL;
		start = end+1;
		end = sContext.findNext('/',start);		
	}
	return currentNode;
}

void CXMLRegistry::setContext(const wchar_t *context) { currentContext = resolveContext(context); }
const wchar_t *CXMLRegistry::convertBoolToText(bool boolval) {
	if(boolval) return L"true";
	else return L"false";
}
bool CXMLRegistry::convertTextToBool(const wchar_t *textval) {
	if(((core::stringw)textval).equals_ignore_case(L"true")) return true;
	return false;
}
bool CXMLRegistry::setValue(const wchar_t *index, bool boolval, const wchar_t *context) {
	return setValue(index,convertBoolToText(boolval),context);
}
bool CXMLRegistry::setValue(const wchar_t *index, u16 intval, const wchar_t *context) {
	return setValue(index,((core::stringw)intval).c_str(),context);
}
bool CXMLRegistry::setValue(const wchar_t *index, const wchar_t * txtval, const wchar_t *context) {
	CXMLNode *targetNode;
	targetNode = resolveContext(context);
	if(!targetNode) return false;
	targetNode = targetNode->findChildByName(index);
	if(!targetNode) return false;
	targetNode->setValue(txtval);
	return true;
}
bool CXMLRegistry::populateTreeView(irr::gui::IGUITreeView *control, const wchar_t *context) {
	CXMLNode *targetNode = NULL;
	if(context == 0) 
		targetNode = registry;
	else 
		targetNode = resolveContext(context);
	if(!targetNode) return false;
	targetNode->populateTreeView(control->getRoot());	
	return true;
	
}

bool CXMLRegistry::getValueAsBool(const wchar_t *index, const wchar_t *context) {
	return convertTextToBool(getValueAsCStr(index,context));
}
// little more robust
u16 CXMLRegistry::getValueAsInt(const wchar_t *index, const wchar_t *context) {
	core::stringw tmp = getValueAsCStr(index,context);
	if(tmp.equals_ignore_case("")) return 0;
	else return _wtoi(tmp.c_str());
}
core::array<const wchar_t*>* CXMLRegistry::listNonNodeChildren(const wchar_t *node,const wchar_t *context) {
	CXMLNode *targetNode;
	targetNode = resolveContext(context);
	if(!targetNode) return 0;
	return targetNode->listNonNodeChildren();
}
core::array<const wchar_t*>* CXMLRegistry::listNodeChildren(const wchar_t *node,const wchar_t *context) {
	CXMLNode *targetNode;
	targetNode = resolveContext(context);
	if(!targetNode) return 0;
	return targetNode->listNodeChildren();
}

//BROKEN
/*
const irr::c8 *CXMLRegistry::getValueAsCharCStr(const wchar_t *index, const wchar_t *context) {
	irr::core::stringc temp;
	temp = getValueAsCStr(index,context);
	return (const irr::c8 *)temp.c_str();

}
*/
const wchar_t *CXMLRegistry::getValueAsCStr(const wchar_t *index, const wchar_t *context) {
	CXMLNode *targetNode;
	targetNode = resolveContext(context);
	if(!targetNode) return 0;
	targetNode = targetNode->findChildByName(index);
	if(!targetNode) return 0;
	return targetNode->getValue();
}
irr::core::rect<u32> CXMLRegistry::getValueAsRect(const wchar_t *context) {
	CXMLNode *targetNode = resolveContext(context);
	irr::u32 tx,ty,bx,by;	
	if(!targetNode) return irr::core::rect<u32>(0,0,0,0);
	tx =  _wtoi(targetNode->findChildByName(L"tlx")->getValue());
	ty =  _wtoi(targetNode->findChildByName(L"tly")->getValue());
	bx =  _wtoi(targetNode->findChildByName(L"brx")->getValue());
	by =  _wtoi(targetNode->findChildByName(L"bry")->getValue());
	// Hrm what to return on err, cant return null, 0,0,0,0 might be a real loc
	// Its u32, maby some HUGE value? maxint?
	// Still takes out a value but its less likely
	if(!tx || !ty || !bx || !by) return irr::core::rect<u32>(0,0,0,0);

	else return irr::core::rect<u32>(tx,ty,bx,by);
}
// Made more robust
irr::video::SColor CXMLRegistry::getValueAsColor(const wchar_t *context) {
	CXMLNode *targetNode = resolveContext(context);
	if(!targetNode) return NULL;
	irr::u32 r,g,b,a;
	irr::core::stringw tmp;
	tmp = targetNode->findChildByName(L"r")->getValue();
	if(tmp.size()) r = _wtoi(tmp.c_str());
	tmp = targetNode->findChildByName(L"g")->getValue();
	if(tmp.size()) g = _wtoi(tmp.c_str());
	tmp = targetNode->findChildByName(L"b")->getValue();
	if(tmp.size()) b = _wtoi(tmp.c_str());
	tmp = targetNode->findChildByName(L"a")->getValue();
	if(tmp.size()) a = _wtoi(tmp.c_str());
	return irr::video::SColor(a,r,g,b);
}
bool CXMLRegistry::writeFile(const irr::c8 *fname,const c8 *path) {
	io::IXMLWriter* xml; 
	CXMLNode *currentnode = 0;
    core::stringw wstmp;
	core::stringc fileName;
	wstmp = fname;
	currentnode = registry->findChildByName(wstmp.c_str());
	if(!currentnode) return false;	
	fileName = path;
	fileName += fname;
	fileName += ".xml";
	xml = fileSystem->createXMLWriter(fileName.c_str());
	xml->writeXMLHeader();
	currentnode->writeOut(xml);
	// Get rid of double top level end tag
	//xml->writeClosingTag(currentnode->getName());
	//xml->writeLineBreak();
	//if(xml) xml->drop();
	delete xml;
	return true;
}
// check current folder, if there isnt anything in current, use default
const c8 *CXMLRegistry::resolveConfigPath(const c8 *fname) {
	core::string <c8> filename;
	bool useCurrent = true;
	filename = "config/current/";
	filename += fname;
	filename += ".xml";
	if(!fileSystem->existFile(filename.c_str())) {
		useCurrent = false;
	}
	return useCurrent?"config/current/":"config/defaults/";
}

bool CXMLRegistry::loadConfigFile(const c8 *fname) {	
	return loadFile(fname, resolveConfigPath(fname));
}
bool CXMLRegistry::writeConfigFile(const c8 *fname) {
	return writeFile(fname,"config/current/");
}


// This is tricky, we have to keep track of which nodes are 'open'
bool CXMLRegistry::loadFile(const c8 *fname, const c8 *path) {
	io::IXMLReader* xml;
	CXMLNode *currentNode = 0;
	CXMLNode *topNode = 0;
	CXMLNode *currentParent;
	core::string <c8> filename;	
	
	filename = path;
	filename += fname;
	// If it dosnt end in .xml add it
	
	if(!filename.subString(filename.size()-4,4).equals_ignore_case(".xml"))
		filename += ".xml";
	
	xml = fileSystem->createXMLReader(filename.c_str());
	
	if(!registry) {
		registry = new CXMLNode;
		registry->setName(L"TopLevelNode");
		registry->setType(CXMLNODETYPE_NODE);
	}
	while(xml && xml->read()) {
		CXMLNode *newNode = 0;			
		switch(xml->getNodeType()) {
			case io::EXN_ELEMENT :
				u16 i;
				newNode = new CXMLNode;
				newNode->setName(xml->getNodeName());
				newNode->setType(CXMLNODETYPE_NODE);
				if(!topNode) { 	
					newNode->setParent(NULL);					
					registry->addChild(newNode);					
					topNode = newNode;
					currentParent = newNode;
				}
				else {					
					newNode->setParent(currentParent);
					currentParent->addChild(newNode);
					if(!xml->isEmptyElement()) 
						currentParent = newNode;

				}
				currentNode = newNode;
				i = xml->getAttributeCount();
				while(i--) {
					newNode = new CXMLNode;
					newNode->setName(xml->getAttributeName(i));
					newNode->setType(CXMLNODETYPE_VALUE);
					newNode->setValue(xml->getAttributeValue(i));
					currentNode->addChild(newNode);
				}
				break;
			case io::EXN_ELEMENT_END :
				// NEVER go back further then the topNode
				// Even if the XML is screwed up
				if(currentParent->getParent() != NULL) 
					currentParent = currentParent->getParent();
				break;
/*	
			case io::EXN_COMMENT :				
				newNode = new CXMLNode;
				newNode->setType(CXMLNODETYPE_COMMENT);
				//newNode->setValue(xml->getNodeType
				currentNode->addChild(newNode);
				break;
*/
		}		
	}
	//if(xml) xml->drop();
	// Documentation says delete not drop
	
	delete xml;
	// To support loading multiple files or 'hives' have to 
	// see if this load ADDED any to decide succcessful or not
	if(topNode == 0) return false;
	return true;
	
}

	
#include "CXMLNode.h"

CXMLNODETYPE CXMLNode::getType() { return type; }

void CXMLNode::setType(CXMLNODETYPE newtype) { type = newtype; }

void CXMLNode::setName(const wchar_t* newname) { name = newname; }

const wchar_t* CXMLNode::getName() { return name.c_str(); }
CXMLNode *CXMLNode::getThis() { return this; }
void CXMLNode::setValue(const wchar_t* newvalue) {
	value = newvalue;
}
CXMLNode *CXMLNode::getParent() {
	return parent;
}
void CXMLNode::setParent(CXMLNode *newparent) {
	parent = newparent;
}
/*

bool CXMLNode::getValueAsBool() {
	if(!value.size()) return false;
	if(value.equals_ignore_case((core::stringw)"true")) return true;
	return false;
}
s32 CXMLNode::getValueAsInt() {
	if(!value.size()) return 0;
	return _wtoi(value.c_str());
}

const wchar_t* CXMLNode::getValueAsCStr() {
	if(!value.size()) return false;
	return value.c_str();
}
*/
/*
u16 CXMLNode::getChildCount() {
	return children.getSize();
}

irr::core::list<CXMLNode*>::Iterator CXMLNode::getFirstChild() {
	//if(children.empty()) return 0;
	return children.begin();
}
irr::core::list<CXMLNode*>::Iterator CXMLNode::getLastChild() {
	//if(children.empty()) return 0;
	return  children.getLast();
}
*/
const wchar_t* CXMLNode::getValue() {
	return value.c_str();
}
void CXMLNode::addChild(CXMLNode *newchild) {
	children.push_back(newchild);
}
void CXMLNode::drop() {
	if(children.getSize() != 0) {
		core::list<CXMLNode*>::Iterator	it;
		for( it = children.begin(); it != children.end(); it++ ) {
			( *it )->drop();
		}
		children.clear();
	}
}
CXMLNode *CXMLNode::findChildByName(const wchar_t *name) {
	core::list<CXMLNode*>::Iterator	it;
	core::stringw tmp;
	CXMLNode *node = NULL;
	tmp = name;
	for( it = children.begin(); it != children.end(); it++ ) {
		if(tmp.equals_ignore_case((*it)->getName())) {
			node = (*it);
			break;
		}
	}
	return node;
}

core::array<const wchar_t*> *CXMLNode::listChildrenByType(CXMLNODETYPE nodetype) {
	core::array<const wchar_t*> *ret = new core::array<const wchar_t*>;
	//core::stringw construct = L"";
	core::list<CXMLNode*>::Iterator	it;
	
	for( it = children.begin(); it != children.end(); it++ ) {
		if((*it)->getType() == nodetype) {
				ret->push_front((*it)->getName());
		}
	}
	
	return ret;
}

core::array<const wchar_t*> *CXMLNode::listNonNodeChildren() {
	return listChildrenByType(CXMLNODETYPE_VALUE);
}
core::array<const wchar_t*> *CXMLNode::listNodeChildren() {
	return listChildrenByType(CXMLNODETYPE_NODE);
}

void CXMLNode::populateTreeView(irr::gui::IGUITreeViewNode *node) {
	irr::gui::IGUITreeViewNode *myNode;
	irr::core::stringw myText;
	core::list<CXMLNode*>::Iterator	it;
	// If I'm a value, I have nothing to add, my parent will do that
	if(type == CXMLNODETYPE_VALUE) return;
	// So I'm a node, if im not the toplevel node, write me 
	if(parent != NULL) {
		//myText = L"(Key) ";
		myText += getName();
		myNode = node->addChildBack(myText.c_str());
	}
	else
		myNode = node;
	// If I dont have children, I'm done
	if(children.empty()) return;
	// setup my children
	for( it = children.begin(); it != children.end(); it++ ) {
		switch((*it)->getType()) {
			case CXMLNODETYPE_NODE :
				myText = L"";
				myText += (core::stringw)(*it)->getName();
				(*it)->populateTreeView(myNode);
				break;
			case CXMLNODETYPE_VALUE : 
				myText = L"";
				myText += (core::stringw)(*it)->getName();				
				myText += L":";
				myText += (core::stringw)(*it)->getValue();
				
				myNode->addChildBack(myText.c_str());
		}
	}

}
void CXMLNode::writeOut(io::IXMLWriter* xml) {	
	core::array<core::stringw> names;
	core::array<core::stringw> values;
	core::list<CXMLNode*>::Iterator	it;
	bool nodeChildren = false;
	// If I'm a value, I have nothing to write, I'll be written by my parent
	if(type == CXMLNODETYPE_VALUE) return;
	// If I'm a node with no values, which also means no children write as empty and return
	if(children.empty()) {
		xml->writeElement(name.c_str(),true);
		xml->writeLineBreak();
		return;
	}	
	// Need to figure out myself first
	for( it = children.begin(); it != children.end(); it++ ) {
		// cache all my values for writing, and set a flag if I have node children, ie non empty
		switch((*it)->getType()) {
			case CXMLNODETYPE_VALUE:
				names.push_front((core::stringw)(*it)->getName());
				values.push_front((core::stringw)(*it)->getValue());
				break;
			case CXMLNODETYPE_NODE:
				nodeChildren = true;
		}		
	}
	// At this point I just need to write myself
	xml->writeElement(name.c_str(),nodeChildren?false:true,names,values);	
	xml->writeLineBreak();
	// Now I need to see if I have to tell my node children to write	
	if(!nodeChildren) return;
	
	for( it = children.begin(); it != children.end(); it++ ) {	
		// If I have a node child, tell it to write, recursion made easy!			
		if((*it)->getType() == CXMLNODETYPE_NODE) {
				(*it)->writeOut(xml);
		}		
	}	
	xml->writeClosingTag(name.c_str());
	xml->writeLineBreak();
}
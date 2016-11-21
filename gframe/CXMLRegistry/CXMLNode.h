// Madoc 05/09

#ifndef __C_XMLNODE_H_INCLUDED__
#define __C_XMLNODE_H_INCLUDED__

#ifdef _WIN32
#include <irrlicht.h>
#else
//	Setup for compile with stripped down IRRlicht on FreeBSD (stripped by Madoc)
#include <irrString.h>
#include <irrArray.h>
#include <irrTypes.h>
#endif
using namespace irr;

enum CXMLNODETYPE {
	CXMLNODETYPE_VALUE,
	CXMLNODETYPE_NODE,
	//CXMLNODETYPE_COMMENT
};

class CXMLNode {
public :
	/* Decided to move this up to the registry object, reduce mem use.
	s32 getValueAsInt();
	const wchar_t* getValueAsCStr();
	bool getValueAsBool();
	*/
	CXMLNODETYPE getType();
	const wchar_t* getName();
	const wchar_t* getValue();
	void setType(CXMLNODETYPE newtype);
	void setName(const wchar_t* newname);
	void setValue(const wchar_t* newvalue);
	void addChild(CXMLNode *newchild);
	void writeOut(irr::io::IXMLWriter* xml);
	void populateTreeView(irr::gui::IGUITreeViewNode *node);
	//u16 getChildCount();
	void drop();
	/*
	irr::core::list<CXMLNode*>::Iterator getFirstChild();
	// get last is not get end point, this causes problems with iterators
	// Screw it, I'll keep all pointers internal
	irr::core::list<CXMLNode*>::Iterator getLastChild(); 	
	*/
	// Careful with these, mem leak waiting to happen
	// Maby I should steal IReferenceCounted from irrlicht, would at 
	// least give me autodetection of mem leaks
	core::array<const wchar_t *> *listNonNodeChildren();
	core::array<const wchar_t *> *listNodeChildren();
	CXMLNode *getThis();
	CXMLNode *findChildByName(const wchar_t *name);	
	CXMLNode *getParent();
	void setParent(CXMLNode *newparent);
private :
	CXMLNode *parent;
	irr::core::stringw value;
	irr::core::stringw name;
	CXMLNODETYPE type;
	irr::core::list<CXMLNode*> children;
	irr::core::array<const wchar_t *> *listChildrenByType(CXMLNODETYPE nodetype);
};
#endif

// Madoc 05/09

#ifndef __C_XMLNODE_H_INCLUDED__
#define __C_XMLNODE_H_INCLUDED__

#include <irrArray.h>
#include <irrString.h>
#include <irrList.h>

namespace irr {
class IReferenceCounted;
namespace io {
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
template<class char_type, class super_class>
class IIrrXMLWriter;
typedef IIrrXMLWriter<wchar_t, IReferenceCounted> IXMLWriter;
#else
class IXMLWriter;
#endif
}
namespace gui {
class IGUITreeViewNode;
}
}

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
	irr::core::array<const wchar_t *> *listNonNodeChildren();
	irr::core::array<const wchar_t *> *listNodeChildren();
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

// Madoc 05/09

#ifndef __C_XMLNODE_H_INCLUDED__
#define __C_XMLNODE_H_INCLUDED__

#include <irrlicht.h>

enum CXMLNODETYPE {
	CXMLNODETYPE_VALUE,
	CXMLNODETYPE_NODE
};

class CXMLNode {
public :
	CXMLNODETYPE getType();
	const wchar_t* getName();
	const wchar_t* getValue();
	void setType(CXMLNODETYPE newtype);
	void setName(const wchar_t* newname);
	void setValue(const wchar_t* newvalue);
	void addChild(CXMLNode *newchild);
	void writeOut(irr::io::IXMLWriter* xml);
	void populateTreeView(irr::gui::IGUITreeViewNode *node);
	void drop();

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

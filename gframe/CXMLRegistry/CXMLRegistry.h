// Madoc 05/09

#ifndef __C_XMLREGISTRY_H_INCLUDED__
#define __C_XMLREGISTRY_H_INCLUDED__
#ifdef _WIN32
#include <irrlicht.h>
#else
//	Setup for compile with stripped down IRRlicht on FreeBSD (stripped by Madoc)
#include <irrString.h>
#include <irrArray.h>
#include <irrTypes.h>
#endif
using namespace irr;
#include "CXMLNode.h"
//File->
//	Node1->
//		Name1/Value
//		Node2->
//			Name2/Value	
// Oddity: Very little error checking, crashes if you look at it funny
// Oddity: When using setValue ala setValue("someindex",100); compiler cant tell if 100 is a bool or an int, silly compiler,
//		   casting to u16 works ala setValue("someindex",(u16)100); I see why, but its still silly, and casts are slow
class CXMLRegistry {
public: 
	CXMLRegistry(io::IFileSystem *fsys);
	~CXMLRegistry();
	bool loadFile(const c8 *fname, const c8 *path);
	bool loadConfigFile(const c8 *fname);
	bool writeFile(const c8 *fname, const c8 *path);
	bool writeConfigFile(const c8 *fname);
	const wchar_t *getValueAsCStr(const wchar_t *index,const wchar_t *context = 0);
	// Dosnt work, just declare a stringc and assign the wchar_t to it	
	//const irr::c8 *getValueAsCharCStr(const wchar_t *index,const wchar_t *context = 0);
	bool getValueAsBool(const wchar_t *index, const wchar_t *context = 0);
	u16 getValueAsInt(const wchar_t *index, const wchar_t *context = 0);
// This one only takes a context because its going to read 4 childrens values
  	irr::core::rect<u32> getValueAsRect(const wchar_t *context = 0);
// This one only takes a context because its going to read 4 childrens values
	video::SColor getValueAsColor(const wchar_t *context = 0);
	irr::core::array<const wchar_t*> *listNonNodeChildren(const wchar_t *node,const wchar_t *context = 0);
	irr::core::array<const wchar_t*> *listNodeChildren(const wchar_t *node,const wchar_t *context = 0);
	bool setValue(const wchar_t *index, bool boolval, const wchar_t *context = 0);
	bool setValue(const wchar_t *index, u16 intval,   const wchar_t *context = 0);
	bool setValue(const wchar_t *index, const wchar_t *txtval,const wchar_t *context = 0);
	// Speed improvement, why search if we want multiple values from the same context
	void setContext(const wchar_t *context);
	bool isTopLevelNode(const wchar_t *node);
	bool populateTreeView(irr::gui::IGUITreeView *control,const wchar_t *context = 0);
	// TODO: These are trivial to do, just dont need them yet
	//	renameNode(context(?),TYPE)
	//  deleteNode(context(?) (CAREFUL WITH CHILDREN)
	//  addNode(context, value, type)
	//  CXMLNODETYPE_COMMENT
	//  Loading/saving of CDATA, easiest would be just save it in the value of 
	//  the CXMLNODETYPE_NODE their accosicated with.
	// 	make registry inherit node instead of containing an instance of it.
	// context(?) might be tricky with CXMLNODETYPE_VALUE
	// 
	// 
private:
	io::IFileSystem* fileSystem;
	CXMLNode* registry;
	CXMLNode* currentContext;
	const wchar_t *convertBoolToText(bool boolval);
	bool convertTextToBool(const wchar_t* textval);
	CXMLNode *resolveContext(const wchar_t* context);
	const irr::c8 *resolveConfigPath(const irr::c8 *fname);

};
#endif

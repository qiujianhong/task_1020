// XMLite.h: interface for the XMLite class.
//
// XMLite : XML Lite Parser Library
// by bro ( Cho,Kyung Min: bro@shinbiro.com ) 2002-10-30
// History.
// 2002-10-29 : First Coded. Parsing XMLElelement and Attributes.
//              get xml parsed string ( looks good )
// 2002-10-30 : Get Node Functions, error handling ( not completed )
// 2002-12-06 : Helper Funtion string to long
// 2002-12-12 : Entity Helper Support
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)
#define AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_

#include "DynString.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LPCTSTR const char*
#define LPTSTR char*
#define TCHAR char
#define _T(x)	x

#include <vector>

struct _tagXMLAttr;
typedef _tagXMLAttr XAttr, *LPXAttr;
typedef std::vector<LPXAttr> XAttrs;
struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;
typedef std::vector<LPXNode> XNodes, *LPXNodes;

// Entity Encode/Decode Support
typedef struct _tagXmlEntity
{
	TCHAR entity;					// entity ( & " ' < > )
	TCHAR ref[10];					// entity reference ( &amp; &quot; etc )
	int ref_len;					// entity reference length
}XENTITY,*LPXENTITY;

typedef struct _tagXMLEntitys : public std::vector<XENTITY>
{
	LPXENTITY GetEntity( int entity );
	LPXENTITY GetEntity( LPTSTR entity );
	int GetEntityCount( LPCTSTR str );
	int Ref2Entity( LPCTSTR estr, LPTSTR str, int strlen );
	int Entity2Ref( LPCTSTR str, LPTSTR estr, int estrlen );
	CDynString Ref2Entity( LPCTSTR estr );
	CDynString Entity2Ref( LPCTSTR str );	

	_tagXMLEntitys(){};
	_tagXMLEntitys( LPXENTITY entities, int count );
}XENTITYS,*LPXENTITYS;
extern XENTITYS entityDefault;	

typedef enum 
{
	PIE_PARSE_WELFORMED	= 0,
	PIE_ALONE_NOT_CLOSED,
	PIE_NOT_CLOSED,
	PIE_NOT_NESTED,
	PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
	BOOLEAN		trim_value;			// [set] do trim when parse?
	BOOLEAN		entity_value;		// [set] do convert from reference to entity? ( &lt; -> < )
	LPXENTITYS	entitys;			// [set] entity table for entity decode

	LPTSTR		xml;				// [get] xml source
	BOOLEAN		erorr_occur;		// [get] is occurance of error?
	LPTSTR		error_pointer;		// [get] error position of xml source
	PCODE		error_code;			// [get] error code
	CDynString	error_string;		// [get] error string

	_tagParseInfo() { trim_value = false; entity_value = true; entitys = &entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; }
}PARSEINFO,*LPPARSEINFO;
extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
	BOOLEAN newline;			// newline when new tag
	BOOLEAN reference_value;	// do convert from entity to reference ( < -> &lt; )
	LPXENTITYS	entitys;	// entity table for entity encode

	int tab_base;			// internal usage
	_tagDispOption() { newline = true; reference_value = true; entitys = &entityDefault; tab_base = 0; }
}DISP_OPT, *LPDISP_OPT;
extern DISP_OPT optDefault;

// XAttr : Attribute Implementation
typedef struct _tagXMLAttr
{
	CDynString name;
	CDynString	value;
	
	_tagXMLNode*	parent;

	CDynString GetXML( LPDISP_OPT opt = &optDefault );
}XAttr, *LPXAttr;

// XMLNode structure
typedef struct _tagXMLNode
{
	// name and value
	CDynString name;
	CDynString	value;

	// internal variables
	LPXNode	parent;		// parent node
	XNodes	childs;		// child node
	XAttrs	attrs;		// attributes

	// Load/Save XML
	LPTSTR	Load( LPCTSTR pszXml, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadAttributes( LPCTSTR pszAttrs, LPPARSEINFO pi = &piDefault );
	CDynString GetXML( LPDISP_OPT opt = &optDefault );

	// in own attribute list
	LPXAttr	GetAttr( LPCTSTR attrname ); 
	LPCTSTR	GetAttrValue( LPCTSTR attrname ); 
	XAttrs	GetAttrs( LPCTSTR name ); 

	// in one level child nodes
	LPXNode	GetChild( LPCTSTR name ); 
	LPCTSTR	GetChildValue( LPCTSTR name ); 
	XNodes	GetChilds( LPCTSTR name ); 
	XNodes	GetChilds(); 

	LPXAttr GetChildAttr( LPCTSTR name, LPCTSTR attrname );
	LPCTSTR GetChildAttrValue( LPCTSTR name, LPCTSTR attrname );
	
	// modify DOM 
	int		GetChildCount();
	LPXNode GetChild( int i );
	XNodes::iterator GetChildIterator( LPXNode node );
	LPXNode CreateNode( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXNode	AppendChild( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXNode	AppendChild( LPXNode node );
	BOOL	RemoveChild( LPXNode node );
	BOOL	RemoveChild( LPCTSTR name );
	LPXNode DetachChild( LPXNode node );


	LPXAttr GetAttr( int i );
	XAttrs::iterator GetAttrIterator( LPXAttr node );
	LPXAttr CreateAttr( LPCTSTR anem = NULL, LPCTSTR value = NULL );
	LPXAttr AppendAttr( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXAttr	AppendAttr( LPXAttr attr );
	BOOL	RemoveAttr( LPXAttr attr );
	LPXAttr DetachAttr( LPXAttr attr );

	// operator overloads
	LPXNode operator [] ( int i ) { return GetChild(i); }

	_tagXMLNode() { parent = NULL; }
	~_tagXMLNode();

}XNode, *LPXNode;


#endif // !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)

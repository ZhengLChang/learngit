#ifndef __CFG_XML_H__
#define __CFG_XML_H__

#include <mxml.h>
#include <mxml_file.h>
#include <mxml_defs.h>


MXML_ATTRIBUTE * xml_find_nodeattr(MXML_NODE *node,char *name);
void xml_get_attrdata_str(MXML_ATTRIBUTE *attr,char *data, int size);
int xml_get_attrdata_int(MXML_ATTRIBUTE *attr);
int xml_get_attrdata_hex(MXML_ATTRIBUTE *attr);
unsigned int xml_get_attrdata_uint(MXML_ATTRIBUTE *attr);
float xml_get_attrdata_float(MXML_ATTRIBUTE *attr);
int xml_get_attrdata_bool(MXML_ATTRIBUTE *attr);

void xml_set_attrdata_str(MXML_ATTRIBUTE *attr,char *str);
void xml_set_attrdata_bool(MXML_ATTRIBUTE *attr,int b);
void xml_set_attrdata_int(MXML_ATTRIBUTE *attr,int d);
void xml_set_attrdata_hex(MXML_ATTRIBUTE *attr, int d);
void xml_set_attrdata_uint(MXML_ATTRIBUTE *attr,unsigned int d);
void xml_set_attrdata_float(MXML_ATTRIBUTE *attr, float f);

MXML_NODE *xml_findchild(MXML_NODE *pnode, const char *name);
MXML_NODE * xml_findnext(MXML_NODE *pNode);

void xml_get_nodedata_str(MXML_NODE *node, char *data, int size);
int xml_get_nodedata_int(MXML_NODE *node);
int xml_get_nodedata_hex(MXML_NODE *node);
unsigned int xml_get_nodedata_uint(MXML_NODE *node);
float xml_get_nodedata_float(MXML_NODE *node);
int xml_get_nodedata_bool(MXML_NODE *node);

void xml_set_nodedata_str(MXML_NODE *node, char *str);
void xml_set_nodedata_bool(MXML_NODE *node, int b);
void xml_set_nodedata_int(MXML_NODE *node, int d);
void xml_set_nodedata_hex(MXML_NODE *node, int d);
void xml_set_nodedata_uint(MXML_NODE *node,unsigned int d);
void xml_set_nodedata_float(MXML_NODE *node, float f);
//void xml_set_nodeattr_allow(MXML_NODE *node, char *name, int b);
//void xml_set_nodeattr_bool(MXML_NODE *node, char *name, pj_bool_t b);

#endif // __CFG_XML_H__

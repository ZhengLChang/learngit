#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "phone_common.h"
#include "cfg_xml.h"


#define THIS_FILE "cfg_xml.c"

// basic operation
MXML_NODE *xml_findchild(MXML_NODE *pnode, const char *name)
{
	MXML_NODE *tmpnode = NULL;

	assert(pnode && name);

	tmpnode = pnode->child;
	while (tmpnode)
	{
		if (tmpnode->name)
		{
			if (phone_strcmp(tmpnode->name,name) == 0)
				return tmpnode;
		}
		tmpnode = tmpnode->next;
	}
	return NULL;
}

MXML_NODE * xml_findnext(MXML_NODE *pNode)
{
	return pNode->next;
}


MXML_ATTRIBUTE * xml_find_nodeattr(MXML_NODE *node,char *name)
{
	MXML_ATTRIBUTE *tmpattr;

	assert(node && name);

	tmpattr = node->attributes;
	while (tmpattr)
	{
		if (tmpattr->name)
		{
			if (phone_strcmp(tmpattr->name, name) == 0)
			{
				return tmpattr;
			}
			tmpattr = tmpattr->next;
		}
	}

	return NULL;
}

void xml_get_attrdata_str(MXML_ATTRIBUTE *attr,char *data, int size)
{
	assert(attr && data);

	if(attr->value)
		phone_strcpy(data,attr->value, size);
}

int xml_get_attrdata_int(MXML_ATTRIBUTE *attr)
{
	assert(attr);

	if (attr->value == NULL)
	{
		return 0;
	} 
	else
	{
		return atoi(attr->value);
	}

}

int xml_get_attrdata_hex(MXML_ATTRIBUTE *attr)
{
	assert(attr);
	if (attr->value == NULL)
	{
		return 0;
	} 
	else
	{
		int tmp;

		sscanf(attr->value, "%x", &tmp);
		return tmp;
	}
}


unsigned int xml_get_attrdata_uint(MXML_ATTRIBUTE *attr)
{
	char *endptr = NULL;

	assert(attr);

	if (attr->value == NULL)
	{
		return 0;
	} 
	else
	{
		//return atoi(attr->value);
		return strtoul(attr->value, &endptr, 10);
	}

}

float xml_get_attrdata_float(MXML_ATTRIBUTE *attr)
{
	assert(attr);

	if (attr->value == NULL)
	{
		return 0.0;
	} 
	else
	{
		return atof(attr->value);
	}

}

int xml_get_attrdata_bool(MXML_ATTRIBUTE *attr)
{
	assert(attr);

	if ((attr->value != NULL) && (!phone_strcmp(attr->value,"Enable")))
		return 1;
	else
		return 0;
}


void xml_set_attrdata_str(MXML_ATTRIBUTE *attr,char *str)
{
	assert(attr && str);

	if (attr->value)
	{
		free(attr->value);
		attr->value = NULL;
	}
	attr->value = phone_malloc(strlen(str) + 1);
	phone_strcpy(attr->value,str, strlen(str)+1);
}

void xml_set_attrdata_bool(MXML_ATTRIBUTE *attr,int b)
{
	assert(attr);

	if (b)
		xml_set_attrdata_str(attr,"Enable");
	else
		xml_set_attrdata_str(attr,"Disable");
}


void xml_set_attrdata_int(MXML_ATTRIBUTE *attr,int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%d", d);
	xml_set_attrdata_str(attr,intdata);
}

void xml_set_attrdata_hex(MXML_ATTRIBUTE *attr, int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%x", d);
	xml_set_attrdata_str(attr,intdata);
}

void xml_set_attrdata_uint(MXML_ATTRIBUTE *attr,unsigned int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%d", d);
	xml_set_attrdata_str(attr,intdata);
}

void xml_set_attrdata_float(MXML_ATTRIBUTE *attr, float f)
{//our date use precision 3
	char intdata[32] = {0};

	sprintf(intdata,"%.3f", f);
	xml_set_attrdata_str(attr,intdata);
}

/////////////////////////////////////////////////////////

void xml_get_nodedata_str(MXML_NODE *node,char *data, int size)
{
	assert(node && data);

	if(node->data)
		phone_strcpy(data,node->data, size);
}

int xml_get_nodedata_int(MXML_NODE *node)
{
	assert(node);

	if (node->data == NULL)
	{
		return 0;
	} 
	else
	{
		return atoi(node->data);
	}

}

int xml_get_nodedata_hex(MXML_NODE *node)
{
	assert(node);
	if (node->data == NULL)
	{
		return 0;
	} 
	else
	{
		int tmp;

		sscanf(node->data, "%x", &tmp);
		return tmp;
	}
}


unsigned int xml_get_nodedata_uint(MXML_NODE *node)
{
	char *endptr = NULL;

	assert(node);

	if (node->data == NULL)
	{
		return 0;
	} 
	else
	{
		//return atoi(node->data);
		return strtoul(node->data, &endptr, 10);
	}

}

float xml_get_nodedata_float(MXML_NODE *node)
{
	assert(node);

	if (node->data == NULL)
	{
		return 0.0;
	} 
	else
	{
		return atof(node->data);
	}

}

int xml_get_nodedata_bool(MXML_NODE *node)
{
	assert(node);

	if ((node->data != NULL) && (!phone_strcmp(node->data,"Enable")))
		return 1;
	else
		return 0;
}


void xml_set_nodedata_str(MXML_NODE *node,char *str)
{
	assert(node && str);

	if (node->data)
	{
		free(node->data);
		node->data = NULL;
	}
	node->data = phone_malloc(strlen(str) + 1);
	phone_strcpy(node->data,str, strlen(str)+1);
}

void xml_set_nodedata_bool(MXML_NODE *node,int b)
{
	assert(node);

	if (b)
		xml_set_nodedata_str(node,"Enable");
	else
		xml_set_nodedata_str(node,"Disable");
}


void xml_set_nodedata_int(MXML_NODE *node,int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%d", d);
	xml_set_nodedata_str(node,intdata);
}

void xml_set_nodedata_hex(MXML_NODE *node, int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%x", d);
	xml_set_nodedata_str(node,intdata);
}

void xml_set_nodedata_uint(MXML_NODE *node,unsigned int d)
{
	char intdata[32] = {'\0'};

	sprintf(intdata,"%d", d);
	xml_set_nodedata_str(node,intdata);
}

void xml_set_nodedata_float(MXML_NODE *node, float f)
{//our date use precision 3
	char intdata[32] = {0};

	sprintf(intdata,"%.3f", f);
	xml_set_nodedata_str(node,intdata);
}



#if 0
gcc timeZoneDaylightMap.c libmxml-0.9.1/src/*.c -Ilibmxml-0.9.1/include
#endif
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "mxml.h"
MXML_NODE * xml_findnext(MXML_NODE *pNode)
{
	return pNode->next;
}


MXML_ATTRIBUTE * xml_find_nodeattr(MXML_NODE *node,char *name)
{
	MXML_ATTRIBUTE *tmpattr;


	tmpattr = node->attributes;
	while (tmpattr)
	{
		if (tmpattr->name)
		{
			if (strcmp(tmpattr->name, name) == 0)
			{
				return tmpattr;
			}
			tmpattr = tmpattr->next;
		}
	}

	return NULL;
}
MXML_NODE *xml_findchild(MXML_NODE *pnode, const char *name)
{
	MXML_NODE *tmpnode = NULL;


	tmpnode = pnode->child;
	while (tmpnode)
	{
		if (tmpnode->name)
		{
			if (strcmp(tmpnode->name,name) == 0)
				return tmpnode;
		}
		tmpnode = tmpnode->next;
	}
	return NULL;
}
int main()
{
    char cfg_filepath[512] = {0};
    MXML_DOCUMENT *doc;
    MXML_NODE *root_node, *l1_node;
    MXML_ATTRIBUTE *attr;
    int cur_zone_id;

    sprintf(cfg_filepath, "./AutoDST.xml");

    doc = mxml_read(cfg_filepath, 0);
    if(!(doc && doc->status == MXML_STATUS_OK))
    	goto ERR;

	if((root_node = xml_findchild(doc->root,"DSTData")) == NULL)
		goto ERR;

	if((l1_node = xml_findchild(root_node,"DST")) != NULL)
	{
		do 
		{
			if((attr = xml_find_nodeattr(l1_node,"szZoneID")) != NULL)
			{
					if((attr = xml_find_nodeattr(l1_node,"szStart")) != NULL)
					{
						printf("1, ");
					}
					else
					{
						printf("0, ");
					}
			}
		} while ((l1_node = xml_findnext(l1_node))!=NULL);
		printf("\n");
	}
ERR:
	mxml_node_destroy(doc->root);
	mxml_document_destroy(doc);
	doc = NULL;
	return 0;
}

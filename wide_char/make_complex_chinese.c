#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define MXML_FILE_BLOCK_SIZE  2048
#define MXML_EOF                  -256
#define MXML_LINE_TERMINATOR      '\n'
#define MXML_SOFT_LINE_TERMINATOR '\r'

#define MXML_ALLOCATOR malloc
#define MXML_REALLOCATOR realloc
#define MXML_DELETOR  free
#define mxml_refil_ungetc( ref, ch )  ref->sparechar = ch
#define MXML_ALLOC_BLOCK      128
#define MXML_MAX_NAME_LEN     128
#define MXML_MAX_ATTRIB_LEN   256
/* Styles */
#define MXML_STYLE_INDENT        0x0001
#define MXML_STYLE_TAB           0x0002
#define MXML_STYLE_THREESPACES   0x0004
#define MXML_STYLE_NOESCAPE      0x0008

// just a shortcut
#define MALFORMED_ERROR( elem, val ) \
   elem->status = MXML_STATUS_MALFORMED;\
   elem->error = val;


typedef enum
{
   MXML_TYPE_TAG=0,
   MXML_TYPE_COMMENT,
   MXML_TYPE_PI,
   MXML_TYPE_DIRECTIVE,
   MXML_TYPE_DATA,
   MXML_TYPE_DOCUMENT   // used for document level root node
} MXML_NODE_TYPE;


typedef enum
{
   MXML_STATUS_ERROR=0,
   MXML_STATUS_OK=1,
   MXML_STATUS_MORE,
   MXML_STATUS_DONE,
   MXML_STATUS_UNDEFINED,
   MXML_STATUS_MALFORMED
} MXML_STATUS;

/* Error codes */

typedef enum
{
   MXML_ERROR_NONE = 0,
   MXML_ERROR_IO = 1,
   MXML_ERROR_NOMEM,

   MXML_ERROR_OUTCHAR,
   MXML_ERROR_INVNODE,
   MXML_ERROR_INVATT,
   MXML_ERROR_MALFATT,
   MXML_ERROR_INVCHAR,
   MXML_ERROR_NAMETOOLONG,
   MXML_ERROR_ATTRIBTOOLONG,
   MXML_ERROR_VALATTOOLONG,
   MXML_ERROR_UNCLOSED,
   MXML_ERROR_UNCLOSEDENTITY,
   MXML_ERROR_WRONGENTITY
} MXML_ERROR_CODE;
/* Attribute */
typedef struct tag_mxml_attribute
{
    char *name;
    char *value;
    struct tag_mxml_attribute *next;
} MXML_ATTRIBUTE;

typedef struct tag_mxml_node
{
   char *name;
   char *data;
   int data_length;
   MXML_NODE_TYPE type;
   struct tag_mxml_attribute *attributes;
   struct tag_mxml_node *next;
   struct tag_mxml_node *prev;
   struct tag_mxml_node *child;
   struct tag_mxml_node *last_child;
   struct tag_mxml_node *parent;
} MXML_NODE;
/* Index for faster node retrival */

typedef struct tag_mxml_index
{
	int length;
	int allocated;
	MXML_NODE *data;
} MXML_INDEX;
typedef struct tag_mxml_document
{
   char *name;
   MXML_NODE *root;
   MXML_STATUS status;
   MXML_INDEX *index;
   MXML_ERROR_CODE error;
   //this is mainly for stats & progress
   int node_count;
   int iLine;
} MXML_DOCUMENT;

struct tag_mxml_refil;

typedef void (*MXML_REFIL_FUNC)(struct tag_mxml_refil *ref);

typedef struct tag_mxml_refil
{
	   // status variables
	   MXML_STATUS status;
	   MXML_ERROR_CODE error;
     	   // buffer for reading data
     	   unsigned char *buffer;
	   int bufsize;  // size of the whole buffer
	   int buflen;   // valid characters in the current buffer
	   int bufpos;   // current position
	   // lenght of the stream for implementing progress indicators
	   long int streampos;
	   long int streamlen;
	
	   // callback funcs
	   MXML_REFIL_FUNC refil_func;

	   // ungetc implementation
	   int sparechar;

	   // data available for callback functions
	   void *data;
} MXML_REFIL;


MXML_STATUS mxml_read_file( FILE *fp, MXML_DOCUMENT *doc, int iStyle );
MXML_DOCUMENT *mxml_document_new();
MXML_STATUS mxml_document_setup( MXML_DOCUMENT *doc);
MXML_NODE *mxml_node_new();
MXML_STATUS mxml_node_read(MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc, int iStyle);
void mxml_node_read_tag( MXML_REFIL *ref, MXML_NODE *pNode,
      MXML_DOCUMENT *doc, int style );
void mxml_node_read_comment( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc );
void mxml_node_read_closing( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc );
void mxml_node_destroy( MXML_NODE *node );
void mxml_node_add_below( MXML_NODE *tg, MXML_NODE *node );

MXML_DOCUMENT *mxml_read( char *filename, int style )
{
   FILE *fp;
   MXML_DOCUMENT *doc = mxml_document_new();

   if ( doc->status != MXML_STATUS_OK )
      return doc;

   fp = fopen( filename, "r" );
   if ( fp == NULL ) {
      doc->status = MXML_STATUS_ERROR;
      doc->error = MXML_ERROR_IO;
      return doc;
   }

   mxml_read_file( fp, doc, style );

   /* A common error is not to check file close return */
   if ( fclose( fp ) != 0 )
      doc->status = MXML_STATUS_UNDEFINED;

   return doc;
}

MXML_STATUS mxml_refil_setup( MXML_REFIL *ref, MXML_REFIL_FUNC func,
   char *buf, int buflen, int bufsize )
{

   if ( buf == NULL && func == NULL )
      return MXML_STATUS_ERROR;

   ref->refil_func = func;
   ref->buffer = buf;

   ref->status = MXML_STATUS_OK;
   ref->error = MXML_ERROR_NONE;

   if (buf == NULL)
      ref->buflen = ref->bufsize = 0;
   else {
      ref->buflen = buflen;
      ref->bufsize = bufsize;
   }

   ref->bufpos = 0;

   //stream length is left for the program to implement progress indicators
   ref->streamlen = 0;
   ref->streampos = 0;

   //theese are for ungetc operations
   ref->sparechar = MXML_EOF;

   //data is left to fill for the program
   return MXML_STATUS_OK;
}

MXML_REFIL *mxml_refil_new( MXML_REFIL_FUNC func, char *buf, int buflen,
		   int bufsize )
{
	   MXML_REFIL * ret = (MXML_REFIL* ) MXML_ALLOCATOR( sizeof( MXML_REFIL ) );

	   if ( ret == NULL )
	           return NULL;

	   if ( mxml_refil_setup( ret, func, buf, buflen, bufsize ) == MXML_STATUS_OK )
	       return ret;

	   MXML_DELETOR( ret );
	       return NULL;
}
void mxml_refill_from_stream_func( MXML_REFIL *ref )
{
   FILE *fp = (FILE *) ref->data;
   int len;

   len = fread( ref->buffer, 1, ref->bufsize, fp );
   if ( ferror( fp ) ) { 
	   ref->status = MXML_STATUS_ERROR;
	   ref->error = MXML_ERROR_IO;
   }   
   else {
	   ref->buflen = len;
	   ref->bufpos = 0;
   }   
}
int mxml_refil_getc( MXML_REFIL *ref )
{
   if ( ref->sparechar != MXML_EOF ) {
      int chr = ref->sparechar;
      ref->sparechar = MXML_EOF;
      return chr;
   }

   if ( ref->bufpos >= ref->buflen ) {
      if ( ref->refil_func != NULL ) {
         ref->refil_func( ref );
         if ( ref->status != MXML_STATUS_OK || ref->buflen == 0)
            return MXML_EOF;
      }
      else
         return MXML_EOF;
   }

   return ref->buffer[ ref->bufpos++ ];
}
/**
  Reads a data node
*/
static void mxml_node_read_data( MXML_REFIL *ref, MXML_NODE *pNode,
      MXML_DOCUMENT *doc, int iStyle )
{
   char *buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
   char *bufc;
   int iAllocated = MXML_ALLOC_BLOCK;
   int iPos = 0;
   int chr;
   int iStatus = 0, iPosAmper;

   chr = mxml_refil_getc( ref );
   while ( chr != MXML_EOF ) {

      // still in a data element
      if ( chr != '<' ) {

         // verify entity or escape
         if ( chr == '&' && ! (iStyle & MXML_STYLE_NOESCAPE) ) {

            if ( iStatus == 0 ) {
               iStatus = 1;
               iPosAmper = iPos;
            }
            else {
               //error - we have something like &amp &amp
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_UNCLOSEDENTITY;
               return;
            }
         }

         // rightful closing of an entity
         if ( chr == ';' && iStatus == 1 ) {
            int iAmpLen = iPos - iPosAmper - 2;
            char *bp = buf + iPosAmper + 1;

            if ( iAmpLen <= 0 ) {
               //error! - we have "&;"
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_WRONGENTITY;
               return;
            }

            iStatus = 0;

            if ( strncmp( bp, "amp", iAmpLen ) == 0 ) chr = '&';
            else if ( strncmp( bp, "lt", iAmpLen ) == 0 ) chr = '<';
            else if ( strncmp( bp, "gt", iAmpLen ) == 0 ) chr = '>';
            else if ( strncmp( bp, "quot", iAmpLen ) == 0 ) chr = '"';
            else if ( strncmp( bp, "apos", iAmpLen ) == 0 ) chr = '\'';

            // if yes, we must put it at the place of the amper, and restart
            // from there
            if ( chr != ';' ) iPos = iPosAmper;
         }

         buf[ iPos++ ] = chr;
         if ( iPos >= iAllocated ) {
            iAllocated += MXML_ALLOC_BLOCK;
            buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
         }

         if ( chr == MXML_LINE_TERMINATOR )
            doc->iLine++;

      }
      else {
         mxml_refil_ungetc( ref, chr );
         break;
      }

      chr = mxml_refil_getc( ref );
   }

   // see if we had an entity open: <item> &amp hello</item> is an error
   if ( iStatus != 0 ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_UNCLOSEDENTITY;
      return;
   }

   if ( ref->status != MXML_STATUS_OK) {
      doc->status = ref->status;
      doc->error = ref->error;
      return;
   }

   // trimming unneded spaces
   while ( iPos >1 && buf[iPos-1] == ' ' || buf[iPos-1] == '\t' )
      iPos--;
   buf[ iPos ] = 0;
   pNode->type = MXML_TYPE_DATA;
   pNode->data = buf;
   pNode->data_length = iPos;

}
static MXML_STATUS mxml_node_read_name( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   char buf[ MXML_MAX_NAME_LEN + 1];
   int iPos = 0;
   int chr;
   int iStatus = 0;

   chr = 1;
   while ( iStatus < 2 && iPos < MXML_MAX_NAME_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         case 0:
            if ( isalpha( chr ) ) {
               buf[ iPos++ ] = chr;
               iStatus = 1;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
               return MXML_STATUS_MALFORMED;
            }
         break;

         case 1:
            if ( isalnum( chr ) || chr == '_' || chr == '-' || chr == ':' ) {
               buf[ iPos++ ] = chr;
            }
            else if ( chr == '>' || chr == ' ' || chr == '/' || chr == '\r'
                  || chr == '\t' || chr == '\n' ) {
               mxml_refil_ungetc( ref, chr );
               iStatus = 2;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
               return MXML_STATUS_MALFORMED;
            }
         break;
      }
  }

   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return doc->status;
   }

   if ( iStatus != 2  ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_NAMETOOLONG;
      return MXML_STATUS_ERROR;
   }

   pNode->name = ( char *) MXML_ALLOCATOR( iPos + 1);
   memcpy( pNode->name, buf, iPos );
   pNode->name[ iPos ] = 0;

   return MXML_STATUS_OK;
}
static void mxml_node_read_pi( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   int iPos = 0, iAllocated;
   int chr;
   char *buf;
   int iStatus = 0;

   pNode->type = MXML_TYPE_PI;
   // let's read the xml PI instruction
   if ( mxml_node_read_name( ref, pNode, doc ) != MXML_STATUS_OK )
      return;

   // and then we'll put all the "data" into the data member, up to ?>

   buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
   iAllocated = MXML_ALLOC_BLOCK ;
   chr = 1;
   while ( iStatus < 2 ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // scanning for ?>
         case 0:
            if ( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine++;
               buf[ iPos ++ ] = chr;
            }
            else if ( chr == '?' )
               iStatus = 1;
            else {
               if ( iPos > 0 || ( chr != ' ' && chr != '\n' ) )
		       // hm, MXML_LINE_TERMINATOR ?
                  buf[ iPos++ ] = chr;
            }
         break;

         case 1:
            if ( chr == '>' )
               iStatus = 2;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '?';
               mxml_refil_ungetc( ref, chr );
            }
         break;

      }

      if ( iPos == iAllocated ) {
         iAllocated += MXML_ALLOC_BLOCK;
         buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
      }
   }

   if ( ref->status == MXML_STATUS_OK ) {
      buf[iPos] = 0;
      pNode->data = buf;
      pNode->data_length = iPos;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }
}

void mxml_node_unlink( MXML_NODE *node )
{
   if ( node->prev != NULL )
      node->prev->next = node->next;

   if ( node->next != NULL )
      node->next->prev = node->prev;

   if ( node->parent != NULL && node->parent->child == node )
      node->parent->child = node->next;

   node->parent = NULL;
   node->next = NULL;
   node->prev = NULL;
}

MXML_STATUS mxml_node_read( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc, int iStyle )
{
   int nLen = 0;
   MXML_NODE *node, *child_node, *data_node;
   int chr;
   /* Stateful machine status */
   int iStatus = 0;

   chr = 1;
   while ( iStatus >= 0) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      // resetting new node foundings
      node = NULL;

      switch ( iStatus ) {

         case 0:  // outside nodes
            switch ( chr ) {
               case MXML_LINE_TERMINATOR: doc->iLine++; break;
               // We repeat line terminator here for portability
               case MXML_SOFT_LINE_TERMINATOR: break;
               case ' ': case '\t': break;
               case '<': iStatus = 1; break;
               default:  // it is a data node
                  mxml_refil_ungetc( ref, chr );
                  node = mxml_node_new();
                  mxml_node_read_data( ref, node, doc, iStyle );
            }
         break;

         case 1: //inside a node, first character
            if ( chr == '/' ) {
               // This can be met only inside current tag
               iStatus = -1; // done
            }
            else if ( chr == '!' ) {
               iStatus = 2;
            }
            else if ( chr == '?' ) {
               node = mxml_node_new();
               mxml_node_read_pi( ref, node, doc );
            }
            else if ( isalpha( chr ) ) {
               mxml_refil_ungetc( ref, chr );
               node = mxml_node_new();
               mxml_node_read_tag( ref, node, doc, iStatus );
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
            }
         break;

         case 3:
            if ( chr == '-') {
               node = mxml_node_new();
               mxml_node_read_comment( ref, node, doc );
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
            }
         break;
      }

      // have I to add a node below our structure ?
      if ( node != NULL ) {
         if ( ref->status == MXML_STATUS_OK ) {
            mxml_node_add_below( pNode, node );
            // beginning again - a new node is born
            doc->node_count++;
            iStatus = 0;
         }
         else {
            doc->status = ref->status;
            doc->error = ref->error;
            mxml_node_destroy( node );
            return doc->status;
         }
      }

   }

   // if we have an hard error on stream
   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return doc->status;
   }

   if ( iStatus == -1 ) { // ARE WE DONE?
      /* Time to close current node. We must verify:
         1) If the closing tag is coherent with the opened tag name.
         2) If the tag has just one data node as child.
         if we have only one data node among the children, the data
         node is destroyed and the data element is moved to the
         "data" field of current node, to simplify the tree structure
         in the most common config oriented XML files.
      */
      mxml_node_read_closing( ref, pNode, doc );
      // malformed closing tag?
      if ( ref->status != MXML_STATUS_OK ) {
         doc->status = ref->status;
         doc->error = ref->error;
         return doc->status;
      }

      //checking for data nodes
      child_node = pNode->child;
      data_node = NULL;
      while ( child_node != NULL ) {
         if ( child_node->type == MXML_TYPE_DATA ) {
            // first data node ?
            if ( data_node == NULL )
               data_node = child_node;
            // ... or have we more than a data node?
            else {
               data_node = NULL;
               break;
            }
         }
         child_node = child_node->next;
      }

      if ( data_node != NULL ) {
         pNode->data = data_node->data;
         pNode->data_length = data_node->data_length;
         data_node->data = NULL;

         mxml_node_unlink( data_node );
         mxml_node_destroy( data_node );
         doc->node_count--;
      }

   }

   return MXML_STATUS_OK;
}

void mxml_attribute_unlink( MXML_ATTRIBUTE *attrib )
{
   attrib->next = NULL;
}

MXML_STATUS mxml_attribute_setup( MXML_ATTRIBUTE *attribute )
{
   attribute->name = NULL;
   attribute->value = NULL;
   mxml_attribute_unlink( attribute );

   return MXML_STATUS_OK;
}

MXML_ATTRIBUTE *mxml_attribute_new()
{
   MXML_ATTRIBUTE *attribute = (MXML_ATTRIBUTE *)
         MXML_ALLOCATOR( sizeof( MXML_ATTRIBUTE ) );

   if ( attribute != NULL )
      mxml_attribute_setup( attribute );

   return attribute;
}

MXML_ATTRIBUTE *mxml_attribute_read( MXML_REFIL *ref, MXML_DOCUMENT *doc, int style )
{
   MXML_ATTRIBUTE *ret;
   int chr, quotechr;
   char buf_name[ MXML_MAX_NAME_LEN + 1];
   char buf_attrib[MXML_MAX_ATTRIB_LEN *2 + 1];
   int iPosn = 0, iPosa = 0;
   int iStatus = 0;
   int iPosAmper;

   while ( iStatus < 6 && iPosn <= MXML_MAX_NAME_LEN && iPosa <= MXML_MAX_ATTRIB_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // begin
         case 0:
            switch ( chr ) {
               case MXML_LINE_TERMINATOR: doc->iLine++; break;
               // We repeat line terminator here for portability
               case MXML_SOFT_LINE_TERMINATOR: break;
               case ' ': case '\t': break;
               // no attributes found
               case '>': case '/': return NULL;
               default:
                  if ( isalpha( chr ) ) {
                     buf_name[ iPosn++ ] = chr;
                     iStatus = 1;
                  }
                  else {
                     MALFORMED_ERROR( doc,  MXML_ERROR_INVATT );
                     return NULL;
                  }
            }
         break;

         // scanning for a name
         case 1:
            if ( isalnum( chr ) || chr == '_' || chr == '-' || chr == ':' ) {
               buf_name[ iPosn++ ] = chr;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
               iStatus = 2; // waiting for a '='
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
               iStatus = 2;
            }
            else if ( chr == '=' ) {
               iStatus = 3;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;

         // waiting for '='
         case 2:
            if ( chr == '=' ) {
               iStatus = 3;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;

         // waiting for ' or "
         case 3:
            if ( chr == '\'' || chr == '"' ) {
               iStatus = 4;
               quotechr = chr;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;
         // scanning the attribute content ( until next quotechr )
         case 4:
            if ( chr == '&' && !( style & MXML_STYLE_NOESCAPE) ) {
               iStatus = 5;
               iPosAmper = iPosa;
               buf_attrib[ iPosa++ ] = chr; //we'll need it
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
               buf_attrib[ iPosa++ ] = chr;
            }
            else if ( chr == quotechr ) {
               iStatus = 6;
            }
            else {
               buf_attrib[ iPosa++ ] = chr;
            }
         break;

         case 5:
            if ( chr == quotechr ) {
               iStatus = 6;
            }
            else if ( chr == ';' ) {
               int iAmpLen = iPosa - iPosAmper - 2;
               char *bp = buf_attrib + iPosAmper + 1;

               if ( iAmpLen <= 0 ) {
                  //error! - we have "&;"
                  doc->status = MXML_STATUS_MALFORMED;
                  doc->error = MXML_ERROR_WRONGENTITY;
                  return;
               }

               iStatus = 4;

               // we see if we have a predef entity (also known as escape)
               if ( strncmp( bp, "amp", iAmpLen ) == 0 ) chr = '&';
               else if ( strncmp( bp, "lt", iAmpLen ) == 0 ) chr = '<';
               else if ( strncmp( bp, "gt", iAmpLen ) == 0 ) chr = '>';
               else if ( strncmp( bp, "quot", iAmpLen ) == 0 ) chr = '"';
               else if ( strncmp( bp, "apos", iAmpLen ) == 0 ) chr = '\'';
               iPosa = iPosAmper;
               buf_attrib[ iPosa++ ] = chr;
            }
            else if ( ! isalpha( chr ) ) {
               //error - we have something like &amp &amp
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_UNCLOSEDENTITY;
               return NULL;
            }
            else {
               buf_attrib[ iPosa++ ] = chr;
            }
         break;
      }
   }

   if ( doc->status != MXML_STATUS_OK )
      return NULL;

   if ( iStatus < 6 ) {
      doc->status = MXML_STATUS_MALFORMED;
      if ( iPosn > MXML_MAX_NAME_LEN )
         doc->error = MXML_ERROR_ATTRIBTOOLONG;
      else if ( iPosa > MXML_MAX_ATTRIB_LEN )
         doc->error = MXML_ERROR_VALATTOOLONG;
      else
         doc->error = MXML_ERROR_MALFATT;

      return NULL;
   }

   // time to create the attribute
   ret = mxml_attribute_new();
   ret->name = (char *) MXML_ALLOCATOR( iPosn + 1);
   memcpy( ret->name, buf_name, iPosn );
   ret->name[ iPosn ] = 0;

   ret->value = (char *) MXML_ALLOCATOR( iPosa + 1 );
   memcpy( ret->value, buf_attrib, iPosa );
   ret->value[ iPosa ] = 0;

   return ret;
}

MXML_STATUS mxml_node_read_attributes( MXML_REFIL *ref, MXML_NODE *pNode,
         MXML_DOCUMENT *doc, int style )
{

   MXML_ATTRIBUTE *head, *tail;

   head = tail = mxml_attribute_read( ref, doc, style );

   while ( doc->status == MXML_STATUS_OK && tail != NULL ) {
      tail->next = mxml_attribute_read( ref, doc, style );
      tail = tail->next;
   }

   if ( ref->status == MXML_STATUS_OK ) {
      pNode->attributes = head;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }

   return doc->status;
}

void mxml_node_read_tag( MXML_REFIL *ref, MXML_NODE *pNode,
      MXML_DOCUMENT *doc, int style )
{
   char chr;

   pNode->type = MXML_TYPE_TAG;

   if ( mxml_node_read_name( ref, pNode, doc ) == MXML_STATUS_OK ) {
      mxml_node_read_attributes( ref, pNode, doc, style );
   }

   // if the list of attributes terminates with a '/', the last '>' is
   // left unread. This means the current node is complete.
   chr = mxml_refil_getc( ref );
   if ( ref->status == MXML_STATUS_OK && chr != '>' ) {
      mxml_refil_ungetc( ref, chr );
      // recurse
      mxml_node_read( ref, pNode, doc, style );
   }
   else if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
   }

   //else the node is complete
}

void mxml_node_read_comment( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   int iPos = 0, iAllocated;
   int chr;
   char *buf;
   int iStatus = 0;

   pNode->type = MXML_TYPE_COMMENT;
   //  we'll put all the comment into the data member, up to ->

   chr = 1;
   buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
	// flo buf == NULL ?
   iAllocated = MXML_ALLOC_BLOCK;

   while ( iStatus < 3 ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // scanning for ->
         case 0:
            if ( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine++;
               buf[ iPos ++ ] = chr;
            }
            else if ( chr == '-' )
               iStatus = 1;
            else
               buf[ iPos++ ] = chr;
         break;
         case 1:
            if ( chr == '-' )
               iStatus = 2;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '-';
               mxml_refil_ungetc( ref, chr );
            }
         break;

         case 2:
            if ( chr == '>' )
               iStatus = 3;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '-';
               mxml_refil_ungetc( ref, chr );
            }
         break;

      }

      if ( iPos == iAllocated ) {
         iAllocated += MXML_ALLOC_BLOCK;
         buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
      }
   }

   if ( ref->status == MXML_STATUS_OK ) {
      buf[ iPos ] = 0;
      pNode->data = buf;
      pNode->data_length = iPos;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }
}
/**
* Adds a node to the bottom of the children list of tg.
*/

void mxml_node_add_below( MXML_NODE *tg, MXML_NODE *node )
{
   MXML_NODE * child;

   node->parent = tg;
   child = tg->last_child;

   if ( child != NULL ) {
      child->next = node;
      node->prev = child;
      tg->last_child = node;
   }
   else {
      tg->last_child = tg->child = node;
   }
}

void mxml_attribute_destroy( MXML_ATTRIBUTE *attrib )
{
   if ( attrib->name != NULL )
      MXML_DELETOR( attrib->name );

   if ( attrib->value != NULL )
      MXML_DELETOR( attrib->value );

   if ( attrib->next != NULL )
      mxml_attribute_destroy( attrib->next );

   MXML_DELETOR( attrib );
}

void mxml_node_destroy( MXML_NODE *node )
{
   if ( node->name != NULL )
      MXML_DELETOR( node->name );

   if ( node->data != NULL )
      MXML_DELETOR( node->data );

   if ( node->attributes != NULL )
      mxml_attribute_destroy( node->attributes );

   if ( node->next != NULL )
      mxml_node_destroy( node->next );

   if ( node->child != NULL )
      mxml_node_destroy( node->child );

   MXML_DELETOR( node );
}

// checking closing tag
void mxml_node_read_closing( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   char buf[ MXML_MAX_NAME_LEN + 1];
   int iPos = 0;
   int chr;
   int iStatus = 0;

   chr = 1;
   while ( iPos <= MXML_MAX_NAME_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF || chr == '>') break;

      buf[ iPos++ ] = chr;
   }

   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return;
   }

   if ( iPos > MXML_MAX_NAME_LEN ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_NAMETOOLONG;
   }

   buf[ iPos ] = 0;
   if ( chr != '>' || strcmp( pNode->name, buf ) != 0 ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_UNCLOSED;
   }
   // all fine
}

void mxml_refil_destroy ( MXML_REFIL *ref ) {
   if ( ref != NULL )
   {
      MXML_DELETOR( ref );
   }
}

MXML_STATUS mxml_read_file( FILE *fp, MXML_DOCUMENT *doc, int iStyle )
{
   MXML_REFIL *refil;
   MXML_STATUS ret;
   char *buf;

   doc->iLine = 1;

   buf = (char *) malloc( MXML_FILE_BLOCK_SIZE );
   refil = mxml_refil_new(mxml_refill_from_stream_func, buf, 0, MXML_FILE_BLOCK_SIZE);
   refil->data = fp;

   ret = mxml_node_read( refil, doc->root, doc, iStyle );

   mxml_refil_destroy( refil );
   MXML_DELETOR( buf );

   return ret;
}

MXML_STATUS mxml_node_setup( MXML_NODE *node )
{
   node->name = NULL;
   node->data = NULL;
   node->data_length = 0;
   node->attributes = NULL;
   node->type = MXML_TYPE_TAG;

   node->parent = NULL;
   node->next = NULL;
   node->prev = NULL;
   node->child = NULL;
   node->last_child = NULL;

   return MXML_STATUS_OK;
}

MXML_NODE *mxml_node_new()
{
   MXML_NODE *node = (MXML_NODE *) MXML_ALLOCATOR( sizeof( MXML_NODE ) );

   if ( node != NULL )
      mxml_node_setup( node );

   return node;
}

MXML_DOCUMENT *mxml_document_new()
{
   MXML_DOCUMENT *doc =(MXML_DOCUMENT *) malloc( sizeof( MXML_DOCUMENT ) );

   if ( doc != NULL )
      mxml_document_setup( doc );

   return doc;
}

MXML_STATUS mxml_index_setup( MXML_INDEX *index )
{
   index->length = 0;
   index->allocated = 0;
   index->data = NULL;

   return MXML_STATUS_OK;
}

MXML_INDEX *mxml_index_new()
{
   MXML_INDEX *index = (MXML_INDEX *) MXML_ALLOCATOR( sizeof( MXML_INDEX ) );

   if ( index != NULL )
      mxml_index_setup( index );

   return index;
}

MXML_STATUS mxml_document_setup( MXML_DOCUMENT *doc )
{
   doc->iLine = 0;
   doc->name = NULL;
   doc->index = mxml_index_new();
   doc->node_count = 0;
   doc->root = mxml_node_new();

   if ( doc->root != NULL && doc->index != NULL ) {
      doc->root->type = MXML_TYPE_DOCUMENT;
      doc->status = MXML_STATUS_OK;
      doc->error = MXML_ERROR_NONE;
      return MXML_STATUS_OK;
   }

   doc->status = MXML_STATUS_ERROR;
   doc->error = MXML_ERROR_NOMEM;
   return MXML_STATUS_ERROR;
}

void mxml_index_destroy( MXML_INDEX *index )
{
   /* Posix free() can be used with NULL, but nothing is known for other
      free() provided by users */
   if ( index->data != NULL )
      MXML_DELETOR( index->data );

   MXML_DELETOR( index );
}

void mxml_document_destroy( MXML_DOCUMENT *doc )
{
   mxml_index_destroy( doc->index );
   MXML_DELETOR( doc );
}

MXML_NODE *mxml_findchild(MXML_NODE *pnode, const char *name)
{
	MXML_NODE *tmpnode = NULL;
	if(pnode == NULL || name == NULL)
		return NULL;
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
MXML_NODE * xml_findnext(MXML_NODE *pNode)
{
		return pNode->next;
}

wchar_t* pj_ansi_to_unicode(const char *utf8, size_t length, wchar_t* outbuf, size_t buf_count)
{
	const char* pc = (const char*)utf8;
	const char* last = pc + length;

	unsigned int b;
	unsigned int num_errors = 0;
	int i = 0;
	int wlen = buf_count/sizeof(wchar_t) - 1;//by nichel,for string complete

	memset(outbuf, 0, buf_count);

	if(!utf8 || length == 0)
		return 1;

	while (pc < last && i < wlen)
	{
		b = *pc++;

		if( !b ) break; // 0 - is eos in all utf encodings

		if ((b & 0x80) == 0)
		{
			// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
			;
		} 
		else if ((b & 0xe0) == 0xc0) 
		{
			// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if(pc == last) { outbuf[i++]='?'; ++num_errors; break; }
			b = (b & 0x1f) << 6;
			b |= (*pc++ & 0x3f);
		} 
		else if ((b & 0xf0) == 0xe0) 
		{
			// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 1) { outbuf[i++]='?'; ++num_errors; break; }

			b = (b & 0x0f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			if(b == 0xFEFF &&
				i == 0) // bom at start
				continue; // skip it
		} 
		else if ((b & 0xf8) == 0xf0) 
		{
			// 4-byte sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 2) { outbuf[i++]='?'; break; }

			b = (b & 0x07) << 18;
			b |= (*pc++ & 0x3f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			// b shall contain now full 21-bit unicode code point.
			////////////assert((b & 0x1fffff) == b);  ////we don't need this?
			if((b & 0x1fffff) != b)
			{
				outbuf[i++]='?';
				++num_errors;
				continue;
			}
			if( sizeof(wchar_t) == 16 ) // Seems like Windows, wchar_t is utf16 code units sequence there.
			{
				outbuf[i++] = (wchar_t)(0xd7c0 + (b >> 10));
				outbuf[i++] = (wchar_t)(0xdc00 | (b & 0x3ff));
			}
			else if( sizeof(wchar_t) >= 21 ) // wchar_t is full ucs-4 
			{
				outbuf[i++] = (wchar_t)(b);
			}
			else
			{
				return NULL;
				assert(0); // what? wchar_t is single byte here?
			}
		} 
		else 
		{
			return NULL;
			assert(0); //bad start for UTF-8 multi-byte sequence"
			++num_errors;
			b = '?';
		}
		outbuf[i++] = (wchar_t)(b);
	}

	return outbuf;
}



int main(int argc, char **argv)
{
	char inputFile[128] = {0}, outputFile[128] = {0};
	int i;
	MXML_DOCUMENT *doc;
	MXML_NODE *root_node, *first_node;
	FILE *fp = NULL;
	for(i = 0; i < argc; i++)
	{
		if(!strncmp(argv[i], "inputFile=", 10))
		{
			strncpy(inputFile, argv[i] + 10, sizeof(inputFile) - 1);
			continue;
		}
		else if(!strncmp(argv[i], "outputFile=", 11))
		{
			strncpy(outputFile, argv[i] + 11, sizeof(outputFile) - 1);
			continue;
		}
	}
	if(strlen(inputFile) <=0 || strlen(outputFile) <= 0)
	{
		return -1;
	}
	doc = mxml_read(inputFile, 0);	
	if(!(doc && doc->status == MXML_STATUS_OK))
	{
		printf("read input file error!\n");
		return -1;
	}
	if(!(root_node = mxml_findchild(doc->root, "Language")))
	{
		printf("cann't find child of Language!!!\n");	
		return -1;
	}
	if((fp = fopen(outputFile, "w")) == NULL)
	{
		printf("can't open/create file\n");
		return -1;
	}
	printf("root name: %s\n", root_node->name);
	printf("root data: %s\n", root_node->data);
	for(first_node = root_node->child; first_node != NULL; first_node = first_node->next)
	{
		wchar_t str[1280];
	        char buf[1280];
		size_t len, n;

		printf("%s\n", first_node->data);
	/*	memcpy(str, first_node->data, (128 - 1) * sizeof(wchar_t));*/
		pj_ansi_to_unicode(first_node->data, strlen(first_node->data), 
				str, (1280 - 1) * sizeof(wchar_t));
		len = wcslen(str);
		n = 0;
		n = snprintf(buf, 1280 * 4 * sizeof(char), "static const wchar_t str_%s[] = {", first_node->name);
		for(i = 0; i < len; i++)
		{
			n += snprintf(buf + n, 1280 * sizeof(char) - n, "%#x, ", (unsigned int)str[i]);
		}
		n += snprintf(buf + n, 1280 * sizeof(wchar_t) - n, "0x00};\n");
		fwrite(buf, n, 1, fp);
	}
	printf("over\n");
/*
	size_t len = wcslen(str);
	for(; i < len; i++)
	{
		printf("%#x, ", (unsigned int)str[i]);
	}
	printf("\n");
	*/
	return 0;
}

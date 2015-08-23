/******************************************************************
 *
 * uEcho for C
 *
 * Copyright (C) Satoshi Konno 2015
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <uecho/node.h>
#include <uecho/object.h>
#include <uecho/profile.h>
#include <uecho/misc.h>

/****************************************
* uecho_object_new
****************************************/

uEchoObject *uecho_object_new(void)
{
	uEchoObject *obj;

	obj = (uEchoObject *)malloc(sizeof(uEchoObject));
    
  if (!obj)
    return NULL;

  uecho_list_node_init((uEchoList *)obj);

  uecho_object_setparentnode(obj, NULL);
  
  uecho_object_setclassgroupcode(obj, 0);
  uecho_object_setclasscode(obj, 0);
  uecho_object_setinstancecode(obj, 0);

  obj->properties = uecho_propertylist_new();
  
  // Property map caches
  
  obj->annoPropMapSize = 0;
  obj->annoPropMapBytes = NULL;
  
  obj->setPropMapSize = 0;
  obj->setPropMapBytes = NULL;
  
  obj->getPropMapSize = 0;
  obj->getPropMapBytes = NULL;

  // Mandatory Properties
  
  uecho_object_addmandatoryproperties(obj);
  
  return obj;
}

/****************************************
* uecho_object_delete
****************************************/

void uecho_object_delete(uEchoObject *obj)
{
	uecho_list_remove((uEchoList *)obj);
  
  uecho_object_clearpropertymapcaches(obj);
  
  uecho_propertylist_delete(obj->properties);
	
  free(obj);
}

/****************************************
 * uecho_object_setparentnode
 ****************************************/

void uecho_object_setparentnode(uEchoObject *obj, uEchoNode *node)
{
  obj->parentNode = node;
}

/****************************************
 * uecho_object_getparentnode
 ****************************************/

uEchoNode *uecho_object_getparentnode(uEchoObject *obj)
{
  return (uEchoNode *)obj->parentNode;
}

/****************************************
 * uecho_object_setcode
 ****************************************/

void uecho_object_setcode(uEchoObject *obj, uEchoObjectCode val)
{
  obj->code[0] = (val & 0xFF0000) >> 16;
  obj->code[1] = (val & 0x00FF00) >>  8;
  obj->code[2] = (val & 0x0000FF);
}

/****************************************
 * uecho_object_getcode
 ****************************************/

uEchoObjectCode uecho_object_getcode(uEchoObject *obj)
{
  int code = 0;
  code |= (obj->code[0] << 16) & 0xFF0000;
  code |= (obj->code[1] <<  8) & 0x00FF00;
  code |= (obj->code[2]      ) & 0x0000FF;
  return code;
}

/****************************************
 * uecho_object_getcode
 ****************************************/

bool uecho_object_iscode(uEchoObject *obj, uEchoObjectCode code)
{
  return (uecho_object_getcode(obj) == code) ? true : false;
}

/****************************************
 * uecho_object_setclassgroupcode
 ****************************************/

void uecho_object_setclassgroupcode(uEchoObject *obj, byte val)
{
  obj->code[0] = val;
}
  
/****************************************
 * uecho_object_getclassgroupcode
 ****************************************/

byte uecho_object_getclassgroupcode(uEchoObject *obj)
{
  return obj->code[0];
}

/****************************************
 * uecho_object_setclasscode
 ****************************************/

void uecho_object_setclasscode(uEchoObject *obj, byte val)
{
  obj->code[1] = val;
}
      
/****************************************
 * uecho_object_getclasscode
 ****************************************/

byte uecho_object_getclasscode(uEchoObject *obj)
{
  return obj->code[1];
}

/****************************************
 * uecho_object_setinstancecode
 ****************************************/

void uecho_object_setinstancecode(uEchoObject *obj, byte val)
{
  obj->code[2] = val;
}
          
/****************************************
 * uecho_object_getinstancecode
 ****************************************/

byte uecho_object_getinstancecode(uEchoObject *obj)
{
  return obj->code[2];
}

/****************************************
 * uecho_object_isdevice
 ****************************************/

bool uecho_object_isdevice(uEchoObject *obj)
{
  return uecho_isdeviceclassgroupcode(obj->code[0]);
}

/****************************************
 * uecho_object_isprofile
 ****************************************/

bool uecho_object_isprofile(uEchoObject *obj)
{
  return uecho_isprofileclassgroupcode(obj->code[0]);
}

/****************************************
 * uecho_object_setpropertymap
 ****************************************/

bool uecho_object_setpropertymap(uEchoObject *obj, uEchoPropertyCode mapCode, uEchoPropertyCode *propCodes, size_t propsCodeSize)
{
  byte propMapData[uEchoPropertyMapMaxLen + 1];
  uEchoPropertyCode *propMap;
  size_t n, propByteIdx;
  
  propMapData[0] = (byte)propsCodeSize;
  propMap = propMapData + 1;
  
  // propsCodeSize <= uEchoPropertyMapMaxLen
  
  if (propsCodeSize <= uEchoPropertyMapMaxLen) {
    memcpy(propMap, propCodes, propsCodeSize);
    uecho_object_addproperty(obj, mapCode, uEchoPropertyAttrRead);
    uecho_object_setpropertydata(obj, mapCode, propMapData, (propsCodeSize + 1));
    return true;
  }
  
  // uEchoPropertyMapMaxLen < propsCodeSize
  
  for (n=0; n<propsCodeSize; n++) {
    byte propCode;
    propCode = propCodes[n];
    if ((propCode < uEchoPropertyCodeMin) || (uEchoPropertyCodeMax < propCode))
      continue;
    propByteIdx = (propCode - uEchoPropertyCodeMin) & 0x0F;
    propMap[propByteIdx] |= (((propCode - uEchoPropertyCodeMin) & 0xF0) >> 8) & 0x0F;
  }
  
  uecho_object_addproperty(obj, mapCode, uEchoPropertyAttrRead);
  uecho_object_setpropertydata(obj, mapCode, propMapData, (uEchoPropertyMapMaxLen + 1));
  
  return true;
}

/****************************************
 * uecho_object_addproperty
 ****************************************/

bool uecho_object_addproperty(uEchoObject *obj, uEchoPropertyCode code, uEchoPropertyAttr attr)
{
  if (!uecho_propertylist_set(obj->properties, code, attr, NULL, 0))
    return false;
  return uecho_object_updatepropertymaps(obj);
}

/****************************************
 * uecho_object_setpropertydata
 ****************************************/

bool uecho_object_setpropertydata(uEchoObject *obj, uEchoPropertyCode code, byte *data, size_t dataLen)
{
  return uecho_propertylist_setdata(obj->properties, code, data, dataLen);
}

/****************************************
 * uecho_object_setpropertyintegerdata
 ****************************************/

bool uecho_object_setpropertyintegerdata(uEchoObject *obj, uEchoPropertyCode code, int data, size_t dataLen)
{
  return uecho_propertylist_setintegerdata(obj->properties, code, data, dataLen);
}

/****************************************
 * uecho_object_setpropertyattribute
 ****************************************/

bool uecho_object_setpropertyattribute(uEchoObject *obj, uEchoPropertyCode code, uEchoPropertyAttr attr)
{
  return uecho_propertylist_setattribute(obj->properties, code, attr);
}

/****************************************
 * uecho_object_getproperties
 ****************************************/

uEchoProperty *uecho_object_getproperties(uEchoObject *obj)
{
  return uecho_propertylist_gets(obj->properties);
}

/****************************************
 * uecho_object_hasproperty
 ****************************************/

bool uecho_object_hasproperty(uEchoObject *obj, uEchoPropertyCode code)
{
  return (uecho_object_getproperty(obj, code) != NULL) ? true : false;
}

/****************************************
 * uecho_object_getproperty
 ****************************************/

uEchoProperty *uecho_object_getproperty(uEchoObject *obj, uEchoPropertyCode code)
{
  return uecho_propertylist_findbycode(obj->properties, code);
}

/****************************************
 * uecho_object_getpropertycount
 ****************************************/

size_t uecho_object_getpropertycount(uEchoObject *obj)
{
  return uecho_propertylist_size(obj->properties);
}

/****************************************
 * uecho_object_clearproperties
 ****************************************/

void uecho_object_clearproperties(uEchoObject *obj)
{
  uecho_propertylist_clear(obj->properties);
  uecho_object_clearpropertymapcaches(obj);
}

/****************************************
 * uecho_object_getpropertyintegerdata
 ****************************************/

bool uecho_object_getpropertyintegerdata(uEchoObject *obj, uEchoPropertyCode code, size_t dataLen, int *data)
{
  return uecho_propertylist_getintegerdata(obj->properties, code, dataLen, data);
}

/******************************************************************
 *
 * uEcho for C
 *
 * Copyright (C) Satoshi Konno 2015
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <string.h>

#include <uecho/profile.h>

/****************************************
 * uecho_object_addsuperclassproperties
 ****************************************/

bool uecho_object_addsuperclassproperties(uEchoObject *obj)
{
  byte manufactureCode[] = {0, 0, 0};
  byte zeroPropMap[] = {0};
  
  if (!obj)
    return false;
  
  // Manufacture Code
  
  uecho_object_addproperty(obj, uEchoProfileObjectSuperClassManufacturerCode, uEchoPropertyAttrRead,  manufactureCode, sizeof(manufactureCode));
  
  // Property map properties
  
  if (!uecho_object_updateproperty(obj, uEchoProfileObjectSuperClassGetPropertyMap, uEchoPropertyAttrRead, zeroPropMap, sizeof(zeroPropMap)))
    return false;
  if (!uecho_object_updateproperty(obj, uEchoProfileObjectSuperClassSetPropertyMap, uEchoPropertyAttrRead, zeroPropMap, sizeof(zeroPropMap)))
    return false;
  if (!uecho_object_updateproperty(obj, uEchoProfileObjectSuperClassAnnoPropertyMap, uEchoPropertyAttrRead, zeroPropMap, sizeof(zeroPropMap)))
    return false;
  
  return uecho_object_updatesuperclassproperties(obj);
}

/****************************************
 * uecho_object_addsuperclassproperties
 ****************************************/

bool uecho_object_updatesuperclassproperties(uEchoObject *obj)
{
  return true;
}

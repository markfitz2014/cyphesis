// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubExternalMindsManager_custom.h file.

#ifndef STUB_SERVER_EXTERNALMINDSMANAGER_H
#define STUB_SERVER_EXTERNALMINDSMANAGER_H

#include "server/ExternalMindsManager.h"
#include "stubExternalMindsManager_custom.h"

#ifndef STUB_ExternalMindsManager_instance
//#define STUB_ExternalMindsManager_instance
   ExternalMindsManager* ExternalMindsManager::instance()
  {
    return nullptr;
  }
#endif //STUB_ExternalMindsManager_instance

#ifndef STUB_ExternalMindsManager_addConnection
//#define STUB_ExternalMindsManager_addConnection
  int ExternalMindsManager::addConnection(const ExternalMindsConnection& connection)
  {
    return 0;
  }
#endif //STUB_ExternalMindsManager_addConnection

#ifndef STUB_ExternalMindsManager_removeConnection
//#define STUB_ExternalMindsManager_removeConnection
  int ExternalMindsManager::removeConnection(const std::string& routerId)
  {
    return 0;
  }
#endif //STUB_ExternalMindsManager_removeConnection

#ifndef STUB_ExternalMindsManager_requestPossession
//#define STUB_ExternalMindsManager_requestPossession
  int ExternalMindsManager::requestPossession(Character& character, const std::string& language, const std::string& script)
  {
    return 0;
  }
#endif //STUB_ExternalMindsManager_requestPossession

#ifndef STUB_ExternalMindsManager_entity_destroyed
//#define STUB_ExternalMindsManager_entity_destroyed
  void ExternalMindsManager::entity_destroyed(Character* character)
  {
    
  }
#endif //STUB_ExternalMindsManager_entity_destroyed

#ifndef STUB_ExternalMindsManager_character_externalLinkChanged
//#define STUB_ExternalMindsManager_character_externalLinkChanged
  void ExternalMindsManager::character_externalLinkChanged(Character* character)
  {
    
  }
#endif //STUB_ExternalMindsManager_character_externalLinkChanged

#ifndef STUB_ExternalMindsManager_requestPossessionFromRegisteredClients
//#define STUB_ExternalMindsManager_requestPossessionFromRegisteredClients
  int ExternalMindsManager::requestPossessionFromRegisteredClients(const std::string& character_id)
  {
    return 0;
  }
#endif //STUB_ExternalMindsManager_requestPossessionFromRegisteredClients

#ifndef STUB_ExternalMindsManager_addPossessionEntryForCharacter
//#define STUB_ExternalMindsManager_addPossessionEntryForCharacter
  void ExternalMindsManager::addPossessionEntryForCharacter(Character& character)
  {
    
  }
#endif //STUB_ExternalMindsManager_addPossessionEntryForCharacter


#endif
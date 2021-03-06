// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubTerrainModTranslator_custom.h file.

#ifndef STUB_RULESETS_TERRAINMODTRANSLATOR_H
#define STUB_RULESETS_TERRAINMODTRANSLATOR_H

#include "rulesets/TerrainModTranslator.h"
#include "stubTerrainModTranslator_custom.h"

#ifndef STUB_TerrainModTranslator_TerrainModTranslator
//#define STUB_TerrainModTranslator_TerrainModTranslator
   TerrainModTranslator::TerrainModTranslator(const Atlas::Message::MapType& data)
  {
    
  }
#endif //STUB_TerrainModTranslator_TerrainModTranslator

#ifndef STUB_TerrainModTranslator_parseData
//#define STUB_TerrainModTranslator_parseData
  Mercator::TerrainMod* TerrainModTranslator::parseData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation)
  {
    return nullptr;
  }
#endif //STUB_TerrainModTranslator_parseData

#ifndef STUB_TerrainModTranslator_isValid
//#define STUB_TerrainModTranslator_isValid
  bool TerrainModTranslator::isValid() const
  {
    return false;
  }
#endif //STUB_TerrainModTranslator_isValid

#ifndef STUB_TerrainModTranslator_reset
//#define STUB_TerrainModTranslator_reset
  void TerrainModTranslator::reset()
  {
    
  }
#endif //STUB_TerrainModTranslator_reset

#ifndef STUB_TerrainModTranslator_parsePosition
//#define STUB_TerrainModTranslator_parsePosition
   float TerrainModTranslator::parsePosition(const WFMath::Point<3> & pos, const Atlas::Message::MapType& modElement)
  {
    return 0;
  }
#endif //STUB_TerrainModTranslator_parsePosition

#ifndef STUB_TerrainModTranslator_buildTranslator
//#define STUB_TerrainModTranslator_buildTranslator
  InnerTranslator* TerrainModTranslator::buildTranslator(const Atlas::Message::MapType& modElement, const std::string & typeName, Shape<2> & shape, const Atlas::Message::Element & shapeElement)
  {
    return nullptr;
  }
#endif //STUB_TerrainModTranslator_buildTranslator


#endif
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Python.h>

#include "EntityFactory.h"

#include "PersistantThingFactory.h"
#include "ScriptFactory.h"
#include "Persistance.h"
#include "Persistor.h"
#include "Player.h"

#include "rulesets/Thing.h"
#include "rulesets/MindFactory.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Food.h"
#include "rulesets/Stackable.h"
#include "rulesets/Structure.h"
#include "rulesets/Line.h"
#include "rulesets/Area.h"
#include "rulesets/World.h"

#include "rulesets/Python_Script_Utils.h"

#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/random.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Entity/GameEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

static const bool debug_flag = false;

// This is a template which requires debug flag to be declared.
#include "rulesets/Entity_getLocation.h"

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory(BaseWorld & w) : m_world(w),
             m_eft(new PersistantThingFactory<Entity>())
{
    installFactory("game_entity", "world", new ForbiddenThingFactory<World>());
    PersistantThingFactory<Thing> * tft = new PersistantThingFactory<Thing>();
    installFactory("game_entity", "thing", tft);
    installFactory("thing", "feature", tft->duplicateFactory());
    installFactory("feature", "line", new PersistantThingFactory<Line>());
    installFactory("feature", "area", new PersistantThingFactory<Area>());
    installFactory("thing", "character",
                   new PersistantThingFactory<Character>());
    installFactory("character", "creator",
                   new PersistantThingFactory<Creator>());
    installFactory("thing", "plant", new PersistantThingFactory<Plant>());
    installFactory("thing", "food", new PersistantThingFactory<Food>());
    installFactory("thing", "stackable",
                   new PersistantThingFactory<Stackable>());
    installFactory("thing", "structure",
                   new PersistantThingFactory<Structure>());
}

void EntityFactory::initWorld()
{
    FactoryDict::const_iterator I = m_factories.find("world");
    if (I == m_factories.end()) {
        log(CRITICAL, "No world factory");
        return;
    }
    ForbiddenThingFactory<World> * wft = dynamic_cast<ForbiddenThingFactory<World> *>(I->second);
    if (wft == 0) {
        log(CRITICAL, "Its not a world factory");
        return;
    }
    wft->m_p.persist((World&)m_world.m_gameWorld);
}

Entity * EntityFactory::newEntity(const std::string & id,
                                  const std::string & type,
                                  const MapType & attributes)
{
    debug(std::cout << "EntityFactor::newEntity()" << std::endl << std::flush;);
    Entity * thing = 0;
    FactoryDict::const_iterator I = m_factories.find(type);
    PersistorBase * pc = 0;
    if (I == m_factories.end()) {
        return 0;
    }
    FactoryBase * factory = I->second;
    if (consts::enable_persistence) {
        thing = factory->newPersistantThing(id, &pc);
    } else {
        thing = factory->newThing(id);
    }
    if (thing == 0) {
        return 0;
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setType(type);
    // Sort out python object
    if (factory->m_scriptFactory != 0) {
        debug(std::cout << "Class " << type << " has a python class"
                        << std::endl << std::flush;);
        factory->m_scriptFactory->addScript(thing);
    }

    // Read the defaults
    thing->merge(factory->m_attributes);
    // And then override with the values provided for this entity.
    thing->merge(attributes);
    // Get location from entity, if it is present
    // The default attributes cannot contain info on location
    if (thing->getLocation(attributes, m_world.getEntities())) {
        // If no info was provided, put the entity in the game world
        thing->m_location.m_loc = &m_world.m_gameWorld;
    }
    if (!thing->m_location.m_pos.isValid()) {
        // If no position coords were provided, put it somewhere near origin
        thing->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
    }
    if (pc != 0) {
        pc->persist();
        thing->clearUpdateFlags();
    }
    delete pc;
    return thing;
}

void EntityFactory::flushFactories()
{
    FactoryDict::const_iterator Iend = m_factories.end();
    for (FactoryDict::const_iterator I = m_factories.begin(); I != Iend; ++I) {
        delete I->second;
    }
    delete m_eft;
    m_eft = NULL;
    m_factories.clear();
}

void EntityFactory::populateFactory(const std::string & className,
                                    FactoryBase * factory,
                                    const MapType & classDesc)
{
    // Establish whether this rule has an associated script, and
    // if so, use it.
    MapType::const_iterator J = classDesc.find("script");
    MapType::const_iterator Jend = classDesc.end();
    if ((J != Jend) && (J->second.isMap())) {
        const MapType & script = J->second.asMap();
        J = script.find("name");
        if ((J != script.end()) && (J->second.isString())) {
            const std::string & scriptName = J->second.asString();
            J = script.find("language");
            if ((J != script.end()) && (J->second.isString())) {
                const std::string & scriptLanguage = J->second.asString();
                if (factory->m_scriptFactory != 0) {
                    std::cout << "Factory exists" << std::endl << std::flush;
                    if (factory->m_scriptFactory->package() != scriptName) {
                        std::cout << "The wrong factory"
                                  << std::endl << std::flush;
                        delete factory->m_scriptFactory;
                        factory->m_scriptFactory = 0;
                    }
                }
                if (factory->m_scriptFactory == 0) {
                    if (scriptLanguage == "python") {
                        factory->m_scriptFactory = new PythonScriptFactory(scriptName, className);
                    } else {
                        log(ERROR, "Unknown script language.");
                    }
                }
            }
        }
    }

    // Establish whether this rule has an associated mind rule,
    // and handle it.
    J = classDesc.find("mind");
    if ((J != Jend) && (J->second.isMap())) {
        const MapType & script = J->second.asMap();
        J = script.find("name");
        if ((J != script.end()) && (J->second.isString())) {
            const std::string mindType = J->second.asString();
            // language is unused. might need it one day
            // J = script.find("language");
            // if ((J != script.end()) && (J->second.isString())) {
                // const std::string & mindLang = J->second.asString();
            // }
            MindFactory::instance()->addMindType(className, mindType);
        }
    }

    // Store the default attribute for entities create by this rule.
    J = classDesc.find("attributes");
    if ((J != Jend) && (J->second.isMap())) {
        const MapType & attrs = J->second.asMap();
        MapType::const_iterator Kend = attrs.end();
        for (MapType::const_iterator K = attrs.begin(); K != Kend; ++K) {
            if (!K->second.isMap()) {
                log(ERROR, "Attribute description in rule is not a map.");
                continue;
            }
            const MapType & attr = K->second.asMap();
            MapType::const_iterator L = attr.find("default");
            if (L != attr.end()) {
                factory->m_attributes[K->first] = L->second;
            }
        }
    }

    // Check whether it should be available to players as a playable character.
    J = classDesc.find("playable");
    if ((J != Jend) && (J->second.isInt())) {
        Player::playableTypes.insert(className);
    }
}

int EntityFactory::installRule(const std::string & className,
                               const MapType & classDesc)
{
    MapType::const_iterator J = classDesc.find("parents");
    MapType::const_iterator Jend = classDesc.end();
    if (J == Jend) {
        std::string msg = std::string("Rule \"") + className 
                          + "\" has no parents. Skipping.";
        log(ERROR, msg.c_str());
        return -1;
    }
    if (!J->second.isList()) {
        std::string msg = std::string("Rule \"") + className 
                          + "\" has malformed parents. Skipping.";
        log(ERROR, msg.c_str());
        return -1;
    }
    const ListType & parents = J->second.asList();
    if (parents.empty()) {
        std::string msg = std::string("Rule \"") + className 
                          + "\" has empty parents. Skipping.";
        log(ERROR, msg.c_str());
        return -1;
    }
    const Element & p1 = parents.front();
    if (!p1.isString() || p1.asString().empty()) {
        std::string msg = std::string("Rule \"") + className 
                          + "\" has malformed first parent. Skipping.";
        log(ERROR, msg.c_str());
        return -1;
    }
    const std::string & parent = p1.asString();
    // Get the new factory for this rule
    FactoryBase * factory = getNewFactory(parent);
    if (factory == 0) {
        debug(std::cout << "Rule \"" << className
                        << "\" has non existant parent \"" << parent
                        << "\". Waiting." << std::endl << std::flush;);
        m_waitingRules.insert(make_pair(parent, make_pair(className, classDesc)));
        return 1;
    }

    populateFactory(className, factory, classDesc);

    debug(std::cout << "INSTALLING " << className << ":" << parent
                    << std::endl << std::flush;);

    // Install the factory in place.
    installFactory(parent, className, factory);

    // Install any rules that were waiting for this rule before they
    // could be installed
    RuleWaitList::iterator I = m_waitingRules.lower_bound(className);
    for (; I != m_waitingRules.upper_bound(className); ++I) {
        const std::string & wClassName = I->second.first;
        const MapType & wClassDesc = I->second.second;
        debug(std::cout << "WAITING rule " << wClassName
                        << " now ready" << std::endl << std::flush;);
        installRule(wClassName, wClassDesc);
    }
    m_waitingRules.erase(className);
    return 0;
}

int EntityFactory::modifyRule(const std::string & className,
                              const MapType & classDesc)
{
    FactoryDict::const_iterator I = m_factories.find(className);
    if (I == m_factories.end()) {
        log(ERROR, "Could not find factory for existing type.");
        return -1;
    }
    FactoryBase * factory = I->second;
    assert(factory != 0);
    
    ScriptFactory * script_factory = factory->m_scriptFactory;
    if (script_factory != 0) {
        script_factory->refreshClass();
    }

    // FIXME Update the other things about this class.
    return 0;
}

void EntityFactory::getRulesFromFiles(MapType & rules)
{
    std::vector<std::string>::const_iterator I = rulesets.begin();
    std::vector<std::string>::const_iterator Iend = rulesets.end();
    for (; I != Iend; ++I) {
        std::string filename = etc_directory + "/cyphesis/" + *I + ".xml";
        AtlasFileLoader f(filename, rules);
        if (!f.isOpen()) {
            log(ERROR, "Unable to open rule file.");
        }
        f.read();
    }
}

void EntityFactory::installRules()
{
    MapType ruleTable;

    if (consts::enable_database) {
        Persistance * p = Persistance::instance();
        p->getRules(ruleTable);
    } else {
        getRulesFromFiles(ruleTable);
    }

    if (ruleTable.empty()) {
        log(ERROR, "Rule database table contains no rules.");
        if (consts::enable_database) {
            log(NOTICE, "Attempting to load temporary ruleset from files.");
            getRulesFromFiles(ruleTable);
        }
    }

    MapType::const_iterator Iend = ruleTable.end();
    for (MapType::const_iterator I = ruleTable.begin(); I != Iend; ++I) {
        const std::string & className = I->first;
        const MapType & classDesc = I->second.asMap();
        installRule(className, classDesc);
    }
    // Report on the non-cleared rules.
    // Perhaps we can keep them too?
    // m_waitingRules.clear();
    RuleWaitList::const_iterator J = m_waitingRules.begin();
    RuleWaitList::const_iterator Jend = m_waitingRules.end();
    for (; J != Jend; ++J) {
        const std::string & wParentName = J->first;
        std::string msg = std::string("Rule \"") + J->first
                          + "\" with parent \"" + wParentName
                          + "\" is an orphan.";
        log(ERROR, msg.c_str());
    }
}

void EntityFactory::installFactory(const std::string & parent,
                                   const std::string & className,
                                   FactoryBase * factory)
{
    assert(factory != 0);

    m_factories[className] = factory;

    Inheritance & i = Inheritance::instance();

    Atlas::Objects::Root * r = new Atlas::Objects::Entity::GameEntity(Atlas::Objects::Entity::GameEntity::Class());
    r->setId(className);
    r->setParents(ListType(1, parent));
    i.addChild(r);
}

FactoryBase * EntityFactory::getNewFactory(const std::string & parent)
{
    FactoryDict::const_iterator I = m_factories.find(parent);
    if (I == m_factories.end()) {
        return 0;
    }
    return I->second->duplicateFactory();
}

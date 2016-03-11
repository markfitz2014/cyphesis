// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Thing.h"

#include "Motion.h"
#include "Domain.h"
#include "TransformsProperty.h"
#include "PropelProperty.h"

#include "common/BaseWorld.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/Property.h"

#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Update.h"
#include "common/Pickup.h"
#include "common/Drop.h"
#include "common/Unseen.h"
#include "common/TypeNode.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Drop;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Pickup;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

/// \brief Constructor for physical or tangible entities.
Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Deleting %1(%2) when it is not "
                                   "in the world.", getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }
    // The actual destruction and removal of this entity will be handled
    // by the WorldRouter

    if (isPerceptive()) {
        //We need to send a sight operation directly to the entity.
        //The reason is that else the entity will be deleted before it can receive the broadcast Sight
        //of the Delete op, which will leave any external clients hanging.
        Sight sToEntity;
        sToEntity->setArgs1(op);
        sToEntity->setTo(getId());
        operation(sToEntity, res);
    }

    Sight s;
    s->setArgs1(op);
    res.push_back(s);

    Entity::DeleteOperation(op, res);
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Thing::move_operation" << std::endl << std::flush;);

    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Moving %1(%2) when it is not in the world.",
                                   getType(), getId()));
        assert(m_location.m_loc != 0);
        return;
    }

    // Check the validity of the operation.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Move has no argument", res, getId());
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        error(op, "Move op arg is malformed", res, getId());
        return;
    }
    if (getId() != ent->getId()) {
        error(op, "Move op does not have correct id in argument", res, getId());
        return;
    }

    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        error(op, "Move op has no loc", res, getId());
        return;
    }
    const std::string & new_loc_id = ent->getLoc();
    LocatedEntity * new_loc = 0;
    if (new_loc_id != m_location.m_loc->getId()) {
        // If the LOC has not changed, we don't need to look it up, or do
        // any of the following checks.
        new_loc = BaseWorld::instance().getEntity(new_loc_id);
        if (new_loc == 0) {
            error(op, "Move op loc does not exist", res, getId());
            return;
        }
        debug(std::cout << "LOC: " << new_loc_id << std::endl << std::flush;);
        LocatedEntity * test_loc = new_loc;
        for (; test_loc != 0; test_loc = test_loc->m_location.m_loc) {
            if (test_loc == this) {
                error(op, "Attempt to move into itself", res, getId());
                return;
            }
        }
        assert(new_loc != 0);
        assert(m_location.m_loc != new_loc);
    }

    if (!ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        error(op, "Move op has no pos", res, getId());
        return;
    }

    // Up until this point nothing should have changed, but the changes
    // have all now been checked for validity.

    const Location old_loc = m_location;

    // Check if the location has changed
    if (new_loc != 0) {
        // new_loc should only be non-null if the LOC specified is
        // different from the current LOC
        assert(m_location.m_loc != new_loc);
        // Check for pickup, ie if the new LOC is the actor, and the
        // previous LOC is the actor's LOC.
        if (new_loc->getId() == op->getFrom() &&
            m_location.m_loc == new_loc->m_location.m_loc) {

            Pickup p;
            p->setFrom(op->getFrom());
            p->setTo(getId());
            Sight s;
            s->setArgs1(p);
            res.push_back(s);

            Anonymous wield_arg;
            wield_arg->setId(getId());
            Wield w;
            w->setTo(op->getFrom());
            w->setArgs1(wield_arg);
            res.push_back(w);
        }
        // Check for drop, ie if the old LOC is the actor, and the
        // new LOC is the actor's LOC.
        if (m_location.m_loc->getId() == op->getFrom() &&
            new_loc == m_location.m_loc->m_location.m_loc) {

            Drop d;
            d->setFrom(op->getFrom());
            d->setTo(getId());
            Sight s;
            s->setArgs1(d);
            res.push_back(s);
        }

        // Update loc
        changeContainer(new_loc);
    }

    std::string mode;

    if (hasAttr("mode")) {
        Element mode_attr;
        getAttr("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a \"%1\" in "
                                       "Thing::MoveOperation",
                                       Element::typeName(mode_attr.getType())));
        }
    }

    // Move ops often include a mode change, so we handle it here, even
    // though it is not a special attribute for efficiency. Otherwise
    // an additional Set op would be required.
    Element attr_mode;
    if (ent->copyAttr("mode", attr_mode) == 0) {
        if (!attr_mode.isString()) {
            log(ERROR, "Non string mode set in Thing::MoveOperation");
        } else {
            // Update the mode
            setAttr("mode", attr_mode);
            if (m_motion) {
                m_motion->setMode(attr_mode.String());
            }
            mode = attr_mode.String();
        }
    }

    const double & current_time = BaseWorld::instance().getTime();

    auto transformsProp = requirePropertyClassFixed<TransformsProperty>();

    //We can only move if there's a domain
    Domain* domain = nullptr;
    if (m_location.m_loc) {
        domain = m_location.m_loc->getMovementDomain();
    }







    if (domain) {

        bool updatedTransform = false;

        // Update pos
        const auto& posVector = ent->getPos();
        if (posVector.size() == 3) {
            Vector3D translate;
            translate.fromAtlas(ent->getPosAsList());
            //Adjust the supplied position by the inverse of all external transformation.
            //This is to offset the fact that any client will send an update for position using
            //the position of the entity as it sees it.
            //TODO: is this really the best way? Should we allow for clients to specify if they want to set
            //the position independent of any transformations? Perhaps this is doable if the client
            //instead sends an update for the "transforms" property?
            for (auto entry : transformsProp->external()) {
                if (entry.second.translate.isValid()) {
                    translate -= entry.second.translate;
                }
            }
            if (m_location.bBox().isValid()) {
                for (auto entry : transformsProp->external()) {
                    if (entry.second.translateScaled.isValid()) {
                        auto size = m_location.bBox().highCorner() - m_location.bBox().lowCorner();
                        translate -= WFMath::Vector<3>(
                            entry.second.translateScaled.x() * size.x(),
                            entry.second.translateScaled.y() * size.y(),
                            entry.second.translateScaled.z() * size.z());
                    }
                }
            }

            transformsProp->getTranslate() = translate;
            updatedTransform = true;
        }


//        // FIXME Quick height hack
//        float height = domain->constrainHeight(*this, m_location.m_loc, m_location.pos(), mode);
//        //Translate height in relation to the standard translation as set in "transforms".
//        transformsProp->getTranslate().z() = height;

        Element attr_orientation;
        if (ent->copyAttr("orientation", attr_orientation) == 0) {
            // Update orientation
            Quaternion rotate;
            rotate.fromAtlas(attr_orientation.asList());

            //Adjust the supplied orientation by the inverse of all external transformation.
            //This is to offset the fact that any client will send an update for orientation using
            //the orientation of the entity as it sees it.
            //TODO: is this really the best way? Should we allow for clients to specify if they want to set
            //the position independent of any transformations? Perhaps this is doable if the client
            //instead sends an update for the "transforms" property?
            for (auto entry : transformsProp->external()) {
                if (entry.second.rotate.isValid()) {

                    Quaternion localRotation(entry.second.rotate.inverse());
                    //normalize to avoid drift
                    localRotation.normalize();
                    rotate = localRotation * rotate;
                }
            }

            transformsProp->getRotate() = rotate;
            updatedTransform = true;

        }

        if (updatedTransform) {
            transformsProp->apply(this);
            domain->applyTransform(*this, m_location.m_orientation, m_location.m_pos);
        }

        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            // Update velocity
            auto propelProp = requirePropertyClassFixed<PropelProperty>();
            fromStdVector(propelProp->data(), ent->getVelocity());
            //FIXME: For now set the velocity directly; in the future we want instead to only set the "propel" property,
            //and have the velocity being calculated by letting Bullet apply all forces.
            fromStdVector(m_location.m_velocity, ent->getVelocity());
            domain->setVelocity(*this, m_location.m_velocity);
            // Velocity is not persistent so has no flag
        }


        m_location.update(current_time);
        m_flags &= ~(entity_clean);

        // At this point the Location data for this entity has been updated.

        bool moving = false;

        if (m_location.velocity().isValid() &&
            m_location.velocity().sqrMag() > WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
            moving = true;
        }

        // Take into account terrain following etc.
        // Take into account mode also.
        // m_motion->adjustNewPostion();

        float update_time = consts::move_tick;

        if (moving) {
            //We've just started moving; create a motion instance.
            if (!m_motion) {
                m_motion = new Motion(*this);
            }


//            // If we are moving, check for collisions
//            update_time = m_motion->checkCollisions(*domain);
//
//            if (m_motion->collision()) {
//                if (update_time < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
//                    moving = m_motion->resolveCollision();
//                } else {
//                    m_motion->m_collisionTime = current_time + update_time;
//                }
//            }

            // Serial number must be changed regardless of whether we will use it
            ++m_motion->serialno();

            // If we are moving, schedule an update to track the movement
            debug(std::cout << "Move Update in " << update_time << std::endl << std::flush;);

            Update u;
            u->setFutureSeconds(update_time);
            u->setTo(getId());

            u->setRefno(m_motion->serialno());

            res.push_back(u);

        } else {
            if (m_motion) {
                //We moved previously, but have now stopped.

                delete m_motion;
                m_motion = nullptr;
            }
        }

        Operation m(op.copy());
        RootEntity marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
        assert(marg.isValid());
        m_location.addToEntity(marg);

        Sight s;
        s->setArgs1(m);

        res.push_back(s);


        // This code handles sending Appearance and Disappearance operations
        // to this entity and others to indicate if one has gained or lost
        // sight of the other because of this movement

        // FIXME Why only for a perceptive moving entity? Surely other entities
        // must gain/lose sight of this entity if it's moving?
        if (isPerceptive()) {
            checkVisibility(old_loc, res);
        }
    }

    //Check if we've moved between domains. First check if the location has changed, and if so
    //check if the domain also has changed.
    if (new_loc != 0) {
        if (old_loc.m_loc != m_location.m_loc && old_loc.m_loc) {
            auto domain_old = old_loc.m_loc->getMovementDomain();
            if (domain_old && domain_old != domain) {
                //Everything that saw us at the old domain should get a disappear op.
                //We shouldn't need to send disappear ops to ourselves though, since the top
                //level entity will have changed.
                //TODO: We can't use a broadcast op here, since the broadcast will look at the
                //location of the entity when it's processed, not when it's created. We should
                //alter this so that any op that's to be broadcast instead should include
                //the location data in the op itself.
                domain_old->processDisappearanceOfEntity(*this, old_loc, res);
            }
        }
    }
    m_seq++;

    onUpdated();
}

/// \brief Check changes in visibility of this entity
///
/// Check how this entity's position has changed since the last update
/// and how this has affected which entities it can see, and which can see
/// it. Return Appearance and Disappearance operations as required.
/// @param old_pos The coordinates of this entity before the update
/// @param res Resulting operations are returned here
void Thing::checkVisibility(const Location & old_loc, OpVector & res)
{
    if (m_location.m_loc) {
        auto domain = m_location.m_loc->getMovementDomain();
        if (domain) {
            domain->processVisibilityForMovedEntity(*this, old_loc, res);
        }
    }
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Set has no argument", res, getId());
        return;
    }
    const Root & ent = args.front();
    merge(ent->asMessage());
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
    m_seq++;
    if (~m_flags & entity_clean) {
        onUpdated();
    }

    //Send an update in case there are properties that were updated as a side effect of the merge
    //TODO: only do this if there actually are changes
    Update update;
    update->setTo(getId());
    res.push_back(update);
}

/// \brief Generate a Sight(Set) operation giving an update on named attributes
///
/// When another operation causes the properties of an entity to be changed,
/// it can trigger propagation of this change by sending an Update operation
/// nameing the attributes or properties that need to be updated. This
/// member function handles the Operation, sending a Sight(Set) for
/// any perceptible changes, and will in future handle persisting those
/// changes. Should this also handle side effects?
/// The main reason for this up is that if other ops need to generate a
/// Set op to update attributes, there are race conditions all over the
/// place.
/// @param op Update operation that notifies of the changes.
/// @param res The result of the operation is returned here.
void Thing::updateProperties(const Operation & op, OpVector & res)
{
    debug(std::cout << "Generating property update" << std::endl << std::flush;);

    bool updateContains = false;
    Anonymous set_arg;
    set_arg->setId(getId());


    bool hadChanges = false;

    for (auto entry : m_properties) {
        PropertyBase * prop = entry.second;
        assert(prop != 0);
        if (prop->flags() & flag_unsent) {
            debug(std::cout << "UPDATE:  " << flag_unsent << " " << entry.first
                            << std::endl << std::flush;);

            prop->add(entry.first, set_arg);
            prop->resetFlags(flag_unsent | per_clean);

            if (entry.first == "outfit" || entry.first == "right_hand_wield") {
                updateContains = true;
            }
            resetFlags(entity_clean);
            hadChanges = true;
            // FIXME Make sure we handle separately for private properties
        }
    }

    resetFlags(entity_clean);

    if (updateContains) {
        if (m_contains != nullptr) {

            //If the observed entity has a domain, let it decide child visibility.
            //Otherwise show all children.
            Domain* domain = getMovementDomain();
            if (domain) {
                domain->processVisibilityForMovedEntity(*this, m_location, res);
//                Atlas::Message::ListType containsList;
//                for (auto& entry : *m_contains) {
//                    if (domain->isEntityVisibleFor(*m_location.m_loc, *entry)) {
//                        containsList.push_back(entry->getId());
//                    }
//                }
//                set_arg->setAttr("contains", containsList);
            }
        }
    }

    if (hadChanges) {
        Set set;
        set->setTo(getId());
        set->setFrom(getId());
        set->setSeconds(op->getSeconds());
        set->setArgs1(set_arg);

        Sight sight;
        sight->setArgs1(set);
        res.push_back(sight);
    }


    //Location changes must be communicated through a Move op.
    if (m_flags & entity_dirty_location) {
        Move m;
        Anonymous move_arg;
        move_arg->setId(getId());
        m_location.addToEntity(move_arg);
        m->setArgs1(move_arg);
        m->setFrom(getId());
        m->setTo(getId());

        Sight s;
        s->setArgs1(m);

        res.push_back(s);
        resetFlags(entity_dirty_location);
        hadChanges = true;
    }


    //Only change sequence number and call onUpdated if something actually changed.
    if (hadChanges) {
        m_seq++;
        if (~m_flags & entity_clean) {
            onUpdated();
        }
    }
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
    // If it has no refno, then it is a generic request to broadcast
    // an update of some properties which have changed.
    if (op->isDefaultRefno()) {
        updateProperties(op, res);
        return;
    }

    // If LOC is null, this cannot be part of the world, or must be the
    // world itself, so should not be involved in any movement.
    if (m_location.m_loc == 0) {
        log(ERROR, String::compose("Updating %1(%2) when it is not in the world.",
                                   getType(), getId()));
        return;
    }

    // If it has a refno, then it is a movement update. If it does not
    // match the current movement serialno, then its obsolete, and can
    // be discarded.
    if (m_motion == nullptr || op->getRefno() != m_motion->serialno()) {
        return;
    }

    // If somehow a movement update arrives with the correct refno, but
    // we are not moving, then something has gone wrong.
    if (!m_location.velocity().isValid() ||
        m_location.velocity().sqrMag() < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
        log(ERROR, "Update got for entity not moving. " + describeEntity());
        return;
    }

    // This is where we will handle movement simulation from now on, rather
    // than in the mind interface. The details will be sorted by a new type
    // of object which will handle the specifics.

    const double & current_time = BaseWorld::instance().getTime();
    float time_diff = (float)(current_time - m_location.timeStamp());

    std::string mode;

    if (hasAttr("mode")) {
        Element mode_attr;
        getAttr("mode", mode_attr);
        if (mode_attr.isString()) {
            mode = mode_attr.String();
        } else {
            log(ERROR, String::compose("Mode on entity is a \"%1\" "
                                       "in Thing::UpdateOperation",
                                       Element::typeName(mode_attr.getType())));
        }
    }

    const Location old_loc = m_location;

    bool moving = true;

    // Check if a predicted collision is due.
//    if (m_motion->collision()) {
//        if (current_time >= m_motion->m_collisionTime) {
//            time_diff = (float)(m_motion->m_collisionTime - m_location.timeStamp());
//            // This flag signals that collision resolution is required later.
//            // Whether or not we are actually moving is determined by the
//            // collision resolution.
//            moving = false;
//        }
//    }
//
//    // Update entity position
//    auto transformsProp = requirePropertyClassFixed<TransformsProperty>();
//    transformsProp->getTranslate() += (m_location.velocity() * time_diff);
//
//    //We need to apply transforms here to figure our position in order to adjust height further down
//    transformsProp->apply(this);
//
//    // Collision resolution has to occur after position has been updated.
//    if (!moving) {
//        moving = m_motion->resolveCollision();
//    }
//
//    Domain* domain = nullptr;
//    // Adjust the position to world constraints - essentially fit
//    // to the terrain height at this stage.
//    // FIXME Get the constraints from the movement domain
//    if (m_location.m_loc) {
//        domain = m_location.m_loc->getMovementDomain();
//        if (domain) {
//            float z = domain->constrainHeight(*this, m_location.m_loc, m_location.pos(), "standing");
//            transformsProp->getTranslate().z() = z;
//            transformsProp->apply(this);
//        } else {
//
//        }
//    }
//    m_location.update(current_time);
//    m_flags &= ~entity_clean;
    float update_time = consts::move_tick;
//
//    if (moving && domain) {
//        // If we are moving, check for collisions
//        update_time = m_motion->checkCollisions(*domain);
//
//        if (m_motion->collision()) {
//            if (update_time < WFMath::numeric_constants<WFMath::CoordType>::epsilon()) {
//                moving = m_motion->resolveCollision();
//            } else {
//                m_motion->m_collisionTime = current_time + update_time;
//            }
//        }
//    }

    Move m;
    Anonymous move_arg;
    move_arg->setId(getId());
    m_location.addToEntity(move_arg);
    m->setArgs1(move_arg);
    m->setFrom(getId());
    m->setTo(getId());
    m->setSeconds(current_time);

    Sight s;
    s->setArgs1(m);

    res.push_back(s);

    if (moving) {
        debug(std::cout << "New Update in " << update_time << std::endl << std::flush;);

        Update u;
        u->setFutureSeconds(update_time);
        u->setTo(getId());

        // If the update op has no serial number, we need our own
        // ref number
        if (op->isDefaultSerialno()) {
            u->setRefno(++m_motion->serialno());
        } else {
            // We should respect the serial number if it is present
            // as the core code will set the reference number
            // correctly.
            m_motion->serialno() = op->getSerialno();
        }

        res.push_back(u);
    } else {
        delete m_motion;
        m_motion = nullptr;
    }

    // This code handles sending Appearance and Disappearance operations
    // to this entity and others to indicate if one has gained or lost
    // sight of the other because of this movement

    // FIXME Why only for a perceptive moving entity? Surely other entities
    // must gain/lose sight of this entity if it's moving?
    if (isPerceptive()) {
        checkVisibility(old_loc, res);
    }
    onUpdated();
}

bool Thing::lookAtEntity(const Operation & op, OpVector & res, const LocatedEntity* watcher) const {

    if (isVisibleForOtherEntity(watcher)) {
        generateSightOp(*watcher, op, res);
        return true;
    }
    return false;
}



void Thing::generateSightOp(const LocatedEntity& observingEntity, const Operation & originalLookOp, OpVector& res) const
{
    debug_print("Thing::generateSightOp() observer " << observingEntity.describeEntity() << " observed " << this->describeEntity());

    Sight s;

    Anonymous sarg;
    addToEntity(sarg);
    s->setArgs1(sarg);

    if (m_contains != nullptr) {

        //If the observed entity has a domain, let it decide child visibility.
        //Otherwise show all children.
        const Domain* observedEntityDomain = getMovementDomain();
        std::list<std::string> & contlist = sarg->modifyContains();
        if (observedEntityDomain) {
            contlist.clear();
            for (auto& entry : *m_contains) {
                if (observedEntityDomain->isEntityVisibleFor(observingEntity, *entry)) {
                    debug_print("child entity " << entry->describeEntity() << " of entity " << describeEntity() << " visible to observer " << observingEntity.describeEntity());
                    contlist.push_back(entry->getId());
                }
            }
        }
//            if (contlist.empty()) {
//                sarg->removeAttr("contains");
//            }
    }

    if (m_location.m_loc) {
        if (!m_location.m_loc->isVisibleForOtherEntity(&observingEntity)) {
            sarg->removeAttr("loc");
        }
    }

    s->setTo(originalLookOp->getFrom());
    if (!originalLookOp->isDefaultSerialno()) {
        s->setRefno(originalLookOp->getSerialno());
    }
    res.push_back(s);

}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
    LocatedEntity * from = BaseWorld::instance().getEntity(op->getFrom());
    if (from == nullptr) {
        log(ERROR, String::compose("Look op has invalid from %1. %2", op->getFrom(), describeEntity()));
        return;
    }
    // Register the entity with the world router as perceptive.
    BaseWorld::instance().addPerceptive(from);

    bool result = lookAtEntity(op, res, from);

    if (!result) {
        Unseen u;
        u->setTo(op->getFrom());
        u->setArgs(op->getArgs());
        if (!op->isDefaultSerialno()) {
            u->setRefno(op->getSerialno());
        }
        res.push_back(u);
    }
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
       return;
    }
    try {
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            error(op, "Entity to be created is malformed", res, getId());
            return;
        }
        const std::list<std::string> & parents = ent->getParents();
        if (parents.empty()) {
            error(op, "Entity to be created has empty parents", res, getId());
            return;
        }

        if (ent->hasAttr("transforms")) {
            ent->removeAttr("pos");
            ent->removeAttr("orientation");
        } else {
            Atlas::Message::MapType transforms;
            if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
                //Only copy x and y values; let terrain adjust z.
                transforms["translate"] = ent->getPosAsList();
                ent->removeAttr("pos");
            }
            Element orientation;
            if (ent->copyAttr("orientation", orientation) == 0) {
                transforms["rotate"] = orientation;
                ent->removeAttr("orientation");
            }
            ent->setAttr("transforms", transforms);
        }



        //If there's no location set we'll use the same one as the current entity.
        if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) &&
            (m_location.m_loc != 0)) {
            ent->setLoc(m_location.m_loc->getId());
            if (!ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
                //Don't actually set the pos; instead set the transform.
                //We don't allow external clients to directly set the pos.
                if (!ent->hasAttr("transforms")) {
                    Atlas::Message::MapType transforms;
                    //Only copy x and y values; let terrain adjust z.
                    transforms["translate"] = Vector3D(m_location.pos().x(), m_location.pos().y(), 0).toAtlas();
                    ent->setAttr("transforms", transforms);
                }
            }
        }
        const std::string & type = parents.front();
        debug( std::cout << getId() << " creating " << type;);

        LocatedEntity * obj = BaseWorld::instance().addNewEntity(type, ent);

        if (obj == 0) {
            error(op, "Create op failed.", res, op->getFrom());
            return;
        }

        Anonymous new_ent;
        obj->addToEntity(new_ent);

        if (!op->isDefaultSerialno()) {
            log(NOTICE, "Sending create response");

            Info i;
            i->setArgs1(new_ent);
            i->setTo(op->getFrom());
            res.push_back(i);
        }

        Operation c(op.copy());
        c->setArgs1(new_ent);

        Sight s;
        s->setArgs1(c);
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}

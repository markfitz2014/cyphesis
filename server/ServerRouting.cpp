#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/DecoderBase.h>

#include "ServerRouting.h"


ServerRouting::ServerRouting(CommServer * server, char * name) : svr_name(name)
{
    //ServerRouting::base_init(kw);
    comm_server=server ; //communication server;
    id_dict[id] = this;
    world=new WorldRouter(this); //game world;
    //obj=add_object(persistence.load_admin_account());
    //obj.server=self;
    //obj.world=ServerRouting::world;
}

BaseEntity * ServerRouting::add_object(BaseEntity * obj, bad_type ent=None) {
    obj=Routing::add_object(obj, ent);
    id_dict.insert(idpair_t(obj->id, obj));
    return obj;
}

void save(char * filename)
{
    //persistence.save_meta(self, filename);
}

void load(char * filename)
{
    //persistence.load_meta(self, filename);
}

int ServerRouting::idle() {
    //return world.idle();
    return(0);
}

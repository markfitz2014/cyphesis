// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAttachProperty_custom.h file.

#ifndef STUB_RULESETS_ATTACHPROPERTY_H
#define STUB_RULESETS_ATTACHPROPERTY_H

#include "rulesets/AttachProperty.h"
#include "stubAttachProperty_custom.h"

#ifndef STUB_AttachProperty_AttachProperty
//#define STUB_AttachProperty_AttachProperty
   AttachProperty::AttachProperty(std::uint32_t flags )
    : EntityProperty(flags)
  {
    
  }
#endif //STUB_AttachProperty_AttachProperty

#ifndef STUB_AttachProperty_install
//#define STUB_AttachProperty_install
  void AttachProperty::install(LocatedEntity *, const std::string &)
  {
    
  }
#endif //STUB_AttachProperty_install

#ifndef STUB_AttachProperty_install
//#define STUB_AttachProperty_install
  void AttachProperty::install(TypeNode *, const std::string &)
  {
    
  }
#endif //STUB_AttachProperty_install

#ifndef STUB_AttachProperty_remove
//#define STUB_AttachProperty_remove
  void AttachProperty::remove(LocatedEntity *, const std::string & name)
  {
    
  }
#endif //STUB_AttachProperty_remove

#ifndef STUB_AttachProperty_apply
//#define STUB_AttachProperty_apply
  void AttachProperty::apply(LocatedEntity *)
  {
    
  }
#endif //STUB_AttachProperty_apply

#ifndef STUB_AttachProperty_operation
//#define STUB_AttachProperty_operation
  HandlerResult AttachProperty::operation(LocatedEntity *, const Operation &, OpVector &)
  {
    return *static_cast<HandlerResult*>(nullptr);
  }
#endif //STUB_AttachProperty_operation

#ifndef STUB_AttachProperty_copy
//#define STUB_AttachProperty_copy
  AttachProperty* AttachProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_AttachProperty_copy

#ifndef STUB_AttachProperty_is_ancestor_to
//#define STUB_AttachProperty_is_ancestor_to
   bool AttachProperty::is_ancestor_to(LocatedEntity* child, LocatedEntity* ancestor)
  {
    return false;
  }
#endif //STUB_AttachProperty_is_ancestor_to


#endif
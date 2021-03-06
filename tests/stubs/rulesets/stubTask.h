// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubTask_custom.h file.

#ifndef STUB_RULESETS_TASK_H
#define STUB_RULESETS_TASK_H

#include "rulesets/Task.h"
#include "stubTask_custom.h"

#ifndef STUB_Task_Task
//#define STUB_Task_Task
   Task::Task(LocatedEntity & owner)
    : m_script(nullptr)
  {
    
  }
#endif //STUB_Task_Task

#ifndef STUB_Task_Task_DTOR
//#define STUB_Task_Task_DTOR
   Task::~Task()
  {
    
  }
#endif //STUB_Task_Task_DTOR

#ifndef STUB_Task_irrelevant
//#define STUB_Task_irrelevant
  void Task::irrelevant()
  {
    
  }
#endif //STUB_Task_irrelevant

#ifndef STUB_Task_initTask
//#define STUB_Task_initTask
  void Task::initTask(const Operation & op, OpVector & res)
  {
    
  }
#endif //STUB_Task_initTask

#ifndef STUB_Task_operation
//#define STUB_Task_operation
  void Task::operation(const Operation & op, OpVector & res)
  {
    
  }
#endif //STUB_Task_operation

#ifndef STUB_Task_nextTick
//#define STUB_Task_nextTick
  Operation Task::nextTick(double interval)
  {
    return *static_cast<Operation*>(nullptr);
  }
#endif //STUB_Task_nextTick

#ifndef STUB_Task_setScript
//#define STUB_Task_setScript
  void Task::setScript(Script * scrpt)
  {
    
  }
#endif //STUB_Task_setScript

#ifndef STUB_Task_getAttr
//#define STUB_Task_getAttr
  int Task::getAttr(const std::string & attr, Atlas::Message::Element & val) const
  {
    return 0;
  }
#endif //STUB_Task_getAttr

#ifndef STUB_Task_setAttr
//#define STUB_Task_setAttr
  void Task::setAttr(const std::string & attr, const Atlas::Message::Element & val)
  {
    
  }
#endif //STUB_Task_setAttr


#endif
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_PROPERTY_H
#define COMMON_PROPERTY_H

#include <Atlas/Message/Element.h>

class PropertyBase {
  protected:
    const unsigned int m_flags;
  public:
    explicit PropertyBase(unsigned int);
    virtual ~PropertyBase();

    unsigned int flags() const { return m_flags; }

    virtual void get(Atlas::Message::Element &) = 0;
    virtual void set(const Atlas::Message::Element &) = 0;
};

template <typename T>
class Property : public PropertyBase {
  protected:
    T & m_data;
  public:
    explicit Property(T & data, unsigned int flags);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
};

// FIXME also needs to handle the setting of flags etc.

#endif // COMMON_PROPERTY_H

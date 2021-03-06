/* -*- coding: utf-8 -*-
 * -----------------------------------------------------------------------------
 * Copyright © 2016-2017, HST Project.
 * Please see the COPYING file in this distribution for license details.
 * -----------------------------------------------------------------------------
 */

#include "hst/event.h"

#include <algorithm>
#include <map>
#include <ostream>
#include <vector>

#include "hst/hash.h"

namespace hst {

using std::map;
using std::string;

class Event::Table {
  public:
    Table() = default;

    map<Event::Index, const string*> names;
    map<const string, Event::Index> indices;
    Event::Index next_index = 1;
};

std::unique_ptr<Event::Table> Event::table_;

Event::Index
Event::find_or_create_event(const string& name)
{
    if (!table_) {
        table_ = std::unique_ptr<Event::Table>(new Event::Table);
    }

    Index& index = table_->indices[name];
    if (index == 0) {
        // This is a new name.  Create an event index for it and stash that
        // away.
        index = table_->next_index++;

        // Find the copy of the name inside of the table so that we can stash
        // that in the reverse table.
        const string& saved_name = table_->indices.find(name)->first;
        table_->names[index] = &saved_name;
    }

    return index;
}

const string& Event::name() const
{
    return *table_->names[index_];
}

std::ostream& operator<<(std::ostream& out, const Event& event)
{
    return out << event.name();
}

std::size_t
Event::Set::hash() const
{
    static hash_scope scope;
    hst::hasher hash(scope);
    std::vector<Event> sorted(begin(), end());
    std::sort(sorted.begin(), sorted.end());
    for (const Event event : sorted) {
        hash.add(event);
    }
    return hash.value();
}

std::ostream& operator<<(std::ostream& out, const Event::Set& events)
{
    bool first = true;
    out << "{";
    for (auto event : events) {
        if (first) {
            first = false;
        } else {
            out << ", ";
        }
        out << event;
    }
    return out << "}";
}

}  // namespace hst

/* -*- coding: utf-8 -*-
 * -----------------------------------------------------------------------------
 * Copyright © 2017, HST Project.
 * Please see the COPYING file in this distribution for license details.
 * -----------------------------------------------------------------------------
 */

#include "hst/process.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "hst/hash.h"

namespace hst {

void
Process::initials(Event::Set* out) const
{
    initials([&out](Event event) { out->insert(event); });
}

void
Process::afters(Event initial, Process::Set* out) const
{
    afters(initial, [&out](const Process& process) { out->insert(&process); });
}

void
Process::subprocesses(Process::Set* out) const
{
    subprocesses([&out](const Process& process) { out->insert(&process); });
}

void
NormalizedProcess::afters(Event initial,
                          std::function<void(const Process&)> op) const
{
    const NormalizedProcess* process = after(initial);
    if (process) {
        op(*process);
    }
}

std::size_t
Process::Bag::hash() const
{
    static hash_scope scope;
    hst::hasher hash(scope);
    std::vector<const Process*> sorted(begin(), end());
    std::sort(sorted.begin(), sorted.end());
    for (const Process* process : sorted) {
        hash.add(*process);
    }
    return hash.value();
}

std::ostream& operator<<(std::ostream& out, const Process::Bag& processes)
{
    // We want reproducible output, so we sort the processes in the set before
    // rendering them into the stream.  We the process's index to print out the
    // processes in the order that they were defined.
    std::vector<const Process*> sorted_processes(processes.begin(),
                                                 processes.end());
    std::sort(sorted_processes.begin(), sorted_processes.end(),
              [](const Process* p1, const Process* p2) {
                  return p1->index() < p2->index();
              });

    bool first = true;
    out << "{";
    for (const Process* process : sorted_processes) {
        if (first) {
            first = false;
        } else {
            out << ", ";
        }
        out << *process;
    }
    return out << "}";
}

std::size_t
Process::Set::hash() const
{
    static hash_scope scope;
    hst::hasher hash(scope);
    std::vector<const Process*> sorted(begin(), end());
    std::sort(sorted.begin(), sorted.end());
    for (const Process* process : sorted) {
        hash.add(*process);
    }
    return hash.value();
}

void
Process::Set::tau_close()
{
    Event tau = Event::tau();
    while (true) {
        Process::Set new_processes;
        for (const Process* process : *this) {
            process->afters(tau, &new_processes);
        }
        size_type old_size = size();
        insert(new_processes.begin(), new_processes.end());
        size_type new_size = size();
        if (old_size == new_size) {
            return;
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Process::Set& processes)
{
    // We want reproducible output, so we sort the processes in the set before
    // rendering them into the stream.  We the process's index to print out the
    // processes in the order that they were defined.
    std::vector<const Process*> sorted_processes(processes.begin(),
                                                 processes.end());
    std::sort(sorted_processes.begin(), sorted_processes.end(),
              [](const Process* p1, const Process* p2) {
                  return p1->index() < p2->index();
              });

    bool first = true;
    out << "{";
    for (const Process* process : sorted_processes) {
        if (first) {
            first = false;
        } else {
            out << ", ";
        }
        out << *process;
    }
    return out << "}";
}

}  // namespace hst

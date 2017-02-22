/* -*- coding: utf-8 -*-
 * -----------------------------------------------------------------------------
 * Copyright © 2017, HST Project.
 * Please see the COPYING file in this distribution for license details.
 * -----------------------------------------------------------------------------
 */

#include "hst/operators.h"

#include <memory>
#include <ostream>

#include "hst/event.h"
#include "hst/hash.h"
#include "hst/process.h"

namespace hst {

class Prefix : public Process {
  public:
    Prefix(Event a, std::shared_ptr<Process> p) : a_(a), p_(std::move(p)) {}
    void initials(Event::Set* out) override;
    void afters(Event initial, Process::Set* out) override;

    std::size_t hash() const override;
    bool operator==(const Process& other) const override;
    unsigned int precedence() const override { return 1; }
    void print(std::ostream& out) const override;

  private:
    Event a_;
    std::shared_ptr<Process> p_;
};

std::shared_ptr<Process>
prefix(Event a, std::shared_ptr<Process> p)
{
    return std::make_shared<Prefix>(a, std::move(p));
}

// Operational semantics for a → P
//
// 1) ─────────────
//     a → P -a→ P

void
Prefix::initials(std::set<Event>* out)
{
    // initials(a → P) = {a}
    out->insert(a_);
}

void
Prefix::afters(Event initial, Process::Set* out)
{
    // afters(a → P, a) = P
    if (initial == a_) {
        out->insert(p_);
    }
}

std::size_t
Prefix::hash() const
{
    static hash_scope prefix;
    return hasher(prefix).add(a_).add(*p_).value();
}

bool
Prefix::operator==(const Process& other_) const
{
    const Prefix* other = dynamic_cast<const Prefix*>(&other_);
    if (other == nullptr) {
        return false;
    }
    return a_ == other->a_ && *p_ == *other->p_;
}

void
Prefix::print(std::ostream& out) const
{
    out << a_ << " → ";
    print_subprocess(out, *p_);
}

}  // namespace hst

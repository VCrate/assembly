#pragma once

#include <vcrate/Alias.hpp>
#include <vcrate/vasm/lexer/Position.hpp>

#include <variant>
#include <vector>

namespace vcrate::vasm::lexer {

struct LeftOverlap;     // [0, 2] overlap? [1, 3]       => LeftOverlap [1, 2]
struct RightOverlap;    // [1, 3] overlap? [0, 2]       => RightOverlap [1, 2]
struct IncludeOverlap;  // [0, 3] overlap? [0, 2]       => IncludeOverlap [0, 2]
struct FullOverlap;     // [0, 2] overlap? [0, 3]       => FullOverlap [0, 2]
struct Equal;           // [0, 2] overlap? [0, 2]       => Equal
struct LeftNoOverlap;   // [0, 1] overlap? [2, 3]       => LeftNoOverlap
struct RightNoOverlap;  // [2, 3] overlap? [0, 1]       => RightNoOverlap

using Overlap = std::variant<LeftOverlap, RightOverlap, IncludeOverlap, FullOverlap, Equal, LeftNoOverlap, RightNoOverlap>;

struct Location : Position {

    ui32 lenght;

    Overlap get_overlaping(Location const& other) const;

    Position first() const;
    Position after_last() const;

};

struct ScatteredLocation {

    void extends(Location const& location);
    void extends(ScatteredLocation const& scattered_location);

    std::vector<Location> locations;

};

struct LeftOverlap : Location {};
struct RightOverlap : Location {};
struct IncludeOverlap : Location {};
struct FullOverlap : Location {};
struct Equal {};
struct LeftNoOverlap {};
struct RightNoOverlap {};


}
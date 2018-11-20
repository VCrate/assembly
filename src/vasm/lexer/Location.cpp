#include <vcrate/vasm/lexer/Location.hpp>

#include <algorithm>

namespace vcrate::vasm::lexer {

Position Location::first() const {
    return static_cast<Position>(*this);
}

Position Location::after_last() const {
    return Position{ line, character + lenght };
}

Overlap Location::get_overlaping(Location const& other) const {
    if (line < other.line) {
        return LeftNoOverlap{};
    }

    if (line > other.line) {
        return RightNoOverlap{};
    }

    auto first1 = first().character;
    auto first2 = other.first().character;
    auto last1 = after_last().character;
    auto last2 = other.after_last().character;

    if (last1 <= first2) {
        return LeftNoOverlap{};
    }

    if (first1 >= last2) {
        return RightNoOverlap{};
    }

    if (first1 == first2 && last1 == last2) {
        return Equal{};
    }

    if (first1 >= first2 && last1 <= last2) {
    }

    if (first1 <= first2 && last1 >= last2) {
        return FullOverlap{ other };
    }

    if (first1 > first2) {
        return LeftOverlap{{ first(), last2 - first1 }};
    }

    // ALWAYS TRUE
    //if (first1 < first2) { 
        return RightOverlap{{ other.first(), last1 - first2 }};
    //}

}


Location merge(Location const& lhs, Location const& rhs, Overlap const& overlap) {
    if (std::holds_alternative<LeftOverlap>(overlap)) {
        return lexer::Location{ rhs.first(), rhs.lenght + lhs.lenght - std::get<LeftOverlap>(overlap).lenght };
    }

    else if (std::holds_alternative<RightOverlap>(overlap)) {
        return lexer::Location{ lhs.first(), rhs.lenght + lhs.lenght - std::get<RightOverlap>(overlap).lenght };
    }

    else if (std::holds_alternative<FullOverlap>(overlap)) {
        return rhs;
    }

    else if (std::holds_alternative<RightNoOverlap>(overlap)) {
        if (rhs.line == lhs.line && rhs.character + rhs.lenght == lhs.character) {
            return lexer::Location{ rhs.first(), rhs.lenght + lhs.lenght };
        }
    }

    else if (std::holds_alternative<LeftNoOverlap>(overlap)) {
        if (rhs.line == lhs.line && lhs.character + lhs.lenght == rhs.character) {
            return lexer::Location{ lhs.first(), rhs.lenght + lhs.lenght };
        }
    }


    return lhs;
}

void ScatteredLocation::extends(ScatteredLocation const& scattered_location) {
    for(auto const& location : scattered_location.locations) {
        extends(location);
    } 
}

void ScatteredLocation::extends(Location const& location) {
    if (locations.empty()) {
        locations.push_back(location);
        return;
    }
    Location merged = location;

    int first{ static_cast<int>(locations.size()) - 1 };
    int last{ 0 };

    for(std::size_t i{0}; i < locations.size(); ++i) {
        auto overlap = merged.get_overlaping(locations[i]);
        
        if (std::holds_alternative<Equal>(overlap)) {
            return;
        }

        else if (std::holds_alternative<RightNoOverlap>(overlap)) {
            if (i == locations.size() - 1) {
                locations.push_back(merged);
                return;
            } else {
                if (locations[i].line == merged.line && locations[i].character + locations[i].lenght == merged.character) {
                    first = std::min<int>(first, i);
                    last = std::max<int>(last, i + 1);
                    merged = merge(merged, locations[i], overlap);
                }
            }
        }

        else if (std::holds_alternative<LeftNoOverlap>(overlap)) {
            if (i == 0) {
                locations.insert(std::begin(locations), merged);
                return;
            } else {
                if (locations[i].line == merged.line && merged.character + merged.lenght == locations[i].character) {
                    first = std::min<int>(first, i);
                    last = std::max<int>(last, i + 1);
                    merged = merge(merged, locations[i], overlap);
                }
            }
        }

        else if (std::holds_alternative<LeftOverlap>(overlap)) {
            first = std::min<int>(first, i);
            last = std::max<int>(last, i + 1);

            merged = merge(merged, locations[i], overlap);
        }

        else if (std::holds_alternative<RightOverlap>(overlap)) {
            first = std::min<int>(first, i);
            last = std::max<int>(last, i + 1);

            merged = merge(merged, locations[i], overlap);
        }

        else if (std::holds_alternative<lexer::FullOverlap>(overlap)) {
            first = std::min<int>(first, i);
            last = std::max<int>(last, i + 1);
        }

        else if (std::holds_alternative<lexer::IncludeOverlap>(overlap)) {
            return;
        }
    }

    locations.erase(std::begin(locations) + first, std::begin(locations) + last);
    locations.insert(std::begin(locations) + first, merged);
}

}
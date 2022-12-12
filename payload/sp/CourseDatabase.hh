#pragma once

#include <Common.hh>

namespace SP {

class CourseDatabase {
public:
    struct Filter {
        bool race;
        bool battle;
    };

    struct Entry {
        u32 databaseId;
        bool race;
        bool battle;
        u32 courseId;
        u32 timestamp;
    };

    u32 count(Filter filter);
    const Entry &entry(Filter filter, u32 index);

    static CourseDatabase &Instance();

private:
    CourseDatabase();

    void refresh(Filter filter);

    std::array<Entry, 42> m_entries{
        Entry {3409, false,  true, 33, 32},
        Entry {3410, false,  true, 34, 35},
        Entry {3412, false,  true, 38, 41},
        Entry {3413, false,  true, 32, 33},
        Entry {3415, false,  true, 35, 34},
        Entry {3416, false,  true, 40, 38},
        Entry {3417, false,  true, 37, 40},
        Entry {3418, false,  true, 41, 39},
        Entry {3419, false,  true, 39, 37},
        Entry {3420, false,  true, 36, 36},
        Entry {3422,  true, false, 12, 14},
        Entry {3424,  true, false,  5,  5},
        Entry {3426,  true, false,  6,  6},
        Entry {3427,  true, false, 23, 22},
        Entry {3428,  true, false, 21, 24},
        Entry {3429,  true, false, 22, 29},
        Entry {3430,  true, false, 20, 17},
        Entry {3431,  true, false,  9,  8},
        Entry {3433,  true, false, 14, 12},
        Entry {3435,  true, false, 30, 25},
        Entry {3436,  true, false, 31, 21},
        Entry {3437,  true, false, 19, 30},
        Entry {3438,  true, false, 17, 27},
        Entry {3439,  true, false, 16, 16},
        Entry {3440,  true, false, 18, 23},
        Entry {3441,  true, false,  3, 11},
        Entry {3443,  true, false, 15,  9},
        Entry {3445,  true, false,  8,  0},
        Entry {3447,  true, false, 11, 10},
        Entry {3448,  true, false,  0,  4},
        Entry {3450,  true, false,  1,  1},
        Entry {3452,  true, false, 10, 13},
        Entry {3454,  true, false,  2,  2},
        Entry {3456,  true, false, 28, 31},
        Entry {3457,  true, false, 29, 26},
        Entry {3458,  true, false, 26, 19},
        Entry {3459,  true, false, 27, 20},
        Entry {3460,  true, false, 13, 15},
        Entry {3462,  true, false, 25, 18},
        Entry {3463,  true, false, 24, 28},
        Entry {3464,  true, false,  4,  3},
        Entry {3466,  true, false,  7,  7},
    };
    Filter m_filter{false, false};
    u32 m_count = 0;
    std::array<u32, 42> m_internalIndices;
};


} // namespace SP

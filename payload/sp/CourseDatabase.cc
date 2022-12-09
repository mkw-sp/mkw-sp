#include "CourseDatabase.hh"

/*namespace SP::CourseDatabase {

static const Entry entries[42] = {
    { 3409, false, true, 33, 32 },
};

static Filter filter{false, false};
static u32 count = 0;
static u32 databaseIds[42];

u32 Count(Filter filter) {
    return count;
}

const Entry &Entry(Filter filter, u32 index) {

}

} // namespace SP::CourseDatabase
*/

namespace SP {

u32 CourseDatabase::count(Filter filter) {
    refresh(filter);

    return m_count;
}

const CourseDatabase::Entry &CourseDatabase::entry(Filter filter, u32 index) {
    refresh(filter);

    return m_entries[m_internalIndices[index]];
}

CourseDatabase &CourseDatabase::Instance() {
    static CourseDatabase courseDatabase;
    return courseDatabase;
}

CourseDatabase::CourseDatabase() = default;

void CourseDatabase::refresh(Filter filter) {
    if (filter.race == m_filter.race && filter.battle == m_filter.battle) {
        return;
    }

    m_filter = filter;
    m_count = 0;
    for (u32 internalIndex = 0; internalIndex < std::size(m_entries); internalIndex++) {
        if (!((m_filter.race && m_entries[internalIndex].race) ||
                (m_filter.battle && m_entries[internalIndex].battle))) {
            continue;
        }

        m_internalIndices[m_count++] = internalIndex;
    }
}

} // namespace SP

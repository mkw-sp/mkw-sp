#include "CourseDatabase.hh"

#include <algorithm>

namespace SP {

u32 CourseDatabase::totalCount() {
    return m_internalIndices.size();
}

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

    std::sort(m_internalIndices.begin(), m_internalIndices.begin() + m_count,
            [&](auto i0, auto i1) { return m_entries[i0].timestamp < m_entries[i1].timestamp; });
}

} // namespace SP

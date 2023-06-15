#include "MultiDvdArchive.hh"

extern "C" {
#include <revolution.h>
#include <stdio.h>
}

namespace System {

static const char *getBaseLanguageCode() {
    switch (REGION) {
    case REGION_P:
        return "E";
    case REGION_E:
        return "U";
    case REGION_J:
        return "J";
    case REGION_K:
        return "K";
    default:
        assert(false);
    }
}

static const char *getLanguageCode() {
    switch (SCGetLanguage()) {
    case SC_LANG_JAPANESE:
        return "J";
    case SC_LANG_ENGLISH:
        return REGION == REGION_E ? "U" : "E";
    case SC_LANG_SIMP_CHINESE:
    case SC_LANG_TRAD_CHINESE:
        return "E";
    case SC_LANG_GERMAN:
        return "G";
    case SC_LANG_FRENCH:
        return REGION == REGION_E ? "Q" : "F";
    case SC_LANG_SPANISH:
        return REGION == REGION_E ? "M" : "S";
    case SC_LANG_ITALIAN:
        return "I";
    case SC_LANG_DUTCH:
        return "N";
    case SC_LANG_KOREAN:
        return "K";
    default:
        assert(false);
    }
}

void MultiDvdArchive::setMission(u32 missionId) {
    m_formats[1] = Format::Single;
    snprintf(m_names[1], 0x80, "Race/MissionRun/mr%02d.szs", missionId);
}

void MultiDvdArchive::init() {
    REPLACED(init)();

    if (m_archiveCount <= 1) {
        return;
    }

    snprintf(m_names[1], 0x80, "_Dif.szs");
    m_formats[1] = Format::Double;
}

DvdArchive &MultiDvdArchive::archive(u16 i) {
    assert(i < m_archiveCount);
    return m_archives[i];
}

u16 MultiDvdArchive::count() const {
    return m_archiveCount;
}

MultiDvdArchive *MultiDvdArchive::Create(ResChannelId type) {
    switch (type) {
    case ResChannelId::Race:
        return new RaceMultiDvdArchive;
    case ResChannelId::Course:
        return new CourseMultiDvdArchive;
    case ResChannelId::Menu:
        return new MenuMultiDvdArchive;
    case ResChannelId::Font:
        return new FontMultiDvdArchive;
    default:
        MultiDvdArchive *archive = new MultiDvdArchive(2);
        archive->init();
        return archive;
    }
}

RaceMultiDvdArchive::RaceMultiDvdArchive() : MultiDvdArchive(5) {
    init();
}

RaceMultiDvdArchive::~RaceMultiDvdArchive() = default;

void RaceMultiDvdArchive::init() {
    snprintf(m_names[0], 0x80, ".szs");

    if (SCGetLanguage() == SC_LANG_KOREAN) {
        snprintf(m_names[1], 0x80, "_J.szs");
    } else {
        snprintf(m_names[1], 0x80, "_%s.szs", getLanguageCode());
    }

    // New archives
    snprintf(m_names[2], 0x80, "SP.szs");
    snprintf(m_names[3], 0x80, "_Dif.szs");
    snprintf(m_names[4], 0x80, "_%s_Dif.szs", getLanguageCode());

    for (size_t i = 0; i < 5; i++) {
        m_formats[i] = Format::Double;
    }
}

MenuMultiDvdArchive::MenuMultiDvdArchive() : MultiDvdArchive(6) {
    init();
}

MenuMultiDvdArchive::~MenuMultiDvdArchive() = default;

void MenuMultiDvdArchive::init() {
    if (REGION == REGION_K) {
        snprintf(m_names[0], 0x80, "_R.szs");
    } else {
        snprintf(m_names[0], 0x80, ".szs");
    }

    snprintf(m_names[1], 0x80, "_%s.szs", getBaseLanguageCode());
    snprintf(m_names[2], 0x80, "SP.szs");
    snprintf(m_names[3], 0x80, "SP_%s.szs", getLanguageCode());
    snprintf(m_names[4], 0x80, "_Dif.szs");
    snprintf(m_names[5], 0x80, "_%s_Dif.szs", getLanguageCode());

    for (size_t i = 0; i < 6; i++) {
        m_formats[i] = Format::Double;
    }
}

FontMultiDvdArchive::FontMultiDvdArchive() : MultiDvdArchive(3) {
    init();
}

FontMultiDvdArchive::~FontMultiDvdArchive() = default;

void FontMultiDvdArchive::init() {
    if (REGION == REGION_K) {
        snprintf(m_names[0], 0x80, "/Scene/UI/Font_K.szs");
        snprintf(m_names[1], 0x80, "/Scene/UI/FontSP_R.szs");
    } else {
        snprintf(m_names[0], 0x80, "/Scene/UI/Font.szs");
        snprintf(m_names[1], 0x80, "/Scene/UI/FontSP_K.szs");
    }
    snprintf(m_names[2], 0x80, "/Scene/UI/Font_Dif.szs");

    for (size_t i = 0; i < 3; i++) {
        m_formats[i] = Format::Single;
    }
}

} // namespace System

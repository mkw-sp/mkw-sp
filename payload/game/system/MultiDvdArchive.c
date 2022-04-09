#include "MultiDvdArchive.h"

#include <stdio.h>
#include <string.h>

#include <revolution.h>

enum {
    RESOURCE_KIND_FILE_DOUBLE_FORMAT = 0x0,
    RESOURCE_KIND_FILE_SINGLE_FORMAT = 0x1,
};

static const char *getBaseLanguageCode(void) {
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

static const char *getLanguageCode(void) {
    switch (SCGetLanguage()) {
    case SC_LANG_JAPANESE:
        return "J";
    case SC_LANG_ENGLISH:
        if (REGION == REGION_E) {
            return "U";
        }
        return "E";
    case SC_LANG_GERMAN:
        return "G";
    case SC_LANG_FRENCH:
        if (REGION == REGION_E) {
            return "Q";
        }
        return "F";
    case SC_LANG_SPANISH:
        if (REGION == REGION_E) {
            return "M";
        }
        return "S";
    case SC_LANG_ITALIAN:
        return "I";
    case SC_LANG_DUTCH:
        return "N";
    case SC_LANG_SIMP_CHINESE:
        return "E";
    case SC_LANG_TRAD_CHINESE:
        return "E";
    case SC_LANG_KOREAN:
        return "K";
    default:
        assert(false);
    }
}



MultiDvdArchive *MultiDvdArchive_ct(MultiDvdArchive *this, u32 archiveCount);

void MultiDvdArchive_dt(MultiDvdArchive *this, s32 type);

void MultiDvdArchive_init(MultiDvdArchive *this);

static void my_MultiDvdArchive_init(MultiDvdArchive *this) {
    snprintf(this->names[0], 0x80, ".szs");
    snprintf(this->names[1], 0x80, "_Dif.szs");

    for (u32 i = 0; i < 2; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    }
}
PATCH_B(MultiDvdArchive_init, my_MultiDvdArchive_init);



static const MultiDvdArchive_vt s_RaceMultiDvdArchive_vt;

static RaceMultiDvdArchive *RaceMultiDvdArchive_ct(RaceMultiDvdArchive *this) {
    MultiDvdArchive_ct(this, 4);
    this->vt = &s_RaceMultiDvdArchive_vt;

    this->vt->init(this);

    return this;
}

static void RaceMultiDvdArchive_init(MultiDvdArchive *base) {
    RaceMultiDvdArchive *this = (RaceMultiDvdArchive *)base;

    snprintf(this->names[0], 0x80, ".szs");
    if (SCGetLanguage() == SC_LANG_KOREAN) {
        snprintf(this->names[1], 0x80, "_J.szs");
    } else {
        snprintf(this->names[1], 0x80, "_%s.szs", getLanguageCode());
    }
    snprintf(this->names[2], 0x80, "_Dif.szs");
    snprintf(this->names[3], 0x80, "_%s_Dif.szs", getLanguageCode());

    for (u32 i = 0; i < 4; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    }
}

static const MultiDvdArchive_vt s_RaceMultiDvdArchive_vt = {
    .dt = MultiDvdArchive_dt,
    .init = RaceMultiDvdArchive_init,
};



CourseMultiDvdArchive *CourseMultiDvdArchive_ct(CourseMultiDvdArchive *this);



static const MultiDvdArchive_vt s_MenuMultiDvdArchive_vt;

static MenuMultiDvdArchive *MenuMultiDvdArchive_ct(MenuMultiDvdArchive *this) {
    MultiDvdArchive_ct(this, 6);
    this->vt = &s_MenuMultiDvdArchive_vt;

    this->vt->init(this);

    return this;
}

static void MenuMultiDvdArchive_init(MultiDvdArchive *base) {
    MenuMultiDvdArchive *this = (MenuMultiDvdArchive *)base;

    if (REGION == REGION_K) {
        snprintf(this->names[0], 0x80, "_R.szs");
    } else {
        snprintf(this->names[0], 0x80, ".szs");
    }
    snprintf(this->names[1], 0x80, "_%s.szs", getBaseLanguageCode());
    snprintf(this->names[2], 0x80, "SP.szs");
    snprintf(this->names[3], 0x80, "SP_%s.szs", getLanguageCode());
    snprintf(this->names[4], 0x80, "_Dif.szs");
    snprintf(this->names[5], 0x80, "_%s_Dif.szs", getLanguageCode());

    for (u32 i = 0; i < 6; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    }
}

static const MultiDvdArchive_vt s_MenuMultiDvdArchive_vt = {
    .dt = MultiDvdArchive_dt,
    .init = MenuMultiDvdArchive_init,
};



static const MultiDvdArchive_vt s_FontMultiDvdArchive_vt;

static FontMultiDvdArchive *FontMultiDvdArchive_ct(FontMultiDvdArchive *this) {
    MultiDvdArchive_ct(this, 3);
    this->vt = &s_FontMultiDvdArchive_vt;

    this->vt->init(this);

    return this;
}

static void FontMultiDvdArchive_init(MultiDvdArchive *base) {
    FontMultiDvdArchive *this = (FontMultiDvdArchive *)base;

    if (REGION == REGION_K) {
        snprintf(this->names[0], 0x80, "/Scene/UI/Font_K.szs");
        snprintf(this->names[1], 0x80, "/Scene/UI/FontSP_R.szs");
    } else {
        snprintf(this->names[0], 0x80, "/Scene/UI/Font.szs");
        snprintf(this->names[1], 0x80, "/Scene/UI/FontSP_K.szs");
    }
    snprintf(this->names[2], 0x80, "/Scene/UI/Font_Dif.szs");

    for (u32 i = 0; i < 3; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_SINGLE_FORMAT;
    }
}

static const MultiDvdArchive_vt s_FontMultiDvdArchive_vt = {
    .dt = MultiDvdArchive_dt,
    .init = FontMultiDvdArchive_init,
};



static MultiDvdArchive *my_MultiDvdArchive_create(u32 type) {
    switch (type) {
    case MULTI_DVD_ARCHIVE_TYPE_RACE:;
        RaceMultiDvdArchive *raceMulti = new(sizeof(RaceMultiDvdArchive));
        RaceMultiDvdArchive_ct(raceMulti);
        return raceMulti;
    case MULTI_DVD_ARCHIVE_TYPE_COURSE:;
        CourseMultiDvdArchive *courseMulti = new(sizeof(CourseMultiDvdArchive));
        CourseMultiDvdArchive_ct(courseMulti);
        return courseMulti;
    case MULTI_DVD_ARCHIVE_TYPE_MENU:;
        MenuMultiDvdArchive *menuMulti = new(sizeof(MenuMultiDvdArchive));
        MenuMultiDvdArchive_ct(menuMulti);
        return menuMulti;
    case MULTI_DVD_ARCHIVE_TYPE_FONT:;
        FontMultiDvdArchive *fontMulti = new(sizeof(FontMultiDvdArchive));
        FontMultiDvdArchive_ct(fontMulti);
        return fontMulti;
    default:;
        MultiDvdArchive *multi = new(sizeof(MultiDvdArchive));
        MultiDvdArchive_ct(multi, 2);
        multi->vt->init(multi);
        return multi;
    }
}
PATCH_B(MultiDvdArchive_create, my_MultiDvdArchive_create);

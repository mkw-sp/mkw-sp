#include "CtrlRaceDebugPanel.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartRollback.hh"
#include "game/kart/VehiclePhysics.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include <sp/cs/RaceClient.hh>

namespace UI {

class WStringWriter {
public:
    WStringWriter();
    ~WStringWriter();

    const wchar_t *data() const;

    void writePlayer(u32 playerId);
    void writeOnline(u32 playerId);

private:
    void write(const wchar_t *restrict format, ...);
    void write(const char *name, const Vec3 &v0);
    void write(const char *name, const Vec3 &v0, const Vec3 &v1);
    void write(const char *name, const Quat &q0);
    void write(const char *name, const Quat &q0, const Quat &q1);

    std::array<wchar_t, 256> m_buffer{};
    size_t m_offset = 0;
};

WStringWriter::WStringWriter() = default;

WStringWriter::~WStringWriter() = default;

const wchar_t *WStringWriter::data() const {
    return m_buffer.data();
}

void WStringWriter::writePlayer(u32 playerId) {
    write(L"T %u\n", System::RaceManager::Instance()->time());
    auto *object = Kart::KartObjectManager::Instance()->object(playerId);
    write("P", *object->getPos());
    write("EV", *object->getVehiclePhysics()->externalVel());
    write("IV", *object->getVehiclePhysics()->internalVel());
    write("MRV", *object->getVehiclePhysics()->movingRoadVel());
    write("MWV", *object->getVehiclePhysics()->movingWaterVel());
    write(L"MTB/MBPB/TZB");
    for (u32 i = 0; i < 3; i++) {
        write(L" %d", object->getTimeBeforeBoostEnd(i * 2));
    }
    write(L"\n");
}

void WStringWriter::writeOnline(u32 playerId) {
    u32 time = System::RaceManager::Instance()->time();
    write(L"T %u\n", time);

    auto *object = Kart::KartObjectManager::Instance()->object(playerId);
    if (auto *rollback = object->getKartRollback()) {
        write("P/RP", *object->getPos(), rollback->posDelta());
        write("MR/RMR", *object->getMainRot(), rollback->mainRotDelta());
        write(L"IS/RIS %f %f\n", object->getInternalSpeed(), rollback->internalSpeedDelta());
    } else {
        write("P", *object->getPos());
        write("MR", *object->getMainRot());
    }
    write("EV", *object->getVehiclePhysics()->externalVel());
    write("IV", *object->getVehiclePhysics()->internalVel());
    write(L"MTB/MBPB/TZB");
    for (u32 i = 0; i < 3; i++) {
        write(L" %d", object->getTimeBeforeBoostEnd(i * 2));
    }
    write(L"\n");
}

void WStringWriter::write(const wchar_t *restrict format, ...) {
    if (m_offset >= m_buffer.size()) {
        return;
    }

    va_list args;
    va_start(args, format);
    s32 result = vswprintf(m_buffer.data() + m_offset, m_buffer.size() - m_offset, format, args);
    va_end(args);

    if (result < 0) {
        m_offset = m_buffer.size();
    } else {
        m_offset += result;
    }
}

void WStringWriter::write(const char *name, const Vec3 &v0) {
    write(L"%s %.2f %.2f %.2f\n", name, v0.x, v0.y, v0.z);
}

void WStringWriter::write(const char *name, const Vec3 &v0, const Vec3 &v1) {
    write(L"%s %.2f %.2f %.2f %.2f %.2f %.2f\n", name, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z);
}

void WStringWriter::write(const char *name, const Quat &q0) {
    Vec3 v0{0.0f, 0.0f, 1.0f};
    Quat::Rotate(q0, v0, v0);
    write(name, v0);
}

void WStringWriter::write(const char *name, const Quat &q0, const Quat &q1) {
    Vec3 v0{0.0f, 0.0f, 1.0f};
    Quat::Rotate(q0, v0, v0);
    Vec3 v1{0.0f, 0.0f, 1.0f};
    Quat::Rotate(q1, v1, v1);
    write(name, v0, v1);
}

CtrlRaceDebugPanel::CtrlRaceDebugPanel() = default;

CtrlRaceDebugPanel::~CtrlRaceDebugPanel() = default;

void CtrlRaceDebugPanel::calcSelf() {
    u32 playerId = getPlayerId();
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugPanel>();
    WStringWriter writer;
    switch (setting) {
    case SP::ClientSettings::DebugPanel::Player:
        writer.writePlayer(playerId);
        break;
    case SP::ClientSettings::DebugPanel::Online:
        writer.writeOnline(playerId);
        break;
    default:
        break;
    }
    MessageInfo info{};
    info.strings[0] = writer.data();
    setMessageAll(6602, &info);
}

void CtrlRaceDebugPanel::load() {
    m_localPlayerId = 0;

    const char *groups[] = {nullptr, nullptr};
    LayoutUIControl::load("game_image", "DebugPanel", "DebugPanel", groups);
}

} // namespace UI

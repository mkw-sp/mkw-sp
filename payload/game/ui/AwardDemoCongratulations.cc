#include "AwardDemoCongratulations.hh"

namespace UI {

AwardDemoCongratulations::AwardDemoCongratulations() = default;

AwardDemoCongratulations::~AwardDemoCongratulations() = default;

void AwardDemoCongratulations::load(bool isWin, bool isDraw, bool isTeams) {
    const char *file;
    if (isWin) {
        file = isTeams ? "AwardCongratulationsWinTeams" : "AwardCongratulationsWin";
    } else if (isDraw) {
        file = "AwardCongratulationsDraw";
    } else {
        file = "AwardCongratulationsLose";
    }
    // clang-format off
    const char *groups[] = {
        "Fade", "Hide", "Fadein", "Show", "Fadeout", nullptr,
        nullptr,
    };
    // clang-format on
    LayoutUIControl::load("award", file, "Congratulations", groups);
}

} // namespace UI

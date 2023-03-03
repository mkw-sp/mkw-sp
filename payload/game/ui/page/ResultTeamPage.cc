#include "ResultTeamPage.hh"

namespace UI {

s32 ResultTeamPage::GetScoreDiff() {
    // Hack: only 1 of the 4 callers remains, and we just need to avoid a draw to get to our actual
    // race outcome logic.
    return 1;
}

} // namespace UI

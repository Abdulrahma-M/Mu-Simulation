#include "action/SteppingAction.hh"

namespace MATHUSLA { namespace MU {

SteppingAction::SteppingAction() 
: G4UserSteppingAction()
  {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
}
} } /* namespace MATHUSLA::MU */

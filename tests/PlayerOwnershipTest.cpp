// Phase 1 regression tests for LISS-0034.

#include <cassert>

#include "Player.h"
#include "Field.h"

void playerOwnershipCopyAssignmentTest()
{
    Player source;
    Player copy;

    copy = source;

    assert(&copy.getStatus() != &source.getStatus());
    assert(&copy.getPosition() != &source.getPosition());

    const long sourceHp = source.getStatus().getHp();
    copy.setHp(sourceHp + 1);
    assert(source.getStatus().getHp() == sourceHp);
}

void playerOwnershipContainerUpdateTest()
{
    Player first;
    Player replacement;

    Field::setPlayer(first);
    replacement.setHp(99);
    Field::setPlayer(replacement);
}

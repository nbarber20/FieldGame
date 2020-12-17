#include "pch.h"
#include "Entity_Firearm.h"
#include "Entity_Living.h"

void Entity_Firearm::Fire(Entity* target)
{
	std::vector<Entity*> inv = GetInventory(Inside);
	for (int i = 0; i < inv.size(); i++) {
		Entity_Clip* clip = dynamic_cast<Entity_Clip*>(inv[i]);
		if (clip) {
			if (clip->FireBullet(1) > 0) {
				Entity_Living* liv = dynamic_cast<Entity_Living*>(target);
				if (liv) {
					liv->TakeDamage(Entity_Living::Piercing, 1, 3);
				}
			}
		}
	}
}

void Entity_Firearm::Reload(Entity* clip)
{
	Entity_Clip* clipTest = dynamic_cast<Entity_Clip*>(clip);
	if (clipTest) {
		if (clipTest->clipType == this->clipType) {
			clipTest->SetParentOverride(Inside, this);
		}
	}
}

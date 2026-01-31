#pragma once

// Minimal quest requirement event interfaces and a tiny manager used by some legacy
// requirement classes (CollectItem/KillAnimal/Interact/PlanetScan/OrbitPlanet).
// The current QuestMgr no longer exposes these, but keeping these lightweight types
// lets the code compile.

#include <vector>

class ICollectedItemEvent
{
public:
	virtual ~ICollectedItemEvent() {}
	virtual void OnCollectedItem(int itemid) = 0;
	virtual const int GetItemID() const = 0;
	virtual const int GetRequiredAmount() const = 0;
};

class IKillAnimalEvent
{
public:
	virtual ~IKillAnimalEvent() {}
	virtual void OnKillAnimal(int animalid) = 0;
	virtual const int GetAnimalID() const = 0;
	virtual const int GetStarID() const = 0;
};

class IInteractEvent
{
public:
	virtual ~IInteractEvent() {}
	virtual void OnInteract(int interactid) = 0;
	virtual const int GetInteractID() const = 0;
};

class IPlanetScanEvent
{
public:
	virtual ~IPlanetScanEvent() {}
	virtual void OnPlanetScan(int planetid) = 0;
	virtual const int GetPlanetID() const = 0;
	virtual const int GetStarID() const = 0;
};

class IOrbitPlanetEvent
{
public:
	virtual ~IOrbitPlanetEvent() {}
	virtual void OnOrbitPlanet(int planetid) = 0;
	virtual const int GetPlanetID() const = 0;
	virtual const int GetStarID() const = 0;
};

template <typename TListener>
class QuestEventManager
{
public:
	void Register(TListener* listener)
	{
		if (listener == nullptr) return;
		listeners.push_back(listener);
	}

	void Unregister(TListener* listener)
	{
		for (auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			if (*it == listener)
			{
				listeners.erase(it);
				return;
			}
		}
	}

private:
	std::vector<TListener*> listeners;
};

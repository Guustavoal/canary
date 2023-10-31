/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "declarations.hpp"
#include "creatures/combat/condition.hpp"
#include "utils/utils_definitions.hpp"
#include "lua/creature/creatureevent.hpp"
#include "map/map.hpp"
#include "game/movement/position.hpp"
#include "items/tile.hpp"

using ConditionList = std::list<std::shared_ptr<Condition>>;
using CreatureEventList = std::list<std::shared_ptr<CreatureEvent>>;

class Map;
class Thing;
class Container;
class Player;
class Monster;
class Npc;
class Item;
class Tile;
class Zone;

static constexpr int32_t EVENT_CREATURECOUNT = 10;
static constexpr int32_t EVENT_CREATURE_THINK_INTERVAL = 1000;
static constexpr int32_t EVENT_CHECK_CREATURE_INTERVAL = (EVENT_CREATURE_THINK_INTERVAL / EVENT_CREATURECOUNT);

class FrozenPathingConditionCall {
public:
	explicit FrozenPathingConditionCall(Position newTargetPos) :
		targetPos(std::move(newTargetPos)) { }

	bool operator()(const Position &startPos, const Position &testPos, const FindPathParams &fpp, int32_t &bestMatchDist) const;

	bool isInRange(const Position &startPos, const Position &testPos, const FindPathParams &fpp) const;

private:
	Position targetPos;
};

//////////////////////////////////////////////////////////////////////
// Defines the Base class for all creatures and base functions which
// every creature has

class Creature : virtual public Thing, public SharedObject {
protected:
	Creature();

public:
	static constexpr double speedA = 857.36, speedB = 261.29, speedC = -4795.01;

	virtual ~Creature();

	// non-copyable
	Creature(const Creature &) = delete;
	Creature &operator=(const Creature &) = delete;

	std::shared_ptr<Creature> getCreature() override final {
		return static_self_cast<Creature>();
	}
	virtual std::shared_ptr<Player> getPlayer() {
		return nullptr;
	}
	virtual std::shared_ptr<Npc> getNpc() {
		return nullptr;
	}
	virtual std::shared_ptr<Monster> getMonster() {
		return nullptr;
	}

	virtual const std::string &getName() const = 0;
	// Real creature name, set on creature creation "createNpcType(typeName) and createMonsterType(typeName)"
	virtual const std::string &getTypeName() const = 0;
	virtual const std::string &getNameDescription() const = 0;

	virtual CreatureType_t getType() const = 0;

	virtual void setID() = 0;
	void setRemoved() {
		isInternalRemoved = true;
	}

	uint32_t getID() const {
		return id;
	}
	virtual void removeList() = 0;
	virtual void addList() = 0;

	virtual bool canSee(const Position &pos);
	virtual bool canSeeCreature(std::shared_ptr<Creature> creature) const;

	virtual RaceType_t getRace() const {
		return RACE_NONE;
	}
	virtual Skulls_t getSkull() const {
		return skull;
	}
	virtual Skulls_t getSkullClient(std::shared_ptr<Creature> creature) {
		return creature->getSkull();
	}
	void setSkull(Skulls_t newSkull);
	Direction getDirection() const {
		return direction;
	}
	void setDirection(Direction dir) {
		direction = dir;
	}

	bool isHealthHidden() const {
		return hiddenHealth;
	}
	void setHiddenHealth(bool b) {
		hiddenHealth = b;
	}

	bool isMoveLocked() const {
		return moveLocked;
	}
	void setMoveLocked(bool locked) {
		moveLocked = locked;
	}

	int32_t getThrowRange() const override final {
		return 1;
	}
	bool isPushable() override {
		return getWalkDelay() <= 0;
	}
	bool isRemoved() override final {
		return isInternalRemoved;
	}
	virtual bool canSeeInvisibility() const {
		return false;
	}
	virtual bool isInGhostMode() const {
		return false;
	}

	int32_t getWalkSize();

	int32_t getWalkDelay(Direction dir = DIRECTION_NONE);
	int64_t getTimeSinceLastMove() const;

	int64_t getEventStepTicks(bool onlyDelay = false);
	uint16_t getStepDuration(Direction dir = DIRECTION_NONE);
	virtual uint16_t getStepSpeed() const {
		return getSpeed();
	}
	uint16_t getSpeed() const {
		return static_cast<uint16_t>(baseSpeed + varSpeed);
	}
	void setSpeed(int32_t varSpeedDelta);

	void setBaseSpeed(uint16_t newBaseSpeed) {
		baseSpeed = newBaseSpeed;
	}
	uint16_t getBaseSpeed() const {
		return baseSpeed;
	}

	int32_t getHealth() const {
		return health;
	}
	virtual int32_t getMaxHealth() const {
		return healthMax;
	}
	uint32_t getMana() const {
		return mana;
	}
	virtual uint32_t getMaxMana() const {
		return mana;
	}

	uint16_t getManaShield() const {
		return manaShield;
	}

	void setManaShield(uint16_t value) {
		manaShield = value;
	}

	uint16_t getMaxManaShield() const {
		return maxManaShield;
	}

	void setMaxManaShield(uint16_t value) {
		maxManaShield = value;
	}

	int32_t getBuff(int32_t buff) {
		return varBuffs[buff];
	}

	int32_t getBuff(int32_t buff) const {
		return varBuffs[buff];
	}

	void setBuff(buffs_t buff, int32_t modifier) {
		varBuffs[buff] += modifier;
	}

	virtual std::vector<CreatureIcon> getIcons() const {
		std::vector<CreatureIcon> icons;
		icons.reserve(creatureIcons.size());
		for (const auto &[_, icon] : creatureIcons) {
			if (icon.isSet()) {
				icons.push_back(icon);
			}
		}
		return icons;
	}

	virtual CreatureIcon getIcon(const std::string &key) const {
		if (!creatureIcons.contains(key)) {
			return CreatureIcon();
		}
		return creatureIcons.at(key);
	}

	void setIcon(const std::string &key, CreatureIcon icon) {
		creatureIcons[key] = icon;
		iconChanged();
	}

	void removeIcon(const std::string &key) {
		creatureIcons.erase(key);
		iconChanged();
	}

	void clearIcons() {
		creatureIcons.clear();
		iconChanged();
	}

	void iconChanged();

	const Outfit_t getCurrentOutfit() const {
		return currentOutfit;
	}
	void setCurrentOutfit(Outfit_t outfit) {
		currentOutfit = outfit;
	}
	const Outfit_t getDefaultOutfit() const {
		return defaultOutfit;
	}
	bool isInvisible() const;
	ZoneType_t getZoneType() {
		if (getTile()) {
			return getTile()->getZoneType();
		}

		return ZONE_NORMAL;
	}

	phmap::flat_hash_set<std::shared_ptr<Zone>> getZones();

	// walk functions
	void startAutoWalk(const std::forward_list<Direction> &listDir, bool ignoreConditions = false);
	void addEventWalk(bool firstStep = false);
	void stopEventWalk();
	virtual void goToFollowCreature();

	// walk events
	virtual void onWalk(Direction &dir);
	virtual void onWalkAborted() { }
	virtual void onWalkComplete() { }

	// follow functions
	std::shared_ptr<Creature> getFollowCreature() const {
		return m_followCreature.lock();
	}
	virtual bool setFollowCreature(std::shared_ptr<Creature> creature);

	// follow events
	virtual void onFollowCreature(std::shared_ptr<Creature>) { }
	virtual void onFollowCreatureComplete(std::shared_ptr<Creature>) { }

	// combat functions
	std::shared_ptr<Creature> getAttackedCreature() {
		return m_attackedCreature.lock();
	}
	virtual bool setAttackedCreature(std::shared_ptr<Creature> creature);

	/**
	 * @brief Mitigates damage inflicted on a creature.
	 *
	 * Used to mitigate the damage inflicted on a creature during combat.
	 *
	 * @note If the server is running in dev mode, this function will also log details about the mitigation process.
	 *
	 * @param creature Reference to the creature that is receiving the damage.
	 * @param combatType Type of combat that is inflicting the damage. Note that mana drain and life drain are not mitigated.
	 * @param blockType Reference to the block type, which may be modified to BLOCK_ARMOR if the damage is reduced to 0.
	 * @param damage Reference to the amount of damage inflicted, which will be reduced by the creature's mitigation factor.
	 */
	void mitigateDamage(const CombatType_t &combatType, BlockType_t &blockType, int32_t &damage) const;
	virtual BlockType_t blockHit(std::shared_ptr<Creature> attacker, CombatType_t combatType, int32_t &damage, bool checkDefense = false, bool checkArmor = false, bool field = false);

	void applyAbsorbDamageModifications(std::shared_ptr<Creature> attacker, int32_t &damage, CombatType_t combatType) const;

	bool setMaster(std::shared_ptr<Creature> newMaster, bool reloadCreature = false);

	void removeMaster() {
		if (getMaster()) {
			m_master.reset();
		}
	}

	bool isSummon() const {
		return !m_master.expired();
	}

	/**
	 * hasBeenSummoned doesn't guarantee master still exists
	 */
	bool hasBeenSummoned() const {
		return summoned;
	}
	std::shared_ptr<Creature> getMaster() const {
		return m_master.lock();
	}

	const phmap::flat_hash_set<std::shared_ptr<Creature>> &getSummons() const {
		return m_summons;
	}

	virtual int32_t getArmor() const {
		return 0;
	}
	virtual float getMitigation() const {
		return 0;
	}
	virtual int32_t getDefense() const {
		return 0;
	}
	virtual float getAttackFactor() const {
		return 1.0f;
	}
	virtual float getDefenseFactor() const {
		return 1.0f;
	}

	virtual uint8_t getSpeechBubble() const {
		return SPEECHBUBBLE_NONE;
	}

	bool addCondition(std::shared_ptr<Condition> condition);
	bool addCombatCondition(std::shared_ptr<Condition> condition);
	void removeCondition(ConditionType_t conditionType, ConditionId_t conditionId, bool force = false);
	void removeCondition(ConditionType_t type);
	void removeCondition(std::shared_ptr<Condition> condition);
	void removeCombatCondition(ConditionType_t type);
	std::shared_ptr<Condition> getCondition(ConditionType_t type) const;
	std::shared_ptr<Condition> getCondition(ConditionType_t type, ConditionId_t conditionId, uint32_t subId = 0) const;
	std::vector<std::shared_ptr<Condition>> getConditionsByType(ConditionType_t type) const;
	void executeConditions(uint32_t interval);
	bool hasCondition(ConditionType_t type, uint32_t subId = 0) const;

	virtual bool isImmune(CombatType_t type) const {
		return false;
	}
	virtual bool isImmune(ConditionType_t type) const {
		return false;
	}
	virtual bool isSuppress(ConditionType_t type) const {
		return false;
	};

	virtual bool isAttackable() const {
		return true;
	}
	virtual Faction_t getFaction() const {
		return FACTION_DEFAULT;
	}

	virtual void changeHealth(int32_t healthChange, bool sendHealthChange = true);
	virtual void changeMana(int32_t manaChange);

	void gainHealth(std::shared_ptr<Creature> attacker, int32_t healthGain);
	virtual void drainHealth(std::shared_ptr<Creature> attacker, int32_t damage);
	virtual void drainMana(std::shared_ptr<Creature> attacker, int32_t manaLoss);

	virtual bool challengeCreature(std::shared_ptr<Creature>, int targetChangeCooldown) {
		return false;
	}

	void onDeath();
	virtual uint64_t getGainedExperience(std::shared_ptr<Creature> attacker) const;
	void addDamagePoints(std::shared_ptr<Creature> attacker, int32_t damagePoints);
	bool hasBeenAttacked(uint32_t attackerId);

	// combat event functions
	virtual void onAddCondition(ConditionType_t type);
	virtual void onAddCombatCondition(ConditionType_t type);
	virtual void onEndCondition(ConditionType_t type);
	void onTickCondition(ConditionType_t type, bool &bRemove);
	virtual void onCombatRemoveCondition(std::shared_ptr<Condition> condition);
	virtual void onAttackedCreature(std::shared_ptr<Creature>) { }
	virtual void onAttacked();
	virtual void onAttackedCreatureDrainHealth(std::shared_ptr<Creature> target, int32_t points);
	virtual void onTargetCreatureGainHealth(std::shared_ptr<Creature>, int32_t) { }
	void onAttackedCreatureKilled(std::shared_ptr<Creature> target);
	virtual bool onKilledCreature(std::shared_ptr<Creature> target, bool lastHit = true);
	virtual void onGainExperience(uint64_t gainExp, std::shared_ptr<Creature> target);
	virtual void onAttackedCreatureBlockHit(BlockType_t) { }
	virtual void onBlockHit() { }
	virtual void onChangeZone(ZoneType_t zone);
	virtual void onAttackedCreatureChangeZone(ZoneType_t zone);
	virtual void onIdleStatus();

	virtual LightInfo getCreatureLight() const;
	virtual void setNormalCreatureLight();
	void setCreatureLight(LightInfo lightInfo);

	virtual void onThink(uint32_t interval);
	void onAttacking(uint32_t interval);
	virtual void onCreatureWalk();
	virtual bool getNextStep(Direction &dir, uint32_t &flags);

	virtual void turnToCreature(std::shared_ptr<Creature> creature);

	void onAddTileItem(std::shared_ptr<Tile> tile, const Position &pos);
	virtual void onUpdateTileItem(std::shared_ptr<Tile> tile, const Position &pos, std::shared_ptr<Item> oldItem, const ItemType &oldType, std::shared_ptr<Item> newItem, const ItemType &newType);
	virtual void onRemoveTileItem(std::shared_ptr<Tile> tile, const Position &pos, const ItemType &iType, std::shared_ptr<Item> item);

	virtual void onCreatureAppear(std::shared_ptr<Creature> creature, bool isLogin);
	virtual void onRemoveCreature(std::shared_ptr<Creature> creature, bool isLogout);

	/**
	 * @brief Check if the summon can move/spawn and if the familiar can teleport to the master
	 *
	 * @param newPos New position to teleport
	 * @param teleportSummon Can teleport normal summon? Default value is "false"
	 * @return true
	 * @return false
	 */
	void checkSummonMove(const Position &newPos, bool teleportSummon = false);
	virtual void onCreatureMove(std::shared_ptr<Creature> creature, std::shared_ptr<Tile> newTile, const Position &newPos, std::shared_ptr<Tile> oldTile, const Position &oldPos, bool teleport);

	virtual void onAttackedCreatureDisappear(bool) { }
	virtual void onFollowCreatureDisappear(bool) { }

	virtual void onCreatureSay(std::shared_ptr<Creature>, SpeakClasses, const std::string &) { }

	virtual void onPlacedCreature() { }

	virtual bool getCombatValues(int32_t &, int32_t &) {
		return false;
	}

	size_t getSummonCount() const {
		return m_summons.size();
	}

	/**
	 * @brief Check if the "summons" list is empty
	 *
	 * @return true = not empty
	 * @return false = empty
	 */
	bool hasSummons() const {
		if (!m_summons.empty()) {
			return true;
		}
		return false;
	}

	void setDropLoot(bool newLootDrop) {
		this->lootDrop = newLootDrop;
	}
	void setSkillLoss(bool newSkillLoss) {
		this->skillLoss = newSkillLoss;
	}
	void setUseDefense(bool useDefense) {
		canUseDefense = useDefense;
	}

	// creature script events
	bool registerCreatureEvent(const std::string &name);
	bool unregisterCreatureEvent(const std::string &name);

	std::shared_ptr<Cylinder> getParent() override final {
		return getTile();
	}

	void setParent(std::weak_ptr<Cylinder> cylinder) override final {
		walk.recache = true;
		walk.groundSpeed = 150;

		if (const auto &lockedCylinder = cylinder.lock()) {
			const auto &newParent = lockedCylinder->getTile();
			position = newParent->getPosition();
			m_tile = newParent;

			if (newParent->getGround()) {
				const auto &it = Item::items[newParent->getGround()->getID()];
				if (it.speed > 0) {
					walk.groundSpeed = it.speed;
				}
			}
		}
	}

	const Position &getPosition() override final {
		return position;
	}

	std::shared_ptr<Tile> getTile() override final {
		return m_tile.lock();
	}

	int32_t getWalkCache(const Position &pos);

	const Position &getLastPosition() const {
		return lastPosition;
	}
	void setLastPosition(Position newLastPos) {
		lastPosition = newLastPos;
	}

	static bool canSee(const Position &myPos, const Position &pos, int32_t viewRangeX, int32_t viewRangeY);

	double getDamageRatio(std::shared_ptr<Creature> attacker) const;

	bool getPathTo(const Position &targetPos, std::forward_list<Direction> &dirList, const FindPathParams &fpp);
	bool getPathTo(const Position &targetPos, std::forward_list<Direction> &dirList, int32_t minTargetDist, int32_t maxTargetDist, bool fullPathSearch = true, bool clearSight = true, int32_t maxSearchDist = 7);

	struct CountBlock_t {
		int32_t total;
		int64_t ticks;
	};
	using CountMap = std::map<uint32_t, CountBlock_t>;
	CountMap getDamageMap() const {
		return damageMap;
	}
	void setWheelOfDestinyDrainBodyDebuff(uint8_t value) {
		wheelOfDestinyDrainBodyDebuff = value;
	}
	uint8_t getWheelOfDestinyDrainBodyDebuff() const {
		return wheelOfDestinyDrainBodyDebuff;
	}

	/**
	 * @brief Retrieves the reflection percentage for a given combat type.
	 *
	 * @param combatType The combat type.
	 * @param useCharges Indicates whether charges should be considered.
	 * @return The reflection percentage for the specified combat type.
	 */
	virtual int32_t getReflectPercent(CombatType_t combatType, bool useCharges = false) const;

	/**
	 * @brief Retrieves the flat reflection value for a given combat type.
	 *
	 * @param combatType The combat type.
	 * @param useCharges Indicates whether charges should be considered.
	 * @return The flat reflection value for the specified combat type.
	 */
	virtual int32_t getReflectFlat(CombatType_t combatType, bool useCharges = false) const;

	/**
	 * @brief Sets the reflection percentage for a given combat type.
	 *
	 * @param combatType The combat type.
	 * @param value The reflection percentage value.
	 */
	virtual void setReflectPercent(CombatType_t combatType, int32_t value);

	/**
	 * @brief Sets the flat reflection value for a given combat type.
	 *
	 * @param combatType The combat type.
	 * @param value The flat reflection value.
	 */
	virtual void setReflectFlat(CombatType_t combatType, int32_t value);

	/**
	 * @brief Retrieves the flat absorption value for a given combat type.
	 *
	 * @param combat The combat type.
	 * @return The flat absorption value for the specified combat type.
	 */
	int32_t getAbsorbFlat(CombatType_t combat) const;

	/**
	 * @brief Sets the flat absorption value for a given combat type.
	 *
	 * @param combat The combat type.
	 * @param value The flat absorption value.
	 */
	void setAbsorbFlat(CombatType_t combat, int32_t value);

	/**
	 * @brief Retrieves the absorption percentage for a given combat type.
	 *
	 * @param combat The combat type.
	 * @return The absorption percentage for the specified combat type.
	 */
	int32_t getAbsorbPercent(CombatType_t combat) const;

	/**
	 * @brief Sets the absorption percentage for a given combat type.
	 *
	 * @param combat The combat type.
	 * @param value The absorption percentage value.
	 */
	void setAbsorbPercent(CombatType_t combat, int32_t value);

	/**
	 * @brief Retrieves the increase percentage for a given combat type.
	 *
	 * @param combat The combat type.
	 * @return The increase percentage for the specified combat type.
	 */
	int32_t getIncreasePercent(CombatType_t combat) const;

	/**
	 * @brief Sets the increase percentage for a given combat type.
	 *
	 * @param combat The combat type.
	 * @param value The increase percentage value.
	 */
	void setIncreasePercent(CombatType_t combat, int32_t value);

	/**
	 * @brief Retrieves the charm percent modifier for the creature.
	 *
	 * @return The charm percent modifier for the creature.
	 */
	int8_t getCharmChanceModifier() const {
		return charmChanceModifier;
	}

	/**
	 * @brief Sets the charm percent modifier for the creature.
	 *
	 * @param value The charm percent modifier value.
	 */
	void setCharmChanceModifier(int8_t value) {
		charmChanceModifier = value;
	}

protected:
	virtual bool useCacheMap() const {
		return false;
	}

	static constexpr int32_t mapWalkWidth = MAP_MAX_VIEW_PORT_X * 2 + 1;
	static constexpr int32_t mapWalkHeight = MAP_MAX_VIEW_PORT_Y * 2 + 1;
	static constexpr int32_t maxWalkCacheWidth = (mapWalkWidth - 1) / 2;
	static constexpr int32_t maxWalkCacheHeight = (mapWalkHeight - 1) / 2;

	Position position;

	CountMap damageMap;

	phmap::flat_hash_set<std::shared_ptr<Creature>> m_summons;
	CreatureEventList eventsList;
	ConditionList conditions;

	std::forward_list<Direction> listWalkDir;

	std::weak_ptr<Tile> m_tile;
	std::weak_ptr<Creature> m_attackedCreature;
	std::weak_ptr<Creature> m_master;
	std::weak_ptr<Creature> m_followCreature;

	/**
	 * We need to persist if this creature is summon or not because when we
	 * increment the bestiary count, the master might be gone before we can
	 * check if this summon has a master and mistakenly count it kill.
	 *
	 * @see BestiaryOnKill
	 * @see Monster::death()
	 */
	bool summoned = false;

	uint64_t lastStep = 0;
	uint32_t id = 0;
	uint32_t scriptEventsBitField = 0;
	uint32_t eventWalk = 0;
	uint32_t walkUpdateTicks = 0;
	uint32_t lastHitCreatureId = 0;
	uint32_t blockCount = 0;
	uint32_t blockTicks = 0;
	uint32_t lastStepCost = 1;
	uint16_t baseSpeed = 110;
	uint32_t mana = 0;
	int32_t varSpeed = 0;
	int32_t health = 1000;
	int32_t healthMax = 1000;

	uint16_t manaShield = 0;
	uint16_t maxManaShield = 0;
	int32_t varBuffs[BUFF_LAST + 1] = { 100, 100, 100 };

	std::array<int32_t, COMBAT_COUNT> reflectPercent = { 0 };
	std::array<int32_t, COMBAT_COUNT> reflectFlat = { 0 };

	std::array<int32_t, COMBAT_COUNT> absorbPercent = { 0 };
	std::array<int32_t, COMBAT_COUNT> increasePercent = { 0 };
	std::array<int32_t, COMBAT_COUNT> absorbFlat = { 0 };

	Outfit_t currentOutfit;
	Outfit_t defaultOutfit;

	Position lastPosition;
	LightInfo internalLight;

	Direction direction = DIRECTION_SOUTH;
	Skulls_t skull = SKULL_NONE;

	bool localMapCache[mapWalkHeight][mapWalkWidth] = { { false } };
	bool isInternalRemoved = false;
	bool isMapLoaded = false;
	bool isUpdatingPath = false;
	bool creatureCheck = false;
	bool inCheckCreaturesVector = false;
	bool skillLoss = true;
	bool lootDrop = true;
	bool cancelNextWalk = false;
	bool hasFollowPath = false;
	bool forceUpdateFollowPath = false;
	bool hiddenHealth = false;
	bool floorChange = false;
	bool canUseDefense = true;
	bool moveLocked = false;
	int8_t charmChanceModifier = 0;

	uint8_t wheelOfDestinyDrainBodyDebuff = 0;

	// use map here instead of phmap to keep the keys in a predictable order
	std::map<std::string, CreatureIcon> creatureIcons = {};

	// creature script events
	bool hasEventRegistered(CreatureEventType_t event) const {
		return (0 != (scriptEventsBitField & (static_cast<uint32_t>(1) << event)));
	}
	CreatureEventList getCreatureEvents(CreatureEventType_t type);

	void updateMapCache();
	void updateTileCache(std::shared_ptr<Tile> tile, int32_t dx, int32_t dy);
	void updateTileCache(std::shared_ptr<Tile> tile, const Position &pos);
	void onCreatureDisappear(std::shared_ptr<Creature> creature, bool isLogout);
	virtual void doAttacking(uint32_t) { }
	virtual bool hasExtraSwing() {
		return false;
	}

	virtual uint64_t getLostExperience() const {
		return 0;
	}
	virtual void dropLoot(std::shared_ptr<Container>, std::shared_ptr<Creature>) { }
	virtual uint16_t getLookCorpse() const {
		return 0;
	}
	virtual void getPathSearchParams(std::shared_ptr<Creature> creature, FindPathParams &fpp);
	virtual void death(std::shared_ptr<Creature>) { }
	virtual bool dropCorpse(std::shared_ptr<Creature> lastHitCreature, std::shared_ptr<Creature> mostDamageCreature, bool lastHitUnjustified, bool mostDamageUnjustified);
	virtual std::shared_ptr<Item> getCorpse(std::shared_ptr<Creature> lastHitCreature, std::shared_ptr<Creature> mostDamageCreature);

	friend class Game;
	friend class Map;
	friend class CreatureFunctions;

private:
	bool canFollowMaster();
	bool isLostSummon();
	void handleLostSummon(bool teleportSummons);

	struct {
		uint16_t groundSpeed { 0 };
		uint16_t calculatedStepSpeed { 1 };
		uint16_t duration { 0 };
		uint16_t diagonalDuration { 0 };

		bool recache = false;
	} walk;

	void updateCalculatedStepSpeed() {
		const int32_t stepSpeed = getStepSpeed();
		walk.calculatedStepSpeed = std::max<uint32_t>(floor((Creature::speedA * log(stepSpeed + Creature::speedB) + Creature::speedC) + .5f), 1);
		walk.calculatedStepSpeed = (stepSpeed > -Creature::speedB) ? walk.calculatedStepSpeed : 1;
		walk.recache = true;
	}
};

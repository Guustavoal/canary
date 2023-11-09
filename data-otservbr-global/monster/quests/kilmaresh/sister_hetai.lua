local mType = Game.createMonsterType("Sister Hetai")
local monster = {}

monster.description = "Sister Hetai"
monster.experience = 20500
monster.outfit = {
	lookType = 1199,
	lookHead = 114,
	lookBody = 19,
	lookLegs = 94,
	lookFeet = 78,
	lookAddons = 0,
	lookMount = 0,
}

monster.health = 25000
monster.maxHealth = 25000
monster.race = "blood"
monster.corpse = 31419
monster.speed = 115
monster.manaCost = 0

monster.changeTarget = {
	interval = 4000,
	chance = 10,
}

monster.bosstiary = {
	bossRaceId = 2104,
	bossRace = RARITY_ARCHFOE,
	storageCooldown = Storage.Kilmaresh.SisterHetaiTimer,
}

monster.strategiesTarget = {
	nearest = 100,
}

monster.flags = {
	summonable = false,
	attackable = true,
	hostile = true,
	convinceable = false,
	pushable = false,
	rewardBoss = false,
	illusionable = false,
	canPushItems = true,
	canPushCreatures = true,
	staticAttackChance = 70,
	targetDistance = 1,
	runHealth = 0,
	healthHidden = false,
	isBlockable = false,
	canWalkOnEnergy = true,
	canWalkOnFire = true,
	canWalkOnPoison = true,
}

monster.light = {
	level = 0,
	color = 0,
}

monster.voices = {
	interval = 5000,
	chance = 10,
}

monster.loot = {
	{ name = "crystal coin", chance = 4323, maxCount = 3 },
	{ name = "sacred tree amulet", chance = 7801 },
	{ name = "gold ingot", chance = 6770, maxCount = 5 },
	{ name = "lightning headband", chance = 4609 },
	{ name = "underworld rod", chance = 4609 },
	{ name = "violet gem", chance = 4609, maxCount = 3  },
	{ name = "wand of cosmic energy", chance = 4069 },
	{ name = "onyx chip", chance = 4017 },
	{ name = "rainbow quartz", chance = 4017 },
	{ name = "small diamond", chance = 4107, maxCount = 3  },
	{ name = "wand of inferno", chance = 4107 },
	{ name = "knight armor", chance = 3065 },
	{ name = "lightning pendant", chance = 3605 },
	{ name = "small emerald", chance = 3605, maxCount = 13  },
	{ name = "focus cape", chance = 3103 },
	{ name = "magma coat", chance = 3103 },
	{ id = 3098, chance = 3130 }, -- ring of healing
	{ name = "wand of starstorm", chance = 3130 },
	{ name = "magma boots", chance = 2600 },
	{ name = "metal spats", chance = 2060 },
	{ name = "warrior's shield", chance = 2600 },
	{ id = 3097, chance = 2008 }, -- dwarven ring
	{ name = "golden mask", chance = 2008 },
	{ name = "terra hood", chance = 2008 },
	{ name = "eye-embroidered veil", chance = 500 },
	{ id = 23531, chance = 1506 }, -- ring of green plasma
	{ name = "terra boots", chance = 1506 },
	{ name = "yellow gem", chance = 1506 },
	{ name = "lightning legs", chance = 1004 },
	{ name = "sea horse figurine", chance = 10004 },
	{ name = "tagralt-inlaid scabbard", chance = 500 },
}

monster.attacks = {
	{ name = "melee", interval = 2000, chance = 100, minDamage = -270, maxDamage = -500 },
	{ name = "targetfirering", interval = 2000, chance = 40, minDamage = -500, maxDamage = -650, target = true },
	{ name = "combat", interval = 2000, chance = 70, type = COMBAT_FIREDAMAGE, minDamage = -350, maxDamage = -500, radius = 2, shootEffect = CONST_ANI_FIRE, effect = CONST_ME_EXPLOSIONHIT, target = true },
	{ name = "combat", interval = 2000, chance = 30, type = COMBAT_ENERGYDAMAGE, minDamage = -500, maxDamage = -750, radius = 4, effect = CONST_ME_ENERGYAREA, target = false },
}

monster.defenses = {
	defense = 60,
	armor = 82,
}

monster.elements = {
	{ type = COMBAT_PHYSICALDAMAGE, percent = 5 },
	{ type = COMBAT_ENERGYDAMAGE, percent = 15 },
	{ type = COMBAT_EARTHDAMAGE, percent = 0 },
	{ type = COMBAT_FIREDAMAGE, percent = 10 },
	{ type = COMBAT_LIFEDRAIN, percent = 0 },
	{ type = COMBAT_MANADRAIN, percent = 0 },
	{ type = COMBAT_DROWNDAMAGE, percent = 0 },
	{ type = COMBAT_ICEDAMAGE, percent = -25 },
	{ type = COMBAT_HOLYDAMAGE, percent = 0 },
	{ type = COMBAT_DEATHDAMAGE, percent = 0 },
}

monster.immunities = {
	{ type = "paralyze", condition = true },
	{ type = "outfit", condition = true },
	{ type = "invisible", condition = true },
	{ type = "bleed", condition = false },
}

mType:register(monster)

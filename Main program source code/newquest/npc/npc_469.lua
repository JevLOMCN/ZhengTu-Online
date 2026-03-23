local sq = the_script();
sq:add(ScriptQuest.NPC_VISIT, 469);

require "define825"

function visit_469(npc)
	return visit_npc(npc);
end

function state_469(npc)
	return state_npc(npc);
end
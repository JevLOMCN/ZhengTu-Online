local sq = the_script();
sq:add(ScriptQuest.NPC_VISIT, 422);

require "define822"

function visit_422(npc)
	return visit_npc(npc);
end

function state_422(npc)
	return state_npc(npc);
end
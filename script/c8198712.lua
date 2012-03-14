--エンド·オブ·ザ·ワールド
function c8198712.initial_effect(c)
	aux.AddRitualProcEqual(c,c8198712.ritual_filter)
end
function c8198712.ritual_filter(c)
	local code=c:GetCode()
	return c==72426662 or c==46427957
end

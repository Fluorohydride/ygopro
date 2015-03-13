--エンド・オブ・ザ・ワールド
function c8198712.initial_effect(c)
	aux.AddRitualProcEqual2(c,c8198712.ritual_filter)
end
function c8198712.ritual_filter(c)
	local code=c:GetCode()
	return code==72426662 or code==46427957
end

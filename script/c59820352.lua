--大地讃頌
function c59820352.initial_effect(c)
	aux.AddRitualProcEqual(c,c59820352.ritual_filter)
end
function c59820352.ritual_filter(c)
	return c:IsType(TYPE_RITUAL) and c:IsAttribute(ATTRIBUTE_EARTH) 
end

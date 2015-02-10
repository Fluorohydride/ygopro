--奈落との契約
function c69035382.initial_effect(c)
	aux.AddRitualProcEqual2(c,c69035382.ritual_filter)
end
function c69035382.ritual_filter(c)
	return c:IsType(TYPE_RITUAL) and c:IsAttribute(ATTRIBUTE_DARK) 
end

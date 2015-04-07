--女王親衛隊
function c71411377.initial_effect(c)
	--at limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetTarget(c71411377.atlimit)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
end
function c71411377.atlimit(e,c)
	local code=c:GetCode()
	return code==87257460 or code==23756165 or code==50140163
end

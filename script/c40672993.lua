--避雷神
function c40672993.initial_effect(c)
	--cannot active
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,1)
	e1:SetCondition(c40672993.limcon)
	e1:SetValue(c40672993.aclimit)
	c:RegisterEffect(e1)
end
function c40672993.limcon(e)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c40672993.aclimit(e,re,tp)
	return re:IsActiveType(TYPE_SPELL)
end
--かつて神と呼ばれた亀
function c74952447.initial_effect(c)
	--disable spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,1)
	e1:SetTarget(c74952447.sumlimit)
	c:RegisterEffect(e1)
end
function c74952447.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return c:IsAttackAbove(1800)
end

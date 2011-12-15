--後に亀と呼ばれる神
function c83061014.initial_effect(c)
	--disable spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,1)
	e1:SetTarget(c83061014.sumlimit)
	c:RegisterEffect(e1)
end
function c83061014.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return c:IsAttackBelow(1800)
end

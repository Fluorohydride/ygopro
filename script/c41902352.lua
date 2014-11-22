--BF－東雲のコチ
function c41902352.initial_effect(c)
	--synchro limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_CANNOT_BE_SYNCHRO_MATERIAL)
	e1:SetCondition(c41902352.synlimit)
	e1:SetValue(1)
	c:RegisterEffect(e1)
end
function c41902352.synlimit(e)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_SPECIAL)==SUMMON_TYPE_SPECIAL
end

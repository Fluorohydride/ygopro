--星間竜パーセク
function c32872833.initial_effect(c)
	--summon with no tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32872833,0))
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c32872833.ntcon)
	c:RegisterEffect(e1)
end
function c32872833.filter(c)
	return c:IsFaceup() and c:GetLevel()==8
end
function c32872833.ntcon(e,c,minc)
	if c==nil then return true end
	return minc==0 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c32872833.filter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end

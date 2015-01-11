--ジェネクス・ヒート
function c53944920.initial_effect(c)
	--summon with no tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(53944920,0))
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c53944920.ntcon)
	c:RegisterEffect(e1)
end
function c53944920.ntfilter(c)
	return c:IsFaceup() and c:IsCode(68505803)
end
function c53944920.ntcon(e,c,minc)
	if c==nil then return true end
	return minc==0 and c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c53944920.ntfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end

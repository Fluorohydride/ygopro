--シャークラーケン
function c71923655.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c71923655.spcon)
	e1:SetOperation(c71923655.spop)
	c:RegisterEffect(e1)
end
function c71923655.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-1
		and Duel.CheckReleaseGroup(c:GetControler(),Card.IsAttribute,1,nil,ATTRIBUTE_WATER)
end
function c71923655.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.SelectReleaseGroup(c:GetControler(),Card.IsAttribute,1,1,nil,ATTRIBUTE_WATER)
	Duel.Release(g,REASON_COST)
end

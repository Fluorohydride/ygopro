--ゲート・ガーディアン
function c25833572.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c25833572.spcon)
	e1:SetOperation(c25833572.spop)
	c:RegisterEffect(e1)
end
function c25833572.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-3
		and Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,25955164)
		and Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,62340868)
		and Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,98434877)
end
function c25833572.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g1=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,25955164)
	local g2=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,62340868)
	local g3=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,98434877)
	g1:Merge(g2)
	g1:Merge(g3)
	Duel.Release(g1,REASON_COST)
end

--マジカル・スター・イリュージョン
function c18752707.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c18752707.condition)
	e1:SetTarget(c18752707.target)
	e1:SetOperation(c18752707.activate)
	c:RegisterEffect(e1)
end
function c18752707.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)<=Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)
end
function c18752707.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
end
function c18752707.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,nil)
	local val1=g1:GetSum(Card.GetLevel)
	local g2=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	local val2=g2:GetSum(Card.GetLevel)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetValue(val1*100)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetTargetRange(0,LOCATION_MZONE)
	e2:SetValue(val2*100)
	Duel.RegisterEffect(e2,tp)
end

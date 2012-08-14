--闇をかき消す光
function c45895206.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c45895206.target)
	e1:SetOperation(c45895206.activate)
	c:RegisterEffect(e1)
end
function c45895206.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsFacedown,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsFacedown,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c45895206.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsFacedown,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.ChangePosition(g,POS_FACEUP_ATTACK,POS_FACEUP_ATTACK,POS_FACEUP_DEFENCE,POS_FACEUP_DEFENCE)
	end
end

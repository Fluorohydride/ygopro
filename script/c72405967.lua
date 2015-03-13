--王家の生け贄
function c72405967.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_HANDES+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c72405967.condition)
	e1:SetTarget(c72405967.target)
	e1:SetOperation(c72405967.activate)
	c:RegisterEffect(e1)
end
function c72405967.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsEnvironment(47355498,tp)
end
function c72405967.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(nil,tp,LOCATION_HAND,LOCATION_HAND,1,e:GetHandler()) end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,PLAYER_ALL,0)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,PLAYER_ALL,LOCATION_HAND)
end
function c72405967.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	Duel.ConfirmCards(tp,g2)
	Duel.ConfirmCards(1-tp,g1)
	local dg1=g1:Filter(Card.IsType,nil,TYPE_MONSTER)
	local dg2=g2:Filter(Card.IsType,nil,TYPE_MONSTER)
	dg1:Merge(dg2)
	if dg1:GetCount()>0 then Duel.SendtoGrave(dg1,REASON_EFFECT+REASON_DISCARD) end
	Duel.ShuffleHand(tp)
	Duel.ShuffleHand(1-tp)
end

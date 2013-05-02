--大革命
function c65396880.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c65396880.condition)
	e1:SetTarget(c65396880.target)
	e1:SetOperation(c65396880.activate)
	c:RegisterEffect(e1)
end
function c65396880.cfilter(c,code)
	return c:IsFaceup() and c:IsCode(code)
end
function c65396880.condition(e,tp,eg,ep,ev,re,r,rp)
	local ph=Duel.GetCurrentPhase()
	return Duel.GetTurnPlayer()==tp and (ph==PHASE_MAIN1 or ph==PHASE_MAIN2)
		and Duel.IsExistingMatchingCard(c65396880.cfilter,tp,LOCATION_ONFIELD,0,1,nil,58538870)
		and Duel.IsExistingMatchingCard(c65396880.cfilter,tp,LOCATION_ONFIELD,0,1,nil,12143771)
		and Duel.IsExistingMatchingCard(c65396880.cfilter,tp,LOCATION_ONFIELD,0,1,nil,85936485)
end
function c65396880.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil)
		or Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)~=0 end
	local g1=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,g1:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g2,g2:GetCount(),0,0)
end
function c65396880.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g1,REASON_EFFECT)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	Duel.SendtoGrave(g2,REASON_EFFECT)
end

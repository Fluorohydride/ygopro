--インフェルニティ・インフェルノ
function c14550855.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_HANDES+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c14550855.target)
	e1:SetOperation(c14550855.activate)
	c:RegisterEffect(e1)
end
function c14550855.filter(c)
	return c:IsSetCard(0xb) and c:IsAbleToGrave()
end
function c14550855.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0
		and Duel.IsExistingMatchingCard(c14550855.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c14550855.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0 then return end
	local ac=Duel.GetMatchingGroupCount(c14550855.filter,tp,LOCATION_DECK,0,nil)
	if ac==0 then return end
	if ac>2 then ac=2 end
	local ct=Duel.DiscardHand(tp,aux.TRUE,1,ac,REASON_DISCARD+REASON_EFFECT)
	Duel.BreakEffect()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c14550855.filter,tp,LOCATION_DECK,0,1,ct,nil)
	Duel.SendtoGrave(g,REASON_EFFECT)
end

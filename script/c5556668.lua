--エクスチェンジ
function c5556668.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c5556668.target)
	e1:SetOperation(c5556668.activate)
	c:RegisterEffect(e1)
end
function c5556668.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0
		and Duel.IsExistingMatchingCard(nil,tp,LOCATION_HAND,0,1,e:GetHandler()) end
end
function c5556668.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g1:GetCount()==0 or g2:GetCount()==0 then return end
	Duel.ConfirmCards(tp,g2)
	Duel.ConfirmCards(1-tp,g1)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local ag1=g2:Select(tp,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_ATOHAND)
	local ag2=g1:Select(1-tp,1,1,nil)
	Duel.SendtoHand(ag1,tp,REASON_EFFECT)
	Duel.SendtoHand(ag2,1-tp,REASON_EFFECT)
end

--スペース・サイクロン
function c69176131.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c69176131.target)
	e1:SetOperation(c69176131.activate)
	c:RegisterEffect(e1)
end
function c69176131.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckRemoveOverlayCard(tp,1,1,1,REASON_EFFECT) end
end
function c69176131.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(Card.CheckRemoveOverlayCard,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp,1,REASON_EFFECT)
	if sg:GetCount()==0 then return end
	if sg:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(69176131,0))
		sg=sg:Select(tp,1,1,nil)
		Duel.HintSelection(sg)
	end
	sg:GetFirst():RemoveOverlayCard(tp,1,1,REASON_EFFECT)
end

--救済のレイヤード
function c67468948.initial_effect(c)
	--tohand
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_CHAIN_SOLVED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c67468948.drop)
	c:RegisterEffect(e1)
end
function c67468948.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_FAIRY) and c:IsAbleToHand()
end
function c67468948.drop(e,tp,eg,ep,ev,re,r,rp)
	if not re:GetHandler():IsType(TYPE_COUNTER) then return end
	Duel.BreakEffect()
	local g=Duel.GetMatchingGroup(c67468948.filter,tp,LOCATION_REMOVED,0,nil)
	if g:GetCount()<2 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local sg=g:Select(tp,2,2,nil)
	Duel.SendtoHand(sg,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,sg)
end

--不幸を告げる黒猫
function c24140059.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetOperation(c24140059.operation)
	c:RegisterEffect(e1)
end
function c24140059.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(24140059,1))
	local g=Duel.SelectMatchingCard(tp,Card.IsType,tp,LOCATION_DECK,0,1,1,nil,TYPE_TRAP)
	local tc=g:GetFirst()
	if tc then
		if Duel.IsEnvironment(47355498) and tc:IsAbleToHand() and Duel.SelectYesNo(tp,aux.Stringid(24140059,0)) then
			Duel.SendtoHand(tc,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,tc)
		else
			Duel.ShuffleDeck(tp)
			Duel.MoveSequence(tc,0)
			Duel.ConfirmDecktop(tp,1)
		end
	end
end

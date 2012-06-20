--レベル変換実験室
function c84397023.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c84397023.tg)
	e1:SetOperation(c84397023.op)
	c:RegisterEffect(e1)
end
function c84397023.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsType,tp,LOCATION_HAND,0,1,nil,TYPE_MONSTER) end
end
function c84397023.op(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local g=Duel.SelectMatchingCard(tp,Card.IsType,tp,LOCATION_HAND,0,1,1,nil,TYPE_MONSTER)
	if g:GetCount()>0 then
		Duel.ConfirmCards(1-tp,g)
		local ct=Duel.TossDice(tp,1)
		if ct==1 then Duel.SendtoGrave(g,REASON_EFFECT)
		else
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_LEVEL)
			e1:SetValue(ct)
			e1:SetReset(RESET_EVENT+0xfe0000+RESET_PHASE+PHASE_END)
			g:GetFirst():RegisterEffect(e1)
			Duel.ShuffleHand(tp)
		end
	end
end

--水晶の占い師
function c82099401.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(82099401,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetOperation(c82099401.operation)
	c:RegisterEffect(e1)
end
function c82099401.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)<2 then return end
	Duel.ConfirmDecktop(tp,2)
	local g=Duel.GetDecktopGroup(tp,2)
	if g:GetCount()>0 then
		Duel.DisableShuffleCheck()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local add=g:Select(tp,1,1,nil)
		if add:GetFirst():IsAbleToHand() then
			Duel.SendtoHand(add,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,add)
			Duel.ShuffleHand(tp)
		else
			Duel.SendtoGrave(add,REASON_EFFECT)
		end
		local back=Duel.GetDecktopGroup(tp,1)
		Duel.MoveSequence(back:GetFirst(),1)
	end
end

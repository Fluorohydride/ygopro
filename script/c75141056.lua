--ヒーロー・マスク
function c75141056.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c75141056.target)
	e1:SetOperation(c75141056.activate)
	c:RegisterEffect(e1)
end
function c75141056.tgfilter(c)
	return c:IsFaceup() and Duel.GetMatchingGroup(c75141056.cfilter,tp,LOCATION_DECK,0,nil,c:GetCode()):FilterCount(Card.IsAbleToGrave,nil)>0
end
function c75141056.cfilter(c,code)
	return c:IsSetCard(0x3008) and c:GetCode()~=code
end
function c75141056.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(c75141056.tgfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c75141056.tgfilter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c75141056.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local g=Duel.GetMatchingGroup(c75141056.cfilter,tp,LOCATION_DECK,0,nil,tc:GetCode())
		if g:GetCount()~=0 then
			g=g:Filter(Card.IsAbleToGrave,nil)
			if g:GetCount()==0 then return end
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
			g=g:Select(tp,1,1,nil)
			Duel.SendtoGrave(g,REASON_EFFECT)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_CODE)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
			e1:SetValue(g:GetFirst():GetCode())
			tc:RegisterEffect(e1)
		else
			local cg=Duel.GetFieldGroup(tp,LOCATION_DECK,0)
			Duel.ConfirmCards(1-tp,cg)
			Duel.ConfirmCards(tp,cg)
			Duel.ShuffleDeck(tp)
		end
	end
end

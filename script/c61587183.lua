--黒蠍－逃げ足のチック
function c61587183.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(61587183,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c61587183.condition)
	e1:SetTarget(c61587183.target)
	e1:SetOperation(c61587183.operation)
	c:RegisterEffect(e1)
end
function c61587183.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c61587183.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToHand() end
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>0
		or Duel.IsExistingTarget(Card.IsAbleToHand,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(61587183,0))
	if Duel.IsExistingTarget(Card.IsAbleToHand,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		and Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>0 then
		op=Duel.SelectOption(tp,aux.Stringid(61587183,1),aux.Stringid(61587183,2))
	elseif Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>0 then
		Duel.SelectOption(tp,aux.Stringid(61587183,2))
		op=1
	else
		Duel.SelectOption(tp,aux.Stringid(61587183,1))
		op=0
	end
	e:SetLabel(op)
	if op==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
		local g=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else e:SetProperty(0) end
end
function c61587183.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SendtoHand(tc,nil,REASON_EFFECT)
		end
	else
		local g=Duel.GetDecktopGroup(1-tp,1)
		if g:GetCount()>0 then
			Duel.ConfirmCards(tp,g)
			Duel.Hint(HINT_SELECTMSG,tp,0)
			local ac=Duel.SelectOption(tp,aux.Stringid(61587183,3),aux.Stringid(61587183,4))
			if ac==1 then Duel.MoveSequence(g:GetFirst(),1) end
		end
	end
end

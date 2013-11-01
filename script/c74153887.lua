--黒蠍－棘のミーネ
function c74153887.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(74153887,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c74153887.condition)
	e1:SetTarget(c74153887.target)
	e1:SetOperation(c74153887.operation)
	c:RegisterEffect(e1)
end
function c74153887.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c74153887.filter(c)
	return c:IsSetCard(0x1a) and c:IsAbleToHand()
end
function c74153887.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c74153887.filter(chkc) end
	if chk==0 then return Duel.IsExistingMatchingCard(c74153887.filter,tp,LOCATION_DECK,0,1,nil)
		or Duel.IsExistingTarget(c74153887.filter,tp,LOCATION_GRAVE,0,1,nil) end
	local op=0
	if Duel.IsExistingMatchingCard(c74153887.filter,tp,LOCATION_DECK,0,1,nil)
		and Duel.IsExistingTarget(c74153887.filter,tp,LOCATION_GRAVE,0,1,nil) then
		op=Duel.SelectOption(tp,aux.Stringid(74153887,1),aux.Stringid(74153887,2))
	elseif Duel.IsExistingTarget(c74153887.filter,tp,LOCATION_GRAVE,0,1,nil) then
		Duel.SelectOption(tp,aux.Stringid(74153887,2))
		op=1
	else
		Duel.SelectOption(tp,aux.Stringid(74153887,1))
		op=0
	end
	e:SetLabel(op)
	if op==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectTarget(tp,c74153887.filter,tp,LOCATION_GRAVE,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
		e:SetProperty(0)
	end
end
function c74153887.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==1 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SendtoHand(tc,nil,REASON_EFFECT)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c74153887.filter,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end

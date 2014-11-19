--サイバー・リペア・プラント
function c86686671.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,86686671+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c86686671.condition)
	e1:SetTarget(c86686671.target)
	e1:SetOperation(c86686671.activate)
	c:RegisterEffect(e1)
end
function c86686671.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(Card.IsCode,tp,LOCATION_GRAVE,0,1,nil,70095154)
end
function c86686671.filter1(c)
	return c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToHand()
end
function c86686671.filter2(c)
	return c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToDeck()
end
function c86686671.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()~=0 then
			return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c86686671.filter2(chkc)
		else return false end
	end
	local b1=Duel.IsExistingMatchingCard(c86686671.filter1,tp,LOCATION_DECK,0,1,nil)
	local b2=Duel.IsExistingTarget(c86686671.filter2,tp,LOCATION_GRAVE,0,1,nil)
	if chk==0 then return b1 or b2 end
	local op=0
	if b1 and b2 then
		if Duel.IsExistingMatchingCard(Card.IsCode,tp,LOCATION_GRAVE,0,3,nil,70095154) then
			op=Duel.SelectOption(tp,aux.Stringid(86686671,0),aux.Stringid(86686671,1),aux.Stringid(86686671,2))
		else
			op=Duel.SelectOption(tp,aux.Stringid(86686671,0),aux.Stringid(86686671,1))
		end
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(86686671,0))
	else
		op=Duel.SelectOption(tp,aux.Stringid(86686671,1))+1
	end
	e:SetLabel(op)
	if op~=0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local g=Duel.SelectTarget(tp,c86686671.filter2,tp,LOCATION_GRAVE,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else
		e:SetProperty(0)
	end
end
function c86686671.activate(e,tp,eg,ep,ev,re,r,rp)
	local op=e:GetLabel()
	if op~=1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c86686671.filter1,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
	if op~=0 then
		local tc=Duel.GetFirstTarget()
		if tc:IsRelateToEffect(e) then
			if op==2 then Duel.BreakEffect() end
			Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
		end
	end
end

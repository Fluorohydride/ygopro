--明と宵の逆転
function c3160805.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c3160805.target1)
	e1:SetOperation(c3160805.operation)
	c:RegisterEffect(e1)
	--instant
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(3160805,0))
	e2:SetCategory(CATEGORY_TOGRAVE+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,TIMING_END_PHASE)
	e2:SetCost(c3160805.cost2)
	e2:SetTarget(c3160805.target2)
	e2:SetOperation(c3160805.operation)
	c:RegisterEffect(e2)
end
function c3160805.tgfilter1(c,tp)
	return c:IsRace(RACE_WARRIOR) and c:IsAttribute(ATTRIBUTE_LIGHT)
		and Duel.IsExistingMatchingCard(c3160805.thfilter1,tp,LOCATION_DECK,0,1,nil,c:GetLevel())
end
function c3160805.thfilter1(c,lv)
	return c:GetLevel()==lv and c:IsRace(RACE_WARRIOR) and c:IsAttribute(ATTRIBUTE_DARK) and c:IsAbleToHand()
end
function c3160805.tgfilter2(c,tp)
	return c:IsRace(RACE_WARRIOR) and c:IsAttribute(ATTRIBUTE_DARK)
		and Duel.IsExistingMatchingCard(c3160805.thfilter2,tp,LOCATION_DECK,0,1,nil,c:GetLevel())
end
function c3160805.thfilter2(c,lv)
	return c:GetLevel()==lv and c:IsRace(RACE_WARRIOR) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToHand()
end
function c3160805.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local b1=Duel.IsExistingMatchingCard(c3160805.tgfilter1,tp,LOCATION_HAND,0,1,nil,tp)
	local b2=Duel.IsExistingMatchingCard(c3160805.tgfilter2,tp,LOCATION_HAND,0,1,nil,tp)
	local op=2
	if Duel.GetFlagEffect(tp,3160805)==0 and (b1 or b2) and Duel.SelectYesNo(tp,aux.Stringid(3160805,1)) then
		if b1 and b2 then
			op=Duel.SelectOption(tp,aux.Stringid(3160805,2),aux.Stringid(3160805,3))
		elseif b1 then
			op=Duel.SelectOption(tp,aux.Stringid(3160805,2))
		else
			op=Duel.SelectOption(tp,aux.Stringid(3160805,3))+1
		end
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
		Duel.RegisterFlagEffect(tp,3160805,RESET_PHASE+PHASE_END,0,1)
	end
	e:SetLabel(op)
end
function c3160805.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==2 or not e:GetHandler():IsRelateToEffect(e) then return end
	if e:GetLabel()==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,c3160805.tgfilter1,tp,LOCATION_HAND,0,1,1,nil,tp)
		if Duel.SendtoGrave(g,REASON_EFFECT)~=0 then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local tg=Duel.SelectMatchingCard(tp,c3160805.thfilter1,tp,LOCATION_DECK,0,1,1,nil,g:GetFirst():GetLevel())
			Duel.SendtoHand(tg,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,tg)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,c3160805.tgfilter2,tp,LOCATION_HAND,0,1,1,nil,tp)
		if Duel.SendtoGrave(g,REASON_EFFECT)~=0 then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local tg=Duel.SelectMatchingCard(tp,c3160805.thfilter2,tp,LOCATION_DECK,0,1,1,nil,g:GetFirst():GetLevel())
			Duel.SendtoHand(tg,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,tg)
		end
	end
end
function c3160805.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,3160805)==0 end
	Duel.RegisterFlagEffect(tp,3160805,RESET_PHASE+PHASE_END,0,1)
end
function c3160805.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	local b1=Duel.IsExistingMatchingCard(c3160805.tgfilter1,tp,LOCATION_HAND,0,1,nil,tp)
	local b2=Duel.IsExistingMatchingCard(c3160805.tgfilter2,tp,LOCATION_HAND,0,1,nil,tp)
	if chk==0 then return b1 or b2 end
	local op=0
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(3160805,2),aux.Stringid(3160805,3))
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(3160805,2))
	else
		op=Duel.SelectOption(tp,aux.Stringid(3160805,3))+1
	end
	e:SetLabel(op)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end

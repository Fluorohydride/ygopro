--マドルチェ・シャトー
function c14001430.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c14001430.target)
	e1:SetOperation(c14001430.activate)
	c:RegisterEffect(e1)
	--atkup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x71))
	e2:SetValue(500)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e3)
	--destroy
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EFFECT_SEND_REPLACE)
	e4:SetTarget(c14001430.reptg)
	e4:SetValue(c14001430.repval)
	c:RegisterEffect(e4)
end
function c14001430.tdfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsSetCard(0x71) and c:IsAbleToDeck()
end
function c14001430.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(c14001430.tdfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c14001430.activate(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetMatchingGroup(c14001430.tdfilter,tp,LOCATION_GRAVE,0,nil)
	if g:GetCount()>0 then
		Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	end
end
function c14001430.repfilter(c,tp)
	return c:IsControler(tp) and c:IsLocation(LOCATION_GRAVE) and c:GetDestination()==LOCATION_DECK and c:IsType(TYPE_MONSTER)
		and c:IsAbleToHand()
end
function c14001430.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return bit.band(r,REASON_EFFECT)~=0 and re and re:IsActiveType(TYPE_MONSTER)
		and re:GetHandler():IsSetCard(0x71) and eg:IsExists(c14001430.repfilter,1,nil,tp) end
	if Duel.SelectYesNo(tp,aux.Stringid(14001430,0)) then
		local g=eg:Filter(c14001430.repfilter,nil,tp)
		local ct=g:GetCount()
		if ct>1 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
			g=g:Select(tp,1,ct,nil)
		end
		local tc=g:GetFirst()
		while tc do
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_TO_DECK_REDIRECT)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetValue(LOCATION_HAND)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e1)
			tc:RegisterFlagEffect(14001430,RESET_EVENT+0x1de0000+RESET_PHASE+PHASE_END,0,1)
			tc=g:GetNext()
		end
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e1:SetCode(EVENT_TO_HAND)
		e1:SetCountLimit(1)
		e1:SetCondition(c14001430.thcon)
		e1:SetOperation(c14001430.thop)
		e1:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(e1,tp)
		return true
	else return false end
end
function c14001430.repval(e,c)
	return false
end
function c14001430.thfilter(c)
	return c:GetFlagEffect(14001430)~=0
end
function c14001430.thcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c14001430.thfilter,1,nil)
end
function c14001430.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c14001430.thfilter,nil)
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
end

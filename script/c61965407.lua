--遡洸する煉獄
function c61965407.initial_effect(c)
	Duel.EnableGlobalFlag(GLOBALFLAG_SELF_TOGRAVE)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_STANDBY_PHASE,TIMING_STANDBY_PHASE)
	e1:SetTarget(c61965407.target)
	e1:SetOperation(c61965407.operation)
	e1:SetLabel(0)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c61965407.thcon)
	e2:SetCost(c61965407.thcost)
	e2:SetTarget(c61965407.thtg)
	e2:SetOperation(c61965407.operation)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	--tograve
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_TOGRAVE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCondition(c61965407.tgcon)
	e3:SetCost(c61965407.tgcost)
	e3:SetTarget(c61965407.tgtg)
	e3:SetOperation(c61965407.operation)
	e3:SetLabel(2)
	c:RegisterEffect(e3)
	--tograve
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EFFECT_SELF_TOGRAVE)
	e4:SetCondition(c61965407.sdcon)
	c:RegisterEffect(e4)
end
function c61965407.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()==1 then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c61965407.thfilter(chkc) end
		if e:GetLabel()==2 then return chkc:IsLocation(LOCATION_REMOVED) and chkc:IsControler(tp) and c61965407.tgfilter(chkc) end
	end
	if chk==0 then return true end
	if Duel.GetTurnPlayer()~=tp and Duel.GetCurrentPhase()==PHASE_STANDBY
		and Duel.IsExistingTarget(c61965407.thfilter,tp,LOCATION_GRAVE,0,1,nil)
		and Duel.GetFlagEffect(tp,61965407)==0
		and Duel.SelectYesNo(tp,aux.Stringid(61965407,0)) then
		e:SetCategory(CATEGORY_TOHAND)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectTarget(tp,c61965407.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
		Duel.RegisterFlagEffect(tp,61965407,RESET_PHASE+PHASE_END,0,1)
		e:SetLabel(1)
	elseif Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()==PHASE_STANDBY
		and Duel.IsExistingTarget(c61965407.tgfilter,tp,LOCATION_REMOVED,0,1,nil)
		and Duel.GetFlagEffect(tp,61965408)==0
		and Duel.SelectYesNo(tp,aux.Stringid(61965407,0)) then
		e:SetCategory(CATEGORY_TOGRAVE)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectTarget(tp,c61965407.tgfilter,tp,LOCATION_REMOVED,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,1,0,0)
		Duel.RegisterFlagEffect(tp,61965408,RESET_PHASE+PHASE_END,0,1)
		e:SetLabel(2)
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetLabel(0)
	end
end
function c61965407.thcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c61965407.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,61965407)==0 end
	Duel.RegisterFlagEffect(tp,61965407,RESET_PHASE+PHASE_END,0,1)
end
function c61965407.thfilter(c)
	return c:IsSetCard(0xbb) and c:IsAbleToHand()
end
function c61965407.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c61965407.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c61965407.thfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c61965407.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c61965407.tgcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c61965407.tgcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,61965408)==0 end
	Duel.RegisterFlagEffect(tp,61965408,RESET_PHASE+PHASE_END,0,1)
end
function c61965407.tgfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xbb)
end
function c61965407.tgtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_REMOVED) and chkc:IsControler(tp) and c61965407.tgfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c61965407.tgfilter,tp,LOCATION_REMOVED,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectTarget(tp,c61965407.tgfilter,tp,LOCATION_REMOVED,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,1,0,0)
end
function c61965407.operation(e,tp,eg,ep,ev,re,r,rp)
	if c61965407.sdcon(e) then return end
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) then return end
	if e:GetLabel()==1 then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	else
		Duel.SendtoGrave(tc,REASON_EFFECT+REASON_RETURN)
	end
end
function c61965407.sdfilter(c)
	return c:IsFacedown() or not c:IsSetCard(0xbb)
end
function c61965407.sdcon(e)
	return Duel.IsExistingMatchingCard(c61965407.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end

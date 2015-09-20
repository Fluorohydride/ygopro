--潤いの風
function c92266279.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c92266279.target)
	c:RegisterEffect(e1)
	--to hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(92266279,1))
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,TIMING_END_PHASE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,92266279)
	e2:SetCost(c92266279.thcost)
	e2:SetTarget(c92266279.thtg)
	e2:SetOperation(c92266279.thop)
	c:RegisterEffect(e2)
	--recover
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(92266279,2))
	e3:SetCategory(CATEGORY_RECOVER)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetHintTiming(0,TIMING_END_PHASE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetCountLimit(1,92266280)
	e3:SetCondition(c92266279.reccon)
	e3:SetTarget(c92266279.rectg)
	e3:SetOperation(c92266279.recop)
	c:RegisterEffect(e3)
end
function c92266279.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local b1=c92266279.thcost(e,tp,eg,ep,ev,re,r,rp,0) and c92266279.thtg(e,tp,eg,ep,ev,re,r,rp,0)
	local b2=c92266279.reccon(e,tp,eg,ep,ev,re,r,rp) and c92266279.rectg(e,tp,eg,ep,ev,re,r,rp,0)
	if (b1 or b2) and Duel.SelectYesNo(tp,94) then
		local opt=0
		if b1 and b2 then
			opt=Duel.SelectOption(tp,aux.Stringid(92266279,1),aux.Stringid(92266279,2))
		elseif b1 then
			opt=Duel.SelectOption(tp,aux.Stringid(92266279,1))
		else
			opt=Duel.SelectOption(tp,aux.Stringid(92266279,2))+1
		end
		if opt==0 then
			e:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
			e:SetProperty(0)
			e:SetOperation(c92266279.thop)
			c92266279.thcost(e,tp,eg,ep,ev,re,r,rp,1)
			c92266279.thtg(e,tp,eg,ep,ev,re,r,rp,1)
		else
			e:SetCategory(CATEGORY_RECOVER)
			e:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
			e:SetOperation(c92266279.recop)
			c92266279.rectg(e,tp,eg,ep,ev,re,r,rp,1)
		end
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetOperation(nil)
	end
end
function c92266279.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) and Duel.GetFlagEffect(tp,92266279)==0 end
	Duel.PayLPCost(tp,1000)
	Duel.RegisterFlagEffect(tp,92266279,RESET_PHASE+RESET_END,0,1)
end
function c92266279.thfilter(c)
	return c:IsSetCard(0xc9) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c92266279.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c92266279.thfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c92266279.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c92266279.thfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
function c92266279.reccon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<Duel.GetLP(1-tp)
end
function c92266279.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,92266280)==0 end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,500)
	Duel.RegisterFlagEffect(tp,92266280,RESET_PHASE+RESET_END,0,1)
end
function c92266279.recop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end

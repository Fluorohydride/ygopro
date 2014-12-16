--殻醒する煉獄
function c94432298.initial_effect(c)
	Duel.EnableGlobalFlag(GLOBALFLAG_SELF_TOGRAVE)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_STANDBY_PHASE,0)
	e1:SetTarget(c94432298.tgtg1)
	e1:SetOperation(c94432298.tgop)
	c:RegisterEffect(e1)
	--tograve
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCountLimit(1)
	e2:SetCondition(c94432298.tgcon)
	e2:SetCost(c94432298.tgcost)
	e2:SetTarget(c94432298.tgtg2)
	e2:SetOperation(c94432298.tgop)
	c:RegisterEffect(e2)
	--tograve
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EFFECT_SELF_TOGRAVE)
	e3:SetCondition(c94432298.sdcon)
	c:RegisterEffect(e3)
end
function c94432298.tgtg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()==PHASE_STANDBY
		and Duel.IsExistingMatchingCard(c94432298.filter,tp,LOCATION_DECK,0,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(94432298,0)) then
		e:SetCategory(CATEGORY_TOGRAVE)
		e:GetHandler():RegisterFlagEffect(94432298,RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END,0,1)
		e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(94432298,1))
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
	else
		e:SetCategory(0)
	end
end
function c94432298.tgcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c94432298.tgcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(94432298)==0 end
	e:GetHandler():RegisterFlagEffect(94432298,RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END,0,1)
end
function c94432298.filter(c)
	return c:IsSetCard(0xbb) and c:IsAbleToGrave()
end
function c94432298.tgtg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c94432298.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c94432298.tgop(e,tp,eg,ep,ev,re,r,rp)
	if c94432298.sdcon(e) then return end
	if e:GetHandler():GetFlagEffect(94432298)==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c94432298.filter,tp,LOCATION_DECK,0,1,2,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
function c94432298.sdfilter(c)
	return c:IsFacedown() or not c:IsSetCard(0xbb)
end
function c94432298.sdcon(e)
	return Duel.IsExistingMatchingCard(c94432298.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end

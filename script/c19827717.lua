--死者の生還
function c19827717.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c19827717.cost)
	e1:SetOperation(c19827717.activate)
	c:RegisterEffect(e1)
end
function c19827717.costfilter(c)
	return c:IsDiscardable() and c:IsAbleToGraveAsCost() and c:IsType(TYPE_MONSTER)
end
function c19827717.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c19827717.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c19827717.costfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c19827717.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetDescription(aux.Stringid(19827717,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetCondition(c19827717.retcon)
	e1:SetTarget(c19827717.rettg)
	e1:SetOperation(c19827717.retop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c19827717.filter(c,tid)
	return c:IsAbleToHand() and c:IsType(TYPE_MONSTER) and c:GetTurnID()==tid and c:IsReason(REASON_BATTLE)
end
function c19827717.retcon(e,tp,eg,ep,ev,re,r,rp)
	local tid=Duel.GetTurnCount()
	return Duel.IsExistingMatchingCard(c19827717.filter,tp,LOCATION_GRAVE,0,1,nil,tid)
end
function c19827717.rettg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local tid=Duel.GetTurnCount()
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c19827717.filter(chkc,tid) end
	if chk==0 then return Duel.IsExistingTarget(c19827717.filter,tp,LOCATION_GRAVE,0,1,nil,tid) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c19827717.filter,tp,LOCATION_GRAVE,0,1,1,nil,tid)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c19827717.retop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end

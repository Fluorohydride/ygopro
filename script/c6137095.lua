--誤作動
function c6137095.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c6137095.condition)
	e1:SetCost(c6137095.cost)
	e1:SetTarget(c6137095.target)
	e1:SetOperation(c6137095.activate)
	c:RegisterEffect(e1)
end
function c6137095.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsActiveType(TYPE_TRAP) and re:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.IsChainNegatable(ev)
end
function c6137095.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500) end
	Duel.PayLPCost(tp,500)
end
function c6137095.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not re:GetHandler():IsStatus(STATUS_ACT_FROM_HAND) then
			return re:GetHandler():IsCanTurnSet()
		else return true end
	end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsRelateToEffect(re) and re:GetHandler():IsStatus(STATUS_ACT_FROM_HAND) then
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,eg,1,0,0)
	end
end
function c6137095.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local rc=re:GetHandler()
	if rc:IsRelateToEffect(re) then
		if rc:IsStatus(STATUS_ACT_FROM_HAND) then
			rc:CancelToGrave()
			Duel.SendtoHand(rc,nil,REASON_EFFECT)
		else
			if rc:IsCanTurnSet() then
				rc:CancelToGrave()
				Duel.ChangePosition(rc,POS_FACEDOWN)
				rc:SetStatus(STATUS_SET_TURN,false)
				Duel.RaiseEvent(rc,EVENT_SSET,e,REASON_EFFECT,tp,tp,0)
			end
		end
	end
end

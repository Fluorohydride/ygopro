--破滅の儀式
function c52913738.initial_effect(c)
	aux.AddRitualProcGreater(c,aux.FilterBoolFunction(Card.IsCode,30646525))
	--To Deck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(52913738,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCondition(c52913738.regcon)
	e1:SetCost(c52913738.regcost)
	e1:SetOperation(c52913738.regop)
	c:RegisterEffect(e1)
end
function c52913738.regcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c52913738.regcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c52913738.regop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLED)
	e1:SetOperation(c52913738.tdop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c52913738.tdop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if d and a:IsType(TYPE_RITUAL) and d:IsStatus(STATUS_BATTLE_DESTROYED) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetCode(EFFECT_SEND_REPLACE)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e1:SetTarget(c52913738.reptg)
		e1:SetOperation(c52913738.repop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE)
		d:RegisterEffect(e1)
	end
end
function c52913738.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:GetDestination()==LOCATION_GRAVE and c:IsReason(REASON_BATTLE) end
	return true
end
function c52913738.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SendtoDeck(e:GetHandler(),nil,0,REASON_EFFECT)
end

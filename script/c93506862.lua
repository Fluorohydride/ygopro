--リチュア・ヴァニティ
function c93506862.initial_effect(c)
	--act limit
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(93506862,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c93506862.cost)
	e1:SetOperation(c93506862.operation)
	c:RegisterEffect(e1)
end
function c93506862.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsDiscardable() end
	Duel.SendtoGrave(c,REASON_COST+REASON_DISCARD)
end
function c93506862.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_CHAINING)
	e1:SetOperation(c93506862.chainop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetOperation(c93506862.sucop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
	local e3=Effect.CreateEffect(e:GetHandler())
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_CHAIN_END)
	e3:SetOperation(c93506862.cedop)
	e3:SetReset(RESET_PHASE+PHASE_END)
	e3:SetLabelObject(e2)
	Duel.RegisterEffect(e3,tp)
end
function c93506862.chainop(e,tp,eg,ep,ev,re,r,rp)
	local rc=re:GetHandler()
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and rc:IsSetCard(0x3a) and rc:IsType(TYPE_RITUAL) then
		Duel.SetChainLimit(c93506862.chainlm)
	end
end
function c93506862.chainlm(e,rp,tp)
	return tp==rp
end
function c93506862.sucfilter(c)
	return c:IsSetCard(0x3a) and c:IsType(TYPE_RITUAL) and bit.band(c:GetSummonType(),SUMMON_TYPE_RITUAL)==SUMMON_TYPE_RITUAL
end
function c93506862.sucop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c93506862.sucfilter,1,nil) then
		e:SetLabel(1)
	else e:SetLabel(0) end
end
function c93506862.cedop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS) and e:GetLabelObject():GetLabel()==1 then
		Duel.SetChainLimitTillChainEnd(c93506862.chainlm)
	end
end

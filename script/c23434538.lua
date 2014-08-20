--増殖するG
function c23434538.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(23434538,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_STANDBY_PHASE+0x1c0)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,23434538)
	e1:SetCost(c23434538.cost)
	e1:SetOperation(c23434538.operation)
	c:RegisterEffect(e1)
end
function c23434538.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c23434538.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetOperation(c23434538.drop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c23434538.filter(c,sp)
	return c:GetSummonPlayer()==sp
end
function c23434538.drop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c23434538.filter,1,nil,1-tp) then
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end

--ガントレット・ウォリアー
function c79337169.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(79337169,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c79337169.condition)
	e1:SetCost(c79337169.cost)
	e1:SetTarget(c79337169.target)
	e1:SetOperation(c79337169.operation)
	c:RegisterEffect(e1)
end
function c79337169.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c79337169.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c79337169.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_WARRIOR)
end
function c79337169.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c79337169.filter,tp,LOCATION_MZONE,0,1,e:GetHandler()) end
end
function c79337169.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c79337169.filter,tp,LOCATION_MZONE,0,e:GetHandler())
	local tc=g:GetFirst()
	local c=e:GetHandler()
	while tc do
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		e2:SetLabelObject(e1)
		tc:RegisterEffect(e2)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e3:SetCode(EVENT_DAMAGE_STEP_END)
		e3:SetOperation(c79337169.resetop)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		e3:SetLabelObject(e2)
		tc:RegisterEffect(e3)
		tc=g:GetNext()
	end
end
function c79337169.resetop(e,tp,eg,ep,ev,re,r,rp)
	local e1=e:GetLabelObject()
	local e2=e1:GetLabelObject()
	e1:Reset()
	e2:Reset()
	e:Reset()
end

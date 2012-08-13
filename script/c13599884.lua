--鉄のサソリ
function c13599884.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(13599884,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetCondition(c13599884.condition)
	e1:SetOperation(c13599884.operation)
	c:RegisterEffect(e1)
end
function c13599884.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler()==Duel.GetAttackTarget() and not Duel.GetAttacker():IsRace(RACE_MACHINE)
end
function c13599884.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetAttacker()
	if tc:IsRelateToBattle() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetRange(LOCATION_MZONE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c13599884.descon)
		e1:SetOperation(c13599884.desop)
		e1:SetLabel(0)
		e1:SetOwnerPlayer(tp)
		tc:RegisterEffect(e1)
	end
end
function c13599884.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetOwnerPlayer()~=Duel.GetTurnPlayer()
end
function c13599884.desop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	ct=ct+1
	e:SetLabel(ct)
	e:GetOwner():SetTurnCounter(ct)
	if ct==3 then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end

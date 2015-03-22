--儀式魔人カースエンチャンター
function c77153811.initial_effect(c)
	--ritual material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_RITUAL_MATERIAL)
	c:RegisterEffect(e1)
	--become material
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetCondition(c77153811.condition)
	e2:SetOperation(c77153811.operation)
	c:RegisterEffect(e2)
end
function c77153811.condition(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_RITUAL
end
function c77153811.operation(e,tp,eg,ep,ev,re,r,rp)
	local rc=e:GetHandler():GetReasonCard()
	if rc:GetFlagEffect(77153811)==0 then
		--cannot special summon
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(77153811,0))
		e1:SetProperty(EFFECT_FLAG_CLIENT_HINT)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetRange(LOCATION_MZONE)
		e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
		e1:SetTarget(c77153811.distg)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		rc:RegisterEffect(e1,true)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_CHAIN_SOLVING)
		e2:SetRange(LOCATION_MZONE)
		e2:SetOperation(c77153811.disop)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		rc:RegisterEffect(e2,true)
		rc:RegisterFlagEffect(77153811,RESET_EVENT+0x1fe0000,0,1)
	end
end
function c77153811.distg(e,c)
	return c:IsType(TYPE_SYNCHRO)
end
function c77153811.disop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_SYNCHRO) then
		Duel.NegateEffect(ev)
	end
end

--魔法族の聖域
function c25407643.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_SELF_DESTROY)
	e2:SetCondition(c25407643.descon)
	c:RegisterEffect(e2)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(25407643,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c25407643.condition)
	e2:SetTarget(c25407643.target)
	e2:SetOperation(c25407643.operation)
	c:RegisterEffect(e2)
	local e4=e2:Clone()
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e4)
end
function c25407643.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_SPELLCASTER)
end
function c25407643.descon(e)
	return not Duel.IsExistingMatchingCard(c25407643.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c25407643.cfilter1(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL)
end
function c25407643.cfilter2(c,tp)
	return c:IsFaceup() and not c:IsRace(RACE_SPELLCASTER) and c:IsControler(tp)
end
function c25407643.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c25407643.cfilter1,tp,LOCATION_SZONE,0,1,e:GetHandler())
		and not Duel.IsExistingMatchingCard(c25407643.cfilter1,tp,0,LOCATION_SZONE,1,nil)
		and eg:IsExists(c25407643.cfilter2,1,nil,1-tp)
end
function c25407643.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetTargetCard(eg)
end
function c25407643.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if not Duel.IsExistingMatchingCard(c25407643.cfilter1,tp,LOCATION_SZONE,0,1,e:GetHandler())
		or Duel.IsExistingMatchingCard(c25407643.cfilter1,tp,0,LOCATION_SZONE,1,nil) then return end
	local tc=eg:GetFirst()
	local c=e:GetHandler()
	while tc do
		if tc:IsRelateToEffect(e) and c25407643.cfilter2(tc,1-tp) then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e1)
			local e2=Effect.CreateEffect(c)
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_CANNOT_TRIGGER)
			e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e2)
		end
		tc=eg:GetNext()
	end
end

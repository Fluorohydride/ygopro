--怒れるもけもけ
function c1965724.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--reg
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetCondition(c1965724.regcon)
	e2:SetOperation(c1965724.regop)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_SET_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetTarget(c1965724.atktg)
	e3:SetCondition(c1965724.atkcon)
	e3:SetValue(3000)
	c:RegisterEffect(e3)
end
function c1965724.cfilter(c,tp)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:IsPreviousPosition(POS_FACEUP)
		and c:GetPreviousControler()==tp and c:IsRace(RACE_FAIRY)
end
function c1965724.cfilter2(c)
	return c:IsFaceup() and c:IsCode(27288416)
end
function c1965724.regcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(1965724)==0 and eg:IsExists(c1965724.cfilter,1,nil,tp)
		and Duel.IsExistingMatchingCard(c1965724.cfilter2,tp,LOCATION_MZONE,0,1,nil)
end
function c1965724.regop(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():RegisterFlagEffect(1965724,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c1965724.atkcon(e)
	return e:GetHandler():GetFlagEffect(1965724)~=0
end
function c1965724.atktg(e,c)
	return c:IsFaceup() and c:IsCode(27288416)
end

--血の沼地
function c60627999.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c60627999.target)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_SELF_DESTROY)
	e2:SetCondition(c60627999.sdescon)
	c:RegisterEffect(e2)
	--disable spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e3:SetCode(EFFECT_CANNOT_TRIGGER)
	e3:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
	e3:SetTarget(c60627999.acttg)
	c:RegisterEffect(e3)
end
function c60627999.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local c=e:GetHandler()
	c:SetTurnCounter(0)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCondition(c60627999.descon)
	e1:SetOperation(c60627999.desop)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN,2)
	c:RegisterEffect(e1)
end
function c60627999.descon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c60627999.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ct=c:GetTurnCounter()
	ct=ct+1
	c:SetTurnCounter(ct)
	if ct==2 then
		Duel.Destroy(c,REASON_EFFECT)
	end
end
function c60627999.sdescon(e)
	return Duel.IsExistingMatchingCard(Card.IsType,e:GetHandlerPlayer(),LOCATION_ONFIELD,0,1,e:GetHandler(),TYPE_SPELL+TYPE_TRAP)
end
function c60627999.acttg(e,c)
	return c:IsFacedown() and c:GetSequence()~=5
end

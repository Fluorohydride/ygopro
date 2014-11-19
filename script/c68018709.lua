--失楽の聖女
function c68018709.initial_effect(c)
	--act qp in hand
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_QP_ACT_IN_NTPHAND)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetCondition(c68018709.actcon)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAINING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c68018709.checkop)
	c:RegisterEffect(e2)
end
function c68018709.actcon(e)
	local tp=e:GetHandlerPlayer()
	return Duel.GetFlagEffect(tp,68018709)==0
end
function c68018709.checkop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()==tp then return end
	if re:GetActiveType()==TYPE_QUICKPLAY+TYPE_SPELL and re:IsHasType(EFFECT_TYPE_ACTIVATE)
		and re:GetHandler():IsStatus(STATUS_ACT_FROM_HAND) then
		Duel.RegisterFlagEffect(tp,68018709,0,0,0)
	end
end

--魔力の棘
function c53119267.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetOperation(c53119267.damop)
	c:RegisterEffect(e2)
end
function c53119267.filter(c,tp)
	return c:IsPreviousLocation(LOCATION_HAND) and c:GetControler()~=tp and c:GetPreviousControler()~=tp
		and c:IsReason(REASON_DISCARD)
end
function c53119267.damop(e,tp,eg,ep,ev,re,r,rp)
	local ct=eg:FilterCount(c53119267.filter,nil,tp)
	Duel.Damage(1-tp,ct*500,REASON_EFFECT)
end

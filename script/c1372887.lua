--相乗り
function c1372887.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,1372887+EFFECT_COUNT_CODE_OATH)
	e1:SetOperation(c1372887.activate)
	c:RegisterEffect(e1)
end
function c1372887.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCondition(c1372887.drcon1)
	e1:SetOperation(c1372887.drop1)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_TO_HAND)
	e2:SetCondition(c1372887.regcon)
	e2:SetOperation(c1372887.regop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetCode(EVENT_CHAIN_SOLVED)
	e3:SetCondition(c1372887.drcon2)
	e3:SetOperation(c1372887.drop2)
	e3:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e3,tp)
end
function c1372887.cfilter(c,tp)
	return c:IsControler(1-tp) and not c:IsReason(REASON_DRAW) and c:IsPreviousLocation(LOCATION_DECK+LOCATION_GRAVE)
end
function c1372887.drcon1(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c1372887.cfilter,1,nil,tp) 
		and (not re:IsHasType(EFFECT_TYPE_ACTIONS) or re:IsHasType(EFFECT_TYPE_CONTINUOUS))
end
function c1372887.drop1(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_CARD,0,1372887)
	Duel.Draw(tp,1,REASON_EFFECT)
end
function c1372887.regcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c1372887.cfilter,1,nil,tp) and Duel.GetFlagEffect(tp,1372887)==0 
		and re:IsHasType(EFFECT_TYPE_ACTIONS) and not re:IsHasType(EFFECT_TYPE_CONTINUOUS)
end
function c1372887.regop(e,tp,eg,ep,ev,re,r,rp)
	Duel.RegisterFlagEffect(tp,1372887,RESET_CHAIN,0,1)
end
function c1372887.drcon2(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,1372887)>0
end
function c1372887.drop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.ResetFlagEffect(tp,1372887)
	Duel.Hint(HINT_CARD,0,1372887)
	Duel.Draw(tp,1,REASON_EFFECT)
end

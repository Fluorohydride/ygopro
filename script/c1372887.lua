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
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetLabel(0)
	e1:SetCondition(c1372887.drcon)
	e1:SetOperation(c1372887.drop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_CHAIN_SOLVED)
	e2:SetLabelObject(e1)
	e2:SetOperation(c1372887.rst)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
end
function c1372887.cfilter(c,tp)
	return c:IsControler(1-tp) and not c:IsReason(REASON_DRAW) and c:IsPreviousLocation(LOCATION_DECK+LOCATION_GRAVE)
end
function c1372887.drcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c1372887.cfilter,1,nil,tp)
end
function c1372887.drop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		Duel.Hint(HINT_CARD,0,1372887)
		Duel.Draw(tp,1,REASON_EFFECT)
		if Duel.GetCurrentChain()>0 then
			e:SetLabel(1)
		end
	end
end
function c1372887.rst(e,tp,eg,ep,ev,re,r,rp)
	e:GetLabelObject():SetLabel(0)
end

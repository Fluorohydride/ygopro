--バブルイリュージョン
function c80075749.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c80075749.condition)
	e1:SetOperation(c80075749.activate)
	c:RegisterEffect(e1)
end
function c80075749.filter(c)
	return c:IsFaceup() and c:IsCode(79979666)
end
function c80075749.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c80075749.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c80075749.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_TRAP_ACT_IN_HAND)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAINING)
	e2:SetOperation(c80075749.resetop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	e2:SetLabelObject(e1)
	Duel.RegisterEffect(e2,tp)
end
function c80075749.resetop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_TRAP) and re:IsHasType(EFFECT_TYPE_ACTIVATE)
		and re:GetHandler():GetTurnID()==Duel.GetTurnCount() then
		e:GetLabelObject():Reset()
		e:Reset()
	end
end

--펜듈럼 모라토리엄
function c60434189.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c60434189.activate)
	c:RegisterEffect(e1)
end
function c60434189.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c60434189.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetTargetRange(LOCATION_ONFIELD,0)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e1:SetTarget(c60434189.indtg)
	e1:SetValue(1)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)	
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetCondition(c60434189.discon)
	e2:SetOperation(c60434189.disop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	e2:SetLabelObject(tc)
	Duel.RegisterEffect(e2,tp)
end
function c60434189.indtg(e,c)
	return c:IsLocation(LOCATION_SZONE) and c:GetSequence()>5
end
function c60434189.indfilter(c,e)
	return c:IsLocation(LOCATION_SZONE) and c:GetSequence()>5
end
function c60434189.discon(e,tp,eg,ep,ev,re,r,rp)
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS):Filter(c60434189.indfilter,nil,e)
	return g and g:GetCount()>0 and ep~=tp
end
function c60434189.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateEffect(ev)
end
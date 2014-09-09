--トライアングルパワー
function c32298781.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DEFCHANGE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c32298781.target)
	e1:SetOperation(c32298781.activate)
	c:RegisterEffect(e1)
end
function c32298781.filter(c)
	local tpe=c:GetType()
	return c:IsFaceup() and bit.band(tpe,TYPE_NORMAL)~=0 and bit.band(tpe,TYPE_TOKEN)==0 and c:GetLevel()==1
end
function c32298781.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c32298781.filter,tp,LOCATION_MZONE,0,1,nil) end
end
function c32298781.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c32298781.filter,tp,LOCATION_MZONE,0,nil)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_BASE_ATTACK)
		e1:SetValue(tc:GetBaseAttack()+2000)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_SET_BASE_DEFENCE)
		e2:SetValue(tc:GetBaseDefence()+2000)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e2)
		tc=g:GetNext()
	end
	local de=Effect.CreateEffect(e:GetHandler())
	de:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	de:SetCode(EVENT_PHASE+PHASE_END)
	de:SetCountLimit(1)
	de:SetCondition(c32298781.descon)
	de:SetOperation(c32298781.desop)
	de:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(de,tp)
end
function c32298781.dfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_NORMAL) and c:GetLevel()==1
end
function c32298781.descon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c32298781.dfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c32298781.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c32298781.dfilter,tp,LOCATION_MZONE,0,nil)
	Duel.Destroy(g,REASON_EFFECT)
end

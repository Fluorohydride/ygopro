--デルタ・アタッカー
function c39719977.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c39719977.target)
	e1:SetOperation(c39719977.activate)
	c:RegisterEffect(e1)
end
function c39719977.filter(c,tp)
	local tpe=c:GetType()
	return c:IsFaceup() and bit.band(tpe,TYPE_NORMAL)~=0 and bit.band(tpe,TYPE_TOKEN)==0
		and Duel.IsExistingMatchingCard(c39719977.filter2,tp,LOCATION_MZONE,0,2,c,c:GetCode())
end
function c39719977.filter2(c,code)
	local tpe=c:GetType()
	return c:IsFaceup() and bit.band(tpe,TYPE_NORMAL)~=0 and bit.band(tpe,TYPE_TOKEN)==0 and c:GetCode()==code
end
function c39719977.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c39719977.filter,tp,LOCATION_MZONE,0,3,nil,tp) end
end
function c39719977.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c39719977.filter,tp,LOCATION_MZONE,0,nil,tp)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end

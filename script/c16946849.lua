--人造天使
function c16946849.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--token
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_CHAIN_SOLVED)
	e2:SetCondition(c16946849.tokencon)
	e2:SetOperation(c16946849.tokenop)
	c:RegisterEffect(e2)
end
function c16946849.tokencon(e,tp,eg,ep,ev,re,r,rp)
	return re:IsActiveType(TYPE_COUNTER) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,16946850,0,0x4011,300,300,1,RACE_FAIRY,ATTRIBUTE_LIGHT)
end
function c16946849.tokenop(e,tp,eg,ep,ev,re,r,rp)
	local token=Duel.CreateToken(tp,16946850)
	Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
end

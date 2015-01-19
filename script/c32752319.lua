--ユーフォロイド・ファイター
function c32752319.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCodeFun(c,7602840,aux.FilterBoolFunction(Card.IsRace,RACE_WARRIOR),1,false,false)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c32752319.atkcon)
	e1:SetOperation(c32752319.atkop)
	c:RegisterEffect(e1)
end
function c32752319.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c32752319.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=c:GetMaterial()
	local tc=g:GetFirst()
	local atk=0
	while tc do
		local catk=tc:GetBaseAttack()
		if catk<0 then catk=0 end
		atk=atk+catk
		tc=g:GetNext()
	end
	if atk~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_BASE_ATTACK)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_SET_BASE_DEFENCE)
		c:RegisterEffect(e2)
	end
end

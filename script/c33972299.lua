--ジオ・ジェネクス
function c33972299.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsCode,68505803),aux.NonTuner(Card.IsAttribute,ATTRIBUTE_EARTH),1)
	c:EnableReviveLimit()
	--ad change
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(33972299,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c33972299.condition)
	e1:SetOperation(c33972299.operation)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetCode(EFFECT_SET_BASE_ATTACK)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c33972299.valcon)
	e2:SetValue(2800)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SET_BASE_DEFENCE)
	e3:SetValue(1800)
	c:RegisterEffect(e3)
end
function c33972299.valcon(e)
	return e:GetHandler():GetFlagEffect(33972299)~=0
		and Duel.IsExistingMatchingCard(c33972299.cfilter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,nil)
end
function c33972299.cfilter(c)
	return c:IsLevelBelow(4) and c:IsFaceup() and c:IsSetCard(0x2)
end
function c33972299.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c33972299.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c33972299.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		c:RegisterFlagEffect(33972299,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end

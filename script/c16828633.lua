--スペア・ジェネクス
function c16828633.initial_effect(c)
	--cos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(16828633,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c16828633.condition)
	e1:SetOperation(c16828633.operation)
	c:RegisterEffect(e1)
end
function c16828633.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x2)
end
function c16828633.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c16828633.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c16828633.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CHANGE_CODE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e1:SetValue(68505803)
	c:RegisterEffect(e1)
end

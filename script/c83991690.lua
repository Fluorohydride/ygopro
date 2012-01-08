--ブーテン
function c83991690.initial_effect(c)
	--tuner
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(83991690,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c83991690.cost)
	e1:SetTarget(c83991690.target)
	e1:SetOperation(c83991690.operation)
	c:RegisterEffect(e1)
end
function c83991690.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c83991690.filter(c)
	return c:IsFaceup() and c:IsLevelBelow(4) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsRace(RACE_FAIRY) and not c:IsType(TYPE_TUNER)
end
function c83991690.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c83991690.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c83991690.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c83991690.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c83991690.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_ADD_TYPE)
		e1:SetValue(TYPE_TUNER)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
	end
end

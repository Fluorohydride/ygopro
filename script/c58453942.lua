--サイコ・ウォールド
function c58453942.initial_effect(c)
	--extra atk
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(58453942,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c58453942.condition)
	e1:SetCost(c58453942.cost)
	e1:SetTarget(c58453942.target)
	e1:SetOperation(c58453942.operation)
	c:RegisterEffect(e1)
end
function c58453942.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsAbleToEnterBP()
end
function c58453942.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_OATH)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
end
function c58453942.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_PSYCHO) and c:GetEffectCount(EFFECT_EXTRA_ATTACK)==0
end
function c58453942.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c58453942.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c58453942.filter,tp,LOCATION_MZONE,0,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c58453942.filter,tp,LOCATION_MZONE,0,1,1,e:GetHandler())
end
function c58453942.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	--extra atk
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EFFECT_EXTRA_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
	end
end

--救世の儀式
function c60234913.initial_effect(c)
	aux.AddRitualProcGreater(c,aux.FilterBoolFunction(Card.IsCode,61757117))
	--untargetable
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60234913,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c60234913.utcost)
	e1:SetTarget(c60234913.uttg)
	e1:SetOperation(c60234913.utop)
	c:RegisterEffect(e1)
end
function c60234913.utcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c60234913.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_RITUAL)
end
function c60234913.uttg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c60234913.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c60234913.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c60234913.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c60234913.utop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		e1:SetValue(aux.tgval)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end

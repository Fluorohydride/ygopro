--ジェムナイト・アイオーラ
function c45662855.initial_effect(c)
	aux.EnableDualAttribute(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetDescription(aux.Stringid(45662855,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(aux.IsDualState)
	e1:SetCost(c45662855.cost)
	e1:SetTarget(c45662855.target)
	e1:SetOperation(c45662855.operation)
	c:RegisterEffect(e1)
end
function c45662855.costfilter(c,tp)
	return c:IsSetCard(0x47) and c:IsType(TYPE_MONSTER) and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingTarget(c45662855.tgfilter,tp,LOCATION_GRAVE,0,1,c)
end
function c45662855.tgfilter(c)
	return c:IsSetCard(0x1047) and c:IsAbleToHand()
end
function c45662855.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c45662855.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c45662855.tgfilter(chkc) end
	if chk==0 then
		if e:GetLabel()==1 then
			e:SetLabel(0)
			return Duel.IsExistingMatchingCard(c45662855.costfilter,tp,LOCATION_GRAVE,0,1,nil,tp)
		else
			return Duel.IsExistingTarget(c45662855.tgfilter,tp,LOCATION_GRAVE,0,1,nil)
		end
	end
	if e:GetLabel()==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local cg=Duel.SelectMatchingCard(tp,c45662855.costfilter,tp,LOCATION_GRAVE,0,1,1,nil,tp)
		Duel.Remove(cg,POS_FACEUP,REASON_COST)
		e:SetLabel(0)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c45662855.tgfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c45662855.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end

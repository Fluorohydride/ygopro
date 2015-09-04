--ロスト・ネクスト
function c7076131.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c7076131.target)
	e1:SetOperation(c7076131.activate)
	c:RegisterEffect(e1)
end
function c7076131.filter(c,code)
	return c:IsCode(code) and c:IsAbleToGrave()
end
function c7076131.tgfilter(c,tp)
	return c:IsFaceup() and Duel.IsExistingMatchingCard(c7076131.filter,tp,LOCATION_DECK,0,1,nil,c:GetCode())
end
function c7076131.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c7076131.tgfilter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c7076131.tgfilter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c7076131.tgfilter,tp,LOCATION_MZONE,0,1,1,nil,tp)
end
function c7076131.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,c7076131.filter,tp,LOCATION_DECK,0,1,1,nil,tc:GetCode())
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end

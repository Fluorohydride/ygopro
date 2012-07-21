--レグルス
function c20210570.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(20210570,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c20210570.target)
	e1:SetOperation(c20210570.operation)
	c:RegisterEffect(e1)
end
function c20210570.filter(c)
	return c:IsType(TYPE_FIELD) and c:IsAbleToDeck()
end
function c20210570.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c20210570.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c20210570.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c20210570.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c20210570.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
	end
end

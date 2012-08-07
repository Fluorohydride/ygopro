--ピクシーナイト
function c35429292.initial_effect(c)
	--to deck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(35429292,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c35429292.condition)
	e1:SetTarget(c35429292.target)
	e1:SetOperation(c35429292.operation)
	c:RegisterEffect(e1)
end
function c35429292.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c35429292.filter(c)
	return c:IsType(TYPE_SPELL) and c:IsAbleToDeck()
end
function c35429292.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c35429292.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(1-tp,c35429292.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c35429292.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
	end
end

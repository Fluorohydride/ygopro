--赤い忍者
function c14618326.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(14618326,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c14618326.target)
	e1:SetOperation(c14618326.operation)
	c:RegisterEffect(e1)
end
function c14618326.filter(c)
	return c:IsFacedown() or c:IsType(TYPE_TRAP)
end
function c14618326.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c14618326.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c14618326.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	if g:GetCount()>0 and g:GetFirst():IsFaceup() then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c14618326.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(tp,tc) end
		if tc:IsType(TYPE_TRAP) then Duel.Destroy(tc,REASON_EFFECT) end
	end
end
--シャドー・トゥーン
function c6958551.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1,6958551+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c6958551.condition)
	e1:SetTarget(c6958551.target)
	e1:SetOperation(c6958551.activate)
	c:RegisterEffect(e1)
end
function c6958551.cfilter(c)
	return c:IsFaceup() and c:IsCode(15259703)
end
function c6958551.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c6958551.cfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c6958551.filter(c)
	return c:IsFaceup() and c:GetAttack()>0
end
function c6958551.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c6958551.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c6958551.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c6958551.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetFirst():GetAttack())
end
function c6958551.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:GetAttack()>0 then
		Duel.Damage(1-tp,tc:GetAttack(),REASON_EFFECT)
	end
end

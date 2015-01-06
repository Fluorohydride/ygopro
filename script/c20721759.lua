--ヘイト・クレバス
function c20721759.initial_effect(c)
	--tograve
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c20721759.condition)
	e1:SetTarget(c20721759.target)
	e1:SetOperation(c20721759.operation)
	c:RegisterEffect(e1)
end
function c20721759.condition(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	return rp~=tp and eg:GetCount()==1 and ec:IsPreviousLocation(LOCATION_MZONE) and ec:GetPreviousControler()==tp
		and ec:IsReason(REASON_DESTROY) and ec:IsReason(REASON_EFFECT)
end
function c20721759.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) end
	if chk==0 then return Duel.IsExistingTarget(nil,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectTarget(tp,nil,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetFirst():GetBaseAttack())
end
function c20721759.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) then return end
	Duel.SendtoGrave(tc,REASON_EFFECT)
	if not tc:IsLocation(LOCATION_GRAVE) then return end
	local atk=tc:GetBaseAttack()
	if atk<0 then atk=0 end
	Duel.Damage(1-tp,atk,REASON_EFFECT)
end

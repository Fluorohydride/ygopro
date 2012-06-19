--ゲットライド！
function c91597389.initial_effect(c)
	--equip
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c91597389.target)
	e1:SetOperation(c91597389.operation)
	c:RegisterEffect(e1)
end
function c91597389.filter(c,tp)
	return c:IsType(TYPE_UNION) and Duel.IsExistingMatchingCard(c91597389.filter2,tp,LOCATION_MZONE,0,1,nil,c)
end
function c91597389.filter2(c,eqc)
	return c:IsFaceup() and c:GetUnionCount()==0 and eqc:CheckEquipTarget(c)
end
function c91597389.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c91597389.filter(chkc,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>(e:GetHandler():IsLocation(LOCATION_SZONE) and 0 or 1)
		and Duel.IsExistingTarget(c91597389.filter,tp,LOCATION_GRAVE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectTarget(tp,c91597389.filter,tp,LOCATION_GRAVE,0,1,1,nil,tp)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,g,1,0,0)
end
function c91597389.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		local g=Duel.SelectMatchingCard(tp,c91597389.filter2,tp,LOCATION_MZONE,0,1,1,nil,tc)
		if g:GetCount()>0 then
			Duel.Equip(tp,tc,g:GetFirst())
			tc:SetStatus(STATUS_UNION,true)
		end
	end
end

--フレグランス・ストーム
function c69584564.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c69584564.target)
	e1:SetOperation(c69584564.operation)
	c:RegisterEffect(e1)
end
function c69584564.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_PLANT) and c:IsDestructable()
end
function c69584564.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c69584564.filter(chkc) end
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1)
		and Duel.IsExistingTarget(c69584564.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c69584564.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c69584564.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		local ct=Duel.Draw(tp,1,REASON_EFFECT)
		if ct==0 then return end
		local dc=Duel.GetOperatedGroup():GetFirst()
		if dc:IsRace(RACE_PLANT) and Duel.IsPlayerCanDraw(tp,1)
			and Duel.SelectYesNo(tp,aux.Stringid(69584564,0)) then
			Duel.BreakEffect()
			Duel.ConfirmCards(1-tp,dc)
			Duel.Draw(tp,1,REASON_EFFECT)
			Duel.ShuffleHand(tp)
		end
	end
end

--拘束解放波
function c98847704.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c98847704.target)
	e1:SetOperation(c98847704.activate)
	c:RegisterEffect(e1)
end
function c98847704.filter1(c)
	return c:IsFaceup() and c:IsType(TYPE_EQUIP) and c:IsDestructable()
end
function c98847704.filter2(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c98847704.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c98847704.filter1(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c98847704.filter1,tp,LOCATION_SZONE,0,1,nil)
		and Duel.IsExistingMatchingCard(c98847704.filter2,tp,0,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c98847704.filter1,tp,LOCATION_SZONE,0,1,1,nil)
	local dg=Duel.GetMatchingGroup(c98847704.filter2,tp,0,LOCATION_SZONE,nil)
	dg:Merge(g)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c98847704.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		local dg=Duel.GetMatchingGroup(c98847704.filter2,tp,0,LOCATION_SZONE,nil)
		dg:AddCard(tc)
		Duel.Destroy(dg,REASON_EFFECT)
	end
end

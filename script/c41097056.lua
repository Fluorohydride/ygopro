--シンクロ・クラッカー
function c41097056.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c41097056.target)
	e1:SetOperation(c41097056.activate)
	c:RegisterEffect(e1)
end
function c41097056.filter(c,tp)
	return c:IsFaceup() and c:IsType(TYPE_SYNCHRO) and c:IsAbleToExtra()
		and Duel.IsExistingMatchingCard(c41097056.desfilter,tp,0,LOCATION_MZONE,1,nil,math.max(0,c:GetTextAttack()))
end
function c41097056.desfilter(c,atk)
	return c:IsFaceup() and c:IsAttackBelow(atk) and c:IsDestructable()
end
function c41097056.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c41097056.filter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c41097056.filter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g1=Duel.SelectTarget(tp,c41097056.filter,tp,LOCATION_MZONE,0,1,1,nil,tp)
	local g2=Duel.GetMatchingGroup(c41097056.desfilter,tp,0,LOCATION_MZONE,nil,math.max(0,g1:GetFirst():GetTextAttack()))
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g1,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g2,g2:GetCount(),0,0)
end
function c41097056.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local atk=tc:GetTextAttack()
	if atk<0 then atk=0 end
	if tc:IsRelateToEffect(e) and Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)~=0 and tc:IsLocation(LOCATION_EXTRA) then
		local g=Duel.GetMatchingGroup(c41097056.desfilter,tp,0,LOCATION_MZONE,nil,atk)
		Duel.Destroy(g,REASON_EFFECT)
	end
end

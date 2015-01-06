--ヒーロー・ブラスト
function c37412656.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetTarget(c37412656.target)
	e1:SetOperation(c37412656.activate)
	c:RegisterEffect(e1)
end
function c37412656.filter(c)
	return c:IsSetCard(0x3008) and c:IsType(TYPE_NORMAL) and c:IsAbleToHand()
end
function c37412656.dfilter(c,atk)
	return c:IsFaceup() and c:IsDestructable() and c:IsAttackBelow(atk)
end
function c37412656.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:GetLocation()==LOCATION_GRAVE and chkc:GetControler()==tp and c37412656.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c37412656.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c37412656.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
	local tc=g:GetFirst()
	local dg=Duel.GetMatchingGroup(c37412656.dfilter,tp,0,LOCATION_MZONE,nil,tc:GetAttack())
	if dg:GetCount()>0 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,1,0,0)
	end
end
function c37412656.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local dg=Duel.SelectMatchingCard(tp,c37412656.dfilter,tp,0,LOCATION_MZONE,1,1,nil,tc:GetAttack())
		Duel.Destroy(dg,REASON_EFFECT)
	end
end

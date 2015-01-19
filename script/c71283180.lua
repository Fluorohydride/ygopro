--トルネード・バード
function c71283180.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(71283180,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c71283180.target)
	e1:SetOperation(c71283180.operation)
	c:RegisterEffect(e1)
end
function c71283180.filter(c,e)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToHand() and c:IsCanBeEffectTarget(e)
end
function c71283180.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c71283180.filter(chkc,e) end
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(c71283180.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil,e)
	if g:GetCount()<2 then
		g:Clear()
		Duel.SetTargetCard(g)
		return
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local sg=g:Select(tp,2,2,nil)
	Duel.SetTargetCard(sg)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,sg,2,0,0)
end
function c71283180.operation(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if not tg then return end
	local dg=tg:Filter(Card.IsRelateToEffect,nil,e)
	if dg:GetCount()==2 then
		Duel.SendtoHand(dg,nil,REASON_EFFECT)
	end
end

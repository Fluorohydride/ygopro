--フォトン・ベール
function c9354555.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c9354555.target)
	e1:SetOperation(c9354555.activate)
	c:RegisterEffect(e1)
end
function c9354555.filter1(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToDeck()
end
function c9354555.filter2(c)
	return c:IsLevelBelow(4) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToHand()
end
function c9354555.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c9354555.filter1,tp,LOCATION_HAND,0,3,nil)
		and Duel.IsExistingMatchingCard(c9354555.filter2,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,3,tp,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c9354555.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c9354555.filter1,tp,LOCATION_HAND,0,nil)
	if g:GetCount()<3 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local dg=g:Select(tp,3,3,nil)
	Duel.ConfirmCards(1-tp,dg)
	Duel.SendtoDeck(dg,nil,2,REASON_EFFECT)
	local sg=Duel.GetMatchingGroup(c9354555.filter2,tp,LOCATION_DECK,0,nil)
	if sg:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local hg=sg:Select(tp,1,1,nil)
	sg:RemoveCard(hg:GetFirst())
	sg=sg:Filter(Card.IsCode,nil,hg:GetFirst():GetCode())
	if sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(9354555,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local tg=sg:Select(tp,1,2,nil)
		hg:Merge(tg)
	end
	Duel.SendtoHand(hg,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,hg)
end

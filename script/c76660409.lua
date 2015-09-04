--ペンデュラム・バック
function c76660409.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c76660409.condition)
	e1:SetTarget(c76660409.target)
	e1:SetOperation(c76660409.activate)
	c:RegisterEffect(e1)
end
function c76660409.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldCard(tp,LOCATION_SZONE,6) and Duel.GetFieldCard(tp,LOCATION_SZONE,7)
end
function c76660409.filter(c,lsc,rsc)
	local lv=c:GetLevel()
	return lv>lsc and lv<rsc and c:IsAbleToHand()
end
function c76660409.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local lsc=Duel.GetFieldCard(tp,LOCATION_SZONE,6):GetLeftScale()
	local rsc=Duel.GetFieldCard(tp,LOCATION_SZONE,7):GetRightScale()
	if lsc>rsc then lsc,rsc=rsc,lsc end
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c76660409.filter(chkc,lsc,rsc) end
	if chk==0 then return Duel.IsExistingTarget(c76660409.filter,tp,LOCATION_GRAVE,0,2,nil,lsc,rsc) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c76660409.filter,tp,LOCATION_GRAVE,0,2,2,nil,lsc,rsc)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,2,0,0)
end
function c76660409.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()==2 then
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sg)
	end
end

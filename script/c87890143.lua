--ジェネレーション・フォース
function c87890143.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c87890143.condition)
	e1:SetTarget(c87890143.target)
	e1:SetOperation(c87890143.activate)
	c:RegisterEffect(e1)
end
function c87890143.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c87890143.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c87890143.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c87890143.filter(c)
	return c:IsSetCard(0x73) and c:IsAbleToHand()
end
function c87890143.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c87890143.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c87890143.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c87890143.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end

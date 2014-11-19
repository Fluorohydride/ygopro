--おねだりゴブリン
function c12152769.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(12152769,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c12152769.condition)
	e1:SetTarget(c12152769.target)
	e1:SetOperation(c12152769.operation)
	c:RegisterEffect(e1)
end
function c12152769.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c12152769.filter(c)
	return c:IsSetCard(0xac) and c:IsAbleToHand()
end
function c12152769.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c12152769.operation(e,tp,eg,ep,ev,re,r,rp)
	local hg=Duel.GetFieldGroup(1-tp,LOCATION_HAND,0)
	if hg:GetCount()>0 and Duel.SelectYesNo(1-tp,aux.Stringid(12152769,1)) then
		Duel.Hint(HINT_SELECTMSG,1-tp,aux.Stringid(12152769,2))
		local sg=hg:Select(1-tp,1,1,nil)
		Duel.SendtoHand(sg,tp,REASON_EFFECT)
		if Duel.IsChainDisablable(0) then
			Duel.NegateEffect(0)
			return
		end
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c12152769.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end

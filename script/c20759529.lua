--キッズ·ガード
function c20759529.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c20759529.condition)
	e1:SetCost(c20759529.cost)
	e1:SetTarget(c20759529.target)
	e1:SetOperation(c20759529.activate)
	c:RegisterEffect(e1)
end
function c20759529.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c20759529.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,32679370) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,32679370)
	Duel.Release(g,REASON_COST)
end
function c20759529.filter(c)
	return c:IsSetCard(0x3008) and c:IsAbleToHand()
end
function c20759529.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c20759529.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c20759529.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateAttack()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c20759529.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
